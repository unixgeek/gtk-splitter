# gtk-splitter
gtk-splitter is a GTK+ application that splits/combines files. gtk-splitter is useful in working around large-file barriers (i.e. floppy disks and email attachments). gtk-splitter can also create a DOS batch file for combining files on Windows/DOS system

## Background
This is a very old open source project I started in college (I think). I was the "tech" guy for my friends and family and I worked as a PC Tech for the college and it was not unusual back then for people to not have internet access or CD-ROM devices, so I wrote this mainly as a way to get installers, device drivers, etc. split to fit on to floppy disks. It was also a way to learn C, GTK+, and GNU Automake. I didn't expect anyone to actually use it other than myself, but I was pleasantly surprised to get emails from many people who found it useful. The original code is still hosted at [sourceforge](https://sourceforge.net/projects/gtk-splitter). The code hosted here is an import from CVS using reposurgeon. 

## Build
There are additional instructions in the old README and INSTALL files.

1. `./autogen.sh`
1. `./configure`
1. `make`