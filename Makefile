
CC = gcc
LIBS = -lgtk -lgdk
OBJ = callbacks.o split.o combine.o error.o batchfile.o progress.o
LIBS_DIR = -L/usr/lib -L/usr/X11R6/lib
FLAGS = -Wall

all: gtk-splitter

gtk-splitter: gtk-splitter.c $(OBJ)
	$(CC) gtk-splitter.c $(OBJ) -o gtk-splitter\
	$(FLAGS) `gtk-config --cflags` $(LIBS_DIR) $(LIBS) 

callbacks.o: callbacks.c
	$(CC) -c callbacks.c $(FLAGS) `gtk-config --cflags`

split.o: split.c
	$(CC) -c split.c $(FLAGS) `gtk-config --cflags`

combine.o: combine.c
	$(CC) -c combine.c $(FLAGS) `gtk-config --cflags`

error.o: error.c
	$(CC) -c error.c $(FLAGS) `gtk-config --cflags`

batchfile.o: batchfile.c
	$(CC) -c batchfile.c $(FLAGS) `gtk-config --cflags`

progress.o: progress.c
	$(CC) -c progress.c $(FLAGS) `gtk-config --cflags`

clean: 
	rm -f *.o core gtk-splitter a.out

install:
	cp gtk-splitter /usr/bin/
	cp gtk-splitter.desktop /usr/share/gnome/apps/Utilities/
	cp gtk-splitter.xpm /usr/share/pixmaps

uninstall:
	rm -f /usr/bin/gtk-splitter
	rm -f /usr/share/gnome/apps/Utilities/gtk-splitter.desktop
	rm -f /usr/share/pixmaps/gtk-splitter.xpm

