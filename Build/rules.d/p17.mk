#
# rules_p17.mk
#
#
# Liste des règles dédiées à la compilation sur carte pilote-17 
#
# 
# 
#

# Position du script pour mettre en place l'environnement de cross compilation.
#SCRIPT_CROSS:=/usr/local/oecore-$(AUTO_ARCH)/environment-setup-armv5te-oe-linux-gnueabi

#
# Configuration pour cross compilation sur cible arm eukrea
#
GLIBC_V=$(ARCHI)
ARCHI_PREFIX=$(TARGET_PREFIX)
CFLAGS := $(subst $(TARGET_PREFIX)gcc,,$(CC)) $(CFLAGS) -fsigned-char
LDFLAGS := $(subst $(TARGET_PREFIX)gcc,,$(CC)) $(LDFLAGS)
#
# Valeur par défaut des outils de construction
LD:=gcc
CC:=gcc
AR:=ar
RANLIB:=ranlib

