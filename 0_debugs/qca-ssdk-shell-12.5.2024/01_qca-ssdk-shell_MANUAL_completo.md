# MANUAL COMPLETO DE SSDK SHELL (ssdk_sh)

- FECHA: 2026, Agosto, 02.


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
ssdk_sh port mtu set <port_id> <mtu_size>
ssdk_sh port mru set <port_id> <mru_size>
ssdk_sh port mtu get <port_id>
ssdk_sh port mru get <port_id>

# Ring Union
ssdk_sh port ringunion set <enable|disable>
ssdk_sh port ringunion get

# Source Filter
ssdk_sh port srcfilter set <port_id> <enable|disable>
ssdk_sh port srcfiltercfg set <port_id> <src_filter_enable> <srcfilter_mode>

# 802.3ah
ssdk_sh port port8023ah set <port_id>

# MTU Config (APPE)
ssdk_sh port mtucfg set <port_id> <mtu_enable> <mtu_type> <extra_header_len> <eg_vlan_tag_flag>

# MRU/MTU Combined
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
# Añadir entrada FDB con cpycpu
ssdk_sh fdb entry add <mac> <port> cpycpu
# PENDIENTE: No fuerza a CPU, ver deps, ver código, ver broken=1

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

# Setear parámetros
ssdk_sh ip globalctrl set <params>

# Ejemplo probado:
ssdk_sh ip globalctrl set rdtcpu no rdtcpu no rdtcpu no rdtcpu no rdtcpu yes yes no
# Resultado: icmp_rdt_action pasó de 0x0 a 0x3 (rdtcpu)
```

### 17.2 IP HOSTENTRY (FDB)

```bash
# Añadir entrada FDB (intento fallido)
ssdk_sh ip hostentry add <params>
# PENDIENTE -> Recabar la sintáxis exacta de los argumentos de funciones en su código

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

# ARP Learn
ssdk_sh ip ptarplearn set <port_id> <flag>
ssdk_sh ip arplearn set <learnlocal|learnall>

# IP/ARP Source Guard
ssdk_sh ip ptipsrcguard set <port_id> <mac_ip|mac_ip_port|mac_ip_vlan|mac_ip_port_vlan|no_guard>
ssdk_sh ip ptarpsrcguard set <port_id> <mac_ip|mac_ip_port|mac_ip_vlan|mac_ip_port_vlan|no_guard>

# Route
ssdk_sh ip routestatus set <enable|disable>

# Interface MAC
ssdk_sh ip intfentry set <entryid> <vrf_id> <vid_low> <vid_high> <mac_addr> <ip4_route> <ip6_route>
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

# Default Route
ssdk_sh ip defaultroute set <valid> <vrf_id> <ip_version> <route_type> <index>

# Host Route
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

# Global Control
ssdk_sh ip globalctrl set <mru_fail_action> <mru_deacclr_en> <mtu_fail_action> <mtu_deacclr_en> <mtu_nonfrag_fail_action> <mtu_nonfrag_deacclr_en> <prefix_bc_action> <prefix_bc_deacclr_en> <icmp_rdt_action> <icmp_rdt_deacclr_en> <hash_mode_0> <hash_mode_1>
```

---

## 18. FLOW (Gestión de flujos)

- ✅ Compilado (`IN_FLOW=TRUE` en config)
- ✅ Funcional en AX3600

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
# La sintaxis REAL es:

ssdk_sh flow mgmt set <type> <dir> <miss_action> <frag_bypass_en> <tcpspec_bypass_en> <all_bypass_en> <key_sel>
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
# NAT Entry
ssdk_sh nat natentry set <entryid> <entryflags> <entrystatus> <select_idx> <vrf_id> <src_addr> <trans_addr> <port_num> <port_range> <action> <mirror> <counter> <counter_id>
ssdk_sh nat natentry add <entryid> <entryflags> <entrystatus> <select_idx> <vrf_id> <src_addr> <trans_addr> <port_num> <port_range> <action> <mirror> <counter> <counter_id>
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

### Comandos:

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

### Comandos:

```bash
# Ethernet Type Profile
ssdk_sh ctrlpkt ethernetType set <profile_id> <ethernet_type> <ethernet_type_mask>

# RFDB Profile
ssdk_sh ctrlpkt rfdb set <profile_id>

# Application Profile
ssdk_sh ctrlpkt appProfile set <port_bitmap> <ethtype_profile_bitmap> <rfdb_profile_bitmap> <eapol_en> <pppoe_en> <igmp_en> <arp_request_en> <arp_response_en> <dhcp4_en> <dhcp6_en> <mld_en> <ip6ns_en> <ip6na_en> <ctrlpkt_profile_action> <sourceguard_bypass> <l2filter_bypass> <ingress_stp_bypass> <ingress_vlan_filter_bypass>

# VP Group
ssdk_sh ctrlpkt vpgroup set

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

## 38. ACL


- ✅ Compilado (`IN_ACL=TRUE` en config)
- ❌ NO FUNCIONAL en AX3600 (no disponible en firmware build .config)

- Hay que invesigar 
- El core `qca-ssdk` no tiene soporte ACL para IPQ8074 y firm de NSS antiguo (testado en FW 11.4.0.5)

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
