CROSS	= 
CC	= $(CROSS)gcc
CXX	= $(CROSS)g++
LD	= $(CROSS)ld
AR	= $(CROSS)ar

cflags-y	= -Wall -g
#静态库的链接参数，推荐第三方库尽量使用静态链接，简化安装运行时所的依赖
#static-ld-y	+= -lstdc++
#共享库的链接参数
shared-ld-y	= -lrt -lm -lc -lpthread  -lstdc++
shared-ld-y     += -lhiredis -lz -ldl 
shared-ld-y     += -lxml2
shared-ld-y     += -Wl,-rpath-link,$(TOPDIR)/libextern/lib:$(TOPDIR)/bin
shared-ld-y     += -Wl,-rpath,$(TOPDIR)/libextern/lib:$(TOPDIR)/bin

cflags-y	+= -I$(TOPDIR)/AdExchange/inc/
cflags-y	+= -I$(TOPDIR)/AdExchange/ssp/inc/
cflags-y	+= -I$(TOPDIR)/libconhash/
cflags-y	+= -I$(TOPDIR)/libjson/inc/
cflags-y	+= -I$(TOPDIR)/libnet/inc/
cflags-y	+= -I$(TOPDIR)/libutil/inc/
cflags-y	+= -I$(TOPDIR)/control/inc/
cflags-y	+= -I$(TOPDIR)/libextern/inc/
#engine 
cflags-y	+= -I/usr/include/libxml2/

build-y           =$(TOPDIR)/build/
target-y        =$(TOPDIR)/bin/

#ALL: built-in.o
