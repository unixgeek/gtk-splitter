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

/*For choosing a file with the gtk open dialog.*/
void choose_file(GtkWidget *, gtk_splitter_window *);

/*Sets the filename in session_data to the selected file from choose_file().*/
void setfilename(GtkWidget *, gtk_splitter_window *);

/*For choosing a directory with the gtk open dialog.*/
void setdirname(GtkWidget *, gtk_splitter_window *);

/*Sets the output_dir in session_data to the selected directory from choose_directory().*/
void choose_directory(GtkWidget *, gtk_splitter_window *);

#endif /* FILE_SELECTION_H */
