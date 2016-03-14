include $(TOPDIR)/config.mk

src-y += AdRtbMain.cpp

static-lib-y	= $(target-y)engine.a
static-lib-y	+= $(target-y)filters.a
static-lib-y	+= $(target-y)libnet.so
static-lib-y	+= $(target-y)linear.a
static-lib-y	+= $(target-y)blas.a
static-lib-y	+= $(target-y)conhash.a
static-lib-y	+= $(target-y)control.a
static-lib-y	+= $(target-y)json.so
static-lib-y	+= $(target-y)util.so
static-lib-y	+= $(target-y)tlv.a

exec-bin = AdRtbEngine
include $(TOPDIR)/build.mk
