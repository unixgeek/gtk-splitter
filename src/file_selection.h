/*
 * file_selection.h
 *
 * Copyright 2001 Gunter Wambaugh
 *
 * This file is part of gtk-splitter.
 *
 * gtk-splitter is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gtk-splitter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gtk-splitter; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef FILE_SELECTION_H
#define FILE_SELECTION_H

#include <gtk/gtk.h>
#include "globals.h"

/* For choosing a file with the gtk open dialog. */
void get_file_name_dialog(GtkWidget *, gtk_splitter_window *);

/* Gets the file name from get_file_name_dialog(). */
void get_file_name(GtkWidget *, gtk_splitter_window *);

/* Sets the file name in the GUI and session_data struct. */
void set_file_name(gtk_splitter_window *, gchar *);

/* For choosing a directory with the gtk open dialog. */
void get_directory_name_dialog(GtkWidget *, gtk_splitter_window *);

/* Gets the directory name from get_file_name_dialog(). */
void get_directory_name(GtkWidget *, gtk_splitter_window *);

/* Sets the directory name in the GUI and session_data struct. */
void set_directory_name(gtk_splitter_window *, gchar *);

#endif /* FILE_SELECTION_H */
