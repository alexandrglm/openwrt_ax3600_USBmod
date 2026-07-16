
## PAQUETES PROHIBIDOS DESDE SOUYRCES

- xdp-tools, libxdp
- scapy
- nada que tenga python, no compil python desde sources, generalmente da problemas de paths largos
- i2c-tools, e i2c NADA (kmod's sí)
- eBPF, BPF, ... en principio va a romper todo
- mc ... JAMAS!
- mwan3 ... Cuidado, rompe rutas ipv6



---

# 📝 NOTAS DE COMPILACIÓN - RELEASE 20260422

**Fork:** AgustinLorenzo/openwrt (AX3600 con NSS)

---

## 🎯 OBJETIVO DE ESTA RELEASE

Generar un firmware personalizado para Xiaomi AX3600 que:

* Incluya todos los paquetes necesarios de serie (sin instalarlos después)
* Tenga las URLs de los repositorios fijas al repositorio del tío que funciona (20260303)
* Incluya la clave pública correcta (public-key.pem) en `/etc/apk/keys/`
* Incorpore todas las configuraciones personalizadas (firewall, red, usuarios, scripts)

---

## 📂 CAMBIOS REALIZADOS EN EL REPOSITORIO

### 0. ARREGLAR QCA-SSDK-SHELL
### 🛠 Solución: qca-ssdk-shell (Error de dependencias .d)

A diferencia de releases anteriores donde se desactivaba por falta de sources, en esta compilación **se ha logrado habilitar** el paquete `qca-ssdk-shell` (v12.5.2024.12.17). Sin embargo, el SDK de Qualcomm presenta un bug en su sistema de construcción (Makefile) que impide la compilación limpia en un solo paso.

#### El Problema
El compilador falla con el error `No rule to make target ... /build/linux/USLIB/xxx.d`. 
Esto ocurre porque el Makefile intenta verificar la existencia de archivos de dependencia (`.d`) antes de invocar a GCC para generarlos, entrando en un bucle lógico que detiene el `make`.

#### Solución aplicada (Workaround de Pre-poblado)
Para "engañar" al sistema de construcción y permitir que el compilador real tome el control, se debe ejecutar un pre-poblado manual de la estructura de directorios de construcción una vez que el código fuente ha sido parcheado por OpenWrt:

1. **Entrar al directorio de construcción del paquete:**
   ```bash
   cd build_dir/target-aarch64_cortex-a53_musl/linux-qualcommax_ipq807x/qca-ssdk-shell-12.5.2024.12.17~a2b7be2
   
2. Generar estructura y archivos de dependencia fantasma:
  ```bash
  mkdir -p build/linux/USLIB build/linux/SHELL
  # Crear archivos .d vacíos basados en los fuentes .c existentes
  find src -name "*.c" | sed 's/.*\/\(.*\)\.c/\1.d/' | xargs -I {} sh -c 'touch build/linux/USLIB/{} build/linux/SHELL/{}'
  ```
3. Continuar compilación:
  Al relanzar el make, el sistema encuentra los archivos .d (aunque vacíos), permite que GCC se ejecute, y es el propio GCC quien sobrescribe estos archivos con las dependencias reales.
  ```bash
  cd ../../../../../
  make package/feeds/nss_packages/qca-ssdk-shell/compile V=s -j1
  ```

4. Continuar con la compilación general del firmware:


### 1. AÑADIR PAQUETES DE SERIE AL FIRMWARE

**Archivo modificado:**
`target/linux/qualcommax/image/ipq807x.mk`

**Cambio:**
Dentro del bloque `define Device/xiaomi_ax3600`, se ha modificado la línea `DEVICE_PACKAGES` para incluir todos los paquetes manuales que antes se instalaban después.

**Resultado:**
Estos paquetes ya vienen instalados de fábrica en el firmware:

```makefile
DEVICE_PACKAGES := ipq-wifi-xiaomi_ax3600 kmod-ath10k-smallbuffers ath10k-firmware-qca9887 \
    usbutils pciutils tcpdump htop irssi owut ncat uuidgen hping3 nmap-full nmap-ssl \
    nmap iftop whois natmap bmon chattr sudo shadow-common shadow-useradd adb
```

**Nota:**
`nmap` y `nmap-ssl` están ambos.
`shadow-common` y `shadow-useradd` son necesarios para que funcione `sudo`.

---

### 2. FIJAR LA URL DEL REPOSITORIO DE PAQUETES (FEEDS)

**Problema original:**
La URL del repositorio cambiaba cada vez que se compilaba porque usaba:

```bash
$(shell date +"%Y%m%d")
```

Además, los repositorios nuevos del tío no tenían la clave pública correcta.

**Archivo modificado:**
`include/feeds.mk`

**Cambio:**
Se ha modificado la función `FeedSourcesAppendAPK` para que la URL apunte a una fecha fija (20260303).

**Antes:**

```bash
echo 'https://openwrtdata.agustinls.com/nss-wifi-unified/$(shell date +"%Y%m%d")/packages/packages.adb'; \
```

**Después:**

```bash
echo 'https://openwrtdata.agustinls.com/nss-wifi-unified/20260303/packages/packages.adb'; \
```

También se ha modificado `FeedSourcesAppendOPKG`:

```bash
echo 'src/gz %d_core https://openwrtdata.agustinls.com/nss-wifi-unified/20260303/packages'; \
```

**Resultado:**
El archivo `/etc/apk/repositories.d/distfeeds.list` tendrá la URL fija a `20260303`.

---

### 3. INCLUIR LA CLAVE PÚBLICA CORRECTA EN EL FIRMWARE

**Problema original:**
Se generaba una clave automática (`key-build.pub`) que no coincidía con la del repositorio.

**Solución:**
Usar `files/` para forzar la clave correcta.

**Archivo añadido:**

```
files/etc/apk/keys/public-key.pem
```

**Origen:**
Extraída de un router funcional:

```bash
scp root@ZTE:/etc/apk/keys/public-key.pem
```

**Mecanismo:**
El `Makefile` de `base-files` copia `files/` al root final:

```bash
$(CP) ./files/* $(1)/
```

**Resultado:**
El firmware incluye la clave correcta desde el inicio.

---

### 4. CONFIGURACIONES PERSONALIZADAS (VIA CARPETA files/)

**Carpeta:**
`files/`

**Archivos incluidos:**

| Ruta en files/                             | Propósito                     |
| ------------------------------------------ | ----------------------------- |
| etc/config/                                | Configuraciones UCI completas |
| etc/firewall.d/wan_ttl_with_nss            | Script TTL (desactivado)      |
| etc/firewall.d/wan_ttl_no_nss              | Alternativo sin NSS           |
| etc/dropbear/                              | Claves SSH                    |
| etc/passwd, shadow, group                  | Usuarios                      |
| etc/sudoers                                | Permisos sudo                 |
| etc/rc.local                               | Inicio personalizado          |
| etc/sysctl.conf                            | Parámetros kernel             |
| etc/profile.d/*.sh                         | Scripts de perfil             |
| etc/apk/keys/public-key.pem                | Clave repo                    |
| etc/uci-defaults/10_disable_services       | Desactiva servicios           |
| etc/modules.conf                           | Módulos kernel                |
| etc/sysupgrade.conf                        | Persistencia                  |
| etc/nftables.d/10-custom-filter-chains.nft | Reglas nftables               |
| etc/owut.d/pre-install.sh                  | Script OWUT                   |

**Permisos:**

* Directorios: `755`
* Configuración: `644`
* Sensibles: `600`
* Scripts: `755`
* sudoers: `440`

---

## 🔧 FLUJO DE COMPILACIÓN PARA FUTURAS RELEASES

```bash
# 1. Limpiar (opcional)
make dirclean

# 2. Feeds
./scripts/feeds update -a
./scripts/feeds install -a

# 3. Configuración
make menuconfig

# Target System -> Qualcomm IPQ807x
# Subtarget -> IPQ807x
# Profile -> Xiaomi AX3600

# 4. Descargar
make download -j$(nproc)

# 5. Compilar
make -j$(nproc)
```

---

## 📁 UBICACIÓN DEL FIRMWARE

```bash
bin/targets/qualcommax/ipq807x/openwrt-qualcommax-ipq8077-xiaomi_ax3600-squashfs-sysupgrade.bin
```

---

## ⚠️ PUNTOS CLAVE

| Tema          | Qué se hizo     | Por qué              |
| ------------- | --------------- | -------------------- |
| Paquetes      | DEVICE_PACKAGES | Preinstalados        |
| URL repo      | Fijada 20260303 | Compatibilidad clave |
| Clave pública | files/          | Confianza repo       |
| Configuración | files/          | Boot listo           |

---

## 🚨 SI EL TÍO ARREGLA SU REPOSITORIO

Si publica uno nuevo (ej: `20260501`):

* Actualizar URL en `include/feeds.mk`
* Actualizar `public-key.pem`

---

## ✅ ESTADO ACTUAL DEL FORK

* Paquetes de serie añadidos
* URL fijada a 20260303
* Clave pública incluida
* Configuraciones completas
* Permisos verificados
* Scripts TTL incluidos (comentados)

---

**TODO LISTO PARA COMPILAR**



20260421:   Upgraded to @AgustinLorenzo 20260407 rel

#### Lista de verificaciones posteriores a instalar firmware:

!!! VERIFICAR ANTES DE INSTALAR/ACTUALIZAR!!!
```
apk add \
  bmon etherwake hping3 iftop irssi ncat nmap-full nmap-ssl tcpdump curl wget-ssl whois \
  adb attendedsysupgrade-common chattr htop owut pciutils shadow-useradd sudo ttyd usbutils \
  ddns-scripts ddns-scripts-services luci-app-ddns luci-app-firewall luci-app-package-manager luci-app-ttyd luci-app-upnp luci-app-wifischedule luci-app-wol miniupnpd-nftables natmap wifischedule \
  luci luci-base luci-light luci-lua-runtime luci-mod-admin-full luci-mod-network luci-mod-status luci-mod-system luci-mod-rpc luci-lib-base luci-lib-ip luci-lib-json luci-lib-jsonc luci-lib-nixio luci-lib-uqr luci-proto-gre luci-proto-ipv6 luci-proto-ppp luci-proto-vxlan luci-proto-wireguard luci-ssl-openssl luci-theme-bootstrap \
  luci-i18n-base-es luci-i18n-ddns-es luci-i18n-firewall-es luci-i18n-package-manager-es luci-i18n-ttyd-es luci-i18n-upnp-es luci-i18n-wifischedule-es luci-i18n-wol-es \
  ethtool iw jshn jsonfilter qrencode resolveip ubi-utils uboot-envtools \
  iptables-mod-account iptables-mod-asn iptables-mod-chaos iptables-mod-condition iptables-mod-delude iptables-mod-dhcpmac iptables-mod-dnetmap iptables-mod-fuzzy iptables-mod-geoip iptables-mod-iface iptables-mod-ipmark iptables-mod-ipp2p iptables-mod-ipv4options iptables-mod-length2 iptables-mod-logmark iptables-mod-lscan iptables-mod-lua iptables-mod-proto iptables-mod-psd iptables-mod-quota2 iptables-mod-sysrq iptables-mod-tarpit iptables-mod-rtpengine
```

1. 
sudo shadow-useradd chattr openssh-client openssh-client-utils openssh-keygen openssl-util htop owut usbutils pciutils ttyd

2. NET
bmon etherwake hping3 iftop tcpdump nmap-full ncat curl wget-ssl whois irssi wireguard-tools

3. LUCI FIULL
luci luci-ssl-openssl luci-base luci-light luci-lua-runtime luci-mod-admin-full luci-mod-network luci-mod-status luci-mod-system luci-mod-rpc luci-lib-base luci-lib-ip luci-lib-jsonc luci-lib-nixio luci-lib-uqr luci-theme-bootstrap luci-proto-gre luci-proto-ipv6 luci-proto-ppp luci-proto-vxlan luci-proto-wireguard

4. LUCI APPS
luci-app-ddns luci-app-firewall luci-app-package-manager luci-app-ttyd luci-app-upnp luci-app-wifischedule luci-app-wol

5. LUCI TRAD
luci-i18n-base-es luci-i18n-ddns-es luci-i18n-firewall-es luci-i18n-package-manager-es luci-i18n-ttyd-es luci-i18n-upnp-es luci-i18n-wifischedule-es luci-i18n-wol-es

6.  NET SERVICES
ddns-scripts ddns-scripts-services miniupnpd-nftables natmap

7.  OTROS
qrencode ethtool iw


2. Tema sudoers, users nuevos, etc:
```bash
# Crear el grupo sudo (si no existe)
groupadd -g 100 sudo 2>/dev/null || true

# Añadir tu usuario al grupo sudo
usermod -a -G sudo root  # o tu usuario si no usas root

# Configurar sudoers (opcional: permitir al grupo sudo sin contraseña)
echo "%sudo ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
```


---



1. ~~`qca-sdk-shell` off  ~~ ARREGLADO
Due to lack of qca sources (even qosmio, and qualcomm in codemanjaro have them), and proved qca-sdk-* packets aren't mandatory for normal-nodebug usage; otherwise, no compile, Error 2.


2. `kmod-usb-*` and related  
For the core purpose of this fork, enabling USB core AFTER device tree modifications (See DT_changes.mod)

[2026/03/04]
3. `iptables-*'`, `iptables-nft`, `conntrack` and related  
For personal research/config purposes

4. Netfilters ALL
#
# Netfilter Extensions
#
CONFIG_PACKAGE_kmod-arptables=y
CONFIG_PACKAGE_kmod-br-netfilter=y
CONFIG_PACKAGE_kmod-ebtables=m
CONFIG_PACKAGE_kmod-ebtables-ipv4=m
CONFIG_PACKAGE_kmod-ebtables-ipv6=m
CONFIG_PACKAGE_kmod-ebtables-watchers=m
CONFIG_PACKAGE_kmod-ip6tables=y
CONFIG_PACKAGE_kmod-ip6tables-extra=y
CONFIG_PACKAGE_kmod-ipt-account=y
CONFIG_PACKAGE_kmod-ipt-asn=y
CONFIG_PACKAGE_kmod-ipt-chaos=y
CONFIG_PACKAGE_kmod-ipt-checksum=y
CONFIG_PACKAGE_kmod-ipt-cluster=y
CONFIG_PACKAGE_kmod-ipt-compat-xtables=y
CONFIG_PACKAGE_kmod-ipt-condition=y
CONFIG_PACKAGE_kmod-ipt-conntrack=y
CONFIG_PACKAGE_kmod-ipt-conntrack-extra=y
CONFIG_PACKAGE_kmod-ipt-conntrack-label=y
CONFIG_PACKAGE_kmod-ipt-coova=y
CONFIG_PACKAGE_kmod-ipt-core=y
CONFIG_PACKAGE_kmod-ipt-debug=y
CONFIG_PACKAGE_kmod-ipt-delude=y
CONFIG_PACKAGE_kmod-ipt-dhcpmac=y
CONFIG_PACKAGE_kmod-ipt-dnetmap=y
CONFIG_PACKAGE_kmod-ipt-extra=y
CONFIG_PACKAGE_kmod-ipt-filter=y
CONFIG_PACKAGE_kmod-ipt-fuzzy=m
CONFIG_PACKAGE_kmod-ipt-geoip=m
CONFIG_PACKAGE_kmod-ipt-hashlimit=m
CONFIG_PACKAGE_kmod-ipt-iface=m
CONFIG_PACKAGE_kmod-ipt-ipmark=y
CONFIG_PACKAGE_kmod-ipt-ipopt=y
CONFIG_PACKAGE_kmod-ipt-ipp2p=y
CONFIG_PACKAGE_kmod-ipt-iprange=m
CONFIG_PACKAGE_kmod-ipt-ipsec=m
CONFIG_PACKAGE_kmod-ipt-ipset=m
CONFIG_PACKAGE_kmod-ipt-ipv4options=m
CONFIG_PACKAGE_kmod-ipt-led=m
CONFIG_PACKAGE_kmod-ipt-length2=y
CONFIG_PACKAGE_kmod-ipt-logmark=y
CONFIG_PACKAGE_kmod-ipt-lscan=m
CONFIG_PACKAGE_kmod-ipt-lua=m
CONFIG_PACKAGE_kmod-ipt-nat=y
CONFIG_PACKAGE_kmod-ipt-nat-extra=y
CONFIG_PACKAGE_kmod-ipt-nat6=y
CONFIG_PACKAGE_kmod-ipt-nflog=y
CONFIG_PACKAGE_kmod-ipt-nfqueue=y
CONFIG_PACKAGE_kmod-ipt-offload=y
CONFIG_PACKAGE_kmod-ipt-physdev=y
CONFIG_PACKAGE_kmod-ipt-proto=y
CONFIG_PACKAGE_kmod-ipt-psd=y
CONFIG_PACKAGE_kmod-ipt-quota2=y
CONFIG_PACKAGE_kmod-ipt-raw=y
CONFIG_PACKAGE_kmod-ipt-raw6=y
CONFIG_PACKAGE_kmod-ipt-rpfilter=y
CONFIG_PACKAGE_kmod-ipt-rtpengine=y
CONFIG_PACKAGE_kmod-ipt-socket=y
CONFIG_PACKAGE_kmod-ipt-sysrq=y
CONFIG_PACKAGE_kmod-ipt-tarpit=y
CONFIG_PACKAGE_kmod-ipt-tee=y
CONFIG_PACKAGE_kmod-ipt-tproxy=y
CONFIG_PACKAGE_kmod-ipt-u32=y
CONFIG_PACKAGE_kmod-netatop=y
CONFIG_PACKAGE_kmod-nf-conncount=y
CONFIG_PACKAGE_kmod-nf-conntrack=y
CONFIG_PACKAGE_kmod-nf-conntrack-netlink=y
CONFIG_PACKAGE_kmod-nf-dup-inet=y
CONFIG_PACKAGE_kmod-nf-flow=y
CONFIG_PACKAGE_kmod-nf-ipt=y
CONFIG_PACKAGE_kmod-nf-ipt6=y
CONFIG_PACKAGE_kmod-nf-ipvs=y
CONFIG_PACKAGE_kmod-nf-ipvs-ftp=y
CONFIG_PACKAGE_kmod-nf-ipvs-sip=y
CONFIG_PACKAGE_kmod-nf-log=y
CONFIG_PACKAGE_kmod-nf-log6=y
CONFIG_PACKAGE_kmod-nf-nat=y
CONFIG_PACKAGE_kmod-nf-nat6=y
CONFIG_PACKAGE_kmod-nf-nathelper=y
CONFIG_PACKAGE_kmod-nf-nathelper-extra=y
CONFIG_PACKAGE_kmod-nf-nathelper-rtsp=y
CONFIG_PACKAGE_kmod-nf-reject=y
CONFIG_PACKAGE_kmod-nf-reject6=y
CONFIG_PACKAGE_kmod-nf-socket=y
CONFIG_PACKAGE_kmod-nf-tproxy=y
CONFIG_PACKAGE_kmod-nfnetlink=y
CONFIG_PACKAGE_kmod-nfnetlink-cthelper=y
CONFIG_PACKAGE_kmod-nfnetlink-cttimeout=y
CONFIG_PACKAGE_kmod-nfnetlink-log=y
CONFIG_PACKAGE_kmod-nfnetlink-queue=y
CONFIG_PACKAGE_kmod-nft-arp=y
CONFIG_PACKAGE_kmod-nft-bridge=y
CONFIG_PACKAGE_kmod-nft-compat=y
CONFIG_PACKAGE_kmod-nft-connlimit=y
CONFIG_PACKAGE_kmod-nft-core=y
CONFIG_PACKAGE_kmod-nft-dup-inet=y
CONFIG_PACKAGE_kmod-nft-fib=y
CONFIG_PACKAGE_kmod-nft-nat=y
CONFIG_PACKAGE_kmod-nft-netdev=y
CONFIG_PACKAGE_kmod-nft-offload=y
CONFIG_PACKAGE_kmod-nft-queue=y
CONFIG_PACKAGE_kmod-nft-socket=y
CONFIG_PACKAGE_kmod-nft-tproxy=y
CONFIG_PACKAGE_kmod-nft-xfrm=y
# end of Netfilter Extensions


[2026/03/05]
5. Upgraded to [@AgustinLorenzo 20260303 NSS UNIFIED](https://github.com/AgustinLorenzo/openwrt/commits/ipq807x-nsswifi-unified-2026-03-03-2055) sources release

---

#### FAILURE POINTS, Prior stepts to fully-compile:

Es muy probable que la compilación en un solo paso no sea posible. Por ello hay que establecer los pasos generales.

1. Un `make` principal compilará primero todo el llvm, toolchains, sus propias herramientas, etc.Es la parte más tediosa (40 minutos en un i5-12450H DDR4 32Gb)
2. Después debería ponerse manos a la obra con el kernel.
3. Después empieza con packages; fallará por múltiples causas:

    No encuentro una forma más limpia que esperar al fallo intencional, que lo lanzará muy probablemente `xdp-tools` (debido a `bpf-headers` y a como clang intenta leer los headers del kernel del SISTEMA, no los cross-compile).


- `xdp-tools` must be disabled

Por diseño, de hecho, `xdp-tools` no debería ser utilizado en estas builds con NSS. ¿Quién o qué añaden xdp-tools y librerías en el kernel? Muy posiblemente al tratar de habilitar iptables-nft o alguno de los binarios ipt.
SOLUCIÓN:   Eliminar xdp-tools y relacionados en menu config.

- ~~`qca-ssdk-shell` disabled. Sources still not available.~~ ARREGLADO
Exactamente lo mismo que con xdp-tools, pero por motivos distintos (No existen las sources para este paquete, y tampoco es realmente necesario para el uso de NSS excepto para edbug. Pendiente contactar con los desarrolladores sobre la manera en que usan las sources en las compils.)


---

##### Enabling USB

- Ax3600 Device Tree overrides:
- `&qusb_phy_0`, `ssphy_0` and `usb_0` overrides in`ipq8071-ax3600.dts` *need to be added* ***AFTER*** *the `pcie*` *callbacks*. Otherwise, when booting with WLAN's dectivated, usb lanes obtain priority resulting in WLAN's that cannot be further activated. 

- Minimal USB core kmods updated: `kmod-usb-core`, `kmod-usb-dwc2`, `kmod-usb-dwc3`, **`kmod-usb-dwc3-qcom`**, `kmod-usb2`, `kmod-usb3`, `kmod-usbmon`



##### Net Packages, kmdos, to enable FULL post-routing rules using NSS:

- KMOD NetFilter extensions: `kmod-ipt-core`, `kmod-iptables`, `kmod-ip6tables`, `kmod-iptables-extra`, `kmod-ip6tables-extra`, `kmod-ipt-nat`, `kmod-ipt-nat-extra`, `kmod-ipt-nat6`, `kmod-ipt-u32`, `kmod-ipt-extra`, `kmod-nf-ipt`, ...

- PACKAGES IPTABLES/NFT: `iptables-nft`, `ip6tables-nft` `iptables-mod-u32`, ...

**Incluso así, NSS sigue modificando todos los TTL (Restando -1), por lo que hay varias formas para "restaurar" el TTL natural (si fuese necesario):

1. Planchado de todos los TTL's a un valor específico + Ajustar con `iptables-nft` --ttl-set sumando +1 al valor del planchado

2. Sin plachado de TTL (No testado) es más complejo (Primero, leer valor ttl del flujo, después editarlo sumando 1). No tiene mucho sentido hacer esto sin planchado previo (Si no necesitas spoofear todos los TTL al mismo valor siempre, ¿Entonces por qué debería preocupar que NSS reste un hop?).

Con esto, realmente no hay parámetro o regla que NSS no vaya a respetar después. Sin iptables-nft, NSS no respeta las reglas. Testado.


##### `tcpdump` issues
Existe desincronización en el repo oficial de OpenWRT (O en el de @AgustinLorenzo) con tcpdump no actualizado que usa símbolos que ya no existen en libpcap1 maś reciente.
Es un error recurrente con `tcpdump`, aunque temporal-estacionario (se soluciona cada vez que los devs de tcpdump se actualizan a las últimas libs).

La mejor solución, compilar libpcap1 y tcpdump directamente desde nuestras sources y añadirlos como paquetes/librería, o compilarlos y SCP/apk add --allow-untrusted después.



