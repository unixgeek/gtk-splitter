#!/bin/bash
#
# [ 1.4.03 | Gunter Wambaugh ]
# Script for building a binary and source rpm. 

# Create a temporary local RPM build environment.
mkdir -p --verbose /tmp/gtk-splitter_rpms/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
mkdir -p --verbose /tmp/gtk-splitter_rpms/RPMS/{i386,noarch}
echo "%_topdir /tmp//gtk-splitter_rpms" > ~/.rpmmacros

# Generate a distribution tarball.
./configure
make dist

# Build the RPMs.
rpm -ta --clean gtk-splitter-*.tar.gz

# Put the RPMs in this directory.
mv -f --verbose /tmp/gtk-splitter_rpms/RPMS/i386/* .
mv -f --verbose /tmp/gtk-splitter_rpms/SRPMS/* .

# Remove the RPM build environment.
rm -fr --verbose /tmp/gtk-splitter_rpms
