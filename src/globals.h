/*
 * globals.h
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

/* This file contains global definitions. */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <gtk/gtk.h>
#include <limits.h>

/* Update the progress bar every 32k. */
#define UPDATE_INTERVAL 32768  

/* session_data is a struct containing information related to a selected file. */
typedef struct
  {
    gchar file_name_and_path[PATH_MAX];
    gchar file_name_only[PATH_MAX];
    gfloat entry;
    enum {BYTES, KILOBYTES, MEGABYTES} unit;
    gboolean split;
    gboolean create_batchfile;
    gchar *home_directory;
    gchar output_directory[PATH_MAX];
    gboolean verify;
  } session_data;

/* gtk_splitter_window is a struct for storing widgets related to the main
   window of gtk-splitter. */
typedef struct
  {
    GtkWidget *base_window;
    GtkWidget *base_box;
    GtkWidget *box1;
    GtkWidget *box2;
    GtkWidget *box3;
    GtkWidget *box4;
    GtkWidget *box5;
    GtkWidget *custom_start_button;
    GtkWidget *custom_start_button_box;
    GtkWidget *custom_start_button_alignment;
    GtkWidget *custom_start_button_image;
    GtkWidget *custom_start_button_label;
    GtkWidget *open_button;
    GtkWidget *output_button;
    GtkWidget *batch_file_button;
    GtkWidget *split_button;
    GtkWidget *combine_button;
    GtkWidget *verify_button;
    GtkWidget *chunk_size_units;
    GtkWidget *units_menu;
    GtkWidget *unit_bytes;
    GtkWidget *unit_kilobytes;
    GtkWidget *unit_megabytes;
    GtkWidget *file_name_box;
    GtkWidget *output_box;
    GtkAdjustment *size_input_adj;
    GtkWidget *size_input;
    GtkWidget *file_selection_dialog;
    GdkPixbuf *icon;
    session_data my_session_data;
  } gtk_splitter_window;

#endif /* GLOBALS_H */
