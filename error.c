/* 
 * error.c
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

void display_error(char *error, gboolean quit)
{

  GtkWidget *dialog, *label, *okay_button;
  
  dialog = gtk_dialog_new();
  gtk_window_set_modal(GTK_WINDOW (dialog), TRUE);  
  gtk_window_set_policy(GTK_WINDOW (dialog), FALSE, FALSE, TRUE);

  label = gtk_label_new (error);
  okay_button = gtk_button_new_with_label("Okay");

  if (quit)   
     gtk_signal_connect_object (GTK_OBJECT (okay_button), "clicked",
                                GTK_SIGNAL_FUNC (gtk_main_quit), NULL);
  else
     gtk_signal_connect_object (GTK_OBJECT (okay_button), "clicked",
                                GTK_SIGNAL_FUNC (gtk_widget_destroy), GTK_OBJECT (dialog));


  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->action_area), okay_button);    
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), label);
   
  gtk_widget_show_all (dialog);
}
