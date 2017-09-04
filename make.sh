#!/bin/sh

#cc=gcc
cc=clang
w='-Wno-int-conversion -Wno-incompatible-pointer-types -Wno-return-type -Wunused-value'
o=''
f='-rdynamic -g'
l='-ldl'

build() {
    ./tool/tangle.js
    xxd -i core.jo > core.h
    $cc $w $o $f $l jojo.c -o jojo
}

build

# if [ $# -eq 0 ]; then
#     default
# else
#     $1
# fi
