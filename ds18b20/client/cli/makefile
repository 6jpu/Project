LIBNAME1=ds18b20
LIBNAME2=socket
LIBNAME3=mysqlite
LIBNAME4=logger
LIBNAME5=pack
LIBPATH=`pwd`/../lib/
APPNAME=clientapp
CFLAGS+=-I ${LIBPATH}
LDFLAGS+=-L ${LIBPATH}
LDFLAGS+=-l${LIBNAME1}
LDFLAGS+=-l${LIBNAME2}
LDFLAGS+=-l${LIBNAME3}
LDFLAGS+=-l${LIBNAME4}
LDFLAGS+=-l${LIBNAME5}

CC=gcc
AR=ar

all:  
	${CC} ${CFLAGS} client.c -o ${APPNAME} ${LDFLAGS} -l sqlite3

	 
clean:
	rm -rf ${APPNAME}

distclean: clean



