#
# Rules.mk
#
#
# Liste des règles...
#
#
#
#


HOME_ABS ?= $(realpath ..)
HOME_BUILD ?= $(realpath .)

#
# Mise en place du Home du projet :
# - le répertoire parent à Build si pas de variable ARCHI défini
# - le grand père du répertoire courant si ARCHI est défini
ifeq ($(ARCHI), xxx)
	override undefine ARCHI
endif
ifdef ARCHI
   	HOME_PRJ =$(realpath ../../..)
else
	HOME_PRJ =$(realpath ..)
endif

# Collecte les données de configuration au dessus
include $(HOME_ABS)/Project.mk 

# Vérification si le projet est géré
GIT_MANAGED = $(shell git rev-parse --is-inside-work-tree 2>/dev/null || echo "false")
ifeq ($(GIT_MANAGED), true)
# SI c'est le cas, alors positionne REVISION pour compatibilité
#	REVISION = $(shell echo "$$(git status -bs | head -n 1 |sed -e 's/## \([^\.]*\)\.\.\..*/\1/'|sed -e 's/## //')-$$(git log --pretty=format:"%h" -1 )")
	REVISION = $(shell echo "$$(git status -bs | head -n 1 |sed -e 's/\## \(.*\)\.\.\..*/\1/')-$$(git log --pretty=format:"%h" -1 )")
else
	REVISION = "xxxx"
endif

HOME_FILE= $(HOME_PRJ)

SHELL=/bin/bash

# Nom du projet
PROJET?=$(shell  basename $$(cd $(HOME_PRJ);pwd))
FHS_APP?=$(PROJET)
HOME_ABS=$(realpath $(HOME_PRJ))
HOME_SRC = $(HOME_PRJ)/src
HOME_LIB = $(realpath $(HOME_PRJ)/../Libraries)
HOME_BUILD = $(HOME_PRJ)/Build
HOME_CACHE = $(HOME_BUILD)/cache
HOME_DISTRIB=$(HOME_PRJ)/distrib
HOME_INSTALL=$(HOME_PRJ)/install
DISTRIB_CACHE=$(HOME_DISTRIB)/cache
HOME_TEMP=$(HOME_CACHE)/.tmp
# répertoire de dépendance
DEPDIR = .depend

# Bibliothèques systèmes par défaut
DEF_LIBS ?= -lm -lpthread -lrt


#
# Définition des chemin d'accès aux fichier d'entêtes
INCLUDE_PATH:=-I$(HOME_SRC) -I$(HOME_LIB)/include -I.. $(INCLUDE_PATH)
#
# Définition des variables d'environnement
DEFINE_VARS:=$(DEFINE_VARS) -DDEF_STACK_SIZE=20000 -DPROJET='"$(PROJET)"' -DFHS_APP='"$(FHS_APP)"'

#
# Valeur par défaut des outils de construction
GLIBC_V?=unknown

#
#
DEL=rm -Rf
ZIP=zip
COPY=echo
MV=mv

#
# Les chemins de recherche des sources et cible
VPATH = $(HOME_SRC)


#
# Récupération des includes de librairies locales
LLIBS=$(shell [[ -d $(HOME_ABS)/libs ]] && find $(HOME_ABS)/libs maxdepth 1 -mindepth 1 -type d || echo "")
LIB_DIRECTORIES=$(shell find $(HOME_PRJ) -maxdepth 2 -mindepth 2 -name "src" -type d | grep -e "\./lib")
LIB_INCLUDE = $(LIB_DIRECTORIES:%=-I %)


TARGET_LIBS=nolib
#inclusion des règles dédiées
ifdef ARCHI
include $(HOME_BUILD)/rules.d/$(ARCHI).mk

#
# Préparation de la liste des bibliothèques locales
#
# Effectif si LOCAL_LIBS n'est pas positionné
LIST_LIBS=$(shell  [ -d "$(HOME_PRJ)/lib/" ] && find $(HOME_PRJ)/lib/ -name "lib*a" -exec basename {} \; | sort | uniq)
LIST_LIBS:=$(subst lib,-l,$(LIST_LIBS))
LOCAL_LIBS?=$(LIST_LIBS:%.a=%)

# Préparation des fichiers de lib pour lancement automatique des make
TARGET_LIBS:=$(subst -l,lib,$(LOCAL_LIBS))

#
# Finalisation des options de configuration
CFLAGS:= $(CFLAGS) -Wall -Wextra -O0 -g3 $(INCLUDE_PATH) $(LIB_INCLUDE) $(DEFINE_VARS) -DGLIBC_VERSION='"$(GLIBC_V)"' -DARCHI='"$(ARCHI)"' -DREVISION='"$(REVISION)"' -DARCHI_$(ARCHI) -fmessage-length=0


#
# Le Path des libraries dépend de l'architecture
LDFLAGS:= $(LDFLAGS) -L$(HOME_PRJ)/lib/$(ARCHI) -L$(HOME_LIB)/lib/$(ARCHI)

# pour debug
ifeq ($(ARCHI), x86_64)
    CFLAGS := $(CFLAGS) -fsanitize=address -Wunused-function -Werror=implicit-function-declaration -Werror=incompatible-pointer-types
    LDFLAGS := $(LDFLAGS) -fsanitize=address -Wunused-function -Werror=implicit-function-declaration -Werror=incompatible-pointer-types
endif

# Remplacer -O0 par -O1 pour x86_64
# ifeq ($(ARCHI), x86_64)
#     CFLAGS := $(filter-out -O0,$(CFLAGS)) -O1
# endif
endif


ifdef NO_LIBS
# Si le flags NO_LIBS est positionné, alors pas de vérification de
# la compilation des bibliothèques.
override undefine TARGET_LIBS
endif

#
# mise en place des outils de compilation...
CC:=$(ARCHI_PREFIX)$(CC)
#LD:=$(ARCHI_PREFIX)$(LD)
LD:=$(CC)
AR:=$(ARCHI_PREFIX)$(AR)
RANLIB:=$(ARCHI_PREFIX)$(RANLIB)


#
# Mise en place des règles par défaut

%: %.o
	$(LD) $(CFLAGS) -o $@ $^

%.o: %.c
	@echo compiling $< $(PWD) '$(ARCHI)'
	$(CC) $(CFLAGS) -D__FILE_BASE_NAME__='"$(subst $(HOME_PRJ),.,$<)"' -c -o $@ $(abspath $<)
	@$(CC) $(CFLAGS) -E -MM -MP -MT $@ $< > $(DEPDIR)/$(subst /,_,$(subst $(HOME_PRJ),,$(<:%.c=%.depend)))


%.o: %.cpp
	@echo compiling $<
	$(CXX) $(CPPFLAGS) -D__FILE_BASE_NAME__='"$(subst $(HOME_SRC),.,$<)"' -c -o $@ $(abspath $<)
	@$(CXX) $(CPPFLAGS) -E -MM -MP -MT $@ $< > $(DEPDIR)/$(subst /,_,$(subst $(HOME_SRC),,$(<:%.c=%.depend)))

.SUFFIXES:






#
# Description des fichiers sources
#
# ici la description est basée sur la liste de tous les fichiers c
# Elle peut aussi être faite sur la base d'une liste de répertoires
# ou bien sur une enumération à la main...
ifdef WITH_TEST
SRC:=$(subst $(HOME_SRC)/,,$(shell find $(HOME_SRC) -name *.c))
else
SRC:=$(subst $(HOME_SRC)/,,$(shell find $(HOME_SRC) -name '*.c' ! -wholename '*/src/test/*'))
endif
# Récupération des objects à créer par substitution
OBJS=$(SRC:%.c=%.o)

# Détermination de l'architecture par défaut en fonction de la cible d'exécution
ifndef AUTO_ARCH
#détermine si c'est Windows => cygwin
ifdef OS
AUTO_ARCH=$(shell uname -o)-$(shell uname -m)
else
AUTO_ARCH=$(shell uname -m)
endif
endif


# Liste des cibles possibles
TARGETS= $(sort $(AUTO_ARCH) $(foreach target, $(shell ls $(HOME_PRJ)/Build/rules.d/), $(target:.mk=)))

#
# Liste des distributions possibles au format dist-<archi>
DISTRIBS=$(sort $(TARGETS:%=dist-%))
#
# Liste des distributions possibles au format dist-<archi>
CLEANS=$(sort $(TARGETS:%=clean-%))

#
# Liste des paquets possibles au format pack-<archi>
PACKAGES=$(sort $(TARGETS:%=pack-%))

.PHONY: all all-targets $(TARGETS) $(DISTRIBS) clean dir depdir src distrib clean-all $(CLEANS) doxygen $(PACKAGES)


#cible principale
ifdef ARCHI
# répertoire de dépendance
DEPDIR = $(HOME_CACHE)/$(ARCHI)/.depend
all : depdir dir $(PROJET)
else
all : depdir $(AUTO_ARCH)
endif

all-targets : $(TARGETS)
all-distrib : $(DISTRIBS)
all-packages : $(PACKAGES)
all-clean : clean-all

# Création du répertoire cible s'il n'existe pas
# Puis lancement d'un make dans le répertoire de l'architecture
# en déclenchant le script des règles ed compilation pour la cible.
$(TARGETS) :
	@echo "*********************************";\
	echo "Construction pour $@ depuis $$(pwd), $(HOME_CACHE)";\
	[ ! -d $@ ] && mkdir -p $(HOME_CACHE)/$@;\
	#[ ! -e $(HOME_CACHE)/$@/Makefile ] && ln -svf $(HOME_BUILD)/Makefile $(HOME_CACHE)/$@/Makefile;\
	ARCHI=$@;\
	if [ -f $(HOME_BUILD)/script.d/$@ ];\
	then \
		script=$$(cat $(HOME_BUILD)/script.d/$@);\
		echo "environnement depuis $$script";\
		. $$script; \
	fi; \
	$(MAKE) -C $(HOME_CACHE)/$@ ARCHI=$@ HOME_ABS=$(HOME_ABS) HOME_BUILD=$(HOME_BUILD) -f $(HOME_BUILD)/Makefile 

#Nettoyage ciblé
clean:
ifdef ARCHI
	@echo Nettoyage $(ARCHI)
	@if [ $$(basename $$(pwd)) !=  $(ARCHI) ];\
	then\
		cd $(HOME_CACHE)/$(ARCHI);\
	fi;\
	rm -rf $(OBJS) $(PROJET) $(DEPDIR)
else
	@echo Nettoyage $(AUTO_ARCH)
	@if [ $$(basename $$(pwd)) !=  $(AUTO_ARCH) ]; then\
		cd $(HOME_CACHE)/$(AUTO_ARCH);\
	fi;\
	rm -rf $(OBJS) $(PROJET) $(DEPDIR)
endif

$(CLEANS) :
	@__target__=$(@:clean-%=%);\
	echo -e "\nNettoyage $$__target__";\
	if [ -d $(HOME_CACHE)/$$__target__ ];\
	then\
		if [ "$(OBJS)" != "" ];\
		then\
			cd $(HOME_CACHE)/$$__target__;\
			rm -rf $(OBJS) $(DEPDIR) $(PROJET);\
			cd $${OLDPATH};\
		fi;\
	fi;


#Nettoyage global
clean-all : $(CLEANS)

#Création des répertoires intermédiaires : sources et répertoires de distribution
dir:
	@echo "Check répertoires $(ARCHI) ";\
	echo "répertoire source $$(pwd)";\
	for D in $$(cd $(HOME_SRC); find . -type d);\
	do\
		[ "$$D" == "." ] && continue; \
		[ "$$D" = "Build" ] && continue;\
		[ ! -d $(HOME_SRC)/$$D ] && continue;\
		[ ! -d $$D ] && mkdir -p $$D;\
	done;\
	echo "Sortie de $@";\


#Création des répertoires intermédiaires : distribution
dir_dist:
	@echo "Mise en place répertoire de distribution $(ARCHI) ";\
	[ -e $(HOME_TEMP)/$(ARCHI) ] && rm -rf $(HOME_TEMP)/$(ARCHI);\
	mkdir -p $(HOME_TEMP)/$(ARCHI);\
	cd $(HOME_TEMP)/$(ARCHI);\
	if [ -d $(HOME_INSTALL)/_ref ];\
	then\
		cd $(HOME_INSTALL)/_ref;\
		cp -rLp . "$$OLDPWD";\
		cd $$OLDPWD;\
		pwd;\
	fi;\
	if [ -d $(HOME_INSTALL)/_ref.$(ARCHI) ];\
	then\
		cd $(HOME_INSTALL)/_ref.$(ARCHI);\
		cp -rLp . "$$OLDPWD";\
		cd $$OLDPWD;\
	fi;\
	cd $(HOME_INSTALL)/_ref/;\
	cp -rLp . "$$OLDPWD";\
	cd $$OLDPWD;\
	if [ -d $(HOME_INSTALL)/_ref.$(ARCHI) ];\
	then\
		cd $(HOME_INSTALL)/_ref.$(ARCHI);\
		pwd;\
		cp -rLp . "$$OLDPWD";\
		cd $$OLDPWD;\
	fi;\
	chmod +x bin/* start.d/*;\
	if [ ! "$(DISTRIB_LINK)_" == "_" ];\
	then \
		for D in "$(DISTRIB_LINK)" ; \
		do \
			[ ! -e "../$$D" ] && continue ;\
			DD=$$(basename "$$D");\
			[ -e "$$DD" ] && unlink "$$DD" ;\
			ln -svfd "../$$D" "$$DD" ;\
		done ;\
	fi;\
	echo "Sortie de $@";



src:
	@echo "";\
	echo "Création des sources";\
	cd $(HOME_PRJ) ;\
	pwd;\
	if [ -e src/config.h ] ;\
	then\
		VER=$$(grep -e "^.define VERSION" src/config.h | sed "s/.*VERSION.*\\"\\(.*\\)\\"/\\1/"|sed "s! !!g");\
		REV=$$(grep -e "^.define REVISION" src/config.h | sed "s/.*Revision: *\\(.*\\)$$\\"/\\1/"|sed "s! !!g");\
		FILENAME=$(PROJET)-V$${VER}-R$${REV}-src.tgz;\
	else\
		FILENAME=$(PROJET)-src.inst.tgz;\
	fi;\
	tar -C $(HOME_PRJ) -cvzf $$FILENAME --exclude-vcs ./src/*/*.[ch] ./src/*.[ch] ./Build/Makefile ./Build/*.mk

#
# Création d'un fichier de distribution par architecture
# Fonctionnement identique à la construction : relance d'un make dans le répertoire
$(PACKAGES) :
	@ARCHI=$(@:pack-%=%);\
	ARCHI_DIR=$(HOME_CACHE)/$$ARCHI;\
	if [ ! -d $$ARCHI_DIR ];\
	then\
		mkdir -vp $$ARCHI_DIR;\
	fi;\
	if [ -f $(HOME_PRJ)/Build/script.d/$$ARCHI ];\
	then \
		script=$$(cat $(HOME_PRJ)/Build/script.d/$$ARCHI);\
		echo "environnement depuis $$script";\
		. $$script; \
	fi; \
	cd $$ARCHI_DIR;\
	$(MAKE) -C $$ARCHI_DIR ARCHI=$$ARCHI HOME_ABS=$(HOME_ABS) HOME_BUILD=$(HOME_BUILD) -f $(HOME_BUILD)/Makefile package
#
# Création du packaging
package : $(PROJET)
	@echo "Construction du packages d'installation pour $(ARCHI)" ;\
	if [ -e "$(HOME_SRC)/config.h" ] ;\
	then\
	    VERSION_MAJOR=$$(grep -e "^.define VERSION_MAJOR" $(HOME_SRC)/config.h | tr -d '"' | sed -e 's/[[:blank:]]\+/ /g' | cut -d ' ' -f 3);\
	    if [ ! -z "$$VERSION_MAJOR" ] ;\
	    then\
	        VERSION_MINOR=$$(grep -e "^.define VERSION_MINOR" $(HOME_SRC)/config.h | tr -d '"' | sed -e 's/[[:blank:]]\+/ /g' | cut -d ' ' -f 3);\
	        [ -z "$$VERSION_MINOR" ] && VERSION_MINOR="0";\
	        VERSION_REV=$$(grep -e "^.define VERSION_REV" $(HOME_SRC)/config.h | tr -d '"' | sed -e 's/[[:blank:]]\+/ /g' | cut -d ' ' -f 3);\
	        [ -z "$$VERSION_REV" ] && VER="$${VERSION_MAJOR}.$${VERSION_MINOR}" || VER="$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_REV}";\
	    else\
	        VER=$$(grep -e "^.define VERSION" $(HOME_SRC)/config.h | tr -d '"' | sed -e 's/[[:blank:]]\+/ /g' | cut -d ' ' -f 3);\
	    fi;\
	else\
	    VER="xxxx";\
	fi;\
	VERSION=$${VER} PROJET="$(PROJET)" ARCHI="$(ARCHI)" APP="$(FHS_APP)" ROOT_PRJ="$(HOME_ABS)" $(HOME_ABS)/Build/binutils/package.sh



#
# Création d'un fichier de distribution par architecture
# Fonctionnement identique à la construction : relance d'un make dans le répertoire
$(DISTRIBS) :
	@ARCHI=$(@:dist-%=%);\
	if [ ! -d $(HOME_CACHE)/$$ARCHI ];\
	then\
		mkdir -p $(HOME_CACHE)/$$ARCHI;\
	fi;\
	if [ -f $(HOME_BUILD)/script.d/$$ARCHI ];\
	then \
		script=$$(cat $(HOME_BUILD)/script.d/$$ARCHI);\
		echo "environnement depuis $$script";\
		. $$script; \
	fi; \
	cd $(HOME_CACHE)/$$ARCHI;\
	$(MAKE) -C $(HOME_CACHE)/$$ARCHI ARCHI=$$ARCHI distrib

#
# Création du fichier de distribution
#
distrib : $(PROJET) dir_dist
	@echo "Construction du fichier d'installation pour $(ARCHI) GLIBC_$(GLIBC_V)" ;\
	if [ -e $(HOME_CACHE)/$(ARCHI)/$(PROJET) ] ;\
	then\
		if [ -e "$(HOME_SRC)/config.h" ] ;\
		then\
			REV=$(REVISION);\
			VER=$$(grep -e "^.define VERSION" $(HOME_SRC)/config.h | tr -d '"' | sed -e 's/ \+/ /g' | cut -d ' ' -f 3);\
			FILENAME=$(HOME_DISTRIB)/$(ARCHI)/$(PROJET)-V$${VER}-R$${REV}-GLIBC_$(GLIBC_V).$(ARCHI).inst.tgz;\
		else\
			FILENAME=$(HOME_DISTRIB)/$(ARCHI)/$(PROJET)-Vxxx-Ryyy-GLIBC_$(GLIBC_V).$(ARCHI).inst.tgz;\
		fi;\
		echo "Création tar $$FILENAME de distribution pour $(ARCHI)";\
		ln -fs $(HOME_CACHE)/$(ARCHI)/$(PROJET) $(HOME_TEMP)/$(ARCHI)/$(PROJET);\
		touch $$FILENAME;\
		tar -C $(HOME_TEMP)/$(ARCHI) -cvzhf $$FILENAME --exclude .gitignore --exclude-vcs .;\
		md5sum $$FILENAME > $$FILENAME.md5;\
	else\
		echo "No $(PROJET), abort";\
		exit 2;\
	fi

# Déclenchement de la génération de la documentation
doxygen:
#	@cd $(HOME_PRJ) && mkdir -p doxygen/tmp && mkdir -p Documentation/html/umt && mkdir -p Documentation/rtf/umt ;
	@cd $(HOME_PRJ) && mkdir -p doxygen/tmp ;\
	fname=./doxygen/tmp/Doxyfile.$(PROJET);\
	sed -e "s/%Rev%/Version $$(grep -e '#define VERSION ".\+\?"' ./src/config.h | sed -re 's/.*"(.*)"/\1/') Revision $$(git log -1 --pretty=oneline)/g" -e "s/%title%/$(PROJET)/g" -e "s!%home%!$(HOME_ABS)!g" ./doxygen/Doxyfile.template > $$fname ;\
	doxygen $$fname 2>&1 | tee ./doxygen/tmp/Doxyfile.$(PROJET).log

# Construction automatique des bibliothèques locales
$(TARGET_LIBS):
	@echo -e "\nBuild Library '$@' $(ARCHI)";\
	_lib=$(HOME_PRJ)/../$@/Build;\
	if [ -d "$$_lib" ]; \
	then \
		make -C $$_lib $(ARCHI) ARCHI=xxx;\
	else \
	 echo "no way, nothing to do";\
	 echo "ret:$$?";\
	fi

depdir:
	@echo "check depencies directory";\
	if [ ! -d $(DEPDIR) ] ;\
	then \
		mkdir -vp $(DEPDIR);\
	fi

$(PROJET): $(OBJS) $(TARGET_LIBS)
	@echo "$(TARGET_LIBS)";\
	echo "Édition de lien $(PROJET)"
	$(LD) $(LDFLAGS) -o$@ $(OBJS) -Wl,--start-group $(LOCAL_LIBS) $(LIBS) -Wl,--end-group  $(DEF_LIBS) $(EXTRA_LIBS)

ifdef ARCHI
-include $(DEPDIR)/*depend
endif
