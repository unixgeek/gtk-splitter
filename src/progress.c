/*
 * $Id: progress.c,v 1.17 2005/04/22 16:17:21 techgunter Exp $
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
#include "progress.h"
#include "callbacks.h"
#include "interface.h"

ProgressWindow *
progress_window_new ()
{
  ProgressWindow *pw;

  pw = g_malloc (sizeof (ProgressWindow));

  if (pw == NULL)
    {
      return pw;
    }

  pw->cancelled = FALSE;
  
  /* Set up the window. */
  pw->base_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_resizable (GTK_WINDOW (pw->base_window), FALSE);
  gtk_window_set_modal (GTK_WINDOW (pw->base_window), TRUE);

  /* Set up the table. */
  pw->table = gtk_table_new (5, 2, FALSE);

  /* Set up the progress bars. */
  pw->current_progress = gtk_progress_bar_new ();
  pw->total_progress = gtk_progress_bar_new ();

  /* Set up the labels. */
  pw->file_label = gtk_label_new ("Current File:  ");
  gtk_misc_set_alignment (GTK_MISC (pw->file_label), 0, 0.5);

  pw->current_progress_label = gtk_label_new ("Current Progress:  ");
  gtk_misc_set_alignment (GTK_MISC (pw->current_progress_label), 0, 0.5);

  pw->total_progress_label = gtk_label_new ("Total Progress:  ");
  gtk_misc_set_alignment (GTK_MISC (pw->total_progress_label), 0, 0.5);

  pw->message = gtk_label_new ("");

  /* Set up a separator. */
  pw->separator = gtk_hseparator_new ();
  
  /* Cancel button. */
  pw->cancel_button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);

  /* Add the widgets to the dialog. */
  gtk_container_add (GTK_CONTAINER (pw->base_window), pw->table);
  /* Row 0 */
  gtk_table_attach (GTK_TABLE (pw->table), pw->file_label, 0, 1, 0, 1,
                    GTK_FILL, GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (pw->table), pw->message, 1, 2, 0, 1, GTK_FILL,
                    GTK_FILL, 0, 0);
  /* Row 1 */
  gtk_table_attach (GTK_TABLE (pw->table), pw->current_progress_label, 0, 1,
                    1, 2, GTK_FILL, GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (pw->table), pw->current_progress, 1, 2, 1, 2,
                    GTK_FILL, GTK_FILL, 0, 0);
  /* Row 2 */
  gtk_table_attach (GTK_TABLE (pw->table), pw->separator, 0, 2, 2, 3,
                    GTK_FILL, GTK_FILL, 0, 0);
  /* Row 3 */
  gtk_table_attach (GTK_TABLE (pw->table), pw->total_progress_label, 0, 1, 3,
                    4, GTK_FILL, GTK_FILL, 0, 0);
  gtk_table_attach (GTK_TABLE (pw->table), pw->total_progress, 1, 2, 3, 4,
                    GTK_FILL, GTK_FILL, 0, 0);
                    
  /* Row 4 */
  gtk_table_attach (GTK_TABLE (pw->table), pw->cancel_button, 1, 2, 4, 5,
                    GTK_SHRINK, GTK_SHRINK, 0, 0);

  g_signal_connect (GTK_OBJECT (pw->base_window), "destroy",
                    G_CALLBACK (gtk_widget_destroyed), &pw->base_window);

  g_signal_connect (GTK_OBJECT (pw->cancel_button), "clicked",
                    G_CALLBACK (cancelled), pw);

  return pw;
}

void
cancelled (GtkWidget *widget, ProgressWindow *pw)
{
  pw->cancelled = TRUE;
}

void
progress_window_destroy (ProgressWindow * pw)
{
  if (pw->base_window != NULL)
    gtk_widget_destroy (pw->base_window);

  g_free (pw);
}
