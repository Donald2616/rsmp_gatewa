#
# rpi.mk
#
#
# Liste des règles dédiées à la compilation dsur carte eukrea imx25
#
# 
# 
#

#
# Configuration pour cross compilation sur cible raspberry pi arm poky
#

GLIBC_V=$(ARCHI)
ARCHI_PREFIX=arm-linux-gnueabihf-
CFLAGS:= $(CFLAGS) -fsigned-char
CC:=gcc
#DNO_ZLIB
#SSHLIB=No
