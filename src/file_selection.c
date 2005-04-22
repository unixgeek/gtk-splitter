/*
 * $Id: file_selection.c,v 1.21 2005/04/18 04:38:46 techgunter Exp $
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
#include <sys/types.h>
#include "window.h"
#include "session.h"
#include "interface.h"
#include "file_selection.h"

void
get_file_name_dialog (GtkWidget * widget, GtkSplitterWindow * gsw)
{
  gchar *selected_file;
  GtkWidget *file_selection_dialog;
  
  /* Set up a simple gtk file selection dialog. */
  file_selection_dialog = gtk_file_chooser_dialog_new (
    "Choose a file",
    GTK_WINDOW (gsw->base_window),
    GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    NULL);

  gtk_file_chooser_set_current_folder (
    GTK_FILE_CHOOSER (file_selection_dialog),
    gsw->session_data->home_directory);
    
  if (gtk_dialog_run (GTK_DIALOG (file_selection_dialog))
      == GTK_RESPONSE_ACCEPT)
  {
    selected_file = gtk_file_chooser_get_filename (
      GTK_FILE_CHOOSER (file_selection_dialog));
    
    g_free (gsw->session_data->file_name_and_path);
    g_free (gsw->session_data->file_name_only);
    
    /* Copy the selected file name (full path). */
    gsw->session_data->file_name_and_path = selected_file;

    gsw->session_data->file_name_only = g_path_get_basename (selected_file);
    gsw->session_data->home_directory = g_path_get_dirname (selected_file);

    /* Set the filename in the entry box. */
    gtk_entry_set_text (GTK_ENTRY (gsw->file_name_box),
                        gsw->session_data->file_name_only);
  
    /* Set the gui so that all of the buttons active. */
    gtk_widget_set_sensitive (gsw->split_button, TRUE);
    gtk_widget_set_sensitive (gsw->combine_button, TRUE);
    gtk_widget_set_sensitive (gsw->batch_file_button, TRUE);
    gtk_widget_set_sensitive (gsw->size_input, TRUE);
    gtk_widget_set_sensitive (gsw->units_menu, TRUE);
    gtk_widget_set_sensitive (gsw->start_button, TRUE);
#ifdef HAVE_LIBMHASH
    gtk_widget_set_sensitive (gsw->verify_button, TRUE);
#endif
  }
  
  gtk_widget_destroy (file_selection_dialog);
}

void
get_directory_name_dialog (GtkWidget * widget, GtkSplitterWindow * gsw)
{
   GtkWidget *file_selection_dialog;
   
  /* Set up a simple gtk file selection dialog. */
  file_selection_dialog = gtk_file_chooser_dialog_new (
    "Choose a directory",
    GTK_WINDOW (gsw->base_window),
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    NULL);

  gtk_file_chooser_set_current_folder (
    GTK_FILE_CHOOSER (file_selection_dialog),
    gsw->session_data->output_directory);
    
  if (gtk_dialog_run (GTK_DIALOG (file_selection_dialog))
      == GTK_RESPONSE_ACCEPT)
  {
    g_free(gsw->session_data->output_directory);
    
    gsw->session_data->output_directory = gtk_file_chooser_get_filename (
      GTK_FILE_CHOOSER (file_selection_dialog));
    
    /* Display the new directory in the main window. */
    gtk_entry_set_text (GTK_ENTRY (gsw->output_box), 
                        gsw->session_data->output_directory);
  }
  
  gtk_widget_destroy (file_selection_dialog);
}

