/* 
 * callbacks.h
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

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>
#include "globals.h"

/* Tell our program which units to use: bytes, kilobytes, or megabytes. */
void set_unit_bytes(GtkWidget *, session_data *);

void set_unit_kilobytes(GtkWidget *, session_data *);

void set_unit_megabytes(GtkWidget *, session_data *);

/* Reads the chunksize inputed in the spin box. */
void set_data(GtkWidget *, gtk_splitter_window *);

/* Tell our program to split the selected file. */
void toggle_split(GtkWidget *, gtk_splitter_window *);

/* Tell our program to combine the selected file. */
void toggle_combine(GtkWidget *, gtk_splitter_window *);

/* Toggle whether or not we will create a dos batch file. */
void toggle_batch(GtkWidget *, session_data *);

/* Toggle whether or not we will verify the file. */
void toggle_verify(GtkWidget *, session_data *);

/* Initial settings for session_data. */
void initialize_session_data(session_data *);

/* Initial settings for the main window. */
void initialize_splitter_window(gtk_splitter_window *);

/* Start the split/combine process. */
void start(GtkWidget *, gtk_splitter_window *);

#endif /* CALLBACKS_H */
