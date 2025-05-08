# Fichier de configuration de la construction du projet

# decommenter la ligne si dessous pour forcer le nom du projet
# La valeur par défaut est le nom du répertoire (le nom du projet git ou pas)
#PROJET=

# Permet de "désynchroniser" le nom applicatif dans le fhs et le programme
# il est donc possible dans ce cas d'installer plusieurs éxécutables sous le même
# nom d'application.
# La valeur par défaut est équivalente à PROJET
#FHS_APP=$(PROJET)


#
# Libraries exterieures
# LIBS:= -lflux -llstdblcyc -lhorodate -luuid -lgnutls -ljson-c -lfhs -lmemnocheck -llock
# LIBS:= -lflux -llstdblcyc -lutil -luuid -ljson-c -lfhs -lmemnocheck -llock -lssl -lcrypto -lmhash
LIBS:= -lflux -llstdblcyc -lutil -luuid -ljson-c -lfhs -lmemnocheck -llock -lssl -lcrypto -lmhash -lcomplete -lz


# exemple 
# LIBS:=-lcomplete -llog -lflux -llist -lioprintf -ltedimaster -lcrypto -lssl -lfhs

# 
# Libraries local
# Si non positionné, toutes les bibliothèques locales seront utilisées
# LOCAL_LIBS=

# Lib systèmes supplémentaires
#EXTRA_LIBS=


#
# Définition des chemin d'accès aux fichier d'entêtes
#INCLUDE_PATH=


#
# Définition des variables d'environnement
DEFINE_VARS=-DWITH_MHASH

ifndef  DISABLE_MHASH
DEFINE_VARS+=-DWITH_MHASH
endif

ifdef  WITH_TEST
DEFINE_VARS+=-DTEST_ENABLED
endif


#
# Répertoires qui doivent être remis dans la distribution (via un ln)
DISTRIB_LINK=$(HOME_FILE)/http

#
# Répertoires qui doivent être créés dans la distribution (via un mkdir)
DISTRIB_DIR=
