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

#define DEBUG 0

#define UPDATE_INTERVAL 1024

#define GTK_SPLITTER_VERSION "gtk-splitter-0.9b Gunter Wambaugh"

/*Session_data is a structure used to store info on the selected file.*/
typedef struct
  {
    gchar *filename_and_path;
    gushort fp_length;
    gchar *filename_only;
    gushort f_length;
    gfloat entry;
    gulong chunk_size;
    gushort unit;
    gboolean split;
    gboolean create_batchfile;
    gchar *home_dir;
    gchar *output_dir;
  } session_data;

typedef struct
  {
/*Widgets for storing visible widgets.*/
    GtkWidget *base_window;
    GtkWidget *base_box;
    GtkWidget *box1;
    GtkWidget *box5; //Need to place in proper order.
    GtkWidget *box2;
    GtkWidget *box3;
    GtkWidget *box4;
/*Various buttons.*/
    GtkWidget *open_button;
    GtkWidget *output_button;
    GtkWidget *batch_file_button;
    GtkWidget *split_button;
    GtkWidget *combine_button;
    GtkWidget *start_button;
/*Widgets for a menu containing bytes, kilobytes, and megabytes.*/
    GtkWidget *chunk_size_units;
    GtkWidget *units_menu;
    GtkWidget *bytesopt;
    GtkWidget *kbytesopt;
    GtkWidget *mbytesopt;
    enum {BYTES, KBYTES, MBYTES};
/*Widgets to display the filename and output directory.*/
    GtkWidget *filename_box;
    GtkWidget *output_box;
/*Widgets for inputing chunk size.*/
    GtkAdjustment *size_input_adj;
    GtkWidget *size_input;
/*Widget for inputing a file name and a directory.*/
    GtkWidget *file_selector;
    GtkWidget *dir_selector;
    session_data *sdata;
  } gtk_splitter_window;

#endif /* GLOBALS_H */
