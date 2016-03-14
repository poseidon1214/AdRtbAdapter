include $(TOPDIR)/config.mk

src-y += AdMain.cpp
src-y += AdAdapter.cpp
src-y += AdAdapterMethod.cpp
src-y += AdAdpFilterBan.cpp
src-y += AdConHash.cpp

static-lib-y	= $(target-y)engine.a
static-lib-y	+= $(target-y)filters.a
static-lib-y	+= $(target-y)linear.a
static-lib-y	+= $(target-y)blas.a
static-lib-y	+= $(target-y)json.so
static-lib-y	+= $(target-y)conhash.a
static-lib-y	+= $(target-y)control.a
static-lib-y	+= $(target-y)libnet.so
static-lib-y	+= $(target-y)util.so
static-lib-y	+= $(target-y)libcommon.so
static-lib-y	+= $(target-y)tlv.a

exec-bin = AdRtbAdapter
include $(TOPDIR)/build.mk
