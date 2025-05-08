#!/bin/bash

# Positionnement de plusieurs informations contextuelles

this=$(realpath $0)
script_basename=${this##*/}
script_dirname=${this%/*}

#
# Message sur stderr + exit
#
# Si plusieurs arguments, le premier est le code d'erreur
# les autres sont le message
#
# Si un seul argument, il est le message et le code d'erreur
# est positionné à 1
function die()
{
    local exit_code=1
    if [[ $# -gt 1 ]] 
    then
        exit_code=$1
        shift
    fi
    [[ $# -gt 0 ]] && echo "$*" 1>&2
    exit $exit_code
}

#
# Nettoyage à la sortie : suppression du répertoire temporaire
#
function cleanup()
{
    if [[ -e "$tmp_dir" ]]
    then
        if [[ -z  "${keep_it}" ]]
        then
            echo "${script_basename}: cleanning $tmp_dir"
            rm -rf "$tmp_dir"
        else   
            echo "$tmp_dir still available"
        fi
    fi
    #echo "ohohoh"
}

#
# Copie entre deux répertoires via tar
# 
# Par tar pour pouvoir copier l'intérieur du 
# répertoire source sans oublier les fichiers cachés
#
# $1 la liste des sources répertoire
# $2 la destination, si le répertoire de destination 
#    n'existe pas, alors il est créé
function copy2package()
{
    local src=$1
    local dest=$2
    [[ ! -d "$dest" ]] && mkdir -p "$dest"
    for s in $src
    do
        if [[ -d ${!s} ]] 
        then 
            tar -C "${!s}" -chl --exclude-vcs . | tar -C "$dest" -x
        fi
    done
}

trap cleanup EXIT



[[ -z "$ARCHI" ]]   && die 10 "Please give me an architecture, abort!"
[[ -z "$VERSION" ]] && die 11 "Please give me an version id, abort!"
[[ -z "$PROJET" ]]  &&  [[ -z "$APP" ]]  && die 12 "Please give me an project or application name, abort!"
[[ -z "$APP" ]] && APP="$PROJET"

# test la présence de tar
which tar 1>/dev/null 2>&1
[[ "_$?" != "_0" ]] && die 13 "Please where is tar command, abort!"

# vérification que les fichiers ${ROOT_PRJ}/install/install-kit/*.d/* sont tous executables
for f in "${ROOT_PRJ}/install/install-kit/"*inst.d/*
do
    [[ -x "$f" ]] || die 14 "Please make $f executable, abort!"
done

# vérification qu'il n'y a pas de fichier modifié
git status --porcelain | grep -q -v '^??' && die 14 "Please commit your changes before, abort!"
 
package_fname="${PROJET}-${VERSION}-${ARCHI}${ext_suffix}"



install_path=$(realpath "${ROOT_PRJ}/install/")
fhs_root_base="${install_path}/fhs/root"
fhs_root_archi="${install_path}/fhs.${ARCHI}/root"
install_script_base="${install_path}/install-kit"
install_script_archi="${install_path}/install-kit.${ARCHI}"
build_path="${script_dirname%/*}/cache/$ARCHI"
package_dest="${build_path}/_distrib/packages"

[[ -z "$TARGET" ]]  && TARGET="${build_path}/$PROJET"

[[ ! -e "${package_dest}" ]] && mkdir -p "${package_dest}"
[[ -z "$tmp_dir" ]] && tmp_dir=$(mktemp -d ${package_dest}/pack-${package_fname}.XXXXXXXXXX)
# tmp_dir vide ou n'existe pas
if [[ -z "$tmp_dir" ]] || [[ ! -d "$tmp_dir" ]]
then
    die 15 "Can't create temporary directory, abort!"
fi


export _root_="${tmp_dir}/root/"
export _app_="$APP"
export _prefix_="opt/svms"

install_kit_path="${install_path}/install-kit"

[[ ! -d "${install_kit_path}" ]] && die 1 "${install_kit_path} No install-kit, abort!"

env_path="${install_kit_path}/env.d"
if [[ -d "$env_path" ]]
then
    for f in "$env_path"/*
    do
        [[ -r "$f" ]] && source "$f"
    done
fi


echo "${script_basename}: stage 0 Use temporary dir ${tmp_dir}"
package_path="${tmp_dir}/package"
mkdir -p "${package_path}"


# Creation d'un package pour une architecture donnée


echo "${script_basename}: stage 1 Copy fhs"
copy2package "fhs_root_base fhs_root_archi" "${tmp_dir}/root"

echo "${script_basename}: stage 2 Copy exefiles from Build environment"
cp -p "$TARGET" "$fhs_bin"

echo "${script_basename}: stage 3 Tar deployment file"
tar -C "${tmp_dir}/root" -czf "${package_path}/${package_fname}.tgz" .


echo "${script_basename}: stage 4 Copy install scripts into $package_path"
copy2package "install_script_base install_script_archi" "$package_path"

echo "${script_basename}: stage 5 Setup environment variables"
cat > "${package_path}/env.d/00-env" <<EOF
#!/bin/bash

# Mise en place 
# - du nom de l'application en fhs, 
# - de la racine de déploiement,
# - du nom du service
# - du nom du package
export _app_="$_app_"
export _binary_="$PROJET"
export _service_="$PROJET"
export _root_="/"
export _package_fname_="${package_fname}.tgz"
EOF

echo "${script_basename}: stage 6 Add signatures"
cd "${package_path}"
[[ -e ./md5sum.txt ]] && rm ./md5sum.txt
md5sum $(find . -type f) > ./md5sum.txt
cd "$OLDPWD"

package_final="${package_dest}/${package_fname}.sh"
echo "${script_basename}: stage 7 Final construction => executable: $package_final"

[[ ! -d "$package_dest" ]] && mkdir -p "$package_dest"

cat > "${package_final}" <<EOF
#!/bin/bash
this=\$(realpath \$0)
script_basename=\${this##*/}
script_dirname=\${this%/*}

function atexit()
{
    if [[ -d "\$WORKDIR" ]]
    then
        echo Deleting...
        rm -rf "\$WORKDIR"
    fi
}
trap atexit EXIT
#Si pas en root, pas la peine
if [ \$(id -u) -ne 0 ]
then
    echo "Please, root expected, abort!" 1>&2
    exit 20
fi
TMP_DIR="/var/opt/svms/.tmp"
[ ! -d "\$TMP_DIR" ] && mkdir -p "\$TMP_DIR"
WORKDIR=\$( mktemp -d -p "\$TMP_DIR")

#find last line +1

SCRIPT_END=\$( awk '
  BEGIN { err=1; }
  /^\w*___END_OF_SHELL_SCRIPT___\w*\$/ { print NR+1; err=0; exit 0; }
  END { if (err==1) print "?"; }
' "\$0" )

# check for error

if [ "\$SCRIPT_END" == '?' ]
then
   echo Can\'t find embedded file
   exit 1
fi
echo "\${script_basename}: stage 1, Extract file"
tail -n +\$SCRIPT_END \$0 $([[ ! -z "$w_base64" ]] && echo -n "| base64 -d") | tar -C \$WORKDIR -xz

echo "\${script_basename}: stage 2, Check Signature"
cd \$WORKDIR
[ -r ./md5sum.txt ] && md5sum -c ./md5sum.txt
if [ "_\$?" != "_0" ]
then
    echo "Failed signature, abort" 1>&2
    exit 120
fi
echo "\${script_basename}: stage 3, Start installation"
./install

exit 0
# Here's the end of the script followed by the embedded file
___END_OF_SHELL_SCRIPT___
EOF
if [[ -z "$w_base64" ]] 
then
    tar -C ${package_path} -cz . >>"${package_dest}/${package_fname}.sh" 
else
    tar -C ${package_path} -cz . | base64 >>"${package_dest}/${package_fname}.sh" 
fi
chmod +x "${package_dest}/${package_fname}.sh"
cd "${package_dest}"
md5sum "./${package_fname}.sh" > "./${package_fname}.sh.md5"
cd "$OLDPWD"
exit 0
