LIBNAME1=mysqlite
LIBNAME2=logger
LIBPATH=`pwd`/../lib/
APPNAME=serverapp
CFLAGS+=-I${LIBPATH}
LDFLAGS+=-L${LIBPATH}
LDFLAGS+=-l${LIBNAME1}
LDFLAGS+=-l${LIBNAME2}

CC=gcc
AR=ar

all:  
	${CC} ${CFLAGS} server.c -o ${APPNAME} ${LDFLAGS} -l sqlite3

	 
clean:
	rm -rf ${APPNAME}

distclean: clean



