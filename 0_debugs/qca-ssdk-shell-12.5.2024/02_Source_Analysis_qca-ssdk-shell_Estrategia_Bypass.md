# 02:  Diseño de `qca-ssdk-shell`

- OBJETO:   Inferir en el Hardware Offload para arquitectura QualcommAX `ipq807x` con los mínimos cambios posibles en kernel/firmware.
- TESTADO EN: Xiaomi AX3600, custom firmware (2026-07-16, fork y feeds de @AgustinLorenzo), NSS v`NSS.HK.11.4.0.5-6-R`.
- MOTIVO PRINCIPAL:  La herramienta parece creada por Qualcomm, no existiendo ningún tipo de documentación; con una implementación a medias, rota, y muy poco intuitiva.
- DESAFIO: Documentar en detalle
- FECHA:  2026, Agosto, 02.


## 0. Sources

```
$ tree ./

./
├── apk-aarch64_cortex-a53/
│   └── qca-ssdk-shell
│       ├── post-install
│       ├── post-upgrade
│       └── pre-deinstall
│
├── build/
│  
├── cmd_groups.txt
├── config
├── include/
│   ├── api/
│   │   ├── api_access.h
│   │   ├── api_desc.h
│   │   ├── sw_api.h
│   │   └── sw_ioctl.h
│   ├── common/
│   │   ├── aos_head.h
│   │   ├── shared_func.h
│   │   ├── sw_config.h
│   │   ├── sw_error.h
│   │   ├── sw.h
│   │   └── util.h
│   ├── fal/
│   │   ├── fal_acl.h
│   │   ├── fal_api.h
│   │   ├── fal_athtag.h
│   │   ├── fal_bm.h
│   │   ├── fal_cosmap.h
│   │   ├── fal_ctrlpkt.h
│   │   ├── fal_fdb.h
│   │   ├── fal_flow.h
│   │   ├── fal_geneve.h
│   │   ├── fal.h
│   │   ├── fal_igmp.h
│   │   ├── fal_init.h
│   │   ├── fal_interface_ctrl.h
│   │   ├── fal_ip.h
│   │   ├── fal_leaky.h
│   │   ├── fal_led.h
│   │   ├── fal_mapt.h
│   │   ├── fal_mib.h
│   │   ├── fal_mirror.h
│   │   ├── fal_misc.h
│   │   ├── fal_multi.h
│   │   ├── fal_nat.h
│   │   ├── fal_pktedit.h
│   │   ├── fal_policer.h
│   │   ├── fal_port_ctrl.h
│   │   ├── fal_portvlan.h
│   │   ├── fal_pppoe.h
│   │   ├── fal_ptp.h
│   │   ├── fal_qm.h
│   │   ├── fal_qos.h
│   │   ├── fal_rate.h
│   │   ├── fal_reg_access.h
│   │   ├── fal_rss_hash.h
│   │   ├── fal_sec.h
│   │   ├── fal_servcode.h
│   │   ├── fal_sfp.h
│   │   ├── fal_shaper.h
│   │   ├── fal_stp.h
│   │   ├── fal_trunk.h
│   │   ├── fal_tunnel.h
│   │   ├── fal_tunnel_program.h
│   │   ├── fal_type.h
│   │   ├── fal_uk_if.h
│   │   ├── fal_vlan.h
│   │   ├── fal_vport.h
│   │   ├── fal_vsi.h
│   │   └── fal_vxlan.h
│   ├── init/
│   │   ├── ssdk_init.h
│   │   └── ssdk_plat.h
│   ├── ref/
│   │   ├── ref_api.h
│   │   └── ref_vlan.h
│   ├── sal/
│   │   ├── os/
│   │   │   ├── aos_lock.h
│   │   │   ├── aos_mem.h
│   │   │   ├── aos_timer.h
│   │   │   ├── aos_types.h
│   │   │   └── linux_user
│   │   │       ├── aos_lock_pvt.h
│   │   │       ├── aos_mem_pvt.h
│   │   │       ├── aos_timer_pvt.h
│   │   │       └── aos_types_pvt.h
│   │   └── sd/
│   │       ├── linux/
│   │       │   └── uk_interface/
│   │       │       └── sw_api_us.h
│   │       └── sd.h
│   └── shell/
│       ├── shell_config.h
│       ├── shell.h
│       ├── shell_io.h
│       ├── shell_lib.h
│       └── shell_sw.h
├── ipkg-aarch64_cortex-a53/
│   └── qca-ssdk-shell/
│       ├── lib/
│       │   └── apk/
│       │       └── packages/
│       │           └── qca-ssdk-shell.list
│       └── usr/
│           └── sbin/
│               └── ssdk_sh
├── make/
│   ├── components.mk
│   ├── config.mk
│   ├── defs.mk
│   ├── linux_opt.mk
│   ├── target.mk
│   └── tools.mk
├── Makefile
├── src/
│   ├── api/
│   │   ├── api_access.c
│   │   └── Makefile
│   ├── fal_uk/
│   │   ├── fal_acl.c
│   │   ├── fal_athtag.c
│   │   ├── fal_bm.c
│   │   ├── fal_cosmap.c
│   │   ├── fal_ctrlpkt.c
│   │   ├── fal_fdb.c
│   │   ├── fal_flow.c
│   │   ├── fal_geneve.c
│   │   ├── fal_igmp.c
│   │   ├── fal_init.c
│   │   ├── fal_interface_ctrl.c
│   │   ├── fal_ip.c
│   │   ├── fal_leaky.c
│   │   ├── fal_led.c
│   │   ├── fal_mapt.c
│   │   ├── fal_mib.c
│   │   ├── fal_mirror.c
│   │   ├── fal_misc.c
│   │   ├── fal_nat.c
│   │   ├── fal_pktedit.c
│   │   ├── fal_policer.c
│   │   ├── fal_port_ctrl.c
│   │   ├── fal_portvlan.c
│   │   ├── fal_pppoe.c
│   │   ├── fal_ptp.c
│   │   ├── fal_qm.c
│   │   ├── fal_qos.c
│   │   ├── fal_rate.c
│   │   ├── fal_reg_access.c
│   │   ├── fal_rss_hash.c
│   │   ├── fal_sec.c
│   │   ├── fal_servcode.c
│   │   ├── fal_sfp.c
│   │   ├── fal_shaper.c
│   │   ├── fal_stp.c
│   │   ├── fal_trunk.c
│   │   ├── fal_tunnel.c
│   │   ├── fal_tunnel_program.c
│   │   ├── fal_uk_if.c
│   │   ├── fal_vlan.c
│   │   ├── fal_vport.c
│   │   ├── fal_vsi.c
│   │   ├── fal_vxlan.c
│   │   └── Makefile
│   ├── ref/
│   │   ├── Makefile
│   │   └── ref_vlan.c
│   ├── sal/
│   │   ├── Makefile
│   │   └── sd/
│   │       ├── linux/
│   │       │   ├── Makefile
│   │       │   └── uk_interface/
│   │       │       ├── Makefile
│   │       │       ├── sw_api_us_ioctl.c
│   │       │       └── sw_api_us_netlink.c
│   │       ├── Makefile
│   │       └── sd.c
│   └── shell/
│       ├── Makefile
│       ├── shell.c
│       ├── shell_config.c
│       ├── shell_io.c
│       ├── shell_lib.c
│       └── shell_sw.c
└── version.date

38 directories, 316 files
```

---

## 1. Arquitectura

### Capas

- **Shell** (`ssdk_sh`): Interfaz de usuario. Los comandos se definen en `shell_config.c` con su sintaxis y funciones callback.
- **FAL** (Feature Abstraction Layer): Archivos `fal_*.c` que exponen funciones como `fal_port_duplex_set()`. Cada una llama a `sw_uk_exec()` con un ID de API (`SW_API_*`).
- **Kernel Interface**: `sw_uk_exec()` empaqueta los parámetros y llama a `sw_uk_if()` (implementado vía ioctl o netlink) para comunicarse con el driver del switch en el kernel.

---

### Mecanismo de Comunicación

- Por defecto se usa **ioctl** a través del dispositivo `/dev/switch_ssdk`.
- También debería poder usarse **netlink** (definido en `sw_api_us_netlink.c`), pero la implementación está a medias, y rota.

### API

1. `api_access.c` mantiene arrays `sw_api_func` y `sw_api_param` que mapean cada ID de API a su función y número de parámetros.
2. `sw_uk_exec` valida y llama correctamente.

> La implementación también tiene algunos problemas con los typos.

---

## 2. Comandos (agrupados por su "capacidad" para poder manejar/influir en el Offload)

### A. GRUPO `flow` (Gestión de Flujos) — **CRÍTICO**

> Dependencia de compilación: `IN_FLOW` (definido como `TRUE` en config, compilado)

Comandos disponibles (según `shell_config.c`):

| Comando | Acción | Sintaxis (en ayuda) | Sintaxis REAL (según código) |
|---|---|---|---|
| `mgmt` | set | `<type> <dir>` | `<type> <dir> <miss_action> <frag_bypass_en> <tcpspec_bypass_en> <all_bypass_en> <key_sel>` |
| `mgmt` | get | `<type> <dir>` | `<type> <dir>` |
| `entry` | add | `<add_mode>` | `<add_mode> <flow_entry>` (estructura) |
| `entry` | del | `<del_mode>` | `<del_mode> <flow_entry>` |
| `entry` | get | `<get_mode>` | `<get_mode> <flow_entry>` |
| `flowipv43tuple` | show | `""` | Muestra todas las entradas IPv4 3-tupla |
| `flowipv45tuple` | show | `""` | Muestra IPv4 5-tupla |
| `flowipv63tuple` | show | `""` | IPv6 3-tupla |
| `flowipv65tuple` | show | `""` | IPv6 5-tupla |
| `host` | add | `<add_mode>` | `<add_mode> <flow_host_entry>` |
| `host` | del | `<del_mode>` | `<del_mode> <flow_host_entry>` |
| `host` | get | `<get_mode>` | `<get_mode> <flow_host_entry>` |
| `status` | set | `<status>` | `<enable\|disable>` |
| `status` | get | `""` | - |

**Detalles de `flow mgmt set`** (el más importante):

```bash
ssdk_sh flow mgmt set <type> <dir> <miss_action> <frag_bypass_en> <tcpspec_bypass_en> <all_bypass_en> <key_sel>
```

- `type`: `0` (puerto físico), `1`, `2` (otros tipos de flujo). Usar `0` para puertos.
- `dir`: `0` (WAN), `1` (LAN1), `2` (LAN2), `3` (LAN3), `4` (CPU/NSS).
- `miss_action`:
  - `0` = Drop
  - `1` = Forward (normal)
  - `2` = (no documentado)
  - `3` = **Forward to CPU** (¡esto es lo que necesitamos!)
- `frag_bypass_en`: `no` / `yes` — Bypass para fragmentos.
- `tcpspec_bypass_en`: `no` / `yes` — Bypass para TCP específico.
- `all_bypass_en`: `no` / `yes` — Bypass global (desactiva offload).
- `key_sel`: `0`, `1`, `2` — Selección de clave de hash.

Ejemplo para forzar tráfico del puerto LAN3 a CPU:

```bash
ssdk_sh flow mgmt set 0 3 forward no no no 3
```

> **Nota:** El comando `flow entry add` y `flow host add` requieren estructuras complejas (no se pueden pasar desde la línea de comandos directamente).
>
> Para añadir flujos específicos, se necesita programación adicional o usar `ssdk_sh` con argumentos posicionales (que no están documentados).
>
> En la práctica, la shell solo proporciona `flow mgmt` para controlar el comportamiento general.

---

### B. GRUPO `ip` (Control de Capa 3) — **CRÍTICO**

> Dependencia: `IN_IP` (`TRUE`)

* Comandos relevantes:

| Comando | Acción | Sintaxis (ayuda) | Sintaxis REAL |
|---|---|---|---|
| `globalctrl` | set | `<12 parámetros>` | `set <mru_fail_action> <mru_deacclr_en> <mtu_fail_action> <mtu_deacclr_en> <mtu_nonfrag_fail_action> <mtu_nonfrag_deacclr_en> <prefix_bc_action> <prefix_bc_deacclr_en> <icmp_rdt_action> <icmp_rdt_deacclr_en> <hash_mode_0> <hash_mode_1>` |
| `globalctrl` | get | `""` | - |
| `routestatus` | set | `<enable\|disable>` | Activa/desactiva el ruteo IP (offload) |
| `hostentry` | add | `""` (vacío) | En realidad espera una estructura `fal_host_entry_t` (no usable directamente en shell) |
| `arplearn` | set | `<learnlocal\|learnall>` | Controla aprendizaje ARP |
| `ptarplearn` | set | `<port_id> <flag>` | flag: bit0=aprender req, bit1=aprender resp |
| `ptipsrcguard` | set | `<port_id> <mode>` | Modos: `mac_ip`, `mac_ip_port`, `mac_ip_vlan`, `mac_ip_port_vlan`, `no_guard` |
| `intf` | set | `<index> <mru> <mtu> <ttl_dec_bypass_en> <ipv4_uc_route_en> <ipv6_uc_route_en> <icmp_trigger_en> <ttl_exceed_action> <ttl_exceed_deacclr_en> <mac_addr_bitmap> <mac_addr>` | Permite configurar una interfaz L3 con opciones de bypass. |

* Parámetros de `globalctrl set` (valores posibles):

- `*_action`: `forward`, `drop`, `rdtcpu`, `admit_all`
- `*_deacclr_en`: `yes` / `no` (desactivar clear?)
- `hash_mode_*`: números (0-3)

* Ejemplo probado (funciona):

```bash
ssdk_sh ip globalctrl set rdtcpu no rdtcpu no rdtcpu no rdtcpu no rdtcpu yes yes no
```

Esto cambia `icmp_rdt_action` a `rdtcpu` (envía ICMP a CPU).

* Comando `ip intf set`, permite desactivar bypass de TTL y enrutamiento:

```bash
ssdk_sh ip intf set <if_index> <mru> <mtu> <ttl_dec_bypass_en> <ipv4_uc_route_en> <ipv6_uc_route_en> <icmp_trigger_en> <ttl_exceed_action> <ttl_exceed_deacclr_en> <mac_addr_bitmap> <mac_addr>
```

- `ttl_dec_bypass_en = no` evita que el switch decida por sí mismo (forzando a CPU).
- `ipv4_uc_route_en = no` desactiva el enrutamiento unicast offload.

> **Nota:** Los comandos `hostentry add`, `nexthop set`, `networkroute add` requieren estructuras complejas; la shell no los soporta directamente con parámetros simples. Se necesita usar `ssdk_sh` con argumentos posicionales no documentados (posiblemente pase los bytes de la estructura).

---

### C. GRUPO `mirror` (Mirroring)

**El tema mirroring merece una sección aparte** (`qca-nss-clients`).

> Dependencia de .config
> -> Kernel modules
>   -> Network Devices
>       -> kmod-qca-nss-drv... Kernel driver for NSS (core driver) (PACKAGE_kmod-qca-nss-drv [=y])                - >           ** Configuration **                                                                                                       >                 Enable MIRROR (`CONFIG_NSS_DRV_MIRROR_ENABLE=y`, `CONFIG_PACKAGE_kmod-qca-nss-drv-mirror=y`)

> Dependencia: `IN_MIRROR` (`TRUE`)

| Comando | Acción | Sintaxis | Descripción |
|---|---|---|---|
| `analyPt` | set | `<port_id>` | Puerto de destino del mirror (donde se envía la copia) |
| `analyPt` | get | - | Obtener puerto de análisis |
| `ptIngress` | set | `<port_id> <enable\|disable>` | Mirrored ingress de un puerto |
| `ptIngress` | get | `<port_id>` | Estado |
| `ptEgress` | set | `<port_id> <enable\|disable>` | Mirrored egress |
| `ptEgress` | get | `<port_id>` | Estado |
| `analyCfg` | set | `<both\|ingress\|egress>` | Dirección del análisis |
| `analyCfg` | get | `<both\|ingress\|egress>` | - |

* ** (APARENTEMENTE) LIMITADO:**

  - Solo soporta mirroring de puertos físicos, no de VLAN, DSA (DSA de hecho, es excluyente frente a NSS) ni puertos virtuales.
  - No puede capturar tráfico que ya está en el NSS (offload) porque ese tráfico no pasa por el switch en modo normal. Es útil solo para tráfico que pasa por la CPU.

* **Hey que investigar más**, es **muy** importante poder conocer los nombres/numerales internos de cada puerto/interfaz.


---

### D. GRUPO `ctrlpkt` (Control de Paquetes)

> Dependencia: `IN_CTRLPKT` (`TRUE`)

| Comando | Acción | Sintaxis |
|---|---|---|
| `ethernettype` | set | `<profile_id> <ethernettype>` |
| `rfdb` | set | `<profile_id>` (MAC?) |
| `appProfile` | set | `<port_bitmap> <ethtype_profile_bitmap> <rfdb_profile_bitmap> <eapol_en> <pppoe_en> <igmp_en> <arp_request_en> <arp_response_en> <dhcp4_en> <dhcp6_en> <mld_en> <ip6ns_en> <ip6na_en> <ctrlpkt_profile_action> <sourceguard_bypass> <l2filter_bypass> <ingress_stp_bypass> <ingress_vlan_filter_bypass>` |

* Debería permitir forzar ciertos tipos de paquetes (ARP, DHCP, IGMP, etc.) a ser enviados a CPU (`ctrlpkt_profile_action = forward`?) o a ser procesados por hardware.
* Al habilitar `dhcp4_en`, `arp_request_en`, etc., se puede hacer que esos paquetes lleguen a CPU aunque estén offload.

Ejemplo para forzar ARP y DHCP a CPU en todos los puertos:

```bash
ssdk_sh ctrlpkt appProfile set 0xFFFFFFFF 0x0 0x0 yes yes yes yes yes yes yes yes yes yes forward no no no no
```

> **Nota:** El parámetro `ctrlpkt_profile_action` puede ser `forward`, `drop`, `cpycpu` o `rdtcpu`. `cpycpu` o `rdtcpu` envían una copia a CPU o redirigen a CPU.

---

### E. GRUPO `qm` (Queue Management)

> Dependencia: `IN_QM` (`TRUE`)

| Comando | Acción | Sintaxis |
|---|---|---|
| `ucastqbase` | set | `<queue_dst> <queue_base> <profile>` |
| `ucastpriclass` | set | `<priority> <class>` |
| `queue` | flush | - |
| `queue` | set | `<queue_id> <enable\|disable>` |
| `enqueue` | ctrl | (no documentado en ayuda) |

* `ucastqbase set` permite asociar un perfil de cola a un destino (puerto).
* Podría forzar que cierto tráfico vaya a CPU si se configura adecuadamente.
* Sintaxis real (de `fal_qm.c`):

  ```c
  fal_ucast_queue_base_profile_set(dev_id, queue_dest, queue_base, profile)
  ```

  - `queue_dest` es una estructura con tipo y valor (puerto, etc.).
  - En la shell, se espera que se pasen tres números: `<queue_dst> <queue_base> <profile>`. `queue_dst` probablemente es el ID del puerto o el tipo de destino.

* Comando `qm enqueue set` (no está documentado en help, pero está en el código) podría permitir activar/desactivar encolado, afectando el offload. Pendiente investigar más, ejemplo:

  ```bash
  ssdk_sh qm enqueue set <queue_id> enable
  ```

---

### F. GRUPO `servcode` (Service Codes) 

> Dependencia: `IN_SERVCODE` (`TRUE`)

| Comando | Acción | Sintaxis |
|---|---|---|
| `Config` | set | `<servcode_index>` |
| `Config` | get | `<servcode_index>` |
| `loopcheck` | set | `<enable\|disable>` |
| `portServcode` | set | `<port_id>` |

* `servcode loopcheck enable` puede activar la detección de bucles y posiblemente forzar que paquetes sospechosos vayan a CPU. No está claro cómo afecta al offload.
* `servcode Config set` permite configurar un código de servicio (parámetros internos). Se necesita investigar el significado de `servcode_index` (0-3?).

---

### G. GRUPO `nat` (NAT/NAPT)

> Dependencia: `IN_NAT` (`TRUE`)

| Comando | Acción | Sintaxis |
|---|---|---|
| `natstatus` | set | `<enable\|disable>` |
| `naptstatus` | set | `<enable\|disable>` |
| `natentry` | add | `""` (estructura) |
| `naptentry` | add | `""` (estructura) |

* Desactivar `natstatus` y `naptstatus` podría evitar que el hardware acelere NAT, forzando a CPU.

```bash
ssdk_sh nat natstatus set disable
ssdk_sh nat naptstatus set disable
```

---

### H. GRUPO `rate` (Rate Limiting)

* Hay que investigar más.

| Comando | Acción | Sintaxis |
|---|---|---|
| `ptIngress` | set | `<port_id> <speed:(kbps)> <enable\|disable>` |
| `ptEgress` | set | `<port_id> <speed:(kbps)> <enable\|disable>` |
| `stormCtrl` | set | `<port_id> <unicast\|multicast\|broadcast> <enable\|disable>` |

No parece afectar directamente al offload, pero puede usarse para limitar tráfico y forzar ciertos flujos a CPU si se aplican límites bajos.

---

### I. GRUPO `misc` (Misceláneos)

| Comando | Acción | Sintaxis |
|---|---|---|
| `ptUnkSaCmd` | set | `<port_id> <forward\|drop\|cpycpu\|rdtcpu>` |
| `ptUnkUcFilter` | set | `<port_id> <enable\|disable>` |
| `ptUnkMcFilter` | set | `<port_id> <enable\|disable>` |
| `ptBcFilter` | set | `<port_id> <enable\|disable>` |
| `cpuPort` | set | `<enable\|disable>` |
| `bcToCpu` | set | `<enable\|disable>` |

* **Posible comando útil?:** `ptUnkSaCmd` permite enviar paquetes con MAC origen desconocida a CPU (`rdtcpu`). Puede forzar aprendizaje.

```bash
ssdk_sh misc ptUnkSaCmd set 3 rdtcpu
```

---

### J. GRUPO `acl` (Access Control Lists) — **NO APLICABLE (en teoría) EN AX3600/`ipq807x`**

> Dependencia de .config
> -> Kernel modules
>   -> Network Devices
>       -> kmod-qca-nss-drv... Kernel driver for NSS (core driver) (PACKAGE_kmod-qca-nss-drv [=y])                - >           ** Configuration **                                                                                                       >                 Enable ACL


> Dependencia: `IN_ACL` (`TRUE` en config, pero no implementado para IPQ8074 en el driver del switch). Los comandos existen en la shell pero devuelven `This request is not support`.

- **NO APLICABLE en AX3600/`ipq807x` (en teoría)**:

    
* Hay que investigar más su código, implementación, cambiar símbolos para que acepte AX3600 y forzar testeo, etc.

---

### K. GRUPO `debug`

| Comando | Acción | Sintaxis |
|---|---|---|
| `reg` | get | `<reg_addr> <4>` |
| `reg` | set | `<reg_addr> <value> <4>` |
| `reg` | dump | `<group_id> <0-6>` |
| `field` | get | `<reg_addr> <offset> <len> <4>` |
| `phy` | get | `<ph_id> <reg_addr>` |
| `phy` | set | `<ph_id> <reg_addr> <value>` |

* Posiblemente útil para leer registros de configuración del switch y verificar el estado del offload.

* **Hay que investigar más**, mapas de memoria, registros, etc.

---

## 3. Dependencias de compilación y cómo activar funcionalidades no disponibles

### Macros en config (todas `TRUE`)

`IN_PORTCONTROL`, `IN_VLAN`, `IN_PORTVLAN`, `IN_FDB`, `IN_ACL`, `IN_QOS`, `IN_IGMP`, `IN_LEAKY`, `IN_MIRROR`, `IN_RATE`, `IN_SEC`, `IN_STP`, `IN_MIB`, `IN_LED`, `IN_COSMAP`, `IN_MISC`, `IN_IP`, `IN_FLOW`, `IN_NAT`, `IN_TRUNK`, `IN_INTERFACECONTROL`, `IN_VSI`, `IN_QM`, `IN_CTRLPKT`, `IN_SERVCODE`, `IN_RSS_HASH`, `IN_POLICER`, `IN_SHAPER`, `IN_BM`, `IN_PTP`, `IN_SFP`, `IN_VPORT`, `IN_TUNNEL`, `IN_VXLAN`, `IN_GENEVE`, `IN_MAPT`, `IN_TUNNEL_PROGRAM`, `IN_ATHTAG`, `IN_PKTEDIT`.

Todas están activadas, pero **ACL no funciona** porque el driver del switch (probablemente en `hsl` o `hppe`) no implementa las funciones para IPQ8074. Para activar ACL se necesitaría:

1. Verificar si el chip soporta ACL (probablemente no).
2. Si soporta, habilitar en el kernel la opción `CONFIG_NSS_ACL` o similar, y recompilar.

Para otros comandos no disponibles, la causa puede ser que el hardware no los soporte o que el firmware esté recortado.

### Recomendación para recompilar

- Revisar el `make menuconfig` de OpenWrt y buscar opciones relacionadas con `qca-ssdk`, `nss`, `acl`, `flow`, etc.
- Habilitar `CONFIG_NSS_OFFLOAD` y `CONFIG_NSS_DEBUG` para obtener más control.
- Añadir `-DIN_ACL` en los `CFLAGS` si no está, pero es inútil si el hardware no lo soporta.

---

## 4. Estrategia para capturar tráfico offload


### Recabar datos previos

#### COnfigs

```bash
# Leer registros actuales
flow mgmt get miss_action <port>
flow mgmt get bypass <port>
ip globalctrl get <port> ipv4_de_acce
ip globalctrl get <port> ipv6_de_acce

# Verificar estadísticas de flujos
flow stats get <port>
ip stats get <port>
qm stats get <queue>
```

#### Counters
```bash
# Verificar contadores NSS
cat /proc/sys/dev/nss/ipq8074/ipq8074/stats

# Verificar flujos en CPU
cat /proc/sys/net/nss/ipq8074/ipq8074_flow_mgr

# Verificar paquetes enviados a CPU
cat /sys/kernel/debug/nss/ipq8074/ipq8074/stats
```

---
### TEST 1

1.  **Desactivar el offload por puerto** usando `flow mgmt set`:

    * **MUY IMPORTANTE**
        - Conover A LA PERFECCIÓN el mapeo de puertos/interfaces internas

    ```bash
    ssdk_sh flow mgmt set 0 3 forward no no no 3   # LAN3 -> CPU
    ssdk_sh flow mgmt set 0 1 forward no no no 3   # LAN1 -> CPU
    ssdk_sh flow mgmt set 0 2 forward no no no 3   # LAN2 -> CPU
    ```

2.  **Desactivar el ruteo IP offload**:

    ```bash
    ssdk_sh ip routestatus set disable
    ```

3.  **Forzar ARP y DHCP a CPU**:

    ```bash
    ssdk_sh ctrlpkt appProfile set 0xFFFFFFFF 0x0 0x0 yes yes yes yes yes yes yes yes yes yes forward no no no no
    ```

4.  **Desactivar NAT offload**:

    ```bash
    ssdk_sh nat natstatus set disable
    ssdk_sh nat naptstatus set disable
    ```

5.  **Configurar mirroring** para capturar el tráfico que ahora pasa por CPU:

    * **MUY IMPORTANTE**
        1.  **testar con todas las interfaces deshabilitadas que vienen por defecto** (tun, gre, miireg, erspan,...)
        2.  **Testar sacando del bridge uno de los lans, a otro dispositivo, vía `tcpdump` en el lan destino**
        3.  **Testar con interfaces virtuales propia, con detalle en una buena config**
        

    ```bash
    ssdk_sh mirror analyPt set 1
    ssdk_sh mirror ptIngress set 3 enable
    ```
---

### TEST 2:  Bypass por stack IP
```
# 1. Desactivar aceleración IPv4/IPv6
ip globalctrl set all ipv4_de_acce 1
ip globalctrl set all ipv6_de_acce 1

# 2. Forzar todos los flujos a CPU
flow mgmt set miss_action all 3
flow mgmt set bypass all 1

# 3. Redirigir paquetes de control a CPU
ctrlpkt appProfile set all 2 0xFFFFFFFF

# 4. Desactivar hash lookup
flow mgmt set hash_en all 0

# 5. Redirigir colas a CPU (si está disponible)
qm queue set all enq_cpu 1
qm queue set all deq_cpu 1
```

---

### TEST 3:  Bypass selectivo (Por flow, stack IP, a nivel de aplicación)
```bash
# 1. Redirigir solo flujos específicos a CPU
flow mgmt set miss_action all 3  # Flujos no encontrados → CPU
flow mgmt set hash_en all 0      # Desactivar lookup de hash

# 2. Desactivar aceleración para IPv4 solo
ip globalctrl set all ipv4_de_acce 1

# 3. Redirigir protocolos específicos
ctrlpkt appProfile set ARP 2 0xFFFFFFFF
ctrlpkt appProfile set DHCP 2 0xFFFFFFFF
ctrlpkt appProfile set OSPF 2 0xFFFFFFFF
```

---

### TEST 4:  Por puerto
```bash
# 1. Configurar bypass por puerto
flow mgmt set bypass <port> 1

# 2. Desactivar ruteo para ese puerto
ip globalctrl set <port> ipv4_uc_route_en 0
ip globalctrl set <port> ipv6_uc_route_en 0

# 3. Redirigir todo el tráfico del puerto a CPU
flow mgmt set miss_action <port> 3
```

---



