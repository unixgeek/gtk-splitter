/*
 * $Id: callbacks.c,v 1.18 2005/04/16 22:56:06 techgunter Exp $
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
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "callbacks.h"
#include "split.h"
#include "combine.h"
#include "session.h"
#include "window.h"
#include "interface.h"

/* Set the units to. */
void
set_unit (GtkWidget * widget, GtkSplitterWindow * gsw)
{
  gsw->session_data->unit = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));
  
  switch (gsw->session_data->unit) 
  {
    case BYTES:
      gtk_spin_button_set_digits(GTK_SPIN_BUTTON (gsw->size_input), 0);
      break;
    case KILOBYTES:
    case MEGABYTES:
      gtk_spin_button_set_digits(GTK_SPIN_BUTTON (gsw->size_input), 1);
      break;
  }
    
}

/* When the value in the spin button is changed, store the value. */
void
set_data (GtkWidget * widget, GtkSplitterWindow * gsw)
{
  switch (gsw->session_data->unit) 
  {
    case BYTES:
      gsw->session_data->entry =
        gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (gsw->size_input));
      break;
    case KILOBYTES:
    case MEGABYTES:
      gsw->session_data->entry =
        gtk_spin_button_get_value (GTK_SPIN_BUTTON (gsw->size_input));
      break;
  }
}

/* Initial settings for the session_data. */
void
initialize_session_data (GtkSplitterSessionData * gssd)
{
  g_free (gssd->file_name_and_path);
  g_free (gssd->file_name_only);
  
  /* Defaults.  Preserve user selection. */
  if (gssd->home_directory == NULL)
  {
    gssd->home_directory = (gchar *) g_get_home_dir ();
  }
  if (gssd->output_directory == NULL)
  {
    gssd->output_directory = (gchar *) g_get_home_dir ();
  }
  
  /* Split is the default action. */
  gssd->split = TRUE;

  /* Don't create a dos batch file by default. */
  gssd->create_batchfile = FALSE;
}

/* Initial settings for the main window.*/
void
initialize_splitter_window (GtkSplitterWindow * gsw)
{
  /* Insure that the split button is clicked. */
  gtk_button_clicked (GTK_BUTTON (gsw->split_button));

  /* Show the defauult output_directory. */
  gtk_entry_set_text (GTK_ENTRY (gsw->output_box),
                      gsw->session_data->home_directory);

  /* Don't create a dos batch file by default. */
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gsw->batch_file_button),
                                FALSE);

  gtk_combo_box_set_active (GTK_COMBO_BOX (gsw->units_menu), 0);
  
  gtk_spin_button_set_digits(GTK_SPIN_BUTTON (gsw->size_input), 0);
  
#ifdef HAVE_LIBMHASH
  /* Verify file by default. */
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gsw->verify_button), TRUE);
  gsw->session_data->verify = TRUE;
#endif

  /* Disable all buttons, except for the open button.
     This is so that nothing is done until a file is selected. */
  gtk_widget_set_sensitive (gsw->split_button, FALSE);
  gtk_widget_set_sensitive (gsw->combine_button, FALSE);
  gtk_widget_set_sensitive (gsw->batch_file_button, FALSE);
  gtk_widget_set_sensitive (gsw->size_input, FALSE);
  gtk_widget_set_sensitive (gsw->units_menu, FALSE);
  gtk_widget_set_sensitive (gsw->start_button, FALSE);
#ifdef HAVE_LIBMHASH
  gtk_widget_set_sensitive (gsw->verify_button, FALSE);
#endif

  /* Clear the filename in the entry box. */
  gtk_entry_set_text (GTK_ENTRY (gsw->file_name_box), "");
}

/* Settings for the split action. */
void
toggle_split (GtkWidget * widget, GtkSplitterWindow * gsw)
{
  gsw->session_data->split = TRUE;

  /* Enable buttons related to the split action. */
  gtk_widget_set_sensitive (gsw->batch_file_button, TRUE);
  gtk_widget_set_sensitive (gsw->size_input, TRUE);
  gtk_widget_set_sensitive (gsw->units_menu, TRUE);
}

/* Settings for the combine action. */
void
toggle_combine (GtkWidget * widget, GtkSplitterWindow * gsw)
{
  gsw->session_data->split = FALSE;

  /* Disable buttons not related to the combine action. */
  gtk_widget_set_sensitive (gsw->batch_file_button, FALSE);
  gtk_widget_set_sensitive (gsw->size_input, FALSE);
  gtk_widget_set_sensitive (gsw->units_menu, FALSE);
}

/* Set the batcfile option. */
void
toggle_batch (GtkWidget * widget, GtkSplitterSessionData * gssd)
{
  gssd->create_batchfile = !gssd->create_batchfile;
}

#ifdef HAVE_LIBMHASH
/* Set the verify option. */
void
toggle_verify (GtkWidget * widget, GtkSplitterSessionData * gssd)
{
  gssd->verify = !gssd->verify;
}
#endif

/* Start either the split or combine process. */
void
start_split_or_combine (GtkWidget * widget, GtkSplitterWindow * gsw)
{
  gboolean do_initialization;

  /* split() and combine() return a boolean value.
     This is used to recover from non-critical errors that may occur in split() or combine(). */
  if (gsw->session_data->split)
    do_initialization = gtk_splitter_split_file (gsw->session_data);
  else
    do_initialization = gtk_splitter_combine_files (gsw->session_data);

  /* If split or combine return TRUE, then reset the session_data and the main window. */
  if (do_initialization)
    {
      initialize_splitter_window (gsw);
      initialize_session_data (gsw->session_data);
    }
}
