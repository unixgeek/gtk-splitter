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
#include <stdio.h>
#include "error.h"

void display_error(gchar *error, gboolean quit)
{
   GtkWidget *dialog, *label, *okay_button;

   /* Error messages are also sent to stderr before setting up this function.
      This is just incase some unexpected problems occur and an error dialog
      becomes impossible. */
   fprintf( stderr, error );

   /* Use a GTK+ stock error dialog. */
   dialog = gtk_dialog_new();

   /* The user must close the error dialog before control will return
      to the main window. */
   gtk_window_set_modal( GTK_WINDOW( dialog ), TRUE );

   /* Set the window so that it is not resizeably by the user.
      The window is automatically sized at run-time and is
      made as small as possible. */
   gtk_window_set_policy( GTK_WINDOW( dialog ), FALSE, FALSE, TRUE );

   gtk_window_set_title( GTK_WINDOW( dialog ), "Error" );

   /* Add the error message to the dialog. */
   label = gtk_label_new( error );

   okay_button = gtk_button_new_with_label( "Okay" );

   /* Set up the dialog with the widgets. */
   gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dialog )->action_area ), okay_button, FALSE, FALSE, 0 );
   gtk_container_add( GTK_CONTAINER( GTK_DIALOG( dialog )->vbox ), label );

   /* If the error is severe enough to require the application to quit,
      then do so when the user clicks the 'okay' button.
      Otherwise, return control to the main window when the user clicks
      the 'okay' button. */
   if ( quit )
     gtk_signal_connect_object(GTK_OBJECT( okay_button ), "clicked",
                               GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
   else
     gtk_signal_connect_object(GTK_OBJECT( okay_button ), "clicked",
                               GTK_SIGNAL_FUNC( gtk_widget_destroy ),
                               GTK_OBJECT( dialog ) );

   /* Show the error dialog to the user. */
   gtk_widget_show_all( dialog );
}
