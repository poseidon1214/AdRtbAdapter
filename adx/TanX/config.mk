CROSS	= 
CC	= $(CROSS)gcc
CXX	= $(CROSS)g++
LD	= $(CROSS)ld
AR	= $(CROSS)ar

cflags-y	= -Wall -g
#静态库的链接参数，推荐第三方库尽量使用静态链接，简化安装运行时所的依赖
#static-ld-y	+= -lstdc++
#共享库的链接参数
shared-ld-y     = -ldl  -lprotobuf

cflags-y	+= -I$(TOPDIR)/inc/
cflags-y	+= -I$(TOPDIR)/libtlv/inc/
cflags-y	+= -I$(TOPDIR)/libutil/inc/
#engine 

build-y           =$(TOPDIR)/build/
target-y        =$(TOPDIR)/bin/

#ALL: built-in.o
