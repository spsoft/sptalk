#--------------------------------------------------------------------

CC = gcc
AR = ar cru
CFLAGS = -Wall -D_REENTRANT -D_GNU_SOURCE -g -fPIC
SOFLAGS = -shared
LDFLAGS = -lstdc++

LINKER = $(CC)
LINT = lint -c
RM = /bin/rm -f

CFLAGS += -I../

LIBEVENT_LIB  = -L$(HOME)/libevent -levent -lrt

LDFLAGS += -L../spserver -lspserver -L../spdict -lspdict \
		-L../spxml -lspxml $(LIBEVENT_LIB)

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

clean:
	@( $(RM) *.o vgcore.* core core.* $(TARGET) )

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

