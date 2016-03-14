include $(TOPDIR)/AdExchange/config.mk

src-y += AdAdx.cpp
src-y += AdAdxMain.cpp

static-lib-y	+= $(target-y)json.so
static-lib-y	+= $(target-y)conhash.a
static-lib-y	+= $(target-y)control.a
static-lib-y	+= $(target-y)libnet.so
static-lib-y	+= $(target-y)util.so
static-lib-y	+= $(target-y)ssp.a

exec-bin = AdRtbAdx
include $(TOPDIR)/build.mk
