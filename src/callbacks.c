/*
 * callbacks.c
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

#include <gtk/gtk.h>
#include "callbacks.h"
#include "split.h"
#include "combine.h"
#include "globals.h"

/*Set the units to 'bytes'.*/
void set_bytes(GtkWidget *tmp, session_data *data)
{
   data->unit = BYTES;
}

/*Set the units to 'kilobytes'*/
void set_kbytes(GtkWidget *tmp, session_data *data)
{
   data->unit = KILOBYTES;
}

/*Set the units to 'megabytes'.*/
void set_mbytes(GtkWidget *tmp, session_data *data)
{
   data->unit = MEGABYTES;
}

/*When the value in the spin button is changed, store the value.*/
void set_data(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gsw->sdata->entry = gtk_spin_button_get_value_as_float( GTK_SPIN_BUTTON( gsw->size_input ) );
}

/*Initial settings for the session_data.*/
void initialize_session_data(session_data *data)
{
   /*Split is the default action.*/
   data->split = TRUE;

   /*Initialize dynamic array information.*/
   /*Our arrays are always cleared in setfilename().*/
   data->filename_and_path = NULL;
   data->filename_only = NULL;
   data->fp_length = 0;
   data->f_length = 0;

   /*Set the default chunk_size. (1.44MB)*/
   data->entry = 1457664;
   data->chunk_size = 1457664;
}

/*Initial settings for the main window.*/
void initialize_splitter_window(gtk_splitter_window *gsw)
{
   /*Insure that the split button is clicked.*/
   gtk_button_clicked( GTK_BUTTON( gsw->split_button ) );
   gtk_adjustment_set_value( gsw->size_input_adj , 1457664 );

   /*Don't create a dos batch file by default.*/
   gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( gsw->batch_file_button ), FALSE );
   gsw->sdata->create_batchfile = FALSE;

   /*Disable all buttons, except for the open button.
     This is so that nothing is done until a file is selected.*/
   gtk_widget_set_sensitive( gsw->split_button, FALSE );
   gtk_widget_set_sensitive( gsw->combine_button, FALSE );
   gtk_widget_set_sensitive( gsw->batch_file_button, FALSE );
   gtk_widget_set_sensitive( gsw->size_input, FALSE );
   gtk_widget_set_sensitive( gsw->chunk_size_units, FALSE );
   gtk_widget_set_sensitive( gsw->start_button, FALSE );

   /*Clear the filename in the entry box.*/
   gtk_entry_set_text( GTK_ENTRY( gsw->filename_box), "" );
}

/*Settings for the split action.*/
void toggle_split(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gsw->sdata->split = TRUE;

   /*Enable buttons related to the split action.*/
   gtk_widget_set_sensitive( gsw->batch_file_button, TRUE );
   gtk_widget_set_sensitive( gsw->size_input, TRUE );
   gtk_widget_set_sensitive( gsw->chunk_size_units, TRUE );
}

/*Settings for the combine action.*/
void toggle_combine(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gsw->sdata->split = FALSE;

   /*Disable buttons not related to the combine action.*/
   gtk_widget_set_sensitive( gsw->batch_file_button, FALSE );
   gtk_widget_set_sensitive( gsw->size_input, FALSE );
   gtk_widget_set_sensitive( gsw->chunk_size_units, FALSE );
}

/*Set the batcfile option.*/
void toggle_batch(GtkWidget *tmp, session_data *data)
{
   data->create_batchfile = !data->create_batchfile;
}

/*Start either the split or combine process.*/
void start(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gboolean do_initialization;

   /*Hide the main window.*/
   gtk_widget_hide_all( gsw->base_window );

   if ( gsw->sdata->split )
     do_initialization = split( tmp, gsw->sdata );
   else
     do_initialization = combine( tmp, gsw->sdata );

   /*If split or combine return TRUE, then reset the session_data and the main window.*/
   if ( do_initialization )
     {
       initialize_session_data( gsw->sdata );
       initialize_splitter_window( gsw );
     }

   /*Show the main window.*/
   gtk_widget_show_all( gsw->base_window );
}
