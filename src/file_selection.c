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

/* For choosing a file with the gtk open dialog. */ //remove file_selection_dialog
void get_file_name_dialog(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   /* Set up a simple gtk file selection dialog. */
   gsw->file_selection_dialog = gtk_file_selection_new( "Choose a file." );
   
   /* Default the dialog to the user's home directory. */
   gtk_file_selection_set_filename( GTK_FILE_SELECTION( gsw->file_selection_dialog ),
                                    gsw->my_session_data.home_directory );
   
   /* Hide the create, delete, and rename buttons. */
   gtk_file_selection_hide_fileop_buttons( GTK_FILE_SELECTION( gsw->file_selection_dialog ) );

   /* Connect some signals to the dialog. */
   gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog ) ->ok_button ),
	                     "clicked", GTK_SIGNAL_FUNC( get_file_name ), gsw );
   gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog ) ->ok_button ),
	                            "clicked", GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                             ( gpointer ) gsw->file_selection_dialog);
   gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION( gsw->file_selection_dialog ) ->cancel_button ),
	                           "clicked", GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                             ( gpointer ) gsw->file_selection_dialog );

   /*Display the dialog.*/
   gtk_widget_show( gsw->file_selection_dialog );
}

/* Gets the file name from get_file_name_dialog(). */ //gsw
void get_file_name(GtkWidget *tmp, gtk_splitter_window *gsw )
{
   gchar *selected_file;
   
   /* Get the selected file name from the dialog. */
   selected_file = gtk_file_selection_get_filename( GTK_FILE_SELECTION( gsw->file_selection_dialog ) );
   
   set_file_name( gsw, selected_file );
}

/* Sets the file name in the GUI and session_data struct. */ //stat
void set_file_name(gtk_splitter_window *gsw, gchar *file_name_to_set)
{
   
   gushort i, j, path_only_count;

   i = 0;
   j = 0;


   /* Copy the selected file name (full path). */
   strcpy(gsw->my_session_data.file_name_and_path, file_name_to_set);

   /* The path only length is going to be AT MOST the length of the file name and path. */
   path_only_count = strlen( gsw->my_session_data.file_name_and_path );

   /* Start at the end and count backwards till we find a '/'. */
   while ( ( gsw->my_session_data.file_name_and_path[path_only_count] != '/' ) && ( path_only_count != 0 ) )
     path_only_count--;

   /* Add the count of the last '/'. */
   if ( path_only_count != 0 )
     path_only_count++;

   /* Copy the file name only. */
   for ( i = path_only_count; i != strlen( gsw->my_session_data.file_name_and_path ); i++ )
     {
       gsw->my_session_data.file_name_only[j] = gsw->my_session_data.file_name_and_path[i];
       j++;
     }

   /* Set the filename in the entry box. */
   gtk_entry_set_text( GTK_ENTRY( gsw->file_name_box ), gsw->my_session_data.file_name_only );

   /* Set the gui so that all of the buttons active. */
   gtk_widget_set_sensitive( gsw->split_button, TRUE );
   gtk_widget_set_sensitive( gsw->combine_button, TRUE );
   gtk_widget_set_sensitive( gsw->batch_file_button, TRUE );
   gtk_widget_set_sensitive( gsw->size_input, TRUE );
   gtk_widget_set_sensitive( gsw->chunk_size_units, TRUE );
   gtk_widget_set_sensitive( gsw->start_button, TRUE );
   gtk_widget_set_sensitive( gsw->verify_button, TRUE );

}

/* For choosing a directory with the gtk open dialog. */
void get_directory_name_dialog(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   /* Set up a simple gtk file selection dialog. */
   gsw->file_selection_dialog = gtk_file_selection_new( "Choose the output directory." );
   
   /* Default the dialog to the last chosen output directory.
      (On first run, output_directory is set to the user's home directory.) */
   gtk_file_selection_set_filename( GTK_FILE_SELECTION( gsw->file_selection_dialog ),
                                    gsw->my_session_data.output_directory );
   
   /* Hide the create, delete, and rename buttons. */
   gtk_file_selection_hide_fileop_buttons( GTK_FILE_SELECTION( gsw->file_selection_dialog ) );

   /* Disable the ability to select files. */
   gtk_widget_set_sensitive( GTK_FILE_SELECTION( gsw->file_selection_dialog )->file_list, FALSE );
   
   /* Connect some signals to the dialog. */
   gtk_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog )->ok_button ),
	                     "clicked", GTK_SIGNAL_FUNC( get_directory_name ), gsw);
   gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog )->ok_button ),
	                            "clicked", GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                              ( gpointer ) gsw->file_selection_dialog );
   gtk_signal_connect_object( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog )->cancel_button ),
	                            "clicked", GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                              ( gpointer ) gsw->file_selection_dialog );

   /* Display the dialog. */
   gtk_widget_show( gsw->file_selection_dialog );
}

/* Gets the directory name from get_file_name_dialog(). */
void get_directory_name(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gchar *selected_directory;
   
  
   /* Get the selected directory name from the user using the standard GTK+ file selection dialog. */
   selected_directory = gtk_file_selection_get_filename( GTK_FILE_SELECTION( gsw->file_selection_dialog ) );
 
   set_directory_name( gsw, selected_directory );
}

/* Sets the directory name in the GUI and session_data struct. */
void set_directory_name(gtk_splitter_window *gsw, gchar *directory_name_to_set)
{
 
   /* Set output_directory to selected_dir. */
   strcpy( gsw->my_session_data.output_directory, directory_name_to_set );

   /* Display the new directory in the main window. */
   gtk_entry_set_text( GTK_ENTRY( gsw->output_box ), gsw->my_session_data.output_directory );
}
