SUBDIRS = src
EXTRA_DIST = gtk-splitter.spec		\
             gtk-splitter.desktop	\
             gtk-splitter.png		\
             depcomp			\
	     build_rpm.sh		\
             build_tgz.sh		\
             build_deb.sh		\
             slack-desc			\
             HOSTS                

gnome_menudir=$(prefix)/share/gnome/apps/Utilities
gnome_menu_DATA=gtk-splitter.desktop

icondir=$(prefix)/share/pixmaps
icon_DATA = gtk-splitter.png

docdir=$(prefix)/doc/@PACKAGE@-@VERSION@
doc_DATA = ChangeLog AUTHORS COPYING HOSTS INSTALL README TODO NEWS
