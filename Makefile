#--------------------------------------------------------------------

CC = gcc
AR = ar cru
CFLAGS = -Wall -D_REENTRANT -D_GNU_SOURCE -g -fPIC
SOFLAGS = -shared
LDFLAGS = -lstdc++

LINKER = $(CC)
LINT = lint -c
RM = /bin/rm -f

ifeq ($(origin version), undefined)
	version = 0.1
endif

SPSERVER_INCL = -I../spserver/
SPSERVER_LIB  = -L../spserver/ -lspserver -levent

SPDICT_INCL = -I../spdict/
SPDICT_LIB  = -L../spdict/ -lspdict

SPXML_INCL = -I../spxml/
SPXML_LIB  = -L../spxml/ -lspxml

CFLAGS  += $(SPSERVER_INCL) $(SPDICT_INCL) $(SPXML_INCL)
LDFLAGS += $(SPSERVER_LIB) $(SPDICT_LIB) $(SPXML_LIB) -lpthread -lresolv

#-------------------Cprof related Macros----------------
ifeq ($(cprof), 1)
	CFLAGS += -finstrument-functions
	LDFLAGS += -lcprof
endif

#--------------------------------------------------------------------

UNITTEST = testxmppjid

TARGET = $(UNITTEST) sptalkd

#--------------------------------------------------------------------

all: $(TARGET)

sptalkd: spxmppjid.o spxmpppacket.o spxmppdecoder.o \
		spxmppaction.o spxmpphandler.o \
		spxmppentity.o spxmpppersist.o spxmppfiledb.o \
		spxmppsession.o spxmppconfig.o sptalkd.o
	$(LINKER) $(LDFLAGS) $^ -o $@

testxmppjid: spxmppjid.o testxmppjid.o
	$(LINKER) $(LDFLAGS) $^ -o $@

dist: clean sptalk-$(version).src.tar.gz

sptalk-$(version).src.tar.gz:
	@ls | grep -v CVS | grep -v .so | grep -v spool | sed 's:^:sptalk-$(version)/:' > MANIFEST
	@(cd ..; ln -s sptalk sptalk-$(version))
	(cd ..; tar cvf - `cat sptalk/MANIFEST` | gzip > sptalk/sptalk-$(version).src.tar.gz)
	@(cd ..; rm sptalk-$(version))

clean:
	@( $(RM) *.o vgcore.* core core.* $(TARGET) )

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

