#!/usr/bin/env bash
$EXTRACTRC `find . -name \*.rc` >> rc.cpp
$XGETTEXT `find . -name '*.cpp'` -o $podir/kubuntu-debug-installer.pot
rm -f *.cpp
