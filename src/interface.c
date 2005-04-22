/* 
 * $Id: interface.c,v 1.7 2005/04/16 23:01:06 techgunter Exp $
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
#include <stdlib.h>
#include "window.h"
#include "session.h"
#include "callbacks.h"
#include "file_selection.h"
#include "interface.h"

GtkSplitterWindow *
gtk_splitter_main_window_new ()
{
  GtkSplitterWindow *gsw;

  gsw = g_malloc (sizeof (GtkSplitterWindow));

  gsw->session_data = g_malloc (sizeof (GtkSplitterSessionData));

  if ((gsw == NULL) || (gsw->session_data == NULL))
    {
      display_error ("Couldn't allocate memory for GtkSplitterWindow");
      exit (1);
    }

  /* Create a new top-level window. */
  gsw->base_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_resizable (GTK_WINDOW (gsw->base_window), FALSE);

  /* Set the window icon. */
  gsw->icon = gdk_pixbuf_new_from_file (ICON_AND_PATH, NULL);
  if (gsw->icon != NULL)
    gtk_window_set_icon (GTK_WINDOW (gsw->base_window), gsw->icon);

  /* Create storage boxes with no spacing. */
  gsw->base_box = gtk_vbox_new (FALSE, 0);
  gsw->box1 = gtk_hbox_new (FALSE, 0);
  gsw->box5 = gtk_hbox_new (FALSE, 0);
  gsw->box2 = gtk_hbox_new (FALSE, 0);
  gsw->box3 = gtk_hbox_new (FALSE, 0);
  gsw->box4 = gtk_hbox_new (FALSE, 0);

  /* Setup widgets for inputing and displaying the selected
     file and selected output directory. */
  gsw->open_button = gtk_button_new_from_stock (GTK_STOCK_OPEN);
  gsw->output_button = gtk_button_new_from_stock (GTK_STOCK_SAVE);
  gsw->file_name_box = gtk_entry_new ();
  gsw->output_box = gtk_entry_new ();
  gtk_editable_set_editable (GTK_EDITABLE (gsw->file_name_box), FALSE);
  gtk_editable_set_editable (GTK_EDITABLE (gsw->output_box), FALSE);

  /* Radio buttons for the split and combine options. */
  gsw->split_button = gtk_radio_button_new_with_label (NULL, "Split");
  gsw->combine_button =
    gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON
                                                 (gsw->split_button),
                                                 "Combine");

  /* Toggle button for the DOS batch file option. */
  gsw->batch_file_button =
    gtk_check_button_new_with_label ("Create DOS batch file");

#ifdef HAVE_LIBMHASH
  /* Toggle button for the verfiy file option. */
  gsw->verify_button = gtk_check_button_new_with_label ("Verify");
#endif

  /* 1 mb = (2^23)/8 = 1048576.  A floppy holds approximately 1.4
     (or so they say--depends on the FS) so (1.39 * 1048576) = 1457664. */
  gsw->size_input_adj =
    GTK_ADJUSTMENT (gtk_adjustment_new (1457664, 1, G_MAXFLOAT, 1, 5, 5));
  gsw->session_data->entry = 1457664;
  gsw->session_data->unit = BYTES;
  /* We can split up to 4 GB max? 4294967296-1 */

  /* Widgets for inputing the chunk size. */
  gsw->size_input = gtk_spin_button_new (gsw->size_input_adj, 1.0, 2);
  gsw->units_menu = gtk_combo_box_new_text ();
  gsw->unit_bytes = gtk_menu_item_new_with_label ("Bytes");
  gsw->unit_kilobytes = gtk_menu_item_new_with_label ("Kilobytes");
  gsw->unit_megabytes = gtk_menu_item_new_with_label ("Megabytes");
  gtk_combo_box_append_text (GTK_COMBO_BOX (gsw->units_menu), "Bytes");
  gtk_combo_box_append_text (GTK_COMBO_BOX (gsw->units_menu), "Kilobytes");
  gtk_combo_box_append_text (GTK_COMBO_BOX (gsw->units_menu), "Megabytes");

  /* The button that starts the split or combine process. */
  gsw->start_button = gtk_button_new_from_stock (GTK_STOCK_OK);

  gtk_window_set_title (GTK_WINDOW (gsw->base_window), "gtk-splitter");
  gtk_container_set_border_width (GTK_CONTAINER (gsw->base_window), 5);

  /* Put the gui together. */
  gtk_container_add (GTK_CONTAINER (gsw->base_window), gsw->base_box);
  gtk_box_pack_start (GTK_BOX (gsw->base_box), gsw->box1, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gsw->base_box), gsw->box5, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gsw->base_box), gsw->box2, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gsw->base_box), gsw->box3, TRUE, TRUE, 0);

#ifdef HAVE_LIBMHASH
  gtk_box_pack_start (GTK_BOX (gsw->base_box), gsw->verify_button, TRUE, TRUE,
                      0);
#endif

  gtk_box_pack_start (GTK_BOX (gsw->base_box), gsw->batch_file_button, TRUE,
                      TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gsw->base_box), gsw->box4, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gsw->box1), gsw->open_button, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gsw->box1), gsw->file_name_box, FALSE, TRUE,
                      0);
  gtk_box_set_spacing (GTK_BOX (gsw->box1), 5);

  gtk_box_pack_start (GTK_BOX (gsw->box5), gsw->output_button, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gsw->box5), gsw->output_box, FALSE, TRUE, 0);
  gtk_box_set_spacing (GTK_BOX (gsw->box5), 5);

  gtk_box_pack_start (GTK_BOX (gsw->box2), gsw->split_button, TRUE, TRUE, 10);
  gtk_box_pack_start (GTK_BOX (gsw->box2), gsw->combine_button, TRUE, TRUE,
                      10);

  gtk_box_pack_start (GTK_BOX (gsw->box3), gsw->size_input, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (gsw->box3), gsw->units_menu, TRUE, TRUE,
                      0);

  gtk_box_pack_end (GTK_BOX (gsw->box4), gsw->start_button, FALSE, FALSE, 0);
  /* The end of putting the gui together. */

  /* Different signals the gui listens for. */
  /* Most callbacks listed here are defined in callbacks.h */
  g_signal_connect (GTK_OBJECT (gsw->base_window), "destroy",
                    G_CALLBACK (gtk_widget_destroyed), &gsw->base_window);

  g_signal_connect (GTK_OBJECT (gsw->base_window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect (GTK_OBJECT (gsw->split_button), "clicked",
                    G_CALLBACK (toggle_split), (gpointer) gsw);

  g_signal_connect (GTK_OBJECT (gsw->combine_button), "clicked",
                    G_CALLBACK (toggle_combine), (gpointer) gsw);

  g_signal_connect (GTK_OBJECT (gsw->size_input_adj), "value_changed",
                    G_CALLBACK (set_data), (gpointer) gsw);

  g_signal_connect (GTK_OBJECT (gsw->start_button), "clicked",
                    G_CALLBACK (start_split_or_combine), (gpointer) gsw);

  g_signal_connect (GTK_OBJECT (gsw->open_button), "clicked",
                    G_CALLBACK (get_file_name_dialog), (gpointer) gsw);

  g_signal_connect (GTK_OBJECT (gsw->output_button), "clicked",
                    G_CALLBACK (get_directory_name_dialog), (gpointer) gsw);

  g_signal_connect (GTK_OBJECT (gsw->batch_file_button), "toggled",
                    G_CALLBACK (toggle_batch), (gpointer) gsw->session_data);

#ifdef HAVE_LIBMHASH
  g_signal_connect (GTK_OBJECT (gsw->verify_button), "toggled",
                    G_CALLBACK (toggle_verify), (gpointer) gsw->session_data);
#endif

  g_signal_connect (GTK_OBJECT (gsw->units_menu), "changed",
                    G_CALLBACK (set_unit),
                    (gpointer) gsw);
  /* End of callbacks. */

  return gsw;
}

void
gtk_splitter_main_window_destroy (GtkSplitterWindow * gsw)
{
  if (gsw->base_window != NULL)
    gtk_widget_destroy (gsw->base_window);

  g_free (gsw->session_data);
  g_free (gsw);
}

void
display_error (gchar * error)
{
  GtkWidget *dialog;

  /* Error messages are also sent to stderr before setting up this function.
     This is just incase some unexpected problems occur and an error dialog
     becomes impossible. */
  g_printerr ("\n%s\n\n", error);

  /* Use a stock message dialog. */
  dialog = gtk_message_dialog_new (NULL,
                                   GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_CLOSE, error);

  /* Display the error dialog. */
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void
display_verification (gboolean verified)
{
  GtkWidget *dialog;

  /* Use a stock message dialog. */
  if (verified)
    dialog = gtk_message_dialog_new (NULL,
                                     GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_CLOSE,
                                     "File verification successful");
  else
    dialog = gtk_message_dialog_new (NULL,
                                     GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_WARNING,
                                     GTK_BUTTONS_CLOSE,
                                     "File verification unsuccessful");

  /* Display the error dialog. */
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}
