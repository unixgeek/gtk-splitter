/* 
 * verify_dialog.c
 *
 * Copyright 2002 Gunter Wambaugh
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
#include "verify_dialog.h"

void display_verify_dialog(gboolean verified)
{
   GtkWidget *dialog;

   /* Use a stock message dialog. */
   if ( verified )
      dialog = gtk_message_dialog_new( NULL,
                                       GTK_DIALOG_MODAL,
                                       GTK_MESSAGE_INFO,
                                       GTK_BUTTONS_CLOSE,
                                       "File verification successful" );
   else
      dialog = gtk_message_dialog_new( NULL,
                                       GTK_DIALOG_MODAL,
                                       GTK_MESSAGE_WARNING,
                                       GTK_BUTTONS_CLOSE,
                                       "File verification unsuccessful" );
                                    
   /* Display the error dialog. */  
   gtk_dialog_run( GTK_DIALOG ( dialog ) );
   gtk_widget_destroy ( dialog );
}
