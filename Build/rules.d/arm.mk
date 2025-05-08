#
# rules_arm.mk
#
#
# Liste des règles dédiées à la compilation dsur carte eukrea imx25
#
# 
# 
#

# Position du script pour mettre en place l'environnement de cross compilation.
SCRIPT_CROSS:=/usr/local/oecore-$(AUTO_ARCH)/environment-setup-armv5te-oe-linux-gnueabi

#
# Configuration pour cross compilation sur cible arm eukrea
#
GLIBC_V=$(ARCHI)
ARCHI_PREFIX=$(TARGET_PREFIX)
CFLAGS:= $(CFLAGS) -fsigned-char
#
# Valeur par défaut des outils de construction
LD:=gcc
CC:=gcc
AR:=ar
RANLIB:=ranlib

