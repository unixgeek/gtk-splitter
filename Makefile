GNOME_LOC = /usr/share
BIN_DIR = /usr/bin

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
	install -s -m 755 gtk-splitter $(BIN_DIR)
	install -m 644 gtk-splitter.desktop $(GNOME_LOC)/gnome/apps/Utilities/
	install -m 644 gtk-splitter.xpm $(GNOME_LOC)/pixmaps

uninstall:
	rm -f $(BIN_DIR)/gtk-splitter
	rm -f $(GNOME_LOC)/gnome/apps/Utilities/gtk-splitter.desktop
	rm -f $(GNOME_LOC)/pixmaps/gtk-splitter.xpm

