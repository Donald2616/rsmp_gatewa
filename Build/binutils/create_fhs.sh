#!/bin/bash

#
# Création d'une arborescence de fichier 
#

this=$(realpath $0)
script_basename=${this##*/}
script_dirname=${this%/*}

. ${script_dirname}/fhs_functions.sh

project_basename=${script_dirname##*/}
project_dirname=${script_dirname%/*}

#
# Création des répertoires spécifiques
#

# export fhs_etc_umt="${fhs_etc}/umt-conf.d"
# export fhs_etc_mcd="${fhs_etc}/mcd-conf.d"
# export fhs_data_lcr="${fhs_var}/LCR"
# export fhs_data_histo="${fhs_var}/histo"
# export fhs_share_html="${fhs_share}/http/html"
# export fhs_share_cgibin="${fhs_share}/http/cgi-bin"

# export fhs_list_extend="fhs_etc_umt fhs_etc_mcd fhs_data_lcr fhs_data_histo"

fhs_create_dir

for i in $fhs_list $fhs_list_extend
do
    echo "$i:${!i}"
done


