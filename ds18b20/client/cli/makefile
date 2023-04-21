LIBNAME1=ds18b20
LIBNAME2=get_time
LIBNAME3=sqlite
LIBNAME4=logger
LIBPATH=`pwd`/../lib/
APPNAME=client
CFLAGS+=-I${LIBPATH}
LDFLAGS+=-L${LIBPATH}
LDFLAGS+=-l${LIBNAME1}
LDFLAGS+=-l${LIBNAME2}
LDFLAGS+=-l${LIBNAME3}
LDFLAGS+=-l${LIBNAME4}

CC=gcc
AR=ar

all:  
	${CC} ${CFLAGS} ${APPNAME}.c -o ${APPNAME} ${LDFLAGS} -l sqlite3

	 
clean:
	rm -rf ${APPNAME}

distclean: clean



