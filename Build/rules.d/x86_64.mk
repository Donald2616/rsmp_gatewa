#
# rules_x86_64.mk
#
#
# Liste des règles dédiées à la compilation dsur carte eukrea imx25
#
#
#
#


#
# Les autres configurations détectée automatiquement par "uname -m" cela peut être i586, i686 etc.
# Mise en place de la version de la GLIB
GLIBC_V:=$(shell /lib/x86_64-linux-gnu/libc.so.6 | egrep ^GNU | sed -e 's:^.*version \+\([0123456789.]\+[01213456789]\+\).*:\1:g')
CC=gcc
