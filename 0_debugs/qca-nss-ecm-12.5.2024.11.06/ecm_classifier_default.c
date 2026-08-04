/*
 * Copyright (c) 2014-2016, 2020-2021, The Linux Foundation. All rights reserved.
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/printk.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/icmp.h>
#include <linux/debugfs.h>
#include <linux/kthread.h>
#include <linux/pkt_sched.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <net/route.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <asm/unaligned.h>
#include <asm/uaccess.h>
#include <net/ipv6.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/udp.h>
#include <linux/tcp.h>

#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_l4proto.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/ipv4/nf_conntrack_ipv4.h>
#include <net/netfilter/ipv4/nf_defrag_ipv4.h>
/*
 * Debug output levels
 * 0 = OFF
 * 1 = ASSERTS / ERRORS
 * 2 = 1 + WARN
 * 3 = 2 + INFO
 * 4 = 3 + TRACE
 */
#define DEBUG_LEVEL ECM_CLASSIFIER_DEFAULT_DEBUG_LEVEL

#include "ecm_types.h"
#include "ecm_db_types.h"
#include "ecm_state.h"
#include "ecm_tracker.h"
#include "ecm_classifier.h"
#include "ecm_front_end_types.h"
#include "ecm_tracker_datagram.h"
#include "ecm_tracker_udp.h"
#include "ecm_tracker_tcp.h"
#include "ecm_db.h"
#include "ecm_classifier_default.h"
#include "ecm_front_end_common.h"

#define ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC 0x8761
#define ECM_CLASSIFIER_DEFAULT_STATE_FILE_INSTANCE_MAGIC 0x3321

struct ecm_classifier_default_internal_instance {
	struct ecm_classifier_default_instance base;
	uint32_t ci_serial;
	int protocol;
	struct ecm_classifier_process_response process_response;
	ecm_db_timer_group_t timer_group;
	ecm_tracker_sender_type_t ingress_sender;
	ecm_tracker_sender_type_t egress_sender;
	struct ecm_tracker_instance *ti;
	bool packet_seen[ECM_CONN_DIR_MAX];
	int refs;
	bool classified;
	bool decelerated;
	#if (DEBUG_LEVEL > 0)
	uint16_t magic;
	#endif
};

static DEFINE_SPINLOCK(ecm_classifier_default_lock);
static int ecm_classifier_default_count = 0;

static ecm_classifier_acceleration_mode_t ecm_classifier_default_accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_ACCEL;
static int ecm_classifier_default_enabled = 1;
static bool ecm_classifier_default_terminate_pending = false;

#define ECM_CLASSIFIER_DEFAULT_STATE_FILE_BUFFER_SIZE 1024
struct ecm_classifier_default_state_file_instance {
	struct ecm_classifier_default_internal_instance *cdii;
	bool doc_start_written;
	bool doc_end_written;
	char msg_buffer[ECM_CLASSIFIER_DEFAULT_STATE_FILE_BUFFER_SIZE];
	char *msgp;
	int msg_len;
	#if (DEBUG_LEVEL > 0)
	uint16_t magic;
	#endif
};
static struct dentry *ecm_classifier_default_dentry;

/*
 * _ecm_classifier_default_ref()
 *	Ref
 */
static void _ecm_classifier_default_ref(struct ecm_classifier_default_internal_instance *cdii)
{
	cdii->refs++;
	DEBUG_TRACE("%px: cdii ref %d\n", cdii, cdii->refs);
	DEBUG_ASSERT(cdii->refs > 0, "%px: ref wrap\n", cdii);
}

/*
 * ecm_classifier_default_ref()
 *	Ref
 */
static void ecm_classifier_default_ref(struct ecm_classifier_instance *ci)
{
	struct ecm_classifier_default_internal_instance *cdii;
	cdii = (struct ecm_classifier_default_internal_instance *)ci;

	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
	spin_lock_bh(&ecm_classifier_default_lock);
	_ecm_classifier_default_ref(cdii);
	spin_unlock_bh(&ecm_classifier_default_lock);
}

/*
 * ecm_classifier_default_deref()
 *	Deref
 */
static int ecm_classifier_default_deref(struct ecm_classifier_instance *ci)
{
	struct ecm_classifier_default_internal_instance *cdii;
	cdii = (struct ecm_classifier_default_internal_instance *)ci;

	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
	spin_lock_bh(&ecm_classifier_default_lock);
	cdii->refs--;
	DEBUG_ASSERT(cdii->refs >= 0, "%px: refs wrapped\n", cdii);
	DEBUG_TRACE("%px: Default classifier deref %d\n", cdii, cdii->refs);
	if (cdii->refs) {
		int refs = cdii->refs;
		spin_unlock_bh(&ecm_classifier_default_lock);
		return refs;
	}

	ecm_classifier_default_count--;
	DEBUG_ASSERT(ecm_classifier_default_count >= 0, "%px: ecm_classifier_default_count wrap\n", cdii);

	spin_unlock_bh(&ecm_classifier_default_lock);

	cdii->ti->deref(cdii->ti);

	DEBUG_INFO("%px: Final default classifier instance\n", cdii);
	kfree(cdii);

	return 0;
}

/*
 * ecm_classifier_default_process()
 *	Process the flow for acceleration decision.
 */
static void ecm_classifier_default_process(struct ecm_classifier_instance *aci, ecm_tracker_sender_type_t sender,
										   struct ecm_tracker_ip_header *ip_hdr, struct sk_buff *skb,
										   struct ecm_classifier_process_response *process_response)
{
	struct ecm_classifier_default_internal_instance *cdii = (struct ecm_classifier_default_internal_instance *)aci;
	struct ecm_db_connection_instance *ci = NULL;
	struct ecm_front_end_connection_instance *feci = NULL;
	struct ecm_classifier_instance *mark_classi = NULL;
	struct ecm_classifier_process_response mark_pr;
	bool mark_denies = false;
	bool mark_present = false;
	bool is_syn = false;

	/* DEBUG: Verificar si es un SYN (TCP) */
	if (skb && skb->protocol == ETH_P_IP) {
		struct iphdr *ip = (struct iphdr *)skb->data;
		if (ip->protocol == IPPROTO_TCP) {
			struct tcphdr *tcp = (struct tcphdr *)(skb->data + (ip->ihl * 4));
			if (tcp->syn && !tcp->ack) {
				is_syn = true;
				// printk(KERN_INFO "DEFAULT: SYN packet detected for serial %u\n", cdii->ci_serial);
			}
		}
	}

	/* DEBUG: SI NO ES SYN, NO PROCESAR (solo el SYN decide el offload) */
	if (!is_syn) {
		spin_lock_bh(&ecm_classifier_default_lock);
		if (cdii->classified) {
			*process_response = cdii->process_response;
			spin_unlock_bh(&ecm_classifier_default_lock);
			return;
		}
		spin_unlock_bh(&ecm_classifier_default_lock);
	}

	// printk(KERN_INFO "DEFAULT: process() called for serial %u\n", cdii->ci_serial);

	/* Verifica si ya está clasificada  */
	spin_lock_bh(&ecm_classifier_default_lock);
	if (cdii->classified) {
		*process_response = cdii->process_response;
		spin_unlock_bh(&ecm_classifier_default_lock);
		// printk(KERN_INFO "DEFAULT: Already classified, returning previous decision\n");
		return;
	}
	spin_unlock_bh(&ecm_classifier_default_lock);

	/* CONSULTAR MARK CLASSIFIER (solo para SYN) */
	ci = ecm_db_connection_serial_find_and_ref(cdii->ci_serial);
	if (ci) {
		mark_classi = ecm_db_connection_assigned_classifier_find_and_ref(ci, ECM_CLASSIFIER_TYPE_MARK);
		if (mark_classi) {
			mark_present = true;
			// printk(KERN_INFO "DEFAULT: MARK classifier FOUND on connection\n");
			mark_classi->last_process_response_get(mark_classi, &mark_pr);
			mark_classi->deref(mark_classi);
			// printk(KERN_INFO "DEFAULT: MARK accel_mode=%d, relevance=%d\n", mark_pr.accel_mode, mark_pr.relevance);

			/* Si MARK dice NO_ACCEL (1), denegar */
			if (mark_pr.relevance == ECM_CLASSIFIER_RELEVANCE_YES &&
				mark_pr.accel_mode == ECM_CLASSIFIER_ACCELERATION_MODE_NO) {
				mark_denies = true;
				// printk(KERN_INFO "MARK: DEFAULT forcing slow path (mark denied)\n");
				} else if (mark_pr.relevance == ECM_CLASSIFIER_RELEVANCE_YES &&
					mark_pr.accel_mode == ECM_CLASSIFIER_ACCELERATION_MODE_ACCEL) {
					// printk(KERN_INFO "MARK: DEFAULT sees MARK says ACCEL\n");
					} else {
						printk(KERN_INFO "MARK: DEFAULT sees MARK says DONT_CARE or MAYBE\n");
					}
		} else {
			// printk(KERN_INFO "DEFAULT: MARK classifier NOT FOUND on connection\n");
		}
		ecm_db_connection_deref(ci);
	} else {
		printk(KERN_INFO "DEFAULT: Connection NOT found for serial %u\n", cdii->ci_serial);
	}

	/* DECISION */
	spin_lock_bh(&ecm_classifier_default_lock);

	if (mark_present) {
		if (mark_denies) {
			/* MARK dice NO_ACCEL */
			cdii->process_response.accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_NO;
			cdii->process_response.process_actions |= ECM_CLASSIFIER_PROCESS_ACTION_ACCEL_MODE;

			if (!cdii->decelerated) {
				feci = ecm_db_connection_front_end_get_and_ref(ci);
				if (feci) {
					if (feci->decelerate) {
						feci->decelerate(feci);
						// printk(KERN_INFO "MARK: DECELERATED connection (destroyed NSS rule)\n");
					}
					ecm_front_end_connection_deref(feci);
				}
				cdii->decelerated = true;
			}
			// printk(KERN_INFO "DEFAULT: MARK says NO_ACCEL, decision = NO_ACCEL\n");
		} else {
			/* MARK dice ACCEL o DONT_CARE → DEFAULT acelera */
			cdii->process_response.accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_ACCEL;
			cdii->process_response.process_actions |= ECM_CLASSIFIER_PROCESS_ACTION_ACCEL_MODE;
			// printk(KERN_INFO "DEFAULT: MARK says ACCEL or DONT_CARE, decision = ACCEL\n");
		}
	} else {
		/* MARK NO está presente, DEFAULT decide por sí mismo */
		cdii->process_response.accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_ACCEL;
		cdii->process_response.process_actions |= ECM_CLASSIFIER_PROCESS_ACTION_ACCEL_MODE;
		// printk(KERN_INFO "DEFAULT: MARK NOT present, decision = ACCEL\n");
	}
	/* DEBUG: Marcar como clasificada para no volver a procesar */
	cdii->classified = true;
	/* Establecer YES para que NSS sepa que debe acelerar */
	cdii->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_YES;
	*process_response = cdii->process_response;

	spin_unlock_bh(&ecm_classifier_default_lock);

	// printk(KERN_INFO "DEFAULT: Decision = %s, classified=%d, mark_present=%d\n", cdii->process_response.accel_mode == ECM_CLASSIFIER_ACCELERATION_MODE_NO ? "NO_ACCEL" : "ACCEL", cdii->classified, mark_present);
}

/*
 * ecm_classifier_default_type_get()
 *	Get type of classifier this is
 */
static ecm_classifier_type_t ecm_classifier_default_type_get(struct ecm_classifier_instance *aci)
{
	struct ecm_classifier_default_internal_instance *cdii;
	cdii = (struct ecm_classifier_default_internal_instance *)aci;

	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
	return ECM_CLASSIFIER_TYPE_DEFAULT;
}

/*
 * ecm_classifier_default_reclassify_allowed()
 *	Get whether reclassification is allowed
 */
static bool ecm_classifier_default_reclassify_allowed(struct ecm_classifier_instance *aci)
{
	struct ecm_classifier_default_internal_instance *cdii;
	cdii = (struct ecm_classifier_default_internal_instance *)aci;

	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
	return true;
}

/*
 * ecm_classifier_default_reclassify()
 *	Reclassify
 */
static void ecm_classifier_default_reclassify(struct ecm_classifier_instance *aci)
{
	struct ecm_classifier_default_internal_instance *cdii;
	cdii = (struct ecm_classifier_default_internal_instance *)aci;
	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
}

/*
 * ecm_classifier_default_last_process_response_get()
 *	Get result code returned by the last process call
 */
static void ecm_classifier_default_last_process_response_get(struct ecm_classifier_instance *aci,
															 struct ecm_classifier_process_response *process_response)
{
	struct ecm_classifier_default_internal_instance *cdii;
	cdii = (struct ecm_classifier_default_internal_instance *)aci;
	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);

	spin_lock_bh(&ecm_classifier_default_lock);
	*process_response = cdii->process_response;
	spin_unlock_bh(&ecm_classifier_default_lock);
}

/*
 * ecm_classifier_default_sync_to_v4()
 *	Front end is pushing accel engine state to us
 */
static void ecm_classifier_default_sync_to_v4(struct ecm_classifier_instance *aci, struct ecm_classifier_rule_sync *sync)
{
	struct ecm_classifier_default_internal_instance *cdii __attribute__((unused));

	cdii = (struct ecm_classifier_default_internal_instance *)aci;
	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
}

/*
 * ecm_classifier_default_sync_from_v4()
 *	Front end is retrieving accel engine state from us
 */
static void ecm_classifier_default_sync_from_v4(struct ecm_classifier_instance *aci, struct ecm_classifier_rule_create *ecrc)
{
	struct ecm_classifier_default_internal_instance *cdii __attribute__((unused));

	cdii = (struct ecm_classifier_default_internal_instance *)aci;
	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
}

/*
 * ecm_classifier_default_sync_to_v6()
 *	Front end is pushing accel engine state to us
 */
static void ecm_classifier_default_sync_to_v6(struct ecm_classifier_instance *aci, struct ecm_classifier_rule_sync *sync)
{
	struct ecm_classifier_default_internal_instance *cdii __attribute__((unused));

	cdii = (struct ecm_classifier_default_internal_instance *)aci;
	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
}

/*
 * ecm_classifier_default_sync_from_v6()
 *	Front end is retrieving accel engine state from us
 */
static void ecm_classifier_default_sync_from_v6(struct ecm_classifier_instance *aci, struct ecm_classifier_rule_create *ecrc)
{
	struct ecm_classifier_default_internal_instance *cdii __attribute__((unused));

	cdii = (struct ecm_classifier_default_internal_instance *)aci;
	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);
}

/*
 * ecm_classifier_tracker_get_and_ref()
 *	Obtain default classifiers tracker (usually for state tracking for the connection as it always exists for the connection)
 */
static struct ecm_tracker_instance *ecm_classifier_tracker_get_and_ref(struct ecm_classifier_default_instance *dci)
{
	struct ecm_classifier_default_internal_instance *cdii;
	struct ecm_tracker_instance *ti;

	cdii = (struct ecm_classifier_default_internal_instance *)dci;
	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);

	ti = cdii->ti;
	ti->ref(ti);
	return ti;
}

#ifdef ECM_STATE_OUTPUT_ENABLE
/*
 * ecm_classifier_default_state_get()
 *	Return state
 */
static int ecm_classifier_default_state_get(struct ecm_classifier_instance *ci, struct ecm_state_file_instance *sfi)
{
	int result;
	struct ecm_classifier_default_internal_instance *cdii;
	struct ecm_classifier_process_response process_response;
	ecm_db_timer_group_t timer_group;
	ecm_tracker_sender_type_t ingress_sender;
	ecm_tracker_sender_type_t egress_sender;

	cdii = (struct ecm_classifier_default_internal_instance *)ci;
	DEBUG_CHECK_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC, "%px: magic failed", cdii);

	if ((result = ecm_state_prefix_add(sfi, "default"))) {
		return result;
	}

	spin_lock_bh(&ecm_classifier_default_lock);
	egress_sender = cdii->egress_sender;
	ingress_sender = cdii->ingress_sender;
	timer_group = cdii->timer_group;
	process_response = cdii->process_response;
	spin_unlock_bh(&ecm_classifier_default_lock);

	if ((result = ecm_state_write(sfi, "ingress_sender", "%d", ingress_sender))) {
		return result;
	}
	if ((result = ecm_state_write(sfi, "egress_sender", "%d", egress_sender))) {
		return result;
	}
	if ((result = ecm_state_write(sfi, "timer_group", "%d", timer_group))) {
		return result;
	}
	if ((result = ecm_state_write(sfi, "accel_delay_pkt_default", "%d", ecm_classifier_accel_delay_pkts))) {
		return result;
	}

	if ((result = ecm_classifier_process_response_state_get(sfi, &process_response))) {
		return result;
	}

	if ((result = ecm_state_prefix_add(sfi, "trackers"))) {
		return result;
	}

	if ((result = cdii->ti->state_text_get(cdii->ti, sfi))) {
		return result;
	}

	if ((result = ecm_state_prefix_remove(sfi))) {
		return result;
	}

	return ecm_state_prefix_remove(sfi);
}
#endif

/*
 * ecm_classifier_default_instance_alloc()
 * Allocate an instance of the default classifier
 */
struct ecm_classifier_default_instance *ecm_classifier_default_instance_alloc(struct ecm_db_connection_instance *ci, int protocol, ecm_db_direction_t dir, int from_port, int to_port)
{
	struct ecm_classifier_default_internal_instance *cdii;
	struct ecm_classifier_default_instance *cdi;

	cdii = (struct ecm_classifier_default_internal_instance *)kzalloc(sizeof(struct ecm_classifier_default_internal_instance), GFP_ATOMIC | __GFP_NOWARN);
	if (!cdii) {
		DEBUG_WARN("Failed to allocate default instance\n");
		return NULL;
	}

	if (protocol == IPPROTO_TCP) {
		DEBUG_TRACE("%px: Alloc tracker for TCP connection: %px\n", cdii, ci);
		cdii->ti = (struct ecm_tracker_instance *)ecm_tracker_tcp_alloc();
		if (!cdii->ti) {
			DEBUG_WARN("%px: Failed to alloc tracker\n", cdii);
			kfree(cdii);
			return NULL;
		}
		ecm_tracker_tcp_init((struct ecm_tracker_tcp_instance *)cdii->ti, ECM_TRACKER_CONNECTION_TRACKING_LIMIT_DEFAULT, 1500, 1500);
	} else if (protocol == IPPROTO_UDP) {
		DEBUG_TRACE("%px: Alloc tracker for UDP connection: %px\n", cdii, ci);
		cdii->ti = (struct ecm_tracker_instance *)ecm_tracker_udp_alloc();
		if (!cdii->ti) {
			DEBUG_WARN("%px: Failed to alloc tracker\n", cdii);
			kfree(cdii);
			return NULL;
		}
		ecm_tracker_udp_init((struct ecm_tracker_udp_instance *)cdii->ti, ECM_TRACKER_CONNECTION_TRACKING_LIMIT_DEFAULT, from_port, to_port);
	} else {
		DEBUG_TRACE("%px: Alloc tracker for non-ported connection: %px\n", cdii, ci);
		cdii->ti = (struct ecm_tracker_instance *)ecm_tracker_datagram_alloc();
		if (!cdii->ti) {
			DEBUG_WARN("%px: Failed to alloc tracker\n", cdii);
			kfree(cdii);
			return NULL;
		}
		ecm_tracker_datagram_init((struct ecm_tracker_datagram_instance *)cdii->ti, ECM_TRACKER_CONNECTION_TRACKING_LIMIT_DEFAULT);
	}

	DEBUG_SET_MAGIC(cdii, ECM_CLASSIFIER_DEFAULT_INTERNAL_INSTANCE_MAGIC);
	cdii->refs = 1;
	cdii->ci_serial = ecm_db_connection_serial_get(ci);
	cdii->protocol = protocol;

	cdii->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_YES;
	cdii->classified = false;
	cdii->decelerated = false;

	if (dir == ECM_DB_DIRECTION_INGRESS_NAT) {
		cdii->ingress_sender = ECM_TRACKER_SENDER_TYPE_SRC;
		cdii->egress_sender = ECM_TRACKER_SENDER_TYPE_DEST;
	} else {
		cdii->egress_sender = ECM_TRACKER_SENDER_TYPE_SRC;
		cdii->ingress_sender = ECM_TRACKER_SENDER_TYPE_DEST;
	}
	DEBUG_TRACE("%px: Ingress sender = %d egress sender = %d\n", cdii, cdii->ingress_sender, cdii->egress_sender);

	cdi = (struct ecm_classifier_default_instance *)cdii;
	cdi->tracker_get_and_ref = ecm_classifier_tracker_get_and_ref;

	cdi->base.process = ecm_classifier_default_process;
	cdi->base.sync_from_v4 = ecm_classifier_default_sync_from_v4;
	cdi->base.sync_to_v4 = ecm_classifier_default_sync_to_v4;
	cdi->base.sync_from_v6 = ecm_classifier_default_sync_from_v6;
	cdi->base.sync_to_v6 = ecm_classifier_default_sync_to_v6;
	cdi->base.type_get = ecm_classifier_default_type_get;
	cdi->base.reclassify_allowed = ecm_classifier_default_reclassify_allowed;
	cdi->base.reclassify = ecm_classifier_default_reclassify;
	cdi->base.last_process_response_get = ecm_classifier_default_last_process_response_get;
	#ifdef ECM_STATE_OUTPUT_ENABLE
	cdi->base.state_get = ecm_classifier_default_state_get;
	#endif
	cdi->base.ref = ecm_classifier_default_ref;
	cdi->base.deref = ecm_classifier_default_deref;

	spin_lock_bh(&ecm_classifier_default_lock);

	if (ecm_classifier_default_terminate_pending) {
		spin_unlock_bh(&ecm_classifier_default_lock);
		DEBUG_INFO("%px: Terminating\n", ci);
		cdii->ti->deref(cdii->ti);
		kfree(cdii);
		return NULL;
	}

	ecm_classifier_default_count++;
	DEBUG_ASSERT(ecm_classifier_default_count > 0, "%px: ecm_classifier_default_count wrap\n", cdii);
	spin_unlock_bh(&ecm_classifier_default_lock);

	DEBUG_INFO("Default classifier instance alloc: %px\n", cdii);
	return cdi;
}
EXPORT_SYMBOL(ecm_classifier_default_instance_alloc);

/*
 * ecm_classifier_default_init()
 */
int ecm_classifier_default_init(struct dentry *dentry)
{
	DEBUG_INFO("Default classifier Module init\n");

	DEBUG_ASSERT(ECM_CLASSIFIER_TYPE_DEFAULT == 0, "DO NOT CHANGE DEFAULT PRIORITY");

	ecm_classifier_default_dentry = debugfs_create_dir("ecm_classifier_default", dentry);
	if (!ecm_classifier_default_dentry) {
		DEBUG_ERROR("Failed to create ecm default classifier directory in debugfs\n");
		return -1;
	}

	if (!ecm_debugfs_create_u32("enabled", S_IRUGO | S_IWUSR, ecm_classifier_default_dentry,
		(u32 *)&ecm_classifier_default_enabled)) {
		DEBUG_ERROR("Failed to create ecm default classifier enabled file in debugfs\n");
	debugfs_remove_recursive(ecm_classifier_default_dentry);
	return -1;
		}

		if (!ecm_debugfs_create_u32("accel_mode", S_IRUGO | S_IWUSR, ecm_classifier_default_dentry,
			(u32 *)&ecm_classifier_default_accel_mode)) {
			DEBUG_ERROR("Failed to create ecm default classifier accel_mode file in debugfs\n");
		debugfs_remove_recursive(ecm_classifier_default_dentry);
		return -1;
			}

			if (!ecm_debugfs_create_u32("accel_delay_pkts", S_IRUGO | S_IWUSR, ecm_classifier_default_dentry,
				(u32 *)&ecm_classifier_accel_delay_pkts)) {
				DEBUG_ERROR("Failed to create accel delay packet counts in debugfs\n");
			debugfs_remove_recursive(ecm_classifier_default_dentry);
			return -1;
				}

				return 0;
}
EXPORT_SYMBOL(ecm_classifier_default_init);

/*
 * ecm_classifier_default_exit()
 */
void ecm_classifier_default_exit(void)
{
	DEBUG_INFO("Default classifier Module exit\n");
	spin_lock_bh(&ecm_classifier_default_lock);
	ecm_classifier_default_terminate_pending = true;
	spin_unlock_bh(&ecm_classifier_default_lock);

	if (ecm_classifier_default_dentry) {
		debugfs_remove_recursive(ecm_classifier_default_dentry);
	}
}
EXPORT_SYMBOL(ecm_classifier_default_exit);
