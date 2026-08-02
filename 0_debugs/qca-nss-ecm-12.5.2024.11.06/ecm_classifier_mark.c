/*
 **************************************************************************
 * Copyright (c) 2018, 2020-2021, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/*
 * Mark Classifier.
 * This module provides an interface to customer's external module to interract with the ECM.
 * It allows the customer to make smart decisions on the packets before offloading the connection to the
 * acceleration engine.
 */

/* DEBUG printk's */
#include <linux/version.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/debugfs.h>
#include <linux/ctype.h>
#include <net/tcp.h>
#include <net/ipv6.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/etherdevice.h>

/*
 * Debug output levels
 * 0 = OFF
 * 1 = ASSERTS / ERRORS
 * 2 = 1 + WARN
 * 3 = 2 + INFO
 * 4 = 3 + TRACE
 */
#define DEBUG_LEVEL ECM_CLASSIFIER_MARK_DEBUG_LEVEL

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
#include "ecm_classifier_mark.h"
#include "ecm_classifier_mark_public.h"
#include "ecm_front_end_common.h"
#include "ecm_front_end_ipv4.h"
#ifdef ECM_IPV6_ENABLE
#include "ecm_front_end_ipv6.h"
#endif

/*
 * Magic numbers
 */
#define ECM_CLASSIFIER_MARK_INSTANCE_MAGIC 0x2567

/*
 * struct ecm_classifier_mark_instance
 * 	State per connection for MARK classifier
 */
struct ecm_classifier_mark_instance {
	struct ecm_classifier_instance base;			/* Base type */

	uint32_t ci_serial;					/* RO: Serial of the connection */

	struct ecm_classifier_mark_instance *next;		/* Next classifier state instance (for accouting and reporting purposes) */
	struct ecm_classifier_mark_instance *prev;		/* Next classifier state instance (for accouting and reporting purposes) */

	uint32_t mark;						/* Mark value which this classifier extracted from the buffer */
	struct ecm_classifier_process_response process_response;
								/* Last process response computed */
	int refs;						/* Integer to trap we never go negative */
#if (DEBUG_LEVEL > 0)
	uint16_t magic;
#endif
};

/*
 * Operational control. By default it is disabled.
 */
/*
 * Ya no va a estar disabled por defecto
 */
static int ecm_classifier_mark_enabled = 1;

/* DEBUG
 *
 *
 * UEVAS variables de configuración
 */
static uint32_t ecm_classifier_mark_deny_value = 0x100;
static uint32_t ecm_classifier_mark_mode = 0;

/*
 * Management thread control
 */
static bool ecm_classifier_mark_terminate_pending;	/* True when the user wants us to terminate */

/*
 * Debugfs dentry object.
 */
static struct dentry *ecm_classifier_mark_dentry;

/*
 * Locking of the classifier structures
 */
static DEFINE_SPINLOCK(ecm_classifier_mark_lock);			/* Protect SMP access. */

/*
 * List of our classifier instances
 */
static struct ecm_classifier_mark_instance *ecm_classifier_mark_instances = NULL;
								/* list of all active instances */
static int ecm_classifier_mark_count = 0;			/* Tracks number of instances allocated */

/*
 * Callbacks to the external modules.
 */
ecm_classifier_mark_get_callback_t mark_get_cb[ECM_CLASSIFIER_MARK_TYPE_MAX] = { NULL };
								/* Callbacks which gets the mark from the packets */
ecm_classifier_mark_sync_to_ipv4_callback_t sync_to_ipv4_cb[ECM_CLASSIFIER_MARK_TYPE_MAX] = { NULL };
								/* Callbacks which syncs the IPv4 stats to the registered external subsystems */
ecm_classifier_mark_sync_to_ipv6_callback_t sync_to_ipv6_cb[ECM_CLASSIFIER_MARK_TYPE_MAX] = { NULL };
/* Callbacks which syncs the IPv6 stats to the registered external subsystems */


/*
 * DEBUG:
 *
 * Default mark callbacks, los externos no hacen nada
 *
 */
static ecm_classifier_mark_result_t ecm_classifier_mark_default_get(struct sk_buff *skb, uint32_t *mark)
{
	/* Extraer la marca del skb (si existe) */
	*mark = skb->mark;

	/* Si la marca es 0x100, denegar aceleración */
	if (*mark == 0x100) {
		DEBUG_INFO("MORK: Denying accel for mark 0x%x\n", *mark);
		return ECM_CLASSIFIER_MARK_RESULT_NOT_RELEVANT;
	}

	/* Cualquier otra marca o 0, permitir aceleración */
	DEBUG_TRACE("MARK: Allowing accel for mark 0x%x\n", *mark);
	return ECM_CLASSIFIER_MARK_RESULT_SUCCESS;
}

static void ecm_classifier_mark_default_sync_ipv4(uint32_t mark,
												  __be32 src_ip, int src_port,
												  __be32 dest_ip, int dest_port,
												  int protocol)
{
	/* Solo logging para depuración */
	DEBUG_TRACE("MARK Sync IPv4: mark=0x%x %pI4:%d -> %pI4:%d proto=%d\n",
				mark, &src_ip, src_port, &dest_ip, dest_port, protocol);
}

static void ecm_classifier_mark_default_sync_ipv6(uint32_t mark,
												  struct in6_addr *src_ip, int src_port,
												  struct in6_addr *dest_ip, int dest_port,
												  int protocol)
{
	DEBUG_TRACE("MARK Sync IPv6: mark=0x%x proto=%d\n", mark, protocol);
}


/*
 * _ecm_classifier_mark_ref()
 *	Ref
 */
static void _ecm_classifier_mark_ref(struct ecm_classifier_mark_instance *ecmi)
{
	ecmi->refs++;
	DEBUG_TRACE("%px: ecmi ref %d\n", ecmi, ecmi->refs);
	DEBUG_ASSERT(ecmi->refs > 0, "%px: ref wrap\n", ecmi);
}

/*
 * ecm_classifier_mark_ref()
 *	Ref
 */
static void ecm_classifier_mark_ref(struct ecm_classifier_instance *ci)
{
	struct ecm_classifier_mark_instance *ecmi;
	ecmi = (struct ecm_classifier_mark_instance *)ci;

	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);
	spin_lock_bh(&ecm_classifier_mark_lock);
	_ecm_classifier_mark_ref(ecmi);
	spin_unlock_bh(&ecm_classifier_mark_lock);
}

/*
 * ecm_classifier_mark_deref()
 *	Deref
 */
static int ecm_classifier_mark_deref(struct ecm_classifier_instance *ci)
{
	struct ecm_classifier_mark_instance *ecmi;
	ecmi = (struct ecm_classifier_mark_instance *)ci;

	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);
	spin_lock_bh(&ecm_classifier_mark_lock);
	ecmi->refs--;
	DEBUG_ASSERT(ecmi->refs >= 0, "%px: refs wrapped\n", ecmi);
	DEBUG_TRACE("%px: Mark classifier deref %d\n", ecmi, ecmi->refs);
	if (ecmi->refs) {
		int refs = ecmi->refs;
		spin_unlock_bh(&ecm_classifier_mark_lock);
		return refs;
	}

	/*
	 * Object to be destroyed
	 */
	ecm_classifier_mark_count--;
	DEBUG_ASSERT(ecm_classifier_mark_count >= 0, "%px: ecm_classifier_mark_count wrap\n", ecmi);

	/*
	 * UnLink the instance from our list
	 */
	if (ecmi->next) {
		ecmi->next->prev = ecmi->prev;
	}
	if (ecmi->prev) {
		ecmi->prev->next = ecmi->next;
	} else {
		DEBUG_ASSERT(ecm_classifier_mark_instances == ecmi, "%px: list bad %px\n", ecmi, ecm_classifier_mark_instances);
		ecm_classifier_mark_instances = ecmi->next;
	}
	ecmi->next = NULL;
	ecmi->prev = NULL;
	spin_unlock_bh(&ecm_classifier_mark_lock);

	/*
	 * Final
	 */
	DEBUG_INFO("%px: Final mark classifier instance\n", ecmi);
	kfree(ecmi);

	return 0;
}

/*
 * ecm_classifier_mark_process()
 *	Process new packet
 *
 * NOTE: This function would only ever be called if all other classifiers have failed.
 */
/*
 * DEBUG
 *
 * DEBUG 1  SOPORTE PARA MODO AUTOMÁTICO
 *
 */
static void ecm_classifier_mark_process(struct ecm_classifier_instance *aci,
										ecm_tracker_sender_type_t sender,
										struct ecm_tracker_ip_header *ip_hdr,
										struct sk_buff *skb,
										struct ecm_classifier_process_response *process_response)
{
	struct ecm_classifier_mark_instance *ecmi = (struct ecm_classifier_mark_instance *)aci;
	ecm_classifier_mark_result_t result;
	struct ecm_db_connection_instance *ci;
	bool skb_header_adjusted = false;
	uint32_t l2_encap_len;
	uint16_t l2_encap_proto;
	uint16_t skb_proto;
	uint32_t mark = 0;
	ecm_classifier_mark_get_callback_t cb = NULL;
	ecm_classifier_mark_type_t type = ECM_CLASSIFIER_MARK_TYPE_L2_ENCAP;

	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: invalid state magic\n", ecmi);

	/*
	 * Not relevant to the connection if not enabled.
	 */
	if (unlikely(!ecm_classifier_mark_enabled)) {
		DEBUG_WARN("%px: Mark classifier is not enabled.\n", aci);
		goto not_relevant;
	}

	/*
	 * Get connection
	 */
	ci = ecm_db_connection_serial_find_and_ref(ecmi->ci_serial);
	if (!ci) {
		DEBUG_WARN("%px: connection instance gone while processing classifier.\n", aci);
		goto not_relevant;
	}

	/* DEBUG 1  SOPORTE PARA MODO AUTOMÁTICO */

	/* Si el modo es 2 (allow all), permitir aceleración directamente */
	if (ecm_classifier_mark_mode == 2) {
		DEBUG_TRACE("%px: MARK mode=2 (allow all)\n", aci);
		spin_lock_bh(&ecm_classifier_mark_lock);
		ecmi->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_YES;
		ecmi->process_response.process_actions = ECM_CLASSIFIER_PROCESS_ACTION_ACCEL_MODE;
		ecmi->process_response.accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_ACCEL;
		*process_response = ecmi->process_response;
		spin_unlock_bh(&ecm_classifier_mark_lock);
		ecm_db_connection_deref(ci);
		return;
	}

	/* Si el modo es 1 (deny all), denegar aceleración siempre */
	if (ecm_classifier_mark_mode == 1) {
		DEBUG_TRACE("%px: MARK mode=1 (deny all)\n", aci);
		spin_lock_bh(&ecm_classifier_mark_lock);
		ecmi->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_YES;
		ecmi->process_response.process_actions = ECM_CLASSIFIER_PROCESS_ACTION_ACCEL_MODE;
		ecmi->process_response.accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_NO;
		*process_response = ecmi->process_response;
		spin_unlock_bh(&ecm_classifier_mark_lock);
		ecm_db_connection_deref(ci);
		return;
	}

	/*
	 * Get the L2 encap protocol of the connection.
	 */
	l2_encap_proto = ecm_db_connection_l2_encap_proto_get(ci);
	if (!l2_encap_proto) {
		DEBUG_WARN("%px: L2 encap protocol is zero.\n", aci);
		ecm_db_connection_deref(ci);
		goto not_relevant;
	}

	/*
	 * Is there an external callback to get the mark value from the packet?
	 */
	rcu_read_lock();
	cb = rcu_dereference(mark_get_cb[type]);
	if (!cb) {
		/* === MODIFICACIÓN: USAR CALLBACK POR DEFECTO === */
		rcu_read_unlock();
		DEBUG_INFO("%px: No external callback, using default\n", aci);

		/* Usar el callback por defecto directamente */
		result = ecm_classifier_mark_default_get(skb, &mark);
		if (result == ECM_CLASSIFIER_MARK_RESULT_SUCCESS) {
			ecm_db_connection_mark_set(ci, mark);
			ecmi->mark = mark;

			spin_lock_bh(&ecm_classifier_mark_lock);
			ecmi->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_YES;
			ecmi->process_response.process_actions = ECM_CLASSIFIER_PROCESS_ACTION_ACCEL_MODE;
			ecmi->process_response.accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_ACCEL;
			*process_response = ecmi->process_response;
			spin_unlock_bh(&ecm_classifier_mark_lock);
			ecm_db_connection_deref(ci);
			return;
		} else {
			/* NOT_RELEVANT */
			ecm_db_connection_deref(ci);
			goto not_relevant;
		}
	}

	/*
	 * Before giving the skb to the external callback for getting the mark value,
	 * let's adjust the L2 encap header.
	 */
	if (l2_encap_proto != ntohs(skb->protocol)) {
		DEBUG_TRACE("%px: Connections L2 encap protocol: %x is different than skb's protocol: %x, need header adjustment.\n",
					aci, l2_encap_proto, ntohs(skb->protocol));
		l2_encap_len = ecm_front_end_l2_encap_header_len(l2_encap_proto);
		skb_header_adjusted = true;
		ecm_front_end_push_l2_encap_header(skb, l2_encap_len);
		skb_proto = skb->protocol;
		skb->protocol = htons(l2_encap_proto);
	}

	/*
	 * Call the external callback and get the result.
	 */
	result = cb(skb, &mark);
	rcu_read_unlock();

	/*
	 * Pull the skb's L2 header, if it is adjusted in this classifier.
	 */
	if (skb_header_adjusted) {
		DEBUG_TRACE("%px: Adjust the L2 header back.\n", aci);
		ecm_front_end_pull_l2_encap_header(skb, l2_encap_len);
		skb->protocol = skb_proto;
	}

	/*
	 * Handle the result
	 */
	switch (result) {
		case ECM_CLASSIFIER_MARK_RESULT_NOT_YET:
			DEBUG_WARN("%px: External callback has not decided yet.\n", aci);
			goto deny_accel;
		case ECM_CLASSIFIER_MARK_RESULT_NOT_RELEVANT:
			DEBUG_WARN("%px: External callback decided not relevant.\n", aci);
			ecm_db_connection_deref(ci);
			goto not_relevant;
		case ECM_CLASSIFIER_MARK_RESULT_SUCCESS:
			DEBUG_WARN("%px: External callback returned the mark value: %x\n", aci, mark);
			ecm_db_connection_mark_set(ci, mark);
			ecmi->mark = mark;
			break;
		default:
			DEBUG_ASSERT(false, "Unhandled result: %d\n", result);
	}

	/*
	 * Acceleration is permitted
	 */
	spin_lock_bh(&ecm_classifier_mark_lock);
	ecmi->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_YES;
	ecmi->process_response.process_actions = ECM_CLASSIFIER_PROCESS_ACTION_ACCEL_MODE;
	ecmi->process_response.accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_ACCEL;
	*process_response = ecmi->process_response;
	spin_unlock_bh(&ecm_classifier_mark_lock);
	ecm_db_connection_deref(ci);
	return;

	not_relevant:
	spin_lock_bh(&ecm_classifier_mark_lock);
	ecmi->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_NO;
	*process_response = ecmi->process_response;
	spin_unlock_bh(&ecm_classifier_mark_lock);
	return;

	deny_accel:
	spin_lock_bh(&ecm_classifier_mark_lock);
	ecmi->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_YES;
	ecmi->process_response.process_actions = ECM_CLASSIFIER_PROCESS_ACTION_ACCEL_MODE;
	ecmi->process_response.accel_mode = ECM_CLASSIFIER_ACCELERATION_MODE_NO;
	*process_response = ecmi->process_response;
	spin_unlock_bh(&ecm_classifier_mark_lock);
	ecm_db_connection_deref(ci);
}

/*
 * ecm_classifier_mark_type_get()
 *	Get type of classifier this is
 */
static ecm_classifier_type_t ecm_classifier_mark_type_get(struct ecm_classifier_instance *aci)
{
	struct ecm_classifier_mark_instance *ecmi;
	ecmi = (struct ecm_classifier_mark_instance *)aci;

	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);
	return ECM_CLASSIFIER_TYPE_MARK;
}

/*
 * ecm_classifier_mark_reclassify_allowed()
 *	Get whether reclassification is allowed
 */
static bool ecm_classifier_mark_reclassify_allowed(struct ecm_classifier_instance *aci)
{
	struct ecm_classifier_mark_instance *ecmi;
	ecmi = (struct ecm_classifier_mark_instance *)aci;

	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);
	return true;
}

/*
 * ecm_classifier_mark_reclassify()
 *	Reclassify
 */
static void ecm_classifier_mark_reclassify(struct ecm_classifier_instance *aci)
{
	struct ecm_classifier_mark_instance *ecmi;
	ecmi = (struct ecm_classifier_mark_instance *)aci;
	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);

	/*
	 * Revert back to MAYBE relevant - we will evaluate when we get the next process() call.
	 */
	spin_lock_bh(&ecm_classifier_mark_lock);
	ecmi->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_MAYBE;
	spin_unlock_bh(&ecm_classifier_mark_lock);
}

/*
 * ecm_classifier_mark_last_process_response_get()
 *	Get result code returned by the last process call
 */
static void ecm_classifier_mark_last_process_response_get(struct ecm_classifier_instance *aci,
							struct ecm_classifier_process_response *process_response)
{
	struct ecm_classifier_mark_instance *ecmi;
	ecmi = (struct ecm_classifier_mark_instance *)aci;
	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);

	spin_lock_bh(&ecm_classifier_mark_lock);
	*process_response = ecmi->process_response;
	spin_unlock_bh(&ecm_classifier_mark_lock);
}

/*
 * ecm_classifier_mark_sync_to_v4()
 *	Front end is pushing accel engine state to us
 */
static void ecm_classifier_mark_sync_to_v4(struct ecm_classifier_instance *aci, struct ecm_classifier_rule_sync *sync)
{
	int protocol;
	int src_port;
	int dst_port;
	ip_addr_t src_ip;
	ip_addr_t dst_ip;
	__be32 src_ip4;
	__be32 dest_ip4;
	struct ecm_db_connection_instance *ci;
	ecm_classifier_mark_sync_to_ipv4_callback_t cb = NULL;
	ecm_classifier_mark_type_t type = ECM_CLASSIFIER_MARK_TYPE_L2_ENCAP;
			/* TODO: By default we use this type. In the future the type will be gotten from the connection instance */
	struct ecm_classifier_mark_instance *ecmi = (struct ecm_classifier_mark_instance *)aci;
	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);

	/*
	 * Nothing to update.
	 * We only care about flows that are actively being accelerated.
	 */
	if (!(sync->tx_packet_count[ECM_CONN_DIR_FLOW] || sync->tx_packet_count[ECM_CONN_DIR_RETURN])) {
		return;
	}

	/*
	 * Handle only the stats sync. We don't care about the evict or flush syncs.
	 */
	if (sync->reason != ECM_FRONT_END_IPV4_RULE_SYNC_REASON_STATS) {
		return;
	}

	ci = ecm_db_connection_serial_find_and_ref(ecmi->ci_serial);
	if (!ci) {
		DEBUG_TRACE("%px: No ci found for %u\n", ecmi, ecmi->ci_serial);
		return;
	}

	protocol = ecm_db_connection_protocol_get(ci);
	src_port = htons(ecm_db_connection_port_get(ci, ECM_DB_OBJ_DIR_FROM));
	dst_port = htons(ecm_db_connection_port_get(ci, ECM_DB_OBJ_DIR_TO));
	ecm_db_connection_address_get(ci, ECM_DB_OBJ_DIR_FROM, src_ip);
	ecm_db_connection_address_get(ci, ECM_DB_OBJ_DIR_TO, dst_ip);

	ecm_db_connection_deref(ci);

	ECM_IP_ADDR_TO_NIN4_ADDR(src_ip4, src_ip);
	ECM_IP_ADDR_TO_NIN4_ADDR(dest_ip4, dst_ip);

	rcu_read_lock();
	cb = rcu_dereference(sync_to_ipv4_cb[type]);
	if (!cb) {
		rcu_read_unlock();
		DEBUG_WARN("%px: IPv4 mark sync callback is not set\n", ecmi);
		return;
	}

	cb(ecmi->mark, src_ip4, src_port, dest_ip4, dst_port, protocol);
	rcu_read_unlock();
}

/*
 * ecm_classifier_mark_sync_from_v4()
 *	Front end is retrieving accel engine state from us
 */
static void ecm_classifier_mark_sync_from_v4(struct ecm_classifier_instance *aci, struct ecm_classifier_rule_create *ecrc)
{
	struct ecm_classifier_mark_instance *ecmi __attribute__((unused));

	ecmi = (struct ecm_classifier_mark_instance *)aci;
	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);
}

/*
 * ecm_classifier_mark_sync_to_v6()
 *	Front end is pushing accel engine state to us
 */
static void ecm_classifier_mark_sync_to_v6(struct ecm_classifier_instance *aci, struct ecm_classifier_rule_sync *sync)
{
	int protocol;
	int src_port;
	int dst_port;
	ip_addr_t src_ip;
	ip_addr_t dst_ip;
	struct in6_addr src_ip6;
	struct in6_addr dest_ip6;
	struct ecm_db_connection_instance *ci;
	ecm_classifier_mark_sync_to_ipv6_callback_t cb = NULL;
	ecm_classifier_mark_type_t type = ECM_CLASSIFIER_MARK_TYPE_L2_ENCAP;
			/* TODO: By default we use this type. In the future the type will be gotten from the connection instance */
	struct ecm_classifier_mark_instance *ecmi = (struct ecm_classifier_mark_instance *)aci;
	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);

	/*
	 * Nothing to update.
	 * We only care about flows that are actively being accelerated.
	 */
	if (!(sync->tx_packet_count[ECM_CONN_DIR_FLOW] || sync->tx_packet_count[ECM_CONN_DIR_RETURN])) {
		return;
	}

	/*
	 * Handle only the stats sync. We don't care about the evict or flush syncs.
	 */
	if (sync->reason != ECM_FRONT_END_IPV4_RULE_SYNC_REASON_STATS) {
		return;
	}

	ci = ecm_db_connection_serial_find_and_ref(ecmi->ci_serial);
	if (!ci) {
		DEBUG_TRACE("%px: No ci found for %u\n", ecmi, ecmi->ci_serial);
		return;
	}

	protocol = ecm_db_connection_protocol_get(ci);
	src_port = htons(ecm_db_connection_port_get(ci, ECM_DB_OBJ_DIR_FROM));
	dst_port = htons(ecm_db_connection_port_get(ci, ECM_DB_OBJ_DIR_TO));
	ecm_db_connection_address_get(ci, ECM_DB_OBJ_DIR_FROM, src_ip);
	ecm_db_connection_address_get(ci, ECM_DB_OBJ_DIR_TO, dst_ip);

	ecm_db_connection_deref(ci);

	ECM_IP_ADDR_TO_NIN6_ADDR(src_ip6, src_ip);
	ECM_IP_ADDR_TO_NIN6_ADDR(dest_ip6, dst_ip);

	rcu_read_lock();
	cb = rcu_dereference(sync_to_ipv6_cb[type]);
	if (!cb) {
		rcu_read_unlock();
		DEBUG_WARN("%px: IPv6 mark sync callback is not set\n", ecmi);
		return;
	}
	cb(ecmi->mark, &src_ip6, src_port, &dest_ip6, dst_port, protocol);
	rcu_read_unlock();
}

/*
 * ecm_classifier_mark_sync_from_v6()
 *	Front end is retrieving accel engine state from us
 */
static void ecm_classifier_mark_sync_from_v6(struct ecm_classifier_instance *aci, struct ecm_classifier_rule_create *ecrc)
{
	struct ecm_classifier_mark_instance *ecmi __attribute__((unused));

	ecmi = (struct ecm_classifier_mark_instance *)aci;
	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);
}

#ifdef ECM_STATE_OUTPUT_ENABLE
/*
 * ecm_classifier_mark_state_get()
 *	Gets the state of this classfier and outputs it to debugfs.
 */
static int ecm_classifier_mark_state_get(struct ecm_classifier_instance *ci, struct ecm_state_file_instance *sfi)
{
	int result;
	struct ecm_classifier_mark_instance *ecmi;
	struct ecm_classifier_process_response process_response;
	uint32_t mark;

	ecmi = (struct ecm_classifier_mark_instance *)ci;
	DEBUG_CHECK_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC, "%px: magic failed", ecmi);

	if ((result = ecm_state_prefix_add(sfi, "mark"))) {
		return result;
	}

	spin_lock_bh(&ecm_classifier_mark_lock);
	mark = ecmi->mark;
	process_response = ecmi->process_response;
	spin_unlock_bh(&ecm_classifier_mark_lock);

	if ((result = ecm_state_write(sfi, "value", "%d", mark))) {
		return result;
	}

	/*
	 * Output our last process response
	 */
	if ((result = ecm_classifier_process_response_state_get(sfi, &process_response))) {
		return result;
	}

	return ecm_state_prefix_remove(sfi);
}
#endif

/*
 * ecm_classifier_mark_instance_alloc()
 *	Allocate an instance of the mark classifier
 */
struct ecm_classifier_mark_instance *ecm_classifier_mark_instance_alloc(struct ecm_db_connection_instance *ci)
{
	struct ecm_classifier_mark_instance *ecmi;

	/*
	 * Allocate the instance
	 */
	ecmi = (struct ecm_classifier_mark_instance *)kzalloc(sizeof(struct ecm_classifier_mark_instance), GFP_ATOMIC | __GFP_NOWARN);
	if (!ecmi) {
		DEBUG_WARN("i%px: Failed to allocate Mark Classifier instance\n", ci);
		return NULL;
	}

	DEBUG_SET_MAGIC(ecmi, ECM_CLASSIFIER_MARK_INSTANCE_MAGIC);
	ecmi->refs = 1;

	/*
	 * Methods generic to all classifiers.
	 */
	ecmi->base.process = ecm_classifier_mark_process;
	ecmi->base.sync_from_v4 = ecm_classifier_mark_sync_from_v4;
	ecmi->base.sync_to_v4 = ecm_classifier_mark_sync_to_v4;
	ecmi->base.sync_from_v6 = ecm_classifier_mark_sync_from_v6;
	ecmi->base.sync_to_v6 = ecm_classifier_mark_sync_to_v6;
	ecmi->base.type_get = ecm_classifier_mark_type_get;
	ecmi->base.reclassify_allowed = ecm_classifier_mark_reclassify_allowed;
	ecmi->base.reclassify = ecm_classifier_mark_reclassify;
	ecmi->base.last_process_response_get = ecm_classifier_mark_last_process_response_get;
#ifdef ECM_STATE_OUTPUT_ENABLE
	ecmi->base.state_get = ecm_classifier_mark_state_get;
#endif
	ecmi->base.ref = ecm_classifier_mark_ref;
	ecmi->base.deref = ecm_classifier_mark_deref;
	ecmi->ci_serial = ecm_db_connection_serial_get(ci);

	ecmi->process_response.process_actions = 0;
	ecmi->process_response.relevance = ECM_CLASSIFIER_RELEVANCE_MAYBE;

	/*
	 * Final check if we are pending termination
	 */
	spin_lock_bh(&ecm_classifier_mark_lock);
	if (ecm_classifier_mark_terminate_pending) {
		spin_unlock_bh(&ecm_classifier_mark_lock);
		DEBUG_WARN("%px: Terminating\n", ci);
		kfree(ecmi);
		return NULL;
	}

	/*
	 * Link the new instance into our list at the head
	 */
	ecmi->next = ecm_classifier_mark_instances;
	if (ecm_classifier_mark_instances) {
		ecm_classifier_mark_instances->prev = ecmi;
	}
	ecm_classifier_mark_instances = ecmi;

	/*
	 * Increment stats
	 */
	ecm_classifier_mark_count++;
	DEBUG_ASSERT(ecm_classifier_mark_count > 0, "%px: ecm_classifier_mark_count wrap for instance: %px\n", ci, ecmi);
	spin_unlock_bh(&ecm_classifier_mark_lock);

	DEBUG_INFO("%px: Mark classifier instance alloc: %px\n", ci, ecmi);
	return ecmi;
}
EXPORT_SYMBOL(ecm_classifier_mark_instance_alloc);

/*
 * ecm_classifier_mark_register_callbacks()
 *	Register external callbacks.
 */
int ecm_classifier_mark_register_callbacks(ecm_classifier_mark_type_t type,
					   ecm_classifier_mark_get_callback_t mark_get,
					   ecm_classifier_mark_sync_to_ipv4_callback_t sync_to_ipv4,
					   ecm_classifier_mark_sync_to_ipv6_callback_t sync_to_ipv6)
{
	spin_lock_bh(&ecm_classifier_mark_lock);
	if (ecm_classifier_mark_terminate_pending) {
		spin_unlock_bh(&ecm_classifier_mark_lock);
		DEBUG_WARN("Terminating\n");
		return -1;
	}
	spin_unlock_bh(&ecm_classifier_mark_lock);

	DEBUG_ASSERT(!mark_get_cb[type] && !sync_to_ipv4_cb[type] && !sync_to_ipv6_cb[type],
			"Mark callbacks are already registered\n");
	rcu_assign_pointer(mark_get_cb[type], mark_get);
	rcu_assign_pointer(sync_to_ipv4_cb[type], sync_to_ipv4);
	rcu_assign_pointer(sync_to_ipv6_cb[type], sync_to_ipv6);

	return 0;
}
EXPORT_SYMBOL(ecm_classifier_mark_register_callbacks);

/*
 * ecm_classifier_mark_unregister_callbacks()
 *	Unregister external callbacks.
 */
void ecm_classifier_mark_unregister_callbacks(ecm_classifier_mark_type_t type)
{
	rcu_assign_pointer(mark_get_cb[type], NULL);
	rcu_assign_pointer(sync_to_ipv4_cb[type], NULL);
	rcu_assign_pointer(sync_to_ipv6_cb[type], NULL);

}
EXPORT_SYMBOL(ecm_classifier_mark_unregister_callbacks);

/*
 * ecm_classifier_mark_init()
 */
/*
 * DEBUG
 * 	a) registrar callbacks por defecto si no hay externos
 * 	b) AÑADIR CONFIGURACIÓN VÍA DEBUGFS
 *
 */
int ecm_classifier_mark_init(struct dentry *dentry)
{
	DEBUG_INFO("Mark classifier Module init\n");

	ecm_classifier_mark_dentry = debugfs_create_dir("ecm_classifier_mark", dentry);
	if (!ecm_classifier_mark_dentry) {
		DEBUG_ERROR("Failed to create ecm mark directory in debugfs\n");
		return -1;
	}

	/* 1. ENABLED Control principal */
	if (!ecm_debugfs_create_u32("enabled", S_IRUGO | S_IWUSR, ecm_classifier_mark_dentry,
					(u32 *)&ecm_classifier_mark_enabled)) {
		DEBUG_ERROR("Failed to create mark enabled file in debugfs\n");
		debugfs_remove_recursive(ecm_classifier_mark_dentry);
		return -1;
	}

	/* 2.	Valor de marca que deniega aceleración */
	if (!ecm_debugfs_create_u32("deny_mark", S_IRUGO | S_IWUSR, ecm_classifier_mark_dentry,
		(u32 *)&ecm_classifier_mark_deny_value)) {
		DEBUG_ERROR("Failed to create deny_mark file in debugfs\n");
	debugfs_remove_recursive(ecm_classifier_mark_dentry);
	return -1;
	}

	/* 3.	 Modo de operación */
	if (!ecm_debugfs_create_u32("mode", S_IRUGO | S_IWUSR, ecm_classifier_mark_dentry,
		(u32 *)&ecm_classifier_mark_mode)) {
		DEBUG_ERROR("Failed to create mode file in debugfs\n");
	debugfs_remove_recursive(ecm_classifier_mark_dentry);
	return -1;
	}

	/*
	 * DEBUG
	 *
	 * De:	ecm_classifier_mark_result_t ecm_classifier_mark_default_get
	 *
	 * Registrar callbacks por defecto si no hay externos
	 * Esto hace que el clasificador funcione aunque no haya módulo externo
	 */
	rcu_assign_pointer(mark_get_cb[ECM_CLASSIFIER_MARK_TYPE_L2_ENCAP],
					   ecm_classifier_mark_default_get);
	rcu_assign_pointer(sync_to_ipv4_cb[ECM_CLASSIFIER_MARK_TYPE_L2_ENCAP],
					   ecm_classifier_mark_default_sync_ipv4);
	rcu_assign_pointer(sync_to_ipv6_cb[ECM_CLASSIFIER_MARK_TYPE_L2_ENCAP],
					   ecm_classifier_mark_default_sync_ipv6);

	DEBUG_INFO("MARK: Default callbacks registered\n");

	return 0;
}
EXPORT_SYMBOL(ecm_classifier_mark_init);

/*
 * ecm_classifier_mark_exit()
 */

void ecm_classifier_mark_exit(void)
{
	DEBUG_INFO("Mark classifier Module exit\n");

	spin_lock_bh(&ecm_classifier_mark_lock);
	ecm_classifier_mark_terminate_pending = true;
	spin_unlock_bh(&ecm_classifier_mark_lock);

	/*
	 * Remove the debugfs files recursively.
	 */
	if (ecm_classifier_mark_dentry) {
		debugfs_remove_recursive(ecm_classifier_mark_dentry);
	}

}
EXPORT_SYMBOL(ecm_classifier_mark_exit);
