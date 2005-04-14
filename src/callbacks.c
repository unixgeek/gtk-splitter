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
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "callbacks.h"
#include "split.h"
#include "combine.h"
#include "session.h"
#include "window.h"
#include "interface.h"

/* Set the units to 'bytes'. */
void
set_unit_bytes (GtkWidget * widget, GtkSplitterSessionData * gssd)
{
  gssd->unit = BYTES;
}

/* Set the units to 'kilobytes'. */
void
set_unit_kilobytes (GtkWidget * widget, GtkSplitterSessionData * gssd)
{
  gssd->unit = KILOBYTES;
}

/* Set the units to 'megabytes'. */
void
set_unit_megabytes (GtkWidget * widget, GtkSplitterSessionData * gssd)
{
  gssd->unit = MEGABYTES;
}

/* When the value in the spin button is changed, store the value. */
void
set_data (GtkWidget * widget, GtkSplitterWindow * gsw)
{
  gsw->session_data->entry =
    gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON (gsw->size_input));
}

/* Initial settings for the session_data. */
void
initialize_session_data (GtkSplitterSessionData * gssd)
{
  gint i;
  gchar *home;

  /* Clear the strings. */
  for (i = 0; i != PATH_MAX; i++)
    {
      gssd->file_name_only[i] = '\0';
      gssd->file_name_and_path[i] = '\0';
    }


  /* Store the path to the user's home directory. */
  home = getenv ("HOME");
  if (home == NULL)
    {
      display_error ("Could not determine home directory.\n"
                     "Check environment variables for $HOME.");

      /* Default to '/' as the user's home directory. */
      sprintf (gssd->home_directory, "/");
    }
  else
    {
      sprintf (gssd->home_directory, "%s/", home);
    }

  /* Set the default output directory to the user's home directory. */
  g_stpcpy (gssd->output_directory, gssd->home_directory);

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
  gtk_widget_set_sensitive (gsw->chunk_size_units, FALSE);
  gtk_widget_set_sensitive (gsw->custom_start_button, FALSE);
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
  gtk_widget_set_sensitive (gsw->chunk_size_units, TRUE);
}

/* Settings for the combine action. */
void
toggle_combine (GtkWidget * widget, GtkSplitterWindow * gsw)
{
  gsw->session_data->split = FALSE;

  /* Disable buttons not related to the combine action. */
  gtk_widget_set_sensitive (gsw->batch_file_button, FALSE);
  gtk_widget_set_sensitive (gsw->size_input, FALSE);
  gtk_widget_set_sensitive (gsw->chunk_size_units, FALSE);
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

  /* Hide the main window. */
  gtk_widget_hide_all (gsw->base_window);

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

  /* Show the main window. */
  gtk_widget_show_all (gsw->base_window);
}
