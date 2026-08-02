# MANUAL COMPLETO DE SSDK SHELL (ssdk_sh)

| | |
|---------|---------|
| **OBJETO** | Inferir en el Hardware Offload para arquitectura QualcommAX `ipq807x` con los mínimos cambios posibles en kernel/firmware |
| **TESTADO EN** | Xiaomi AX3600, custom firmware (2026-07-16, fork y feeds de @AgustinLorenzo), NSS v`NSS.HK.11.4.0.5-6-R` |
| **MOTIVO PRINCIPAL** | La herramienta parece creada por Qualcomm, no existiendo ningún tipo de documentación; con una implementación a medias, rota, y muy poco intuitiva |
| **DESAFIO** | Documentar en detalle |
| **FECHA** | 2026, Agosto, 02 |
| **DESVIACIÓN DEL OBJETIVO PRINCIPAL** | `qca-ssdk-shell`, v.2024.12.17 está rotísimo e incompleto |
| **SOURCES EDITADAS** | `./src/shell/shell_config.c`, `Makefile`. Todos los cambios en el repo, y documentado aquí |

---

## ÍNDICE DE COMANDOS

1. Port Control (Gestión de Puertos)
2. VLAN
3. Port VLAN
4. FDB (Forwarding Database)
5. ACL (Access Control List)
6. QoS (Quality of Service)
7. IGMP/MLD
8. Leaky
9. Mirror
10. Rate (Rate Limiting)
11. Security
12. STP (Spanning Tree Protocol)
13. MIB (Management Information Base)
14. LED Control
15. CoS Map
16. Miscellaneous
17. IP (Layer 3)
18. Flow
19. NAT/NAPT
20. Trunk
21. Interface Control
22. VSI
23. Policer
24. Shaper
25. QM (Queue Management)
26. BM (Buffer Management)
27. Service Code
28. RSS Hash
29. Control Packet
30. VXLAN
31. GENEVE
32. Tunnel Program
33. Tunnel
34. MAPT
35. VPort
36. Atheros Tag
37. Debug
38. Device
39. ACL

---

## 1. PORT CONTROL

Gestión de puertos físicos del switch.

### Comandos:

```bash
# Configurar duplex
ssdk_sh port duplex set <port_id> <half|full>

# Configurar velocidad
ssdk_sh port speed set <port_id> <10|100|1000>

# Auto-negociación
ssdk_sh port autoNeg enable <port_id>
ssdk_sh port autoNeg restart <port_id>
ssdk_sh port autoNegenable set <port_id>
ssdk_sh port autoNegrestart set <port_id>

# Configurar advertisement de auto-negociación
ssdk_sh port autoAdv set <port_id> <cap_bitmap>

# Header Atheros
ssdk_sh port header set <port_id> <enable|disable>
ssdk_sh port txhdr set <port_id> <noheader|onlymanagement|allframe>
ssdk_sh port rxhdr set <port_id> <noheader|onlymanagement|allframe>
ssdk_sh port hdrtype set <enable|disable> <type_hex>

# Flow Control
ssdk_sh port flowCtrl set <port_id> <enable|disable>
ssdk_sh port flowCtrlforcemode set <port_id> <enable|disable>

# Power Management
ssdk_sh port powersave set <port_id> <enable|disable>
ssdk_sh port hibernate set <port_id> <enable|disable>
ssdk_sh port poweroff set <port_id>
ssdk_sh port poweron set <port_id>

# Cable Diagnostic Test
ssdk_sh port cdt run <port_id> <mdi_pair>

# MAC Status
ssdk_sh port txmacstatus set <port_id> <enable|disable>
ssdk_sh port rxmacstatus set <port_id> <enable|disable>
ssdk_sh port txfcstatus set <port_id> <enable|disable>
ssdk_sh port rxfcstatus set <port_id> <enable|disable>

# Back Pressure
ssdk_sh port bpstatus set <port_id> <enable|disable>

# Link Force Mode
ssdk_sh port linkforcemode set <port_id> <enable|disable>

# Loopback
ssdk_sh port macLoopback set <port_id> <enable|disable>
ssdk_sh port localLoopback set <port_id> <enable|disable>
ssdk_sh port remoteLoopback set <port_id> <enable|disable>

# Congestion Drop
ssdk_sh port congedrop set <port_id> <queue_id> <enable|disable>

# Ring Flow Control Threshold
ssdk_sh port ringfcthresh set <ring_id> <on_thres> <off_thres>
ssdk_sh port ringfcen set <ring_id> <enable|disable>

# 802.3az (Energy Efficient Ethernet)
ssdk_sh port Ieee8023az set <port_id> <enable|disable>

# Crossover/MDIX
ssdk_sh port crossover set <port_id> <auto|mdi|mdix>
ssdk_sh port crossover get <port_id>
ssdk_sh port crossover status <port_id>

# Combo Ports (Cobre/Fibra)
ssdk_sh port preferMedium set <port_id> <copper|fiber>
ssdk_sh port preferMedium get <port_id>
ssdk_sh port mediumType get <port_id>
ssdk_sh port fiberMode set <port_id> <100fx|1000bx>
ssdk_sh port fiberMode get <port_id>

# Reset
ssdk_sh port reset set <port_id>

# Magic Frame MAC para WOL
ssdk_sh port magicFrameMac set <port_id> <mac_address>
ssdk_sh port magicFrameMac get <port_id>
ssdk_sh port phyId get <port_id>
ssdk_sh port wolstatus set <port_id> <enable|disable>
ssdk_sh port wolstatus get <port_id>

# Interface Mode
ssdk_sh port interfaceMode set <port_id> <mode>
ssdk_sh port interfaceMode get <port_id>
ssdk_sh port interfaceMode status <port_id>

# Counters
ssdk_sh port counter set <port_id> <enable|disable>
ssdk_sh port counter get <port_id>
ssdk_sh port counter show <port_id>

# MTU/MRU
# MTU - 2 PARÁMETROS
ssdk_sh port mtu set <port_id> <mtu_size>
# MRU - 2 PARÁMETROS
ssdk_sh port mru set <port_id> <mru_size>

# Ring Union
ssdk_sh port ringunion set <enable|disable>
ssdk_sh port ringunion get

# Source Filter
ssdk_sh port srcfilter set <port_id> <enable|disable>
ssdk_sh port srcfiltercfg set <port_id> <src_filter_enable> <srcfilter_mode>

# 802.3ah
ssdk_sh port port8023ah set <port_id>

# MTU Config (APPE) - 5 PARÁMETROS
ssdk_sh port mtucfg set <port_id> <mtu_enable> <mtu_type> <extra_header_len> <eg_vlan_tag_flag>

# MRU/MTU Combined - 3 PARÁMETROS
ssdk_sh port mrumtu set <port_id> <mru_size> <mtu_size>
ssdk_sh port mrumtu get <port_id>

# Counter Config
ssdk_sh port Cntcfg set <port_id> <rx_cnt_en> <uc_tx_cnt_en> <mc_tx_cnt_en>

# Promiscuous Mode
ssdk_sh port promiscmode set <port_id> <enable|disable>

# EEE Configuration
ssdk_sh port eeecfg set <port_id> <eee_enable> <eee_capability> <lpi_sleep_timer> <advertisement>

# Switch Port Loopback
ssdk_sh port switchportloopback set <port_id> <loopback_enable> <crc_stripped_enable> <loopback_rate>
```

### Tipos de Modos:

- `half|full`: Half duplex o Full duplex
- `10|100|1000`: Velocidad en Mbps
- `noheader|onlymanagement|allframe`: Modos de header
- `auto|mdi|mdix`: Modos de crossover
- `copper|fiber`: Medio físico
- `100fx|1000bx`: Modos de fibra

---

## 2. VLAN

Gestión de VLANs.

### Comandos:

```bash
# Crear VLAN
ssdk_sh vlan entry set <vlan_id>
ssdk_sh vlan entry create <vlan_id>

# Eliminar VLAN
ssdk_sh vlan entry del <vlan_id>

# Actualizar miembros de VLAN
ssdk_sh vlan entry update <vlan_id> <member_bitmap> <0>

# Buscar VLAN
ssdk_sh vlan entry find <vlan_id>

# Siguiente VLAN
ssdk_sh vlan entry next <vlan_id>

# Añadir VLAN (append)
ssdk_sh vlan entry append

# Flush todas las VLANs
ssdk_sh vlan entry flush

# Configurar FID de VLAN
ssdk_sh vlan fid set <vlan_id> <fid>

# Añadir miembro a VLAN
ssdk_sh vlan member set <vlan_id> <port_id> <unmodified|untagged|tagged>
ssdk_sh vlan member add <vlan_id> <port_id> <unmodified|untagged|tagged>

# Eliminar miembro de VLAN
ssdk_sh vlan member del <vlan_id> <port_id>

# Estado de aprendizaje
ssdk_sh vlan learnsts set <vlan_id> <enable|disable>


# lan_wan_cfg set - 4 PARÁMETROS
ssdk_sh vlan lan_wan_cfg set <lan_ports> <lan_vids> <wan_ports> <wan_vids>
# Ejemplo: Configurar puertos 1,2,3 como LAN y 4 como WAN
ssdk_sh vlan lan_wan_cfg set 1,2,3 1,1,1 4 2
```

---

## 3. PORT VLAN

Configuración VLAN por puerto.

### Comandos:

```bash
# Modo Ingress VLAN
ssdk_sh portVlan ingress set <port_id> <disable|secure|check|fallback>

# Modo Egress VLAN
ssdk_sh portVlan egress set <port_id> <unmodified|untagged|tagged|hybrid|untouched>

# Miembros de VLAN por puerto
ssdk_sh portVlan member set <port_id> <port_bitmap>
ssdk_sh portVlan member add <port_id> <memport_id>
ssdk_sh portVlan member del <port_id> <memport_id>
ssdk_sh portVlan member update <port_id> <port_bitmap>

# VLAN ID por defecto
ssdk_sh portVlan defaultVid set <port_id> <vid>

# Forzar VID
ssdk_sh portVlan forceVid set <port_id> <enable|disable>

# Forzar modo port VLAN
ssdk_sh portVlan forceMode set <port_id> <enable|disable>

# Nest VLAN
ssdk_sh portVlan nestVlan set <port_id> <enable|disable>

# Service VLAN TPID
ssdk_sh portVlan sVlanTPID set <tpid>

# Modo Ingress VLAN
ssdk_sh portVlan invlan set <port_id> <admit_all|admit_tagged|admit_untagged>

# TLS Mode
ssdk_sh portVlan tlsMode set <port_id> <enable|disable>

# Propagación de Prioridad
ssdk_sh portVlan priPropagation set <port_id> <enable|disable>

# Default SVID/CVID
ssdk_sh portVlan defaultSVid set <port_id> <vlan_id>
ssdk_sh portVlan defaultCVid set <port_id> <vlan_id>

# Propagación VLAN
ssdk_sh portVlan vlanPropagation set <port_id> <disable|clone|replace>

# Traducción VLAN
ssdk_sh portVlan translation set <port_id>
ssdk_sh portVlan translation add <port_id>
ssdk_sh portVlan translation del <port_id>
ssdk_sh portVlan translation iterate <port_id> <iterator>

# QinQ
ssdk_sh portVlan qinqMode set <ctag|stag>
ssdk_sh portVlan qinqRole set <port_id> <edge|core>

# MAC VLAN XLT
ssdk_sh portVlan macvlanxlt set <port_id> <enable|disable>

# Network Isolate
ssdk_sh portVlan netiso set <enable|disable>

# Egress Bypass
ssdk_sh portVlan egbypass set <enable|disable>

# VRF ID
ssdk_sh portVlan ptvrfid set <port_id> <vrf_id>

# Filtro Ingress VLAN
ssdk_sh portVlan ingressFilter set <port_id> <membership_filter_en> <tagged_filter_en> <untagged_filter_en> <priority_tagged_filter_en>

# QinQ Global/Port
ssdk_sh portVlan globalQinQMode set <mask> <ingress_qinq_mode> <egress_qinq_mode>
ssdk_sh portVlan ptQinQMode set <mask> <ingress_qinq_role> <egress_qinq_role>

# TPID Configuración
ssdk_sh portVlan inTpid set <mask> <ctagtpid> <stagtpid>
ssdk_sh portVlan egTpid set <mask> <ctagtpid> <stagtpid>

# Default VLAN Tag
ssdk_sh portVlan defaultVlanTag set <port_id> <mask> <default_ctag_vid> <default_stag_vid> <default_ctag_pri> <default_stag_pri> <default_ctag_dei> <default_stag_dei>

# Tag Propagation
ssdk_sh portVlan tagPropagation set <port_id> <mask> <vid_propagation_en> <pri_propagation_en> <dei_propagation_en>

# Translation Miss Action
ssdk_sh portVlan translationMissAction set <port_id> <cmd>

# VSI Egress Mode
ssdk_sh portVlan vsiEgMode set <vsi> <port_id> <egmode>
ssdk_sh portVlan vsiEgModeEn set <port_id> <enable|disable>

# VLAN Counter
ssdk_sh portVlan counter set <port_id> <enable|disable>
ssdk_sh portVlan counter get <port_id>

# Advanced Translation
ssdk_sh portVlan translationAdv set <port_id>

# Isolación
ssdk_sh portVlan Isol set <port_id> <isol_en> <group_id>
ssdk_sh portVlan IsolGroup set <isol_group_id> <isol_group_bmp>
```

### Modos:

- `disable|secure|check|fallback`: Modos Ingress
- `unmodified|untagged|tagged|hybrid|untouched`: Modos Egress
- `admit_all|admit_tagged|admit_untagged`: Modos Ingress VLAN
- `ctag|stag`: Modos QinQ
- `edge|core`: Roles QinQ
- `disable|clone|replace`: Propagación VLAN

---

## 4. FDB (FORWARDING DATABASE)

Tabla de reenvío (Forwarding Database).
```bash
# fdb entry add - 14 PARÁMETROS
ssdk_sh fdb entry add <addr> <fid> <dacmd> <sacmd> <dest_port> <static> <leaky> <mirror> <entry_ver> <queue_override> <cross_pt_state> <white_list_en> <load_balance_en> <type>

# Parámetros:
#   addr: MAC en formato xx-xx-xx-xx-xx-xx
#   fid: VLAN FID (0-4095 o 65535)
#   dacmd: forward|drop|cpycpu|rdtcpu (acción para destino)
#   sacmd: forward|drop|cpycpu|rdtcpu (acción para origen)
#   dest_port: puerto destino
#   static: yes|no
#   leaky: yes|no
#   mirror: yes|no
#   entry_ver: 0/1
#   queue_override: yes|no
#   cross_pt_state: yes|no
#   white_list_en: yes|no
#   load_balance_en: yes|no
#   type: 0=software, 1=hardware

# Ejemplo: Añadir entrada FDB estática para MAC 00:11:22:33:44:55 en puerto 3
ssdk_sh fdb entry add 00-11-22-33-44-55 1 forward forward 3 yes no no 1 no no no no 1
```


```bash
# Ver entradas
ssdk_sh fdb entry show
```


### Comandos:

```bash
# Añadir entrada FDB
ssdk_sh fdb entry set <addr> <fid> <dacmd> <sacmd> <dest_port> <static> <leaky> <mirror> <entry_ver> <queue_override> <cross_pt_state> <white_list_en> <load_balance_en> <type>
ssdk_sh fdb entry add <addr> <fid> <dacmd> <sacmd> <dest_port> <static> <leaky> <mirror> <entry_ver> <queue_override> <cross_pt_state> <white_list_en> <load_balance_en> <type>

# Eliminar entrada FDB
ssdk_sh fdb entry del <addr> <fid> <port>

# Flush FDB
ssdk_sh fdb entry flush <0:dynamic only|1:dynamic and static>

# Buscar entrada FDB
ssdk_sh fdb entry find <addr> <fid> <dacmd> <sacmd> <dest_port> <static> <leaky> <mirror> <entry_ver> <queue_override> <cross_pt_state> <white_list_en> <load_balance_en> <type>

# Iterar FDB
ssdk_sh fdb entry iterate <iterator>

# Extender modo
ssdk_sh fdb entry extendnext <addr> <fid> <dacmd> <sacmd> <dest_port> <static> <leaky> <mirror> <entry_ver> <queue_override> <cross_pt_state> <white_list_en> <load_balance_en> <type>
ssdk_sh fdb entry extendfirst <addr> <fid> <dacmd> <sacmd> <dest_port> <static> <leaky> <mirror> <entry_ver> <queue_override> <cross_pt_state> <white_list_en> <load_balance_en> <type>

# Transferir puerto
ssdk_sh fdb entry transfer <old_port_id> <new_port_id> <fid>

# Flush por puerto
ssdk_sh fdb portEntry flush <port_id> <0:dynamic only|1:dynamic and static>

# Primera/Siguiente entrada
ssdk_sh fdb firstEntry find
ssdk_sh fdb nextEntry find

# Aprendizaje por puerto
ssdk_sh fdb portLearn set <port_id> <enable|disable>

# Control de edad
ssdk_sh fdb ageCtrl set <enable|disable>

# Modo VLAN
ssdk_sh fdb vlansmode set <ivl|svl>

# Tiempo de edad
ssdk_sh fdb ageTime set <time:s>

# Límite de aprendizaje por puerto
ssdk_sh fdb ptlearnlimit set <port_id> <enable|disable> <limitcounter>
ssdk_sh fdb ptlearnexceedcmd set <port_id> <forward|drop|cpycpu|rdtcpu>

# Límite global de aprendizaje
ssdk_sh fdb learnlimit set <enable|disable> <limitcounter>
ssdk_sh fdb learnexceedcmd set <forward|drop|cpycpu|rdtcpu>

# Entrada reservada
ssdk_sh fdb resventry set
ssdk_sh fdb resventry add
ssdk_sh fdb resventry del
ssdk_sh fdb resventry find
ssdk_sh fdb resventry iterate <iterator>

# Aprendizaje estático
ssdk_sh fdb ptLearnstatic set <port_id> <enable|disable>

# Añadir/Eliminar puerto a FDB
ssdk_sh fdb port add <fid> <macaddr> <port_id>
ssdk_sh fdb port del <fid> <macaddr> <port_id>

# Control de aprendizaje
ssdk_sh fdb LearnCtrl set <vsi> <lrn_en> <action>
ssdk_sh fdb PtLearnCtrl set <port_id> <en> <action>
ssdk_sh fdb PtStationMove set <port_id> <en> <action>
ssdk_sh fdb PtMacLimitCtrl set <port_id> <enable> <limit_num> <action>
```

### Valores:

- `forward|drop|cpycpu|rdtcpu`: Acciones de reenvío
- `ivl|svl`: Modos VLAN (Independent VLAN Learning / Shared VLAN Learning)
- `enable|disable`: Estados

---

## 5. ACL

Listas de Control de Acceso.

### Comandos:

```bash
# Crear lista ACL
ssdk_sh acl list create <list_id> <priority>

# Destruir lista ACL
ssdk_sh acl list destroy <list_id>

# Bind/Unbind lista ACL
ssdk_sh acl list bind <list_id> <direction> <objtype> <objindex>
ssdk_sh acl list unbind <list_id> <direction> <objtype> <objindex>

# Añadir reglas ACL
ssdk_sh acl rule add <list_id> <rule_id> <rule_nr>

# Eliminar reglas ACL
ssdk_sh acl rule del <list_id> <rule_id> <rule_nr>

# Consultar regla ACL
ssdk_sh acl rule query <list_id> <rule_id>

# Activar/Desactivar reglas
ssdk_sh acl rule active <list_id> <rule_id> <rule_nr>
ssdk_sh acl rule deactive <list_id> <rule_id> <rule_nr>

# Source filter status
ssdk_sh acl srcfiltersts set <rule_id> <enable|disable>

# Estado ACL
ssdk_sh acl status set <enable|disable>

# UDF Profile
ssdk_sh acl udfprofile set <port_id> <l2/l2snap/l3/l3plus/l4> <offset> <length>

# UDF Set
ssdk_sh acl udf set <pkt_type> <udf_idx> <udf_type> <offset>

# UDF Profile Entry
ssdk_sh acl udfprofileEntry set <profile_id>

# UDF Profile Config
ssdk_sh acl udfprofilecfg set <profile_id>

# VP Group
ssdk_sh acl vpgroup set <vport_id> <vport_type> <vpgroup_id>
```

### Tipos:

- `l2|l2snap|l3|l3plus|l4`: Tipos UDF
- `mac|ip4|ip6|udf`: Tipos de reglas

---

## 6. QoS

Calidad de Servicio.

### Comandos:

```bash
# Modo de scheduling global
ssdk_sh qos schMode set <sp|wrr|mix|mix_plus> <q0,q1,q3,q4>

# Queue TX Buffer Status
ssdk_sh qos qTxBufSts set <port_id> <enable|disable>

# Queue TX Buffer Number
ssdk_sh qos qTxBufNr set <port_id> <queueid:0-3> <number>

# Port TX Buffer Status
ssdk_sh qos ptTxBufSts set <port_id> <enable|disable>

# Port RED Enable
ssdk_sh qos ptRedEn set <port_id> <enable|disable>

# Port TX Buffer Number
ssdk_sh qos ptTxBufNr set <port_id> <number>

# Port RX Buffer Number
ssdk_sh qos ptRxBufNr set <port_id> <number>

# User Priority to Queue Mapping
ssdk_sh qos up2q set <up:0-7> <queueid:0-3>

# DSCP to Queue Mapping
ssdk_sh qos dscp2q set <dscp:0-63> <queueid:0-3>

# QoS Mode por puerto
ssdk_sh qos ptMode set <port_id> <da|up|dscp|port|flow> <enable|disable>
ssdk_sh qos ptModePri set <port_id> <da|up|dscp|port|flow> <priority:0-3>

# Default User Priority
ssdk_sh qos ptDefaultUp set <port_id> <up:0-7>

# Port Scheduling Mode
ssdk_sh qos ptschMode set <port_id> <sp|wrr|mix|mixplus> <q0,q1,q2,q3,q4,q5>

# Default Stag/Ctag Priority
ssdk_sh qos ptDefaultSpri set <port_id> <spri:0-7>
ssdk_sh qos ptDefaultCpri set <port_id> <cpri:0-7>

# Force Stag/Ctag Priority
ssdk_sh qos ptFSpriSts set <port_id> <enable|disable>
ssdk_sh qos ptFCpriSts set <port_id> <enable|disable>

# Queue Remark
ssdk_sh qos ptQuRemark set <port_id> <queue_id> <table_id> <enable|disable>

# Port Group
ssdk_sh qos ptgroup set <port_id> <pcp_group> <dscp_group> <flow_group>

# Port Priority Precedence
ssdk_sh qos ptpriprece set <port_id>

# Port Remark
ssdk_sh qos ptremark set <port_id>

# PCP Map
ssdk_sh qos pcpmap set <group_id> <pcp> <internal_pcp> <internal_dei> <internal_pri> <internal_dscp> <internal_dp> <dscp_mask> <dscp_en> <pcp_en> <dei_en> <pri_en> <dp_en> <qos_prec>

# DSCP Map
ssdk_sh qos dscpmap set <group_id> <dscp> <internal_pcp> <internal_dei> <internal_pri> <internal_dscp> <internal_dp> <dscp_mask> <dscp_en> <pcp_en> <dei_en> <pri_en> <dp_en> <qos_prec>

# Flow Map
ssdk_sh qos flowmap set <group_id> <flow> <internal_pcp> <internal_dei> <internal_pri> <internal_dscp> <internal_dp> <dscp_mask> <dscp_en> <pcp_en> <dei_en> <pri_en> <dp_en> <qos_prec>

# Queue Scheduler
ssdk_sh qos qscheduler set <node_id> <level> <port_id> <sp_id> <e_pri> <c_pri> <e_drr_id> <c_drr_id> <e_drr_wt> <c_drr_wt> <c_drr_ut> <e_drr_ut> <drr_frame_mode>

# Ring Queue Map
ssdk_sh qos ringqueue set <ring_id> <bmp>

# Dequeue Control
ssdk_sh qos dequeue set <queue_id> <enable|disable>

# Port Scheduler Reset
ssdk_sh qos portscheduler set <port_id>
```

### Modos:

- `sp|wrr|mix|mix_plus`: Modos de scheduling
- `da|up|dscp|port|flow`: Modos QoS
- `0-7`: Prioridades

---

## 7. IGMP/MLD

Gestión de multicast.

### Comandos:

```bash
# Modo IGMP/MLD por puerto
ssdk_sh igmp mode set <port_id> <enable|disable>

# Forwarding command
ssdk_sh igmp cmd set <forward|drop|cpycpu|rdtcpu>

# Hardware joining/leaving
ssdk_sh igmp portJoin set <port_id> <enable|disable>
ssdk_sh igmp portLeave set <port_id> <enable|disable>

# Router ports
ssdk_sh igmp rp set <port_bit_map>

# Crear entrada
ssdk_sh igmp createStatus set <enable|disable>

# Static/Leaky/V3
ssdk_sh igmp static set <enable|disable>
ssdk_sh igmp leaky set <enable|disable>
ssdk_sh igmp version3 set <enable|disable>

# Queue
ssdk_sh igmp queue set <enable|disable> <queue_id>

# Límite de aprendizaje
ssdk_sh igmp ptlearnlimit set <port_id> <enable|disable> <limitcounter>
ssdk_sh igmp ptlearnexceedcmd set <port_id> <forward|drop|cpycpu|rdtcpu>

# Entrada Multicast
ssdk_sh igmp multi set <group_type> <group_ip> <source_type> <source_ip> <portmap> <vlanid>
ssdk_sh igmp multi clear <group_type> <group_ip> <source_type> <source_ip> <portmap> <vlanid>
ssdk_sh igmp multi show
```

---

## 8. LEAKY

Control de paquetes leaky.

### Comandos:

```bash
# Modo Unicast leaky
ssdk_sh leaky ucMode set <port|fdb>

# Modo Multicast leaky
ssdk_sh leaky mcMode set <port|fdb>

# Modo ARP leaky
ssdk_sh leaky arpMode set <port_id> <enable|disable>

# Por puerto
ssdk_sh leaky ptUcMode set <port_id> <enable|disable>
ssdk_sh leaky ptMcMode set <port_id> <enable|disable>
```

---

## 9. MIRROR

Mirroring de tráfico.

- ✅ Compilado (`IN_IP=TRUE` en config)
- ✅ Parcialmente funcional en AX3600
- ❌ NO hay comandos para mirroring de VLAN
- ❌ NO hay comandos para mirroring de DSA
- ❌ NO hay comandos para mirroring de puertos virtuales
- ❌ NO hay comandos para mirroring de tráfico offload

### Contexto con QCA-SSDK

* Desde la perspectiva de HPPE:

    - ** Solo hay registros para mirroring de puertos físicos (port_mirror_u), no para VLAN, DSA o puertos virtuales.**
    
    


### Comandos de Mirroring

| Comando | Sintaxis | Descripción |
| :--- | :--- | :--- |
| **analyPtset** | `<port_id>` | Configurar puerto de análisis (mirror destination) |
| **analyPtget** | `-` | Obtener puerto de análisis actual |
| **ptIngressset** | `<port_id> <enable|disable>` | Activar/desactivar mirror de ingreso en puerto |
| **ptIngressget** | `<port_id>` | Ver estado de mirror de ingreso |
| **ptEgressset** | `<port_id> <enable|disable>` | Activar/desactivar mirror de egreso en puerto |
| **ptEgressget** | `<port_id>` | Ver estado de mirror de egreso |
| **analyCfgset** | `<both|ingress|egress>` | Configurar dirección de análisis |
| **analyCfgget** | `-` | Ver configuración de análisis |




```bash
# Puerto de análisis
ssdk_sh mirror analyPt set <port_id>

# Mirror de ingreso/egreso por puerto
ssdk_sh mirror ptIngress set <port_id> <enable|disable>
ssdk_sh mirror ptEgress set <port_id> <enable|disable>

# Configuración de análisis
ssdk_sh mirror analyCfg set <port_id> <priority>


```


    
```bash
mirror_analyzer_u {
    in_analyzer_port:5,
    eg_analyzer_port:5
}

port_mirror_u {
    in_mirr_en:1,
    eg_mirr_en:1
}

hppe_mirror_analyzer_get(dev_id, &value)
hppe_mirror_analyzer_set(dev_id, &value)
hppe_port_mirror_get(dev_id, index, &value)
hppe_port_mirror_set(dev_id, index, &value)
```
---

## 10. RATE

Limitación de velocidad.

### Comandos:

```bash
# Queue Egress Rate
ssdk_sh rate qEgress set <port_id> <queueid:0-3> <speed:(kbps)> <enable|disable>

# Port Egress/Ingress Rate
ssdk_sh rate ptEgress set <port_id> <speed:(kbps)> <enable|disable>
ssdk_sh rate ptIngress set <port_id> <speed:(kbps)> <enable|disable>

# Storm Control
ssdk_sh rate stormCtrl set <port_id> <unicast|multicast|broadcast> <enable|disable>
ssdk_sh rate stormCtrlRate set <port_id> <rate:(packets/s)>

# Port Policer
ssdk_sh rate portpolicer set <port_id>

# Port/Queue Shaper
ssdk_sh rate portshaper set <port_id> <enable|disable>
ssdk_sh rate queueshaper set <port_id> <queue_id> <enable|disable>

# ACL Policer
ssdk_sh rate aclpolicer set <policer_id>

# Add Rate Byte
ssdk_sh rate ptAddRateByte set <port_id> <number:0-255>

# Global Flow Control
ssdk_sh rate ptgolflowen set <port_id> <enable|disable>
```

---

## 11. SECURITY

Seguridad de capas 2/3/4.

### Comandos:

```bash
# MAC Security
ssdk_sh sec mac set <resv_vid|invalid_src_addr> <value>

# IP Security
ssdk_sh sec ip set <invalid_ver|same_addr|ttl_change_status|ttl_val> <value>

# IPv4 Security
ssdk_sh sec ip4 set <invalid_hl|hdr_opts|invalid_df|frag_offset_min_len|frag_offset_min_size|frag_offset_max_len|invalid_frag_offset|invalid_sip|invalid_dip|invalid_chksum|invalid_pl|df_clear_status|ipid_random_status> <value>

# IPv6 Security
ssdk_sh sec ip6 set <invalid_dip|invalid_sip|invalid_pl> <value>

# TCP Security
ssdk_sh sec tcp set <blat|invalid_hl|min_hdr_size|invalid_syn|su_block|sp_block|sap_block|xmas_scan|null_scan|sr_block|sf_block|sar_block|rst_scan|rst_with_data|fa_block|pa_block|ua_block|invalid_chksum|invalid_urgptr|invalid_opts> <value>

# UDP Security
ssdk_sh sec udp set <blat|invalid_len|invalid_chksum> <value>

# ICMP Security
ssdk_sh sec icmp4 set <ping_pl_exceed|ping_frag|ping_max_pl> <value>
ssdk_sh sec icmp6 set <ping_pl_exceed|ping_frag|ping_max_pl> <value>

# L3/L4 Parser
ssdk_sh sec l3parser set <small_ip4ttl> <small_ip6hoplimit>
ssdk_sh sec l4parser set <tcp_flags0..7> <tcp_flags_mask0..7>

# Exception Control
ssdk_sh sec expctrl set <excep_cmd> <deacclr_en> <l3route_only_en> <l2fwd_only_en> <l2flow_en> <l3flow_en> <multicast_en>

# Tunnel Exception Control
ssdk_sh sec tunnelexpctrl set <excep_cmd> <deacclr_en> <profile0..7_en>
ssdk_sh sec tunnelflagsparser set <entry_valid> <equal> <tunnel_header_type> <flags> <mask>
```

---

## 12. STP

Spanning Tree Protocol.

### Comandos:

```bash
# Estado STP por puerto
ssdk_sh stp portState set <st_id> <port_id> <disable|block|listen|learn|forward>
```

### Estados:

- `disable`: Puerto deshabilitado
- `block`: Bloqueo
- `listen`: Escucha
- `learn`: Aprendizaje
- `forward`: Reenvío

---

## 13. MIB

Estadísticas de gestión.

### Comandos:

```bash
# Estado MIB
ssdk_sh mib status set <enable|disable>

# Flush counters por puerto
ssdk_sh mib counters flush <port_id>

# CPU Keep
ssdk_sh mib cpuKeep set <enable|disable>
```

---

## 14. LED

Control de LEDs.

### Comandos:

```bash
# Patrón de control LED
ssdk_sh led ctrlpattern set <group_id> <led_id> <active_level> <pattern_mode> <blink_freq> <map>
```

### Parámetros:

- `active_level`: high|low
- `pattern_mode`: always_off|always_blink|always_on|map
- `blink_freq`: 2HZ|4HZ|8HZ|16HZ|32HZ|64HZ|128HZ|256HZ|TXRX

---

## 15. COS MAP

Mapeo de Clase de Servicio.

### Comandos:

```bash
# DSCP to Priority/DP
ssdk_sh cosmap dscp2pri set <dscp> <priority>
ssdk_sh cosmap dscp2dp set <dscp> <dp>

# Dot1p to Priority/DP
ssdk_sh cosmap up2pri set <up> <priority>
ssdk_sh cosmap up2dp set <up> <dp>

# DSCP/UP to Egress H Priority/DP
ssdk_sh cosmap dscp2ehpri set <dscp> <priority>
ssdk_sh cosmap dscp2ehdp set <dscp> <dp>
ssdk_sh cosmap up2ehpri set <up> <priority>
ssdk_sh cosmap up2ehdp set <up> <dp>

# Priority to Queue
ssdk_sh cosmap pri2q set <priority> <queueid>
ssdk_sh cosmap pri2ehq set <priority> <queueid>

# Egress Remark
ssdk_sh cosmap egRemark set <tableid>
```

---

## 16. MISCELLANEOUS

Comandos varios.

### Comandos:

```bash
# ARP
ssdk_sh misc arp set <enable|disable>
ssdk_sh misc arpcmd set <forward|drop|cpycpu|rdtcpu>

# Frame Max Size
ssdk_sh misc frameMaxSize set <size:byte>

# Unknown Source Address
ssdk_sh misc ptUnkSaCmd set <port_id> <forward|drop|cpycpu|rdtcpu>

# Flooding Control
ssdk_sh misc ptUnkUcFilter set <port_id> <enable|disable>
ssdk_sh misc ptUnkMcFilter set <port_id> <enable|disable>
ssdk_sh misc ptBcFilter set <port_id> <enable|disable>

# CPU Port
ssdk_sh misc cpuPort set <enable|disable>

# Broadcast to CPU
ssdk_sh misc bctoCpu set <enable|disable>

# PPPoE
ssdk_sh misc PppoeCmd set <forward|drop|cpycpu|rdtcpu>
ssdk_sh misc Pppoe set <enable|disable>
ssdk_sh misc rtdPppoe set <enable|disable>

# DHCP
ssdk_sh misc ptDhcp set <port_id> <enable|disable>

# EAPOL
ssdk_sh misc eapolcmd set <forward|drop|cpycpu|rdtcpu>
ssdk_sh misc eapolstatus set <port_id> <enable|disable>

# RIP
ssdk_sh misc rip set <enable|disable>

# ARP Request/Reply
ssdk_sh misc ptarpreq set <port_id> <enable|disable>
ssdk_sh misc ptarpack set <port_id> <enable|disable>

# PPPoE Session
ssdk_sh misc pppoesession add <session_id> <enable|disable>
ssdk_sh misc pppoesession del <session_id>
ssdk_sh misc extendpppoe set
ssdk_sh misc extendpppoe add
ssdk_sh misc extendpppoe del
ssdk_sh misc pppoeid set <index> <id>

# Interrupts
ssdk_sh misc intrmask set <intr_mask>
ssdk_sh misc intrstatus clear <intr_mask>
ssdk_sh misc intrportlinkmask set <port_id> <intr_mask>
ssdk_sh misc intrmaskmaclinkchg set <port_id> <enable|disable>
ssdk_sh misc intrstatusmaclinkchg clear

# CPU VID
ssdk_sh misc cpuVid set <enable|disable>

# Global MAC Address
ssdk_sh misc glomacaddr set <macaddr>

# LLDP
ssdk_sh misc lldp set <enable|disable>

# Frame CRC
ssdk_sh misc framecrc set <enable|disable>
```

---

## 17. IP (LAYER 3)

Comandos de capa 3.

### 17.1 IP Global Control



- ✅ Compilado (`IN_IP=TRUE` en config)
- ✅ Parcialmente funcional en AX3600


### Contexto con QCA-SSDK

#### Registro L3_ROUTE_CTRL
**Dirección:** L3_ROUTE_CTRL_BASE

| Campo | Bits | Valores | Descripción | Shell Command |
| :--- | :---: | :--- | :--- | :--- |
| ipv4_uc_route_en | 0 | 0=off, 1=on | Habilitar ruteo unicast IPv4 | `ip globalctrl set ipv4_uc_route_en` |
| ipv6_uc_route_en | 1 | 0=off, 1=on | Habilitar ruteo unicast IPv6 | `ip globalctrl set ipv6_uc_route_en` |
| ipv4_mc_route_en | 2 | 0=off, 1=on | Habilitar ruteo multicast IPv4 | `ip globalctrl set ipv4_mc_route_en` |
| ipv6_mc_route_en | 3 | 0=off, 1=on | Habilitar ruteo multicast IPv6 | `ip globalctrl set ipv6_mc_route_en` |
| ttl_dec_bypass | 4 | 0=off, 1=on | Bypass de decremento TTL | `ip globalctrl set ttl_dec_bypass` |
| ipv4_l3_fwd_bypass | 5 | 0=off, 1=on | Bypass de forwarding IPv4 | `ip globalctrl set ipv4_l3_fwd_bypass` |
| ipv6_l3_fwd_bypass | 6 | 0=off, 1=on | Bypass de forwarding IPv6 | `ip globalctrl set ipv6_l3_fwd_bypass` |
| ipv4_de_acce | 7 | 0=off, 1=on | Desactivar aceleración IPv4 | `ip globalctrl set ipv4_de_acce` |
| ipv6_de_acce | 8 | 0=off, 1=on | Desactivar aceleración IPv6 | `ip globalctrl set ipv6_de_acce` |
| fragment_bypass | 9 | 0=off, 1=on | Bypass de fragmentación | `ip globalctrl set fragment_bypass` |
| icmp_bypass | 10 | 0=off, 1=on | Bypass de ICMP | `ip globalctrl set icmp_bypass` |
| redirect_cpu | 11 | 0=off, 1=on | Forzar redirección a CPU | NO EXPUESTO |
| l3_filter_en | 12 | 0=off, 1=on | Habilitar filtrado L3 | NO EXPUESTO |


*  Desde la perspectiva de HPPE:
    - En `qca-ssdk`, `hppe_ip.c` muestra que hay múltiples formas de desactivar offload:
        ```text
        ttl_dec_bypass - Bypass del decremento de TTL
        ipv4_uc_route_en - Desactivar ruteo unicast IPv4
        ipv6_uc_route_en - Desactivar ruteo unicast IPv6
        *_de_acce - Desactivar aceleración para varios casos
        ```
    
    - **`ssdk_sh ip intf set` con `ttl_dec_bypass_en=no` y `ipv4_uc_route_en=no` deberían forzar el paso por CPU.
    
    - `redirect_cpu`. NO expuesto pero permite redirigir TODO el tráfico L3 a CPU
    - `ipv4_de_acce` e `ipv6_de_acce`, ¿Desactivan aceleración?
    - `reirect_cpu=1` + `ipv4_de_acce=1` para desactivación completa ??

    

* Sintáxis reaL:
```bash

```
```text
# 12 parámetros:

mru_fail_action → ip_mru_check_fail
mru_deacclr_en → ip_mru_check_fail_de_acce
mtu_fail_action → ip_mtu_fail
mtu_deacclr_en → ip_mtu_fail_de_acce
mtu_nonfrag_fail_action → ip_mtu_df_fail
mtu_nonfrag_deacclr_en → ip_mtu_df_fail_de_acce
prefix_bc_action → ip_prefix_bc_cmd
prefix_bc_deacclr_en → ip_prefix_bc_de_acce
icmp_rdt_action → icmp_rdt_cmd
icmp_rdt_deacclr_en → icmp_rdt_de_acce
hash_mode_0 → host_hash_mode_0
hash_mode_1 → host_hash_mode_1
```




| Comando | Sintaxis / Acción | Estado | Notas |
| :--- | :--- | :--- | :--- |
| **globalctrlset** | `<12 parámetros>` | ✅ Funcional | CRÍTICO, controla bypass |
| **globalctrlget** | `-` | ✅ Funcional | Ver configuración actual |
| **routestatusset** | `<enable|disable>` | ✅ Funcional | Activar/desactivar ruteo |
| **hostentryadd** | `-` | ❓ SINTAXIS VACÍA | Hay que investigar |

### Valores posibles

| Parámetro | Valores |
| :--- | :--- |
| **mru_fail_action** | `forward \| drop \| rdtcpu \| admit_all` |
| **mtu_fail_action** | `forward \| drop \| rdtcpu \| admit_all` |
| **icmp_rdt_action** | `forward \| drop \| rdtcpu \| admit_all` |
| **prefix_bc_action** | `forward \| drop \| rdtcpu \| admit_all` |
| ***_deacclr_en** | `yes \| no` |
| **hash_mode_\*** | `0-3` |


```bash
# Ver estado global de IP
ssdk_sh ip globalctrl get

ip globalctrl set <port> ipv4_uc_route_en <0|1>
ip globalctrl set <port> ipv6_uc_route_en <0|1>

ip globalctrl set <port> ttl_dec_bypass <0|1>

ip globalctrl set <port> ipv4_de_acce <0|1> 
ip globalctrl set <port> ipv6_de_acce <0|1> 

# ip intf set
ip intf set <port> <vlan> <IP> <mask>
    → Configura interfaz L3 con VLAN específica

# Setear parámetros
ssdk_sh ip globalctrl set <params>
```
```bash
# Ejemplo probado:

ssdk_sh ip globalctrl set rdtcpu no rdtcpu no rdtcpu no rdtcpu no rdtcpu yes yes no
# Resultado: icmp_rdt_action pasó de 0x0 a 0x3 (rdtcpu)
```

### 17.2 IP HOSTENTRY (FDB)

~~PENDIENTE -> Recabar la sintáxis exacta de los argumentos de funciones en su código~~
~~Añadir entrada FDB (intento fallido)~~
~~ssdk_sh ip hostentry add <params>~~

```bash
# ip hostentry add - 22 PARÁMETROS
ssdk_sh ip hostentry add <entryid> <flags> <status> <ip4_addr> <ip6_addr> <mac> <intf_id> <lb_num> <vrf_id> <port_id> <action> <mirror> <counter> <counter_id> <dst_info> <syn_toggle> <lan_wan> <mcast_vsi> <mcast_sip4_addr> <mcast_sip6_addr> <pppoe_en> <pppoe_id>

# Parámetros:
#   entryid: ID de la entrada
#   flags: FAL_IP_IP4_ADDR (1), FAL_IP_IP6_ADDR (2), FAL_IP_IP4_ADDR_MCAST (4), FAL_IP_IP6_ADDR_MCAST (8)
#   status: 0=invalid, 1=valid
#   ip4_addr: dirección IPv4 (ej: 192.168.1.100)
#   ip6_addr: dirección IPv6 (ej: :: para IPv4)
#   mac: MAC en formato xx-xx-xx-xx-xx-xx
#   intf_id: ID de interfaz L3
#   lb_num: load balance number
#   vrf_id: VRF ID
#   port_id: puerto
#   action: forward|drop|cpycpu|rdtcpu
#   mirror: yes|no
#   counter: yes|no
#   counter_id: ID de contador
#   dst_info: información de destino
#   syn_toggle: 0/1
#   lan_wan: 0=LAN, 1=WAN
#   mcast_vsi: VSI multicast
#   mcast_sip4_addr: dirección IPv4 multicast
#   mcast_sip6_addr: dirección IPv6 multicast
#   pppoe_en: yes|no
#   pppoe_id: ID PPPoE

# Ejemplo: Añadir host IPv4
ssdk_sh ip hostentry add 1 1 1 192.168.1.100 :: 00-11-22-33-44-55 0 0 0 3 forward no no 0 0 0 0 0 0.0.0.0 :: no 0
```

```bash
# Ver entradas
ssdk_sh ip hostentry show
```

```bash
# Host Entry
ssdk_sh ip hostentry set <entryid> <entryflags> <entrystatus> <ip4/ip6_addr> <mac_addr> <interface_id> <load_balance_num> <vrf_id> <port_id> <action> <mirror> <counter> <counter_id> <dst_info> <sync_toggle> <lan_wan>
ssdk_sh ip hostentry add <entryid> <entryflags> <entrystatus> <ip4/ip6_addr> <mac_addr> <interface_id> <load_balance_num> <vrf_id> <port_id> <action> <mirror> <counter> <counter_id> <dst_info> <sync_toggle> <lan_wan>
ssdk_sh ip hostentry del <del_mode>
ssdk_sh ip hostentry next <next_mode>
ssdk_sh ip hostentry bindcnt <host entry id> <cnt id> <enable|disable>
ssdk_sh ip hostentry bindpppoe <host entry id> <pppoe id> <enable|disable>
```


```bash
# ARP Learn
ssdk_sh ip ptarplearn set <port_id> <flag>
ssdk_sh ip arplearn set <learnlocal|learnall>

# IP/ARP Source Guard
ssdk_sh ip ptipsrcguard set <port_id> <mac_ip|mac_ip_port|mac_ip_vlan|mac_ip_port_vlan|no_guard>
ssdk_sh ip ptarpsrcguard set <port_id> <mac_ip|mac_ip_port|mac_ip_vlan|mac_ip_port_vlan|no_guard>
```

```bash
# Route
ssdk_sh ip routestatus set <enable|disable>
```


```bash
# Interface MAC

# ip entry set
ssdk_sh ip intfentry set <entryid> <vrf_id> <vid_low> <vid_high> <mac_addr> <ip4_route> <ip6_route>

# ip intfentry add - 7 PARÁMETROS
ssdk_sh ip intfentry add <entryid> <vrf_id> <vid_low> <vid_high> <mac_addr> <ip4_route> <ip6_route>
ssdk_sh ip intfentry del

# Unknown Source
ssdk_sh ip ipunksrc set <forward|drop|cpycpu|rdtcpu>
ssdk_sh ip arpunksrc set <forward|drop|cpycpu|rdtcpu>

# Age Time
ssdk_sh ip ipagetime set <time>

# WCMP
ssdk_sh ip wcmphashmode set <hashmode>
ssdk_sh ip wcmpentry set <wcmp_id>

# VRF
ssdk_sh ip vrfbaseaddr set <ip4_addr>
ssdk_sh ip vrfbasemask set <ip4_addr>

# ip default route
# ip defaultroute set - 5 PARÁMETROS
ssdk_sh ip defaultroute set <valid> <vrf_id> <ip_version> <route_type> <index>

# Host Route
# ip hostroute set - 5 PARÁMETROS
ssdk_sh ip hostroute set <valid> <vrf_id> <ip_version> <ip_addr> <prefix_length>

# RFS
ssdk_sh ip rfsip4 set <mac_addr> <ip4_addr> <vid> <loadbalance>
ssdk_sh ip rfsip6 set <mac_addr> <ip6_addr> <vid> <loadbalance>

# Default Flow Command
ssdk_sh ip defaultflowcmd set <vrf_id> <lan2lan|wan2lan|lan2wan|wan2wan> <forward|drop|rdtcpu|admit_all>
ssdk_sh ip defaultrtflowcmd set <vrf_id> <lan2lan|wan2lan|lan2wan|wan2wan> <forward|drop|rdtcpu|admit_all>

# Network Route
ssdk_sh ip networkroute set <type> <ip_addr> <ip_mask> <action> <dst_info> <lan_wan>
ssdk_sh ip networkroute add <type> <ip_addr> <ip_mask> <action> <dst_info> <lan_wan>

# VSI ARP SG
ssdk_sh ip vsiarpsg set

# Interface
ssdk_sh ip intf set <if_index> <mru> <mtu> <ttl_dec_bypass_en> <ipv4_uc_route_en> <ipv6_uc_route_en> <icmp_trigger_en> <ttl_exceed_action> <ttl_exceed_deacclr_en> <mac_addr_bitmap> <mac_addr>

# VSI Interface
ssdk_sh ip vsiintf set <l3_if_valid> <l3_if_index>

# Port Interface
ssdk_sh ip portintf set

# Nexthop
ssdk_sh ip nexthop set <type> <vsi/port> <if_index> <ip_to_me_en> <pub_ip_index> <stag_fmt> <svid> <ctag_fmt> <cvid> <mac_addr> <dnat_ip>

# Source Guard
ssdk_sh ip portsg set
ssdk_sh ip vsisg set

# Public IP
ssdk_sh ip pubip set <pub_ip_addr>

# Port MAC
ssdk_sh ip portmac set <valid> <mac_addr>

# Route Miss
ssdk_sh ip routemiss set

# Port ARP SG
ssdk_sh ip portarpsg set

# MC Mode
ssdk_sh ip mcmode set <ipv4_mc_en> <ipv4_igmpv3_mode> <ipv6_mc_en> <ipv6_mldv2_mode>
```

```bash
# Ver estado global de IP
ssdk_sh ip globalctrl get

# ip globalctrl set - 12 PARÁMETROS OBLIGATORIOS
ssdk_sh ip globalctrl set <mru_fail_action> <mru_deacclr_en> <mtu_fail_action> <mtu_deacclr_en> <mtu_nonfrag_fail_action> <mtu_nonfrag_deacclr_en> <prefix_bc_action> <prefix_bc_deacclr_en> <icmp_rdt_action> <icmp_rdt_deacclr_en> <hash_mode_0> <hash_mode_1>

# Valores posibles:
#   *_action: forward|drop|cpycpu|rdtcpu
#   *_deacclr_en: yes|no
#   hash_mode_*: 0-3

# Ejemplo: Desactivar aceleración IPv4 y forzar ICMP a CPU
ssdk_sh ip globalctrl set forward no forward no forward no forward no rdtcpu no 0 1

# ip intf set - 11 PARÁMETROS OBLIGATORIOS
ssdk_sh ip intf set <if_index> <mru> <mtu> <ttl_dec_bypass_en> <ipv4_uc_route_en> <ipv6_uc_route_en> <icmp_trigger_en> <ttl_exceed_action> <ttl_exceed_deacclr_en> <mac_addr_bitmap> <mac_addr>

# Ejemplo: Desactivar ruteo offload en lan3 (if_index=3)
ssdk_sh ip intf set 3 1500 1500 no no no no 3 yes 0x4 80-2d-1a-29-e4-03
```

---

## 18. FLOW (Gestión de flujos)

- ✅ Compilado (`IN_FLOW=TRUE` en config)
- ✅ Funcional en AX3600

### Contexto con QCA-SSDK

* Desde la perspectiva de HPPE:
    - En `qca-ssdk`, `hppe_flow.c` muestra que hay 5 grupos de control (flow_ctl0 a flow_ctl4), y dentro de cada uno hay campos para:
        ```text
        miss_action (0=drop, 1=forward, 3=CPU)
        bypass (1=bypass del procesamiento)
        frag_bypass (bypass de fragmentos)
        tcp_special (bypass de TCP especial)
        key_sel (selección de clave de hash)
        ```
    - El comando `ssdk_sh flow mgmt set` con `miss_action=3` debería ser la forma correcta de enviar tráfico a CPU.
    
    
### Comandos Críticos para Offload

| Comando | Sintaxis | Estado | Notas |
| :--- | :--- | :--- | :--- |
| **mgmtset** | `<type> <dir>` | ⚠️ SINTAXIS INCOMPLETA | Ver nota abajo |
| **mgmtget** | `<type> <dir>` | ✅ Funcional | - |
| **statusset** | `<status>` | ✅ Funcional | Activar/desactivar flow |
| **entryadd** | `<add_mode>` | ❓ SINTAXIS VACÍA | Hay que investigar |

### ⚠️ ¡IMPORTANTE! Sintaxis real de `flow mgmt set`

`flow mgmt set` acepta MÁS parámetros de los que muestra la ayuda:


```bash
# flow mgmt set - 7 PARÁMETROS (SINTAXIS REAL)
ssdk_sh flow mgmt set <type> <dir> <miss_action> <frag_bypass> <tcpspec_bypass> <all_bypass> <key_sel>

# Parámetros:
#   type: 0 (puerto físico), 1, 2
#   dir: 0=WAN, 1=LAN1, 2=LAN2, 3=LAN3, 4=CPU/NSS
#   miss_action: 0=drop, 1=forward, 3=CPU
#   frag_bypass: 0=off, 1=on
#   tcpspec_bypass: 0=off, 1=on
#   all_bypass: 0=off, 1=on (desactiva offload global)
#   key_sel: 0, 1, 2

# Ejemplo: Forzar tráfico de LAN3 a CPU
ssdk_sh flow mgmt set 0 3 3 0 0 1 0

# Ejemplo: Desactivar offload en LAN1
ssdk_sh flow mgmt set 0 1 1 0 0 1 0

# flow entry add - 43 PARÁMETROS (COMPLETO)
ssdk_sh flow entry add <add_mode> <entry_id> <entry_type> <host_addr_type> <host_addr_index> <protocol> <age> <src_intf_valid> <src_intf_index> <fwd_type> <snat_nexthop> <snat_srcport> <dnat_nexthop> <dnat_dstport> <route_nexthop> <port_valid> <route_port> <bridge_port> <deacclr_en> <copy_tocpu_en> <syn_toggle> <pri_profile> <service_code> <ip_type> <src_port> <dst_port> <flow_ip4> <flow_ip6> <tree_id> <flow_cookie_ext> <wifi_qos_en> <wifi_qos> <qos_type> <pmtu_check_l3> <pmtu> <vpn_id> <vlan_fmt_valid> <svlan_fmt> <cvlan_fmt> <bridge_nexthop_valid> <bridge_nexthop> <policer_valid> <policer_index>

```

### Flow Management - Type 0 (Puertos físicos)

| Puerto | frag_bypass | tcp_spec_bypass | all_bypass | key_sel | miss_action |
| :--- | :---: | :---: | :---: | :---: | :---: |
| WAN (0) | 0 | 0 | 0 | 0 | 0 |
| LAN1 (1) | 0 | 0 | 0 | 0 | /10 |
| LAN2 (2) | 0 | 0 | 0 | 1 | 0 |
| LAN3 (3) | 0 | 0 | 0 | 0 | /10 |
| CPU (4) | - | - | - | - | - |

### Flow Management - Type 1

| Puerto | key_sel | miss_action |
| :--- | :---: | :---: |
| WAN (0) | 0 | 0 |
| LAN1 (1) | 0 | 0 |
| LAN2 (2) | 1 | 0 |
| LAN3 (3) | 0 | 0 |

### Flow Management - Type 2

| Puerto | key_sel | miss_action |
| :--- | :---: | :---: |
| WAN (0) | 0 | 3 |
| LAN1 (1) | 0 | 3 |
| LAN2 (2) | 1 | 3 |
| LAN3 (3) | 0 | 3 |

### Valores de miss_action
- 0 = Drop
- 3 = Forward to CPU



### Comandos:
```bash
# Sintaxis
ssdk_sh flow mgmt get <type> <dir>
ssdk_sh flow mgmt set <type> <dir> <frag_bypass> <tcp_spec_bypass> <all_bypass> <miss_action>

# Parámetros:
# type: 0, 1, 2 (0=puerto físico, 1=VLAN/bridge?, 2=algo más)
# dir: 0-4 (0=WAN, 1=LAN1, 2=LAN2, 3=LAN3, 4=CPU/NSS)
# frag_bypass: no/no/yes
# tcp_spec_bypass: no/no/yes
# all_bypass: no/no/yes
# miss_action: 0 (drop), 1, 2, 3 (forward to CPU)

# Ejemplos probados:
ssdk_sh flow mgmt get 0 0      # WAN?
ssdk_sh flow mgmt get 0 1      # LAN1
ssdk_sh flow mgmt get 0 2      # LAN2
ssdk_sh flow mgmt get 0 3      # LAN3
ssdk_sh flow mgmt get 0 4      # CPU/NSS?

ssdk_sh flow mgmt get 1 0      # Type 1, WAN?
ssdk_sh flow mgmt get 1 1      # Type 1, LAN1
ssdk_sh flow mgmt get 1 2      # Type 1, LAN2
ssdk_sh flow mgmt get 1 3      # Type 1, LAN3
ssdk_sh flow mgmt get 1 4      # Type 1, CPU?

ssdk_sh flow mgmt get 2 0      # Type 2, WAN?
ssdk_sh flow mgmt get 2 1      # Type 2, LAN1
ssdk_sh flow mgmt get 2 2      # Type 2, LAN2
ssdk_sh flow mgmt get 2 3      # Type 2, LAN3
ssdk_sh flow mgmt get 2 4      # Type 2, CPU?

# Setear valores
ssdk_sh flow mgmt set 0 3 forward no no no 0   # all_bypass=0, miss_action=0
ssdk_sh flow mgmt set 0 3 forward no no no 3   # miss_action=3 (CPU)

# Resultados:
# miss_action=0 → Drop
# miss_action=3 → Forward to CPU
# all_bypass=0 → No bypass global
# key_sel=0 → Sin selección especial
# key_sel=1 → Algo específico (cambió con el comando)
# key_sel=2 → Otro modo aún no documentado
```


```bash
# Estado
ssdk_sh flow status set <enable|disable>

# Age Timer
ssdk_sh flow agetime set <age_time> <unit>

# Management
ssdk_sh flow mgmt set <type> <dir> <miss_action> <frag_bypass_en> <tcpspec_bypass_en> <all_bypass_en> <key_sel>

# Entrada FLOW
ssdk_sh flow entry add <add_mode>
ssdk_sh flow entry set <add_mode>
ssdk_sh flow entry del <del_mode>

# Host
ssdk_sh flow host set <add_mode>
ssdk_sh flow host add <add_mode>
ssdk_sh flow host del <del_mode>

# Global Config
ssdk_sh flow global set
```


---

## 19. NAT/NAPT

Traducción de direcciones de red.

### Comandos:

```bash
# natentry add - 13 PARÁMETROS
ssdk_sh nat natentry add <entryid> <flags> <status> <select_idx> <vrf_id> <src_addr> <trans_addr> <port_num> <port_range> <action> <mirror> <counter> <counter_id>

# naptentry add - 18 PARÁMETROS
ssdk_sh nat naptentry add <entryid> <flags> <status> <vrf_id> <flow_cookie> <load_balance> <src_addr> <dst_addr> <trans_addr> <src_port> <dst_port> <trans_port> <action> <mirror> <counter> <counter_id> <priority> <priority_val>

# flowentry add - 16 PARÁMETROS (versión NAT)
ssdk_sh nat flowentry add <entryid> <flags> <status> <vrf_id> <flow_cookie> <load_balance> <src_addr> <dst_addr> <src_port> <dst_port> <action> <mirror> <counter> <counter_id> <priority> <priority_val>

# pubaddr add - 2 PARÁMETROS
ssdk_sh nat pubaddr add <entryid> <pub_addr>
```

```bash
ssdk_sh nat natentry del <del_mode>
ssdk_sh nat natentry next <next_mode>
ssdk_sh nat natentry bindcnt <entry id> <cnt id> <enable|disable>

# NAPT Entry
ssdk_sh nat naptentry set <entryid> <entryflags> <entrystatus> <vrf_id> <flow_cookie> <load_balance> <src_addr> <dst_addr> <trans_addr> <src_port> <dst_port> <trans_port> <action> <mirror> <counter> <counter_id> <priority> <priority_val>
ssdk_sh nat naptentry add <entryid> <entryflags> <entrystatus> <vrf_id> <flow_cookie> <load_balance> <src_addr> <dst_addr> <trans_addr> <src_port> <dst_port> <trans_port> <action> <mirror> <counter> <counter_id> <priority> <priority_val>
ssdk_sh nat naptentry del <del_mode>
ssdk_sh nat naptentry next <next_mode>
ssdk_sh nat naptentry bindcnt <entry id> <cnt id> <enable|disable>

# Status
ssdk_sh nat natstatus set <enable|disable>
ssdk_sh nat naptstatus set <enable|disable>

# Hash Mode
ssdk_sh nat nathash set <flag>

# NAPT Mode
ssdk_sh nat naptmode set <fullcone|strictcone|portstrict|synmatric>

# Private Base Address
ssdk_sh nat prvbaseaddr set <ip4_addr>
ssdk_sh nat prvbasemask set <ip4_mask>
ssdk_sh nat prvaddrmode set <enable|disable>

# Public Address
ssdk_sh nat pubaddr set
ssdk_sh nat pubaddr add
ssdk_sh nat pubaddr del <del_mode>

# Unknown Session
ssdk_sh nat natunksess set <forward|drop|cpycpu|rdtcpu>

# Global
ssdk_sh nat global set <enable|disable> <sync_counter>

# Flow Entry
ssdk_sh nat flowentry set
ssdk_sh nat flowentry add
ssdk_sh nat flowentry del <del_mode>
ssdk_sh nat flowentry next <next_mode>

# Flow Cookie/RFS
ssdk_sh nat flowcookie set
ssdk_sh nat flowrfs set <action>
```

---

## 20. TRUNK

Agregación de enlaces.

### Comandos:

```bash
# Grupo Trunk
ssdk_sh trunk group set <trunk_id> <disable|enable> <port_bitmap>

# Hash Mode
ssdk_sh trunk hashmode set <hash_mode>

# Manipulable SA
ssdk_sh trunk mansa set <macaddr>

# Failover
ssdk_sh trunk failover set <enable|disable>
```

---

## 21. INTERFACE CONTROL

Control de interfaces.

### Comandos:

```bash
# MAC Mode
ssdk_sh interface macmode set <port_id>

# Port 3AZ Status
ssdk_sh interface pt3azstatus set <port_id> <enable|disable>

# PHY Mode
ssdk_sh interface phymode set <phy_id>

# FX100 Control
ssdk_sh interface fx100ctrl set

# MAC0/MAC6 Exchange
ssdk_sh interface mac06exch set <enable|disable>
```

---

## 22. VSI

Interfaces de Servicio Virtual.

### Comandos:

```bash
# Alloc/Free VSI
ssdk_sh vsi vsi alloc
ssdk_sh vsi vsi free

# Port Based VSI
ssdk_sh vsi portbasedvsi set <port_id> <vsi>

# VLAN Based VSI
ssdk_sh vsi vlanbasedvsi set <port_id> <vlan_id> <vsi>

# Learn Control
ssdk_sh vsi learnctrl set <vsi> <learnstatus_en> <learnaction>

# Station Move
ssdk_sh vsi stationmove set <vsi> <stationmove_en> <stationmove_action>

# Member
ssdk_sh vsi member set <vsi> <membership> <unknown_unicast_membership> <unknown_multicast_membership> <broadcast_membership>

# Counter
ssdk_sh vsi counter cleanup <vsi>

# Bridge VSI
ssdk_sh vsi bridgevsi set <vsi> <bridge_vsi_en> <bridge_vsi_id>

# Invalid VSI Control
ssdk_sh vsi invalidvsi_ctrl set <dest_en> <dest_info_type> <dest_info_value>
```

---

## 23. POLICER

Policing de tráfico.

### Comandos:

```bash
# Timeslot
ssdk_sh policer timeslot set <100us|1ms|10ms|100ms>

# FCS Compensation
ssdk_sh policer fcscompensation set <enable|disable>

# Port Entry
ssdk_sh policer portentry set <port_id>

# ACL Entry
ssdk_sh policer aclentry set <policer_id>

# Bypass
ssdk_sh policer bypass set <enable|disable>

# Priority Remap
ssdk_sh policer priremap set

# Control
ssdk_sh policer ctrl set <head> <tail>
```

---

## 24. SHAPER

Modelado de tráfico.

### Comandos:

```bash
# Timeslots
ssdk_sh shaper porttimeslot set <timeslot>
ssdk_sh shaper flowtimeslot set <timeslot>
ssdk_sh shaper queuetimeslot set <timeslot>

# Token Numbers
ssdk_sh shaper porttoken set <port_id> <ctoken_negative_enable> <ctoken_number>
ssdk_sh shaper flowtoken set <flow_id> <ctoken_negative_enable> <ctoken_number> <etoken_negative_enable> <etoken_number>
ssdk_sh shaper queuetoken set <queue_id> <ctoken_negative_enable> <ctoken_number> <etoken_negative_enable> <etoken_number>

# Shaper Config
ssdk_sh shaper portshaper set <port_id> <meter_unit> <cshaper_enable> <cir> <cbs> <shaper_frame_mode>
ssdk_sh shaper flowshaper set <flow_id> <meter_type> <couple_enable> <meter_unit> <cshaper_enable> <cir> <cbs> <eshaper_enable> <eir> <ebs> <shaper_frame_mode>
ssdk_sh shaper queueshaper set <queue_id> <meter_type> <couple_enable> <meter_unit> <cshaper_enable> <cir> <cbs> <eshaper_enable> <eir> <ebs> <shaper_frame_mode>

# IPG Compensation
ssdk_sh shaper ipgcompensation set <ipg_pre_length>

# Shaper Control
ssdk_sh shaper queueshaperctrl set <head> <tail>
ssdk_sh shaper flowshaperctrl set <head> <tail>
```

---

## 25. QM (QUEUE MANAGEMENT)

Gestión de colas.


### Contexto con QCA-SSDK

#### Registro OQ_ENQ_OPR
-   **Dirección:** `QM_OQ_ENQ_OPR_BASE + (queue * 0x4)`

| Campo | Bits | Valores | Descripción | Shell Command |
| :--- | :---: | :--- | :--- | :--- |
| enq_disable | 0 | 0=off, 1=on | Deshabilitar encolado | `qm queue set enq_disable` |
| enq_drop | 1 | 0=off, 1=on | Descartar al encolar | `qm queue set enq_drop` |
| enq_bypass | 2 | 0=off, 1=on | Bypass de encolado | `qm queue set enq_bypass` |
| enq_cpu | 3 | 0=off, 1=on | Enviar a CPU en vez de encolar | `qm queue set enq_cpu` |
| drop_tail | 4 | 0=off, 1=on | Drop tail en cola | NO EXPUESTO |
| drop_random | 5 | 0=off, 1=on | Drop random temprano | NO EXPUESTO |
| red_en | 6 | 0=off, 1=on | Habilitar RED | NO EXPUESTO |


#### Registro OQ_DEQ_OPR
-   **Dirección:** `QM_OQ_DEQ_OPR_BASE + (queue * 0x4)`

| Campo | Bits | Valores | Descripción | Shell Command |
| :--- | :---: | :--- | :--- | :--- |
| deq_drop | 0 | 0=off, 1=on | Descartar al desencolar | `qm queue set deq_drop` |
| deq_cpu | 1 | 0=off, 1=on | Enviar a CPU al desencolar | `qm queue set deq_cpu` |
| deq_bypass | 2 | 0=off, 1=on | Bypass de desencolado | `qm queue set deq_bypass` |

* Desde la perspectiva de HPPE:

    - En `qca-ssdk`, el archivo `hppe_qm.c` muestra:
    
        ```text
        enq_disable - Deshabilitar encolado (forzar CPU)
        deq_drop - Descartar al desencolar
        Mapeo de colas por hash y prioridad
        ```
        
    - **`ssdk_sh qm queue set <queue_id> enable` podría forzar ciertas colas a CPU**
    
    - `enq_cpu` y `deq_cpu` **NO expuestos, redirigen tráfico de cola a CPU**
    - Las colas se mapean por hash, prioridad, y puerto
    - `enq_cpu=1` + `deq_cpu=1` para redirección total de colas







### Comandos:

#### SET
```bash
# qm queue set
qm queue set <queue> enq_disable <0|1>
qm queue set <queue> enq_drop <0|1>
qm queue set <queue> deq_drop <0|1>

qm queue set <queue> enq_cpu <0|1>      # NO expuesto
qm queue set <queue> deq_cpu <0|1>      # NO expuesto
qm queue set <queue> enq_bypass <0|1>   # NO expuesto
```


```bash
# Ucast Queue Base
ssdk_sh qm ucastqbase set <src_profile> <service_code_en> <service_code> <cpu_code_en> <cpu_code> <dst_port>

# Ucast Priority Class
ssdk_sh qm ucastpriclass set <priority> <class>

# Mcast Priority Class
ssdk_sh qm mcastpriclass set <priority> <class>

# Queue
ssdk_sh qm queue flush
ssdk_sh qm queue set <queue_id> <enable|disable>

# Ucast Hash
ssdk_sh qm ucasthash set
ssdk_sh qm ucastdflthash set

# Mcast CPU Code
ssdk_sh qm mcastcpucode set <cpu_code> <class>

# AC Control
ssdk_sh qm acctrl set <ac_en> <ac_fc_en>

# AC Pre-buffer
ssdk_sh qm acprebuffer set <type> <obj_id>

# AC Queue Group
ssdk_sh qm acqgroup set

# AC Static/Dynamic Threshold
ssdk_sh qm acstaticthresh set <ac> <color_en> <wred_en> <green_max> <green_min_off> <yel_max_off> <yel_min_off> <red_max_off> <red_min_off> <green_resume_off> <yel_resume_off> <red_resume_off>
ssdk_sh qm acdynamicthresh set <ac> <color_en> <wred_en> <shared_weight> <green_min_off> <yel_max_off> <yel_min_off> <red_max_off> <red_min_off> <green_resume_off> <yel_resume_off> <red_resume_off> <ceiling>

# AC Group Buffer
ssdk_sh qm acgroupbuff set <prealloc_buffer> <total_buffer>

# Counter Control
ssdk_sh qm cntctrl set
ssdk_sh qm cnt cleanup <cnt_index>
ssdk_sh qm cnt set

# Enqueue Control
ssdk_sh qm enqueue set

# Source Profile
ssdk_sh qm srcprofile set
```

---

## 26. BM (BUFFER MANAGEMENT)

Gestión de buffers.

### Comandos:

```bash
# Control
ssdk_sh bm ctrl set <enable|disable>

# Port Group Map
ssdk_sh bm portgroupmap set <port> <group>

# Group Buffer
ssdk_sh bm groupbuff set <group> <buff_num>

# Port Reserved Buffer
ssdk_sh bm portrsvbuff set <port> <prealloc_buff> <react_buff>

# Static/Dynamic Threshold
ssdk_sh bm portsthresh set <port> <max_thresh> <resume_off>
ssdk_sh bm portdthresh set <port> <weight> <shared_ceiling> <resume_off> <resume_min_thresh>
```

---

## 27. SERVICE CODE

Códigos de servicio.

### Comandos:

```bash
# Configuración
ssdk_sh servcode config set

# Loop Check
ssdk_sh servcode loopcheck set <enable|disable>

# Port Service Code
ssdk_sh servcode portServcode set <port_id>

# Atheros Tag
ssdk_sh servcode athtag set
```

---

## 28. RSS HASH

Hash para RSS.

### Comandos:

```bash
# Configuración
ssdk_sh rsshash config set <hash_mask> <hash_fragment_mode> <hash_seed> <hash_sip_mix> <hash_dip_mix> <hash_protocol_mix> <hash_sport_mix> <hash_dport_mix> <hash_fin_inner> <hash_fin_outer>
```

---

## 29. CONTROL PACKET

Paquetes de control.


### Contexto con QCA-SSDK

#### Registro APP_CTRL
- **Dirección:** `CTRL_PKT_APP_CTRL_BASE + (app * 0x4)`

| Campo | Bits | Valores | Descripción | Shell Command |
| :--- | :---: | :--- | :--- | :--- |
| cmd | 0-1 | 0=DROP, 1=FWD, 2=CPU, 3=TRAP | Acción para paquete de control | `ctrlpkt appProfile set` |
| portbitmap | 2-15 | Bitmap de puertos | Puertos afectados | `ctrlpkt appProfile set` |





* Desde la perspectiva de HPPE:
    - En `qca-ssdk`, `hppe_ctrlpkt.c` muestra:
    ```text
    app_ctrl - Control de aplicaciones (EAPOL, PPPoE, IGMP, ARP, DHCP, etc.)
    cmd - Comando para el paquete (forward, drop, CPU)
    portbitmap - Puertos a los que aplicar
    ```
    - **`ssdk_sh ctrlpkt appProfile set` podría forzar paquetes de control a CPU**
    
    - **Protocolos soportados (apps):**
        - ARP, DHCP, IGMP, MLD, STP, RSTP, MSTP, OSPF, PIM, etc.



### Comandos:

```bash
# Ethernet Type Profile
ssdk_sh ctrlpkt ethernetType set <profile_id> <ethernet_type> <ethernet_type_mask>
```

```bash
# RFDB Profile
ssdk_sh ctrlpkt rfdb set <profile_id>
```


```bash
# ctrlpkt appProfile set
ctrlpkt appProfile set <app> <cmd> <portbitmap>
    → cmd: 0=DROP, 1=FWD, 2=CPU, 3=TRAP
    → portbitmap: hex (ej: 0xFFFFFFFF para todos)
```

```bash
# ctrlpkt appProfile set - 19 PARÁMETROS
ssdk_sh ctrlpkt appProfile set <port_map> <ethtype_bmp> <rfdb_bmp> <eapol_en> <pppoe_en> <igmp_en> <arp_req_en> <arp_res_en> <dhcp4_en> <dhcp6_en> <mld_en> <ip6ns_en> <ip6na_en> <8023ah_oam_en> <action> <sg_bypass> <l2filter_bypass> <in_stp_bypass> <in_vlan_filter_bypass>

# Parámetros:
#   port_map: bitmap de puertos (ej: 0xFFFFFFFF para todos)
#   ethtype_bmp: bitmap de perfiles Ethernet
#   rfdb_bmp: bitmap de perfiles RFDB
#   eapol_en: yes|no
#   pppoe_en: yes|no
#   igmp_en: yes|no
#   arp_req_en: yes|no
#   arp_res_en: yes|no
#   dhcp4_en: yes|no
#   dhcp6_en: yes|no
#   mld_en: yes|no
#   ip6ns_en: yes|no (IPv6 Neighbor Solicitation)
#   ip6na_en: yes|no (IPv6 Neighbor Advertisement)
#   8023ah_oam_en: yes|no
#   action: forward|drop|cpycpu|rdtcpu
#   sg_bypass: yes|no (Source Guard bypass)
#   l2filter_bypass: yes|no
#   in_stp_bypass: yes|no (Ingress STP bypass)
#   in_vlan_filter_bypass: yes|no

# Ejemplo: Forzar ARP, DHCP e IGMP a CPU en todos los puertos
ssdk_sh ctrlpkt appProfile set 0xFFFFFFFF 0 0 yes yes yes yes yes yes yes yes yes yes yes rdtcpu no no no no

```

```bash
# VP Group
ssdk_sh ctrlpkt vpgroup set
```

```bash
# Tunnel Decap
ssdk_sh ctrlpkt tunneldecap set
```

---

## 30. VXLAN

VXLAN.

### Comandos:

```bash
# Entry
ssdk_sh vxlan entry set <ip_ver> <udp_type> <l4_port_type> <l4_port>

# GPE Protocol Config
ssdk_sh vxlan gpeprotocfg set <ipv4> <ipv6> <ethernet>
```

---

## 31. GENEVE

GENEVE.

### Comandos:

```bash
# Entry
ssdk_sh geneve entry set <ip_ver> <udp_type> <l4_port_type> <l4_port>
```

---

## 32. TUNNEL PROGRAM

Programación de túneles.

### Comandos:

```bash
# Entry
ssdk_sh tunnelprogram entry set <program_type> <ip_ver> <outer_hdr_type> <protocol> <protocol_mask>

# Config
ssdk_sh tunnelprogram cfg set <program_type> <program_pos_mode> <inner_type_mode> <inner_hdr_type> <basic_hdr_len> <opt_len_unit> <opt_len_mask> <udf0_offset> <udf1_offset> <udf2_offset>

# UDF
ssdk_sh tunnelprogram udf set <udf_idx> <udf_type>
```

---

## 33. TUNNEL

Gestión de túneles.

### Comandos:

```bash
# UDF Profile
ssdk_sh tunnel udfprofileEntry set
ssdk_sh tunnel udfprofilecfg set

# Global Config
ssdk_sh tunnel Globalcfg set <deacce_action> <src_if_check_deacce_en> <src_if_check_action> <vlan_check_deacce_en> <vlan_check_action> <udp_csum_zero_deacce_en> <udp_csum_zero_action> <pppoe_multicast_deacce_en> <pppoe_multicast_action> <hash_mode0> <hash_mode1>

# Port Interface
ssdk_sh tunnel Portintf set <port_id> <tl_l3if_en> <tl_l3if> <dmac_addr> <pppoe_en> <pppoe_group_id> <vlan_group_id>

# Interface
ssdk_sh tunnel Intf set <if_index> <ipv4_decap_en> <ipv6_decap_en> <dmac_check_en> <ttl_exceed_deacce_en> <ttl_exceed_action> <lpm_en> <mini_ipv6_mtu>

# VLAN Interface
ssdk_sh tunnel Vlanintf set <port_id> <svlan_en> <svlan_fmt> <svlan_id> <cvlan_en> <cvlan_fmt> <cvlan_id> <pppoe_en> <tl_l3if_en> <tl_l3if>

# Encap Rule
ssdk_sh tunnel Encaprule set <src1_sel> <src1_start> <src2_sel> <src2_en0> <src2_start0> <src2_width0> <dest2_pos0> <src2_en1> <src2_start1> <src2_width1> <dest2_pos1> <src3_sel> <src3_en0> <src3_start0> <src3_width0> <dest3_pos0> <src3_en1> <src3_start1> <src3_width1> <dest3_pos1>

# Tunnel ID
ssdk_sh tunnel Encapintftunnelid set <l3_if_index> <tunnel_id_en> <tunnel_id>
ssdk_sh tunnel Encapporttunnelid set <port_id> <tunnel_id_en> <tunnel_id>

# Decap/Encap Entry
ssdk_sh tunnel Decapentry set
ssdk_sh tunnel Encapentry set

# Encap Header Control
ssdk_sh tunnel Encapheaderctrl set <ipv4_id_seed> <ipv4_df_set> <udp_sport_base> <udp_sport_mask> <ipv4_addr_map_data> <ipv4_proto_map_data> <ipv6_addr_map_data> <ipv6_proto_map_data>

# ECN
ssdk_sh tunnel Encapecn set <ecn_mode> <inner_ecn>
ssdk_sh tunnel Decapecn set <ecn_mode> <outer_ecn> <inner_ecn>

# Exp Decap
ssdk_sh tunnel Decapexpfmtctrl set <key_sip_en> <key_dip_en> <key_l4proto_en> <key_sport_en> <key_dport_en> <key_tlinfo_en> <tunnel_info_mask> <key_udf0_en> <udf0_idx> <udf0_mask> <key_udf1_en> <udf1_idx> <udf1_mask>

# Decap Key
ssdk_sh tunnel Decapkey set
```

---

## 34. MAPT

MAP-T (Mapping of Address and Port).

### Comandos:

```bash
# Decap Control
ssdk_sh mapt Decapctrl set <src_check_action> <dst_check_action> <no_tcp_udp_action> <udp_csum_zero_action> <ipv4_df_set>

# Decap Rule Entry
ssdk_sh mapt Decapruleentry set <ipv4_prefix> <ipv6_addr_type> <suffix_start> <suffix_width> <suffix_pos> <psid1_en> <psid1_start> <psid1_width> <proto_type> <psid2_en> <psid2_start> <psid2_width> <psid_check_en>

# Decap Entry
ssdk_sh mapt Decapentry set <dst_is_local> <ipv6_addr> <prefix_len> <svlan_check> <svlan_fmt> <svlan_id> <cvlan_check> <cvlan_fmt> <cvlan_id> <tl_l3if_check> <tl_l3if> <src_info_en> <src_info_type> <src_info> <edit_rule_id> <exp_profile>
```

---

## 35. VPORT

Puertos Virtuales.

### Comandos:

```bash
# Physical Port
ssdk_sh vport Phyport set <port_id> <vport_type> <vport_id>

# State Check
ssdk_sh vport Statecheck set <vport_id> <check_en> <vport_type> <vport_active> <tunnel_active>
```

---

## 36. ATHEROS TAG

Atheros Tagging.

### Comandos:

```bash
# Priority Mapping
ssdk_sh athtag primapping set <direction> <ath_pri> <int_pri>

# Port Mapping
ssdk_sh athtag portmapping set <direction> <ath_port> <int_port>

# RX Configuration
ssdk_sh athtag rx set <port_id> <athtag_en> <athtag_type>

# TX Configuration
ssdk_sh athtag tx set <port_id> <athtag_en> <athtag_type> <athtag_version> <athtag_action> <bypass_fwd_en> <disable_field>
```

### Parámetros:

- `direction`: both|ingress|egress
- `athtag_version`: v2|v3
- `athtag_action`: normal|read_write_reg|disable_learn|disable_offload|disable_learn_offload

---

## 37. DEBUG

Diagnóstico y depuración.

### Comandos:

```bash
# PHY Register
ssdk_sh debug phy get <ph_id> <reg_addr>
ssdk_sh debug phy set <ph_id> <reg_addr> <value>

# Switch Register
ssdk_sh debug reg get <reg_addr> <4>
ssdk_sh debug reg set <reg_addr> <value> <4>
ssdk_sh debug reg dump <group_id> <0-6>

# Debug Register
ssdk_sh debug dbgreg dump

# Register Field
ssdk_sh debug field get <reg_addr> <offset> <len> <4>
ssdk_sh debug field set <reg_addr> <offset> <len> <value> <4>

# ACL Dump
ssdk_sh debug aclList dump
ssdk_sh debug aclRule dump

# Device Reset
ssdk_sh debug device reset

# SSDK Config
ssdk_sh debug ssdk config

# PSGMII Register
ssdk_sh debug preg get <reg_addr> <4>
ssdk_sh debug preg set <reg_addr> <value> <4>

# MMIO (Memory Mapped I/O)
ssdk_sh debug mmio <address>
```

---

## 38. DEVICE

Gestión de dispositivos.

### Comandos:

```bash
# Set Device ID
ssdk_sh device id set <dev_id>

# Help
ssdk_sh help

# Quit
ssdk_sh quit
ssdk_sh q
```

---

## 39. ACL


- ✅ Compilado (`IN_ACL=TRUE` en config)
- ❌ NO FUNCIONAL en AX3600 (no disponible en firmware build .config)

- Hay que invesigar 
- El core `qca-ssdk` no tiene soporte ACL para IPQ8074 y firm de NSS antiguo (testado en FW 11.4.0.5)


### Contexto con QCA-SSDK

#### Registro ACL_CTRL
- **Dirección:** `ACL_CTRL_BASE`

| Campo | Bits | Valores | Descripción | Shell Command |
| :--- | :---: | :--- | :--- | :--- |
| acl_en | 0 | 0=off, 1=on | Habilitar ACL | `acl enable` |
| rule_count | 1-15 | 0-32767 | Número de reglas | `acl rule set` |
| default_action | 16-17 | 0=DROP, 1=FWD, 2=CPU, 3=TRAP | Acción por defecto | `acl default_action set` |


- Desde la perspectiva de HPPE:
    - ACL usa UDF para campos personalizados. Esto permitiría hacer match en casi cualquier campo del paquete.

```bash
# Comandos ACL (documentados pero no funcionales en AX3600)

acl enable
acl rule set <idx> <match> <action>
acl default_action set <0|1|2|3>
```


```bash
# Ver estado de ACL
ssdk_sh acl status get

# PENDIENTE: Test con FW >11.4, "en teoría" no aplica a ipq80x, `qca-nss-dvr-acl` sí podŕía ser adaptado

# Ver reglas ACL
ssdk_sh acl rule show
PENDIENTE: Test con FW >11.4, "en teoría" no aplica a ipq80x, `qca-nss-dvr-acl` sí podŕía ser adaptado

ssdk_sh acl rule show 0
ssdk_sh acl rule show 1
ssdk_sh acl rule show 0 0
ssdk_sh acl rule show 1 0
ssdk_sh acl rule show yes
ssdk_sh acl rule show help
# TODOS: invalid or incomplete command en ipq807x

```



---

## Notas Importantes

- No existe documentación de la shell, todos los usos, comandos y parámetros se han extraído del análisis del código fuente.
- Algunas opciones no están disposnibles según plataforma, según firmware utilizado, etc.
- Conviene repasar su código fuente para terminar algunas implementaciones que se dejaron a medias

### Sintaxis General:

- Los comandos siguen el formato: `ssdk_sh <comando> <subcomando> <acción> [parámetros]`
- Los parámetros entre `<>` son obligatorios
- Los parámetros entre `[]` son opcionales
- Algunos comandos requieren todos los argumentos posicionales explíticos (más de 30 argumentos)

### Parámetros Comunes:

- `port_id`: Número de puerto (0-N)
- `enable|disable`: Activar/Desactivar
- `forward|drop|cpycpu|rdtcpu`: Acciones de reenvío
- `queue_id`: ID de cola (0-3, 0-5, etc.)
- `vlan_id`: ID de VLAN (1-4095)
- `mac_addr`: Dirección MAC en formato xx-xx-xx-xx-xx-xx
- `ip4_addr`: Dirección IPv4 en formato x.x.x.x
- `ip6_addr`: Dirección IPv6 en formato xxxx::xxxx

### Valores Numéricos:

- Hexadecimal: `0x` prefijo
- Decimal: Sin prefijo
- Binario: No soportado directamente (nunca son numerales, solo `YES/Y/NO/N`)


### Modos de Operación:

- La mayoría de los comandos tienen modo `set` y `get`
- Algunos tienen modos especiales como `add`, `del`, `find`, etc.

### Interfaz de Usuario:

- La propia interfaz está rota (no permite borrado, y otros fallos); comando se ejecutan perfectamente desde Ash vía `$ ssdk_sh <comando> ...`
- El shell es sensible a mayúsculas/minúsculas (case-insensitive)
- Los comandos `quit` / `q`, disponibles para salir, `help`apenas aporta información, pero la UX es muy buena ante errores de parámetros (indica en cada caso el motivo de error)

---

## EJEMPLOS PRÁCTICOS

### Configuración Básica de Puerto:

```bash
# Configurar puerto 3 a 1Gbps Full Duplex
ssdk_sh port speed set 3 1000
ssdk_sh port duplex set 3 full
ssdk_sh port autoNeg enable 3
```

### Configuración VLAN:

```bash
# Crear VLAN 100
ssdk_sh vlan entry create 100

# Añadir puertos 1,2,3 tagged
ssdk_sh vlan member add 100 1 tagged
ssdk_sh vlan member add 100 2 tagged
ssdk_sh vlan member add 100 3 tagged
```

### Mirroring de Tráfico:

```bash
# Configurar analyzer point (punto de mirroring)
ssdk_sh mirror analyPt set <0-7>
# 0 = Puerto físico
# 1 = VLAN
# 2 = DSA
# 3+ = Otros

# Ver configuración actual
ssdk_sh mirror analyPt get 0

# Configurar mirror en ingress para un puerto
ssdk_sh mirror ptIngress set <phyad> enable
ssdk_sh mirror ptIngress set 3 enable   # lan3

# Ver estado de mirror en un puerto
ssdk_sh mirror ptIngress get 3

# Configurar mirror en egress
ssdk_sh mirror ptEgress set <phyad> enable
ssdk_sh mirror ptEgress get <phyad>

# Deshabilitar mirror
ssdk_sh mirror ptIngress set 3 disable
```


```bash
# Configurar puerto 6 como análisis
ssdk_sh mirror analyPt set 6

# Mirror de ingreso en puerto 3
ssdk_sh mirror ptIngress set 3 enable
```

### QoS:

```bash
# Configurar scheduling SP
ssdk_sh qos schMode set sp

# Mapear UP 5 a cola 2
ssdk_sh qos up2q set 5 2
```

### Rate Limiting:

```bash
# Limitar puerto 3 a 100 Mbps
ssdk_sh rate ptEgress set 3 100000 enable
```

### Debug:

```bash
# Leer registro
ssdk_sh debug reg get 0x100000 4

# Escribir registro
ssdk_sh debug reg set 0x100000 0x12345678 4
```


---


## ANEXO - SETTERS - GETTERS con sintax completa

### ARREGLOS DE SINTaxis

La shell `ssdk_sh` está rotísima, sintaxis incompleta o vacía para la mayoría de los setters y getters, pese a tener toda la lógica de parámetros definida en cada función. Esto causaba errores como:
- `Illegal parameter(s)`
- `parameter (xxx) or default (yyy) absent`
- `invalid or incomplete command`


Qualcomm debió abandonar el diseño de una interfaz para poder controlar NSS; parece un trabajo incompleto, o una ruptura entre versiones.



| Comando | Original | Arreglado | Parámetros |
|---------|----------|-----------|------------|
| `ip globalctrl set` | `""` | 12 params | Acciones y booleanos |
| `ip intf set` | `"<index>"` | 11 params | MRU, MTU, bypass flags |
| `ip hostentry add` | `""` | 22 params | Estructura completa |
| `flow mgmt set` | `"<type> <dir>"` | 7 params | miss_action, bypass, key_sel |
| `flow entry add` | `"<add_mode>"` | 43 params | Estructura completa |
| `acl rule add` | `"<list_id> <rule_id> <rule_nr>"` | 7 params | rule_type, post_routing |
| `ctrlpkt appprofile set` | `"<app_entry>"` | 19 params | Perfil completo |
| `port mtu set` | `""` | 2 params | port_id, mtu_size |
| `port mru set` | `""` | 2 params | port_id, mru_size |
| `port mtucfg set` | `""` | 5 params | Configuración MTU |
| `port mrumtu set` | `""` | 3 params | MRU y MTU |
| `vlan lan_wan_cfg set` | `""` | 4 params | Puertos y VLANs |
| `fdb entry add` | `""` | 14 params | Estructura FDB |
| `ip intfentry add` | `""` | 7 params | Interface MAC |
| `ip defaultroute set` | `""` | 5 params | Ruta por defecto |
| `ip hostroute set` | `""` | 5 params | Ruta host |
| `ip wcmpentry set` | `""` | 2 params | WCMP |
| `flow agetime set` | `""` | 2 params | Age timer |
| `flow global set` | `""` | 11 params | Configuración global |
| `nat natentry add` | `""` | 13 params | NAT |
| `nat naptentry add` | `""` | 18 params | NAPT |
| `nat flowentry add` | `""` | 16 params | Flow NAT |
| `nat pubaddr add` | `""` | 2 params | Public address |


###  `ctrlpkt appprofile set`

**API corregida:** `SW_API_MGMTCTRL_CTRLPKT_PROFILE_ADD` (no `_SET`), es saturación de contexto.

El CORE de SSDK no tiene `SW_API_MGMTCTRL_CTRLPKT_PROFILE_SET`. La API `_ADD` se usa tanto para `add` como para `set`, interpretando los parámetros según el contexto.


Todos los arreglos fueron verificados contra el código fuente del CORE (`qca-ssdk/src/shell/shell_config.c` y `qca-ssdk/src/fal_uk/fal_*.c`), asegurando que las APIs coinciden.

### Comandos pendieentes de revisión / arreglo

- `vlan entry append` - Sin parámetros
- `vlan entry flush` - Sin parámetros
- `fdb entry iterate` - Ya tiene `<iterator>`
- `fdb firstEntry find` - Sin parámetros
- `fdb nextEntry find` - Sin parámetros
- `ip defaultroute get` - Solo lectura
- `ip hostroute get` - Solo lectura
- `ip wcmpentry get` - Solo lectura
- `flow agetime get` - Solo lectura
- `flow global get` - Solo lectura
- `debug aclList dump` - Sin parámetros
- `debug aclRule dump` - Sin parámetros




## Comandos - Subcomandos - Parámetros actualizado

### IP Global Control (12 params)
```shell
ssdk_sh ip globalctrl set <mru_fail_action> <mru_deacclr_en> <mtu_fail_action> <mtu_deacclr_en> <mtu_nonfrag_fail_action> <mtu_nonfrag_deacclr_en> <prefix_bc_action> <prefix_bc_deacclr_en> <icmp_rdt_action> <icmp_rdt_deacclr_en> <hash_mode_0> <hash_mode_1>
```



### IP Interface (11 params)
```bash
ssdk_sh ip intf set <if_index> <mru> <mtu> <ttl_dec_bypass_en> <ipv4_uc_route_en> <ipv6_uc_route_en> <icmp_trigger_en> <ttl_exceed_action> <ttl_exceed_deacclr_en> <mac_addr_bitmap> <mac_addr>
```



### IP Host Entry (22 params)
```bash
ssdk_sh ip hostentry add <entryid> <flags> <status> <ip4_addr> <ip6_addr> <mac> <intf_id> <lb_num> <vrf_id> <port_id> <action> <mirror> <counter> <counter_id> <dst_info> <syn_toggle> <lan_wan> <mcast_vsi> <mcast_sip4_addr> <mcast_sip6_addr> <pppoe_en> <pppoe_id>
```


### Flow Management (7 params)
```bash
ssdk_sh flow mgmt set <type> <dir> <miss_action> <frag_bypass> <tcpspec_bypass> <all_bypass> <key_sel>
```



### Flow Entry Add (43 params)
```bash
ssdk_sh flow entry add <add_mode> <entry_id> <entry_type> <host_addr_type> <host_addr_index> <protocol> <age> <src_intf_valid> <src_intf_index> <fwd_type> <snat_nexthop> <snat_srcport> <dnat_nexthop> <dnat_dstport> <route_nexthop> <port_valid> <route_port> <bridge_port> <deacclr_en> <copy_tocpu_en> <syn_toggle> <pri_profile> <service_code> <ip_type> <src_port> <dst_port> <flow_ip4> <flow_ip6> <tree_id> <flow_cookie_ext> <wifi_qos_en> <wifi_qos> <qos_type> <pmtu_check_l3> <pmtu> <vpn_id> <vlan_fmt_valid> <svlan_fmt> <cvlan_fmt> <bridge_nexthop_valid> <bridge_nexthop> <policer_valid> <policer_index>
```



### Control Packet AppProfile (19 params)
```bash
ssdk_sh ctrlpkt appProfile set <port_map> <ethtype_bmp> <rfdb_bmp> <eapol_en> <pppoe_en> <igmp_en> <arp_req_en> <arp_res_en> <dhcp4_en> <dhcp6_en> <mld_en> <ip6ns_en> <ip6na_en> <8023ah_oam_en> <action> <sg_bypass> <l2filter_bypass> <in_stp_bypass> <in_vlan_filter_bypass>
```



### ACL Rule Add (7 params)
```bash
ssdk_sh acl rule add <list_id> <rule_id> <rule_nr> <rule_type> <post_routing> <pri> <acl_pool>
```



### FDB Entry Add (14 params)
```bash
ssdk_sh fdb entry add <addr> <fid> <dacmd> <sacmd> <dest_port> <static> <leaky> <mirror> <entry_ver> <queue_override> <cross_pt_state> <white_list_en> <load_balance_en> <type>
```



### NAT Entries
```bash
ssdk_sh nat natentry add <entryid> <flags> <status> <select_idx> <vrf_id> <src_addr> <trans_addr> <port_num> <port_range> <action> <mirror> <counter> <counter_id>
```

```bash
ssdk_sh nat natentry add <entryid> <flags> <status> <select_idx> <vrf_id> <src_addr> <trans_addr> <port_num> <port_range> <action> <mirror> <counter> <counter_id>
```

```bash
ssdk_sh nat flowentry add <entryid> <flags> <status> <vrf_id> <flow_cookie> <load_balance> <src_addr> <dst_addr> <src_port> <dst_port> <action> <mirror> <counter> <counter_id> <priority> <priority_val>
```

```bash
ssdk_sh nat pubaddr add <entryid> <pub_addr>
```




### VLAN LAN/WAN Config (4 params)
```bash
ssdk_sh vlan lan_wan_cfg set <lan_ports> <lan_vids> <wan_ports> <wan_vids>
```



### Port MTU/MRU
```bash
ssdk_sh port mtu set <port_id> <mtu_size>
ssdk_sh port mru set <port_id> <mru_size>
ssdk_sh port mtucfg set <port_id> <mtu_enable> <mtu_type> <extra_header_len> <eg_vlan_tag_flag>
ssdk_sh port mrumtu set <port_id> <mru_size> <mtu_size>
```



### IP Interface MAC (7 params)
```bash
ssdk_sh ip intfentry add <entryid> <vrf_id> <vid_low> <vid_high> <mac_addr> <ip4_route> <ip6_route>
```




### IP Routes
```bash
ssdk_sh ip defaultroute set <valid> <vrf_id> <ip_version> <route_type> <index>
ssdk_sh ip hostroute set <valid> <vrf_id> <ip_version> <ip_addr> <prefix_length>
```



### Flow Age/Global
```bash
ssdk_sh flow agetime set <age_time> <unit>
ssdk_sh flow global set <src_intf_check_action> <src_intf_check_deacclr_en> <service_loop_en> <service_loop_action> <service_loop_deacclr_en> <flow_deacclr_action> <sync_mismatch_action> <sync_mismatch_deacclr_en> <hash_mode_0> <hash_mode_1> <flow_mismatch_copy_escape_en>
```



### IP WCMP (2 params)
```bash
ssdk_sh ip wcmpentry set <nh_nr> <nh_id_list>
```
