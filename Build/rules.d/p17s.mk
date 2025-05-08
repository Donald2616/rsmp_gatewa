#
# rules_arm.mk
#
#
# Liste des règles dédiées à la compilation dsur carte p17
#
# 
# 
#


#
# Configuration pour cross compilation sur cible arm p17
#
GLIBC_V=$(ARCHI)
ARCHI_PREFIX=$(TARGET_PREFIX)

SSHLIB=No

CFLAGS := $(subst $(TARGET_PREFIX)gcc,,$(CC)) $(CFLAGS) -fsigned-char -Wno-format-security
#LDFLAGS := $(LD:$(TARGET_PREFIX)ld=) $(LDFLAGS)
# Je n'ai toujours pas compris pourquoi, mais bon si cela fonctionne...
LDFLAGS := $(subst $(TARGET_PREFIX)gcc,,$(CC)) $(LDFLAGS)
#
# Valeur par défaut des outils de construction
LD:=gcc
CC:=gcc
AR:=ar
RANLIB:=ranlib



