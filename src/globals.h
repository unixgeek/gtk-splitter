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

/* This file contains global definitions.*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <gtk/gtk.h>

#define UPDATE_INTERVAL 32768  /*Update the progress bar every 32k.*/

/*Session_data is a struct used to store information related the selected file.*/
typedef struct
  {
    gchar *filename_and_path;
    gushort fp_length;
    gchar *filename_only;
    gushort f_length;
    gfloat entry;
    gulong chunk_size;
    enum {BYTES, KILOBYTES, MEGABYTES} unit;
    gboolean split;
    gboolean create_batchfile;
    gchar *home_dir;
    gchar *output_dir;
    gboolean verify;
  } session_data;

/*gtk_splitter_window is a struct for storing widgets related to the main
  window of gtk-splitter.*/
typedef struct
  {
    GtkWidget *base_window;
    GtkWidget *base_box;
    GtkWidget *box1;
    GtkWidget *box2;
    GtkWidget *box3;
    GtkWidget *box4;
    GtkWidget *box5; 
    GtkWidget *open_button;
    GtkWidget *output_button;
    GtkWidget *batch_file_button;
    GtkWidget *split_button;
    GtkWidget *combine_button;
    GtkWidget *start_button;
    GtkWidget *verify_button;
    GtkWidget *chunk_size_units;
    GtkWidget *units_menu;
    GtkWidget *bytesopt;
    GtkWidget *kbytesopt;
    GtkWidget *mbytesopt;
    GtkWidget *filename_box;
    GtkWidget *output_box;
    GtkAdjustment *size_input_adj;
    GtkWidget *size_input;
    GtkWidget *selection_dialog;
    session_data *sdata;
  } gtk_splitter_window;

#endif /* GLOBALS_H */
