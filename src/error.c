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
   GtkWidget *dialog;

   /* Error messages are also sent to stderr before setting up this function.
      This is just incase some unexpected problems occur and an error dialog
      becomes impossible. */
   fprintf( stderr, "\n%s\n\n", error );

   /* Use a stock message dialog. */
   dialog = gtk_message_dialog_new( NULL,
                                    GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_CLOSE,
                                    error );
                                    
   /* Display the error dialog. */  
   gtk_dialog_run( GTK_DIALOG ( dialog ) );
   gtk_widget_destroy ( dialog );
}
