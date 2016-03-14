ifeq ($(TOPDIR),)
TOPDIR	= $(shell pwd)
PROJECT	= $(shell pwd | awk -F'/' '{print $$NF}')
endif

Q = @
FLAG = -rR
SPACE   = "        "
ECHO	= echo $(SPACE)
MAKE	= make
CFLAGS  += $(cflags-y)
LDFLAGS  += $(ldflags-y)
LDFLAGS  += -Wl,-Bstatic $(static-ld-y) -Wl,-Bdynamic $(shared-ld-y) -Wl,--as-needed


ifneq ($(build-y),)
OBJPATH   =$(build-y)
endif
#### Target File ####
ifneq ($(target-y),)
TARGETPATH   =$(target-y)
endif
#### EXECUTABLE BIN ####
ifneq ($(exec-bin),)
EXECUTABLE	=$(TARGETPATH)$(exec-bin)
endif
#### SHARED LIB ####
ifneq ($(lib-m),)
SHAREDLIB	=$(TARGETPATH)$(lib-m)
SHAREDFLAGS	=-fPIC -shared
endif
####STATIC LIB ####
ifneq ($(lib-y),)
STATICLIB	=$(TARGETPATH)$(lib-y)
AR_OPTS		=rcs
endif
#### STATIC OBJS ####
ifneq ($(static-lib-y),)
STATICOBJ   =$(static-lib-y)
endif
#### OBJS ####
ifneq ($(src-y),)
SOURCES	= $(filter %.c,$(src-y))
SOURCES_CPP = $(filter %.cpp,$(src-y))
OBJS	= $(SOURCES:%.c=$(OBJPATH)%.o)
OBJS	+= $(SOURCES_CPP:%.cpp=$(OBJPATH)%.o)
DEPEND	= $(OBJS:$(OBJPATH)%.o=$(OBJPATH)%.d)
SUBDIR  = $(filter %/, $(src-y))
SUBOBJS = $(addsuffix built-in.o, $(SUBDIR))
MODULE  = $(filter %/, $(module-y))
endif


ALL: $(OBJS) $(SUBOBJS) $(MODULE)
ALL: $(EXECUTABLE)
ALL: $(SHAREDLIB) $(STATICLIB)

default:
	$(Q) $(AR) rcs built-in.o

$(OBJPATH)%.d : %.cpp
	$(Q)set -e; rm -f $@; \
		$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
	$(Q) $(ECHO) CXX $@

$(OBJPATH)%.o : %.c
	$(Q) $(CC) -c $(CFLAGS) $(SHAREDFLAGS) $< -o $@
	$(Q) $(ECHO) CC $@

$(OBJPATH)%.o : %.cpp
	$(Q) $(CXX) -c $(CFLAGS) $(SHAREDFLAGS)  $< -o $@
	$(Q) $(ECHO) CXX $@

$(EXECUTABLE):$(OBJS) $(SUBOBJS) $(STATICOBJ)
	$(Q) $(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	$(Q) $(ECHO) Create $@

$(SHAREDLIB):$(OBJS) $(SUBOBJS) $(STATICOBJ)
	$(Q) $(CC) $(CFLAGS) $(SHAREDFLAGS) -o $@ $^ $(LDFLAGS)
	$(Q) $(ECHO) CC $@

$(STATICLIB):$(OBJS) $(STATICOBJ)
	$(Q) $(AR) $(AR_OPTS) $@ $^
	$(Q) $(ECHO) AR $@

$(SUBOBJS):FORCE
	$(Q) $(MAKE) -C $(dir $@) TOPDIR=$(TOPDIR) $(FLAG)

$(MODULE):FORCE
	$(Q) $(MAKE) -C $(dir $@) $(FLAG)

test:ALL
	$(Q) for i in $(SUBDIR); do $(MAKE) -C $$i TOPDIR=$(TOPDIR) TEST=true $(FLAG); done

clean-module:FORCE
	$(Q) for i in $(MODULE); do $(MAKE) clean -C $$i $(FLAG); done

clean-subdir:FORCE
	$(Q) for i in $(SUBDIR); do $(MAKE) clean -C $$i TOPDIR=$(TOPDIR) TEST=true $(FLAG); done

built-in.o:$(OBJS) $(SUBOBJS)
	$(Q) $(LD) -r -o $@ $^

clean:clean-module clean-subdir FORCE
	$(Q) rm -f $(OBJS) $(STATICLIB) $(SHAREDLIB) $(EXECUTABLE) $(DEPEND)
#	$(Q) -rm -f $(shell find . -name "*.d.*")
	$(Q) $(ECHO) Clean Done.
tar:FORCE
ifeq ($(exec-bin), $(wildcard $(exec-bin)))
	$(Q) rm -rf ./update/AdRtbAdapter.tar.gz
	$(Q) mkdir $(PROJECT)
	$(Q) cp -rf $(TARGETPATH) $(PROJECT)
	$(Q) cp -rf libextern $(PROJECT)
	$(Q) cp -rf conf $(PROJECT)
	$(Q) cp -rf AdAdapterStart.sh $(PROJECT)
	$(Q) cp -rf Release.txt $(PROJECT)
	$(Q) chmod 777  $(PROJECT)/AdAdapterStart.sh
	$(Q) tar zcvf ./update/$(PROJECT).tar.gz  $(PROJECT)
	$(Q) rm $(PROJECT) -rf
endif

config:FORCE
	$(Q) PWD_DIR=`echo $(TOPDIR) | sed "s/\//\\\\\\\\\//g"` ;\
	CONFIG=$(TOPDIR)/conf/AdAdp.conf ;\
	UPDATE=$(TOPDIR)/update/update.sh ;\
	DEMON=$(TOPDIR)/AdAdapterStart.sh ;\
	OLD_DIR=`cat $$CONFIG | grep "PROJECT_PATH" | cut -d = -f 2 | sed "s/\//\\\\\\\\\//g"` ;\
	echo $$OLD_DIR to $$PWD_DIR ;\
	if [ "$$PWD_DIR" != "$$OLD_DIR" ] ; \
		then sed -i "s/$$OLD_DIR/$$PWD_DIR/g" $$CONFIG ;\
		sed -i "s/$$OLD_DIR/$$PWD_DIR/g" $$UPDATE ;\
		sed -i "s/$$OLD_DIR/$$PWD_DIR/g" $$DEMON ; fi

sinclude $(DEPEND)
.PHONY : clean FORCE
