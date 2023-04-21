LIBNAME1=sqlite
LIBNAME2=logger
LIBPATH=`pwd`/../lib/
APPNAME=server
CFLAGS+=-I${LIBPATH}
LDFLAGS+=-L${LIBPATH}
LDFLAGS+=-l${LIBNAME1}
LDFLAGS+=-l${LIBNAME2}

CC=gcc
AR=ar

all:  
	${CC} ${CFLAGS} ${APPNAME}.c -o ${APPNAME} ${LDFLAGS} -l sqlite3

	 
clean:
	rm -rf ${APPNAME}

distclean: clean



