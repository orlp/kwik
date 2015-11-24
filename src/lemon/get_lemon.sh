#!/usr/bin/env bash
pushd `dirname "$0"`
rm -f lemon.c lempar.c
wget https://raw.githubusercontent.com/mackyle/sqlite/master/tool/lempar.c
wget https://raw.githubusercontent.com/mackyle/sqlite/master/tool/lemon.c
patch < lemon.patch
popd
