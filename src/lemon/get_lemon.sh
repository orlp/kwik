#!/usr/bin/env bash
pushd `dirname "$0"`
wget https://raw.githubusercontent.com/mackyle/sqlite/master/tool/lempar.c
wget https://raw.githubusercontent.com/mackyle/sqlite/master/tool/lemon.c
patch < lemon.patch
popd
