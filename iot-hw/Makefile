include $(TOPDIR)/rules.mk

PKG_NAME:=iot-hw
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

PKG_SOURCE_DIR:=$(HOME)/teltonika/openwrt-gpl-ipq40xx.Linux-x86_64/package/iot-hw

include $(INCLUDE_DIR)/package.mk

define Package/iot-hw
	CATEGORY:=Base system
	TITLE:=iot-hw
	DEPENDS:=+iot-embeddedc +uci +ubus +ubox +libblobmsg-json
endef

define Package/iot-hw/description
	This is iot-embeddedc simple program
endef

define Package/iot-hw/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iot-hw $(1)/usr/bin
	$(INSTALL_BIN) ./files/iot-hw.init $(1)/etc/init.d/iot-hw
	$(INSTALL_CONF) ./files/iot-hw.config $(1)/etc/config/iot-hw
endef

$(eval $(call BuildPackage,iot-hw))
