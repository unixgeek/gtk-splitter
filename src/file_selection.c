/*
 * file_selection.c
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
#include <string.h>
#include "globals.h"
#include "file_selection.h"
#include "error.h"

/*A dialog for getting a file from the user.*/
void choose_file(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   /*Set up a simple gtk file selection dialog.*/
   gsw->selection_dialog = gtk_file_selection_new( "Choose a file." );
   /*Default the dialog to the user's home directory.*/
   gtk_file_selection_set_filename( GTK_FILE_SELECTION( gsw->selection_dialog ),
                                    gsw->sdata->home_dir );
   /*Hide the create, delete, and rename buttons.*/
   gtk_file_selection_hide_fileop_buttons( GTK_FILE_SELECTION( gsw->selection_dialog ) );

   /*Connect some signals to the dialog.*/
   gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( gsw->selection_dialog ) ->ok_button ),
	                     "clicked", GTK_SIGNAL_FUNC( setfilename ), gsw );
   gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION( gsw->selection_dialog ) ->ok_button ),
	                            "clicked", GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                             ( gpointer ) gsw->selection_dialog);
   gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION( gsw->selection_dialog ) ->cancel_button ),
	                           "clicked", GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                             ( gpointer ) gsw->selection_dialog );

   /*Display the dialog.*/
   gtk_widget_show( gsw->selection_dialog );
}

/*This function is called from choose_file() and sets the appropriate data members of session_data.*/
void setfilename(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gchar *selcted_file;
   gushort i, j, path_only_count;

   i = 0;
   j = 0;

 
   /*Get the selected file name from the dialog.*/
   selcted_file = gtk_file_selection_get_filename( GTK_FILE_SELECTION( gsw->selection_dialog ) );

   /* Copy the selected file name (full path). */
   strcpy(gsw->sdata->filename_and_path, selcted_file);

   /*The path only length is going to be AT MOST the length of the file name and path.*/
   path_only_count = strlen( gsw->sdata->filename_and_path );

   /*Start at the end and count backwards till we find a '/'.*/
   while ( gsw->sdata->filename_and_path[path_only_count] != '/' )
     path_only_count--;

   /*Add the count of the last '/'.*/
   path_only_count++;

   /*Copy the filename only.*/
   for ( i = path_only_count; i != strlen( gsw->sdata->filename_and_path ); i++ )
     {
       gsw->sdata->filename_only[j] = gsw->sdata->filename_and_path[i];
       j++;
     }

   /*Set the filename in the entry box.*/
   gtk_entry_set_text( GTK_ENTRY( gsw->filename_box ), gsw->sdata->filename_only );

   /*Set our gui so that all of our buttons active.*/
   gtk_widget_set_sensitive( gsw->split_button, TRUE );
   gtk_widget_set_sensitive( gsw->combine_button, TRUE );
   gtk_widget_set_sensitive( gsw->batch_file_button, TRUE );
   gtk_widget_set_sensitive( gsw->size_input, TRUE );
   gtk_widget_set_sensitive( gsw->chunk_size_units, TRUE );
   gtk_widget_set_sensitive( gsw->start_button, TRUE );
   gtk_widget_set_sensitive( gsw->verify_button, TRUE );

}

/*A dialog for getting the output directory from the user.*/
void choose_directory(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   /*Set up a simple gtk file selection dialog.*/
   gsw->selection_dialog = gtk_file_selection_new( "Choose the output directory." );
   /*Default the dialog to the last chosen output directory.
     (On first run, output_dir is set to the user's home directory.)*/
   gtk_file_selection_set_filename( GTK_FILE_SELECTION( gsw->selection_dialog ),
                                    gsw->sdata->output_dir );
   /*Hide the create, delete, and rename buttons.*/
   gtk_file_selection_hide_fileop_buttons( GTK_FILE_SELECTION( gsw->selection_dialog ) );

   /*Disable the ability to select files.*/
   gtk_widget_set_sensitive( GTK_FILE_SELECTION( gsw->selection_dialog )->file_list, FALSE );
   /*Connect some signals to the dialog.*/
   gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( gsw->selection_dialog )->ok_button ),
	                     "clicked", GTK_SIGNAL_FUNC( setdirname ), gsw);
   gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION( gsw->selection_dialog )->ok_button ),
	                            "clicked", GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                              ( gpointer ) gsw->selection_dialog );
   gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION( gsw->selection_dialog )->cancel_button ),
	                            "clicked", GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                              ( gpointer ) gsw->selection_dialog );

   /*Display the dialog.*/
   gtk_widget_show( gsw->selection_dialog );
}

/*This function is called from choose_dir() and sets the output directory in session_data.*/
void setdirname(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gchar *selected_dir;

   /* Get the selected directory name from the user using the standard GTK+ file selection dialog. */
   selected_dir = gtk_file_selection_get_filename( GTK_FILE_SELECTION( gsw->selection_dialog ) );

   /* Set output_dir to selected_dir. */
   strcpy( gsw->sdata->output_dir, selected_dir );

   /* Display the new directory in the main window. */
   gtk_entry_set_text( GTK_ENTRY( gsw->output_box ), gsw->sdata->output_dir );
}
