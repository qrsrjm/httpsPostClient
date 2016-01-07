# udhcp makefile

DESTDIR=
prefix=/usr
sbindir=/sbin
usrsbindir=$(prefix)/sbin
usrbindir=$(prefix)/bin
datadir=$(prefix)/share

# Uncomment this to get a shared binary. Call as udhcpd for the server,
# and udhcpc for the client
#COMBINED_BINARY=1

# Uncomment this for extra output and to compile with debugging symbols
#DEBUG=1

# Uncomment this to output messages to syslog, otherwise, messages go to stdout
CFLAGS += -DSYSLOG

#CROSS_COMPILE=arm-uclibc-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
INSTALL = install

VER := 0.9.8

OBJS_SHARED = options.o socket.o packet.o pidfile.o
DHCPD_OBJS = dhcpd.o arpping.o files.o leases.o serverpacket.o
DHCPC_OBJS = dhcpc.o clientpacket.o script.o

ifdef COMBINED_BINARY
EXEC1 = udhcpd
OBJS1 = $(DHCPD_OBJS) $(DHCPC_OBJS) $(OBJS_SHARED) frontend.o
CFLAGS += -DCOMBINED_BINARY
else
EXEC1 = udhcpd
OBJS1 = $(DHCPD_OBJS) $(OBJS_SHARED)

EXEC2 = udhcpc
OBJS2 = $(DHCPC_OBJS) $(OBJS_SHARED)
endif

EXEC3 = dumpleases
OBJS3 = dumpleases.o

BOOT_PROGRAMS = udhcpc
DAEMONS = udhcpd
COMMANDS = dumpleases

ifdef SYSLOG
CFLAGS += -DSYSLOG
endif

CFLAGS += -W -Wall -Wstrict-prototypes -DVERSION='"$(VER)"'

ifdef DEBUG
CFLAGS += -g -DDEBUG
else
CFLAGS += -Os -fomit-frame-pointer
endif

all: $(EXEC1) $(EXEC2) $(EXEC3)

$(OBJS1) $(OBJS2) $(OBJS3): *.h Makefile
$(EXEC1) $(EXEC2) $(EXEC3): Makefile

.c.o:
	$(CC) -c $(CFLAGS) $<
	
$(EXEC1): $(OBJS1)
	$(LD) $(LDFLAGS) $(OBJS1) -o $(EXEC1) -static

$(EXEC2): $(OBJS2)
	$(LD) $(LDFLAGS) $(OBJS2) -o $(EXEC2)

$(EXEC3): $(OBJS3)
	$(LD) $(LDFLAGS) $(OBJS3) -o $(EXEC3)


install: all
	$(INSTALL) -d $(DESTDIR)$(usrsbindir)
	$(INSTALL) $(DAEMONS) $(DESTDIR)$(usrsbindir)
	$(INSTALL) -d $(DESTDIR)$(usrbindir)
	$(INSTALL) $(COMMANDS) $(DESTDIR)$(usrbindir)
	$(INSTALL) -d $(DESTDIR)$(sbindir)
ifdef COMBINED_BINARY
	ln -sf $(usrsbindir)/$(DAEMONS) $(DESTDIR)$(sbindir)/$(BOOT_PROGRAMS)
else
	$(INSTALL) $(BOOT_PROGRAMS) $(DESTDIR)$(sbindir)
endif
	$(INSTALL) -d $(DESTDIR)$(datadir)/udhcpc
	for name in bound deconfig renew script ; do \
		$(INSTALL) samples/sample.$$name \
			$(DESTDIR)$(datadir)/udhcpc/default.$$name ; \
	done
	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL) dumpleases.1 $(DESTDIR)$(mandir)/man1
	$(INSTALL) -d $(DESTDIR)$(mandir)/man5
	$(INSTALL) udhcpd.conf.5 $(DESTDIR)$(mandir)/man5
	$(INSTALL) -d $(DESTDIR)$(mandir)/man8
	$(INSTALL) udhcpc.8 udhcpd.8 $(DESTDIR)$(mandir)/man8

clean:
	-rm -f udhcpd udhcpc dumpleases *.o core

