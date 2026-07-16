include $(TOPDIR)/rules.mk

PKG_NAME:=qca-ssdk-shell
PKG_RELEASE:=4

PKG_SOURCE_URL:=https://git.codelinaro.org/clo/qsdk/oss/ssdk-shell.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2024-12-17
PKG_SOURCE_VERSION:=a2b7be2
PKG_MIRROR_HASH:=7f546cc9db6627ee8c06db47c957c6a4452e72194b7af355e7f4e164d0848295
QSDK_VERSION:=12.5
PKG_VERSION:=$(QSDK_VERSION).$(subst -,.,$(PKG_SOURCE_DATE))~$(PKG_SOURCE_VERSION)

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define Package/qca-ssdk-shell
  SECTION:=QCA
  CATEGORY:=Utilities
  TITLE:=Shell application for QCA SSDK
  DEPENDS:=+kmod-qca-ssdk
endef

define Package/qca-ssdk-shell/Description
  This package contains a qca-ssdk shell application for QCA chipset
endef

ifndef CONFIG_TOOLCHAIN_BIN_PATH
CONFIG_TOOLCHAIN_BIN_PATH=$(TOOLCHAIN_DIR)/bin
endif

QCASSDK_CONFIG_OPTS+= TOOL_PATH=$(CONFIG_TOOLCHAIN_BIN_PATH) \
                SYS_PATH=$(LINUX_DIR) \
                TOOLPREFIX=$(TARGET_CROSS) \
                KVER=$(LINUX_VERSION) \
                CFLAGS="$(TARGET_CFLAGS)" \
                LDFLAGS="$(TARGET_LDFLAGS)" \
                ARCH=$(LINUX_KARCH)

define Build/Compile
	# PATCH 2026 DEPS DIRS MUY ESTRICTO, INICIO
	# 1. Pre-Crea dirs de salida para evitar errores de ruta en el Makefile de QCA
	mkdir -p $(PKG_BUILD_DIR)/build/linux/USLIB
	mkdir -p $(PKG_BUILD_DIR)/build/linux/SHELL

	# 2. Genera files .d vacios para no error con check deps de Qualcomm
	find $(PKG_BUILD_DIR)/src -name "*.c" | sed 's/.*\/\(.*\)\.c/\1.d/' | \
		xargs -I {} touch $(PKG_BUILD_DIR)/build/linux/USLIB/{} $(PKG_BUILD_DIR)/build/linux/SHELL/{} || true

	# 3. Invoca al MAKE original forzado -j1 para evitar colisiones de files .d
	$(MAKE) -C $(PKG_BUILD_DIR) \
		-j1 \
		$(strip $(QCASSDK_CONFIG_OPTS))
endef

define Package/qca-ssdk-shell/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/bin/ssdk_sh $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,qca-ssdk-shell))
