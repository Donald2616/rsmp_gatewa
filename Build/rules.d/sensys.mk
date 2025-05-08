#
# sensys.mk
#
# Commun pour apx et apcc les deux cibles emnbarquée sensys/karrus
#
#
# Liste des règles dédiées à la compilation dsur carte eukrea imx25
#
# 
# 
#

ifeq "$(ARCHI)" "apcc"
sensys=$(ARCHI)
endif

ifeq "$(ARCHI)" "apx"
sensys=$(ARCHI)
endif


# Variation des configurations de compilation en fonction des architectures 
ifneq ($(sensys),)
#
# Configuration pour cross compilation sur cible raspberry pi arm poky
#
DEFINE_VARS:=$(DEFINE_VARS) -DARCHI_sensys
ifeq ($(sensys),apcc)

else
DEFINE_VARS:=$(DEFINE_VARS) -DWITH_RESOLV -DNO_STATIC
endif

GLIBC_V=$(ARCHI)
ARCHI_PREFIX=arm-linux-gnueabi-
CC=gcc-4.3
CFLAGS:= $(CFLAGS) -fsigned-char -DNO_ZLIB
SSHLIB=No
