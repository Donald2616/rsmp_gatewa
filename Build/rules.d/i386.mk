#
# rules_i386.mk
#
#
# Liste des règles dédiées à la compilation dsur carte eukrea imx25
#
# 
# 
#


#
#configuration pour compilation sous linux i386
#
ifneq "$(AUTO_ARCH)" "i386"
# Si On est pas sur une machine 64bits alors il faut demander la compilation en 32 bits
CFLAGS := $(CFLAGS) -m32
LDFLAGS := $(LDFLAGS) -m32
# La version de GLIB est ailleurs, il faut vérifier sur les machines qui ne sont pas des i686...
GLIBC_V=$(shell /lib/i386-linux-gnu/i686/cmov/libc.so.6 | egrep ^GNU | sed -e 's!GNU C.*version \(.*\),.*!\1!g')
endif 
