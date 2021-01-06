include $(TOPDIR)/rules.mk

PKG_NAME:=iot-embeddedc
PKG_RELEASE:=1
PKG_VERSION:=1.0
PKG_SOURCE_PROTO=git
PKG_SOURCE_URL=https://github.com/ibm-watson-iot/iot-embeddedc
PKG_SOURCE_VERSION=1bbc817cb38db683d1d6995a718ad0f576734557

PKG_SOURCE_DIR:=$(HOME)/teltonika/openwrt-gpl-ipq40xx.Linux-x86_64/package/iot-embeddedc

include $(INCLUDE_DIR)/package.mk

define Package/iot-embeddedc
	CATEGORY:=Base system
	TITLE:=iot-embeddedc
endef

define Build/Patch
	patch -f $(PKG_BUILD_DIR)/CMakeLists.txt $(PKG_SOURCE_DIR)/patches/001-disabling_tests.patch
endef

define Build/Prepare
	$(PKG_UNPACK)
	$(Build/Patch)
	$(call Build/Prepare/Default)
	$(PKG_BUILD_DIR)/setup.sh
	cp -r $(PKG_SOURCE_DIR)/lib/* $(PKG_BUILD_DIR)/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/*.so $(STAGING_DIR)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/src/*.h $(STAGING_DIR)/usr/include/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/lib/*.h $(STAGING_DIR)/usr/include/
endef

define Package/iot-embeddedc/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/etc/watson/cert
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/*.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/IoTFoundation.pem $(1)/etc/watson/cert/
endef
$(eval $(call BuildPackage,iot-embeddedc))
