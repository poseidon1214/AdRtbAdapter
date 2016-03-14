include config.mk

ifneq ($(path),)
src-y += $(path)/
else
src-y += libconhash/
src-y += libjson/src/
src-y += libnet/
src-y += libtlv/src/
src-y += libutil/
src-y += libcommon/src/
src-y += control/src/
src-y += ibidding/
src-y += adapter/
src-y += adx/
src-y	+= AdRtbInfo/
src-y	+= AdRtbCopy/
src-y	+= AdExchange/
src-y	+= AdAdpLog/

endif
include build.mk
