#!/bin/bash
 
# library install path

INST_PATH=`pwd`/../install
 
# LingYun studio FTP server address for all the open source code
LYFTP_SRC=ftp://master.iot-yun.club/src/
 
# set shell script exit when any command failure
set -e
 
#define a funciton to build sqlite source code
function build_sqlite()
{
   SRC_NAME=sqlite-autoconf-3380200
 
   if [ -L $INST_PATH/lib/libsqlite3.so ] ; then
      echo "$SRC_NAME already compile and installed"
      return ;
   fi
 
   # If source code tarball file not exist, it will download the packet.
   if [ ! -f ${SRC_NAME}.tar.gz ] ; then
      #wget https://sqlite.org/2022/${SRC_NAME}.tar.gz
      wget ${LYFTP_SRC}/${SRC_NAME}.tar.gz
   fi
 
 
   # If source code folder not exist, decompress the tarball packet
   if [ ! -d ${SRC_NAME} ] ; then
      tar -xzf ${SRC_NAME}.tar.gz
   fi
 
   cd ${SRC_NAME}
 
   ./configure --prefix=${INST_PATH} --enable-static
 
   make && make install
}
 
 
# call function to start build sqlite
 
build_sqlite
