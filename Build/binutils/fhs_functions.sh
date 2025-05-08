#!/bin/bash

#
# Mise en place des options
#

function die()
{
    local exit_code=1
    if [[ $# -gt 1 ]] 
    then
        exit_code=$1
        shift
    fi
    echo "$*" 1>&2
    exit $exit_code
}

[[ -z ${_root_} ]] && _root_="./root"

[[ -z ${_prefix_} ]] && _prefix_="opt/svms"

[[ -z ${_app_} ]] && die 1 "Please specify app name, bye!"


fhs_list="fhs_base fhs_bin fhs_sbin fhs_share fhs_doc fhs_etc fhs_var fhs_log fhs_data"
export fhs_base="${_root_}/${_prefix_}/${_app_}"
export fhs_bin="${fhs_base}/bin"
export fhs_sbin="${fhs_base}/sbin"
export fhs_share="${fhs_base}/share"
export fhs_doc="${fhs_share}/doc"
export fhs_etc="${_root_}/etc/${_prefix_}/${_app_}"
export fhs_var="${_root_}/var/${_prefix_}/${_app_}"
export fhs_log="${fhs_var}/log"
export fhs_data="${fhs_var}/data"

fhs_ext_path="$(realpath ${project_dirname}/install/fhs/fhs_ext.sh)"
[[ -r "${fhs_ext_path}" ]] && . "${fhs_ext_path}" || echo -e "***********\nNo extension found!\n*******"

function fhs_create_dir()
{
    for var in $fhs_list $fhs_list_extend
    do
        echo -n "$var:${!var} =>"
        mkdir -vp "${!var}" && eval ${var}=$(realpath "${!var}")
        echo "${!var}"
    done
}



