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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define DEBUG 0

#define UPDATE_INTERVAL 1024

#define GTK_SPLITTER_VERSION "gtk-splitter-0.9a Gunter Wambaugh"

GtkWidget *split_button, *combine_button, *start_button, *filename_box, *chunk_size_units,
	  *bytesopt, *kbytesopt, *mbytesopt, *size_input, *file_selector, *batch_file_button;	
GtkAdjustment *size_input_adj;

gchar *home_dir;
enum {BYTES, KBYTES, MBYTES};

typedef struct
  {
    GtkWidget *window;
    gchar *filename_and_path;
    gushort fp_length;
    gchar *filename_only;
    gushort f_length;
    gfloat entry;
    gulong chunk_size;
    gushort unit;
    gboolean split;
    gboolean create_batchfile;
  } session_data;

#endif /* __GLOBALS_H__ */
