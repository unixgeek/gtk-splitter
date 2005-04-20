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

#include <glib.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "globals.h"
#include "interface.h"
#include "file_selection.h"

/* For choosing a file with the gtk open dialog. */ //remove file_selection_dialog
void get_file_name_dialog( GtkWidget *widget, GtkSplitterWindow *gsw )
{
   gchar *dir;
   dir = g_malloc( strlen (gsw->session_data->home_directory) * sizeof(gchar) + 1 );
   g_stpcpy( dir, gsw->session_data->home_directory );
   strcat(dir, "/");
   
   /* Set up a simple gtk file selection dialog. */
   gsw->file_selection_dialog = gtk_file_selection_new( "Select a file." );

   /* Default the dialog to the user's home directory. */
   gtk_file_selection_set_filename( GTK_FILE_SELECTION( gsw->file_selection_dialog ), dir );
   
   /* Hide the create, delete, and rename buttons. */
   gtk_file_selection_hide_fileop_buttons( GTK_FILE_SELECTION( gsw->file_selection_dialog ) );

   /* Connect some signals to the dialog. */
   g_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog ) ->ok_button ),
	                  "clicked", G_CALLBACK( get_file_name ), ( gpointer ) gsw );
   
   g_signal_connect_swapped( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog ) ->ok_button ),
	                          "clicked", G_CALLBACK( gtk_widget_destroy ),
                             ( gpointer ) gsw->file_selection_dialog);
   
   g_signal_connect_swapped( GTK_OBJECT(GTK_FILE_SELECTION( gsw->file_selection_dialog ) ->cancel_button ),
	                          "clicked", G_CALLBACK( gtk_widget_destroy ),
                             ( gpointer ) gsw->file_selection_dialog );

   /* Display the dialog. */
   gtk_widget_show( gsw->file_selection_dialog );

   g_free( dir );
}

/* Gets the file name from get_file_name_dialog(). */
void get_file_name( GtkWidget *widget, GtkSplitterWindow *gsw )
{
   gchar *selected_file;
   
   /* Get the selected file name from the dialog. */
   selected_file = ( gchar * ) gtk_file_selection_get_filename( GTK_FILE_SELECTION( gsw->file_selection_dialog ) );
   
   set_file_name( gsw, selected_file );
}

/* Sets the file name in the GUI and session_data struct. */
void set_file_name( GtkSplitterWindow *gsw, gchar *file_name_to_set )
{
   gchar *pos;
   struct stat file_information;
   
   if ( stat( file_name_to_set, &file_information )  == -1 )
   {
       display_error( "file_selection.c:  Could not stat file." );
   }
   /* Copy the selected file name (full path). */
   g_stpcpy( gsw->session_data->file_name_and_path, file_name_to_set );

   pos = g_strrstr( gsw->session_data->file_name_and_path, "/" );
   
   /* Increment past the '/'. */
   pos++; 
   
   if ( pos != NULL )
       g_stpcpy( gsw->session_data->file_name_only, pos );
   else
       g_stpcpy( gsw->session_data->file_name_only, gsw->session_data->file_name_and_path );
   
    /* Set the filename in the entry box. */
   gtk_entry_set_text( GTK_ENTRY( gsw->file_name_box ), gsw->session_data->file_name_only );

   /* Set the gui so that all of the buttons active. */
   gtk_widget_set_sensitive( gsw->split_button, TRUE );
   gtk_widget_set_sensitive( gsw->combine_button, TRUE );
   gtk_widget_set_sensitive( gsw->batch_file_button, TRUE );
   gtk_widget_set_sensitive( gsw->size_input, TRUE );
   gtk_widget_set_sensitive( gsw->chunk_size_units, TRUE );
   gtk_widget_set_sensitive( gsw->custom_start_button, TRUE );
#ifdef HAVE_LIBMHASH
   gtk_widget_set_sensitive( gsw->verify_button, TRUE );
#endif
}

/* For choosing a directory with the gtk open dialog. */
void get_directory_name_dialog( GtkWidget *widget, GtkSplitterWindow *gsw )
{
   gchar *dir;
   dir = g_malloc( strlen (gsw->session_data->output_directory) * sizeof(gchar) + 1 );
   g_stpcpy( dir, gsw->session_data->output_directory );
   strcat(dir, "/");

   /* Set up a simple gtk file selection dialog. */
   gsw->file_selection_dialog = gtk_file_selection_new( "Select the output directory." );
   
   /* Default the dialog to the last chosen output directory.
      (On first run, output_directory is set to the user's home directory.) */
   gtk_file_selection_set_filename ( GTK_FILE_SELECTION( gsw->file_selection_dialog ), dir );
   
   /* Hide the create, delete, and rename buttons. */
   gtk_file_selection_hide_fileop_buttons( GTK_FILE_SELECTION( gsw->file_selection_dialog ) );

   /* Disable the ability to select files. */
   gtk_widget_set_sensitive( GTK_FILE_SELECTION( gsw->file_selection_dialog )->file_list, FALSE );
   
   /* Connect some signals to the dialog. */
   g_signal_connect( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog )->ok_button ),
	                  "clicked", G_CALLBACK( get_directory_name ), ( gpointer ) gsw);
   
   g_signal_connect_swapped( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog )->ok_button ),
	                          "clicked", G_CALLBACK( gtk_widget_destroy ),
                             ( gpointer ) gsw->file_selection_dialog );
                             
   g_signal_connect_swapped( GTK_OBJECT( GTK_FILE_SELECTION( gsw->file_selection_dialog )->cancel_button ),
	                          "clicked", G_CALLBACK( gtk_widget_destroy ),
                             ( gpointer ) gsw->file_selection_dialog );

   /* Display the dialog. */
   gtk_widget_show( gsw->file_selection_dialog );

   g_free( dir );
}

/* Gets the directory name from get_file_name_dialog(). */
void get_directory_name( GtkWidget *widget, GtkSplitterWindow *gsw )
{
   gchar *selected_directory;
   
   /* Get the selected directory name from the user using the standard GTK+ file selection dialog. */
   selected_directory = ( gchar * ) gtk_file_selection_get_filename( GTK_FILE_SELECTION( gsw->file_selection_dialog ) );
 
   set_directory_name( gsw, selected_directory );
}

/* Sets the directory name in the GUI and session_data struct. */
void set_directory_name( GtkSplitterWindow *gsw, gchar *directory_name_to_set )
{
   /* Set output_directory to selected_dir. */
   g_stpcpy( gsw->session_data->output_directory, directory_name_to_set );

   /* Display the new directory in the main window. */
   gtk_entry_set_text( GTK_ENTRY( gsw->output_box ), gsw->session_data->output_directory );
}
