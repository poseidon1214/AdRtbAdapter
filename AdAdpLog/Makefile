include $(TOPDIR)/config.mk

src-y += AdAdpLog.cpp

static-lib-y    += $(target-y)libcommon.so
static-lib-y	+= $(target-y)util.so
static-lib-y	+= $(target-y)tlv.a
static-lib-y	+= $(target-y)control.a
static-lib-y	+= $(target-y)libnet.so

exec-bin = AdAdpLog
include $(TOPDIR)/build.mk
