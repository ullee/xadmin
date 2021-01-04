#
# Makefile for SecureGuard Prime src ver 1.x.x.x(renewal)
#

include .config

CC				=	gcc
DEFS			=	-D_REENTRANT -D_GNU_SOURCE=1

INC				=	-I. -I$(AM_INC)
CFLAGS			= 	-g -W -Wall -fPIE $(DEFS) $(INC)
#CFLAGS			= 	-W -Wall -O2 -fPIE $(DEFS) $(INC)
BASE_LDFLAGS 	=	-lpthread -lrt -lm
#XLIB_LDFLAGS	=	-L$(AM_LIB) -lx -lxcrypt -lssl -lcrypto 
XLIB_LDFLAGS	=	-L$(AM_LIB) -lssl -lcrypto 
DB_LDFLAGS		=	-L$(AM_LIB) -lxdb -lpq -lyaml
LDFLAGS			=	$(BASE_LDFLAGS) $(XLIB_LDFLAGS) $(DB_LDFLAGS) 

XADMIN			=	xadmin.o
XSTRING			=	xstring.o
XARRAY			=	xarray.o
XADMIN_CONF		=	xadmin_conf.o
XDB_POOL		=	xdb_pool.o
XDBBACKUP		=	xdbbackup.o

XADMIN_SRCS		=	$(XADMIN:.o=.c) $(XSTRING:.o=.c) $(XARRAY:.o=.c) $(XADMIN_CONF:.o=.c) \
					$(XDB_POOL:.o=.c)
#					$(XDB_POOL:.o=.c) $(XDBBACKUP:.o=.c)
				
XADMIN_OBJS		=	$(XADMIN_SRCS)

TARGET			=	xadmin
   
all:				$(TARGET)

xadmin: $(XADMIN_OBJS)
	$(CC) $(CFLAGS) -o xadmin $(XADMIN_OBJS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
	@echo -e " $(BLUE_COLOR)Complete SecureGuard $(AM_SRC)/app/xadmin Compile$(NO_COLOR) $(OK_COLOR)[OK]$(NO_COLOR)"

install:
	cp $(TARGET) $(AM_BIN)
	@echo -e " $(BLUE_COLOR)Complete SecureGuard $(AM_SRC)/app/xadmin Install$(NO_COLOR) $(OK_COLOR)[OK]$(NO_COLOR)"

clean:
	rm -f *.o $(TARGET)
	@echo -e " $(BLUE_COLOR)Complete SecureGuard $(AM_SRC)/app/xadmin Clean$(NO_COLOR) $(OK_COLOR)[OK]$(NO_COLOR)"

xstring.o: xstring.h xstring.c
xarray.o: xarray.h xarray.c
xadmin_conf.o: xadmin_conf.h xadmin_conf.c
xdb_pool.o: xdb_pool.h xdb_pool.c
xdbbackup.o: xdbbackup.h xdbbackup.c

#EOF
