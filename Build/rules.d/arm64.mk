#
# rules_arm64.mk
#
#
# Liste des règles dédiées à la compilation pour arm64 depuis une debian avec cross compil
#
# 
# 
#


#
# Les autres configurations détectée automatiquement par "uname -m" cela peut être i586, i686 etc.
# Mise en place de la version de la GLIB
GLIBC_V=$(shell /lib/$(AUTO_ARCH)-linux-gnu/libc.so.6 | egrep ^GNU | sed -e 's!GNU C.*version \(.*\),.*!\1!g')

# Valeur par défaut des outils de construction
LD:=gcc
CC:=gcc
AR:=ar
RANLIB:=ranlib

ARCHI_PREFIX:=aarch64-linux-gnu-