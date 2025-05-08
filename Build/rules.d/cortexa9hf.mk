#
# rules_eldim.mk
#
#
# Liste des règles dédiées à la compilation dsur carte eukrea imx25
#
# 
# 
#

#
# Configuration pour cross compilation sur cible arm v7 (fullcolor)
#
GLIBC_V=$(ARCHI)
ARCHI_PREFIX=$(TARGET_PREFIX)
SSHLIB=No
DEFINE_VARS += -DNO_ZLIB
CFLAGS := $(subst $(TARGET_PREFIX)gcc,,$(CC)) $(CFLAGS) -fsigned-char
#LDFLAGS := $(LD:$(TARGET_PREFIX)ld=) $(LDFLAGS)
# Je n'ai toujours pas compris pourquoi, mais bon si cela fonctionne...
LDFLAGS := $(subst $(TARGET_PREFIX)gcc,,$(CC)) $(LDFLAGS)

#
# Valeur par défaut des outils de construction
CC:=gcc
LD:=$(CC)
AR:=ar
RANLIB:=ranlib

# Il faut virer la lz de LIBS
LIBS := $(LIBS:-lz=)
