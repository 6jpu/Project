LIBNAME1=mysqlite
LIBNAME2=logger
INSTPATH=`pwd`/../lib/

CC=gcc
AR=ar

all: static_lib
	@make clear
	@make install

static_lib:
	${CC} -c *.c
	${AR} -rcs lib${LIBNAME1}.a ${LIBNAME1}.o
	${AR} -rcs lib${LIBNAME2}.a ${LIBNAME2}.o

install:
	cp -rf lib*.a ${INSTPATH}
	cp -rf *.h ${INSTPATH}

uninstall:
	rm -rf ${INSTPATH}/lib*.a
	rm -rf ${INSTPATH}/*.h

	 
clear:
	rm -rf *.o

clean: clear
	rm -rf lib*.a
	
