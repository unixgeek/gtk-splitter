#!/bin/sh

make clean
rm -f config.status config.cache config.log configure Makefile.in Makefile aclocal.m4 confdefs.h gtk-splitter.spec build_rpm.sh build_tgz.sh build_deb.sh
rm -f *.rpm
rm -f *.tgz
rm -f *.deb
rm -f src/Makefile.in
rm -f src/Makefile
rm -fr src/.deps
rm -fr autom4te*

