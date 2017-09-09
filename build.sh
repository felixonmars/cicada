#!/bin/sh

#cc=gcc
cc=clang
w='-Wno-int-conversion -Wno-incompatible-pointer-types -Wno-return-type -Wunused-value'
o='-O2'
f='-rdynamic'
l='-ldl'
d='-g'

copy() {
    rsync --recursive --links --perms --times --group --owner --devices --specials --verbose --human-readable $@
}

tangle() {
    ./tools/tangle.js
    xxd -i core.jo > core.h
}

build() {
    $cc $w $f $l $o jojo.c -o jojo
}

fast_build() {
    $cc $w $f $l jojo.c -o jojo
}

debug_build() {
    $cc $w $f $l $d jojo.c -o jojo
}

clean() {
    rm -f jojo
    rm -f core.jojo.*
}

run() {
    ./jojo
}

default() {
    clean
    tangle
    time build
}

install_modules_to_default_usr_jojo_dir () {
    copy modules ~/.jojo/
}

remove_all_modules_in_default_usr_jojo_dir () {
    rm -rf ~/.jojo/modules/
}

fast() {
    clean
    tangle
    time fast_build
}

if [ $# -eq 0 ]; then
    default
else
    $1
fi
