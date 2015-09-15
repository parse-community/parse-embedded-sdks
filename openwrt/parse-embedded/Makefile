# Made available under the Creative Commons CC0 1.0 Universal Dedication (the "Dedication").
# You may obtain a copy of the Dedication at https://creativecommons.org/publicdomain/zero/1.0/.
#
# Unless required by applicable law or agreed to in writing, software distributed under
# the Dedication is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
# ANY KIND, either express or implied. See the Dedication for the specific language governing
# permissions and limitations under the Dedication.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=parse-embedded
PKG_VERSION:=1.0.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-sdks-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://github.com/ParsePlatform/parse-embedded-sdks.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-sdks-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_VERSION)
PKG_SOURCE_PROTO:=git

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-sdks-$(PKG_VERSION)

PKG_FIXUP:=autoreconf
PKG_FIXUP:=patch-libtool
PKG_FIXUP:=gettext-version

include $(INCLUDE_DIR)/package.mk

define Package/parse-embedded
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS+= +libcurl +libuuid +libc
  TITLE:=Library implementing the Parse interface
  URL:=https://www.parse.com/
endef

define Package/parse-embedded/description
  Embedded C Parse Library
endef

define Package/parse-embedded-yun
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS+= +parse-embedded
  TITLE:=Library implementing the Parse interface
  URL:=https://www.parse.com/
endef

define Package/parse-embedded-yun/description
  Embedded C Parse support for Arduino Yun bridge
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	( cd $(PKG_BUILD_DIR) ; \
		[ -f ./configure ] || { \
			autoreconf -fi ; \
		} \
	)
endef

define Build/Configure
  $(call Build/Configure/Default,--enable-yun=yes)
endef

define Build/InstallDev
	$(INSTALL_DIR)  $(STAGING_DIR)/usr/include
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/include/parse.h $(STAGING_DIR)/usr/include/
	$(INSTALL_DIR)  $(STAGING_DIR)/usr/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/unix/src/.libs/libparse.so* $(STAGING_DIR)/usr/lib/
endef

define Package/parse-embedded/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/unix/src/.libs/libparse.so* $(1)/usr/lib/
	$(INSTALL_DIR)  $(STAGING_DIR)/usr/include
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/include/parse.h $(STAGING_DIR)/usr/include/
	$(INSTALL_DIR)  $(STAGING_DIR)/usr/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/unix/src/.libs/libparse.so* $(STAGING_DIR)/usr/lib/
endef

define Package/parse-embedded-yun/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/unix/yun/.libs/parse_push $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/unix/yun/.libs/parse_request $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/ssl/certs
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/unix/yun/certs/* $(1)/etc/ssl/certs
endef

$(eval $(call BuildPackage,parse-embedded))
$(eval $(call BuildPackage,parse-embedded-yun))
