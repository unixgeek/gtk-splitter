/*
 * progress.c
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

void create_progress_window(progress_window *pwindow, gchar *title)
{
   /* Set up the window. */
   pwindow->main_window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
   gtk_window_set_title( GTK_WINDOW( pwindow->main_window ), title );
   gtk_window_set_resizable( GTK_WINDOW( pwindow->main_window ), FALSE );
   
   /* Set up the table. */
   pwindow->table = gtk_table_new( 4, 2, FALSE );
   
   /* Set up the progress bars. */
   pwindow->current_progress = gtk_progress_bar_new( );
   pwindow->total_progress = gtk_progress_bar_new( );
   
   /* Set up the labels. */
   pwindow->file_label = gtk_label_new( "File:" );
   pwindow->current_progress_label = gtk_label_new( "Current Progress:" );
   pwindow->total_progress_label = gtk_label_new( "Total Progress:" );
   pwindow->message = gtk_label_new( "" );

   /* Set up a separator. */
   pwindow->separator = gtk_hseparator_new( );
   
   /* Add the widgets to the dialog. */
   gtk_container_add( GTK_CONTAINER( pwindow->main_window ), pwindow->table );
   /* Row 0 */
   gtk_table_attach( GTK_TABLE( pwindow->table ), pwindow->file_label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0 );
   gtk_table_attach( GTK_TABLE( pwindow->table ), pwindow->message, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0 );
   /* Row 1 */
   gtk_table_attach( GTK_TABLE( pwindow->table ), pwindow->current_progress_label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0 );
   gtk_table_attach( GTK_TABLE( pwindow->table ), pwindow->current_progress, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0 );
   /* Row 2 */
   gtk_table_attach( GTK_TABLE( pwindow->table ), pwindow->separator, 0, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0 );
   /* Row 3 */
   gtk_table_attach( GTK_TABLE( pwindow->table ), pwindow->total_progress_label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 0, 0 );
   gtk_table_attach( GTK_TABLE( pwindow->table ), pwindow->total_progress, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0 );
}

void destroy_progress_window(progress_window *pwindow)
{
   gtk_widget_destroy( pwindow->main_window );
}

void progress_window_set_message_text(GtkWidget *msg, gchar *text)
{
   gtk_label_set_text( GTK_LABEL( msg ), text );
   while ( g_main_iteration( FALSE ) );
}

void progress_window_set_percentage(GtkWidget *pbar, gfloat fraction)
{
   gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( pbar ), fraction );
   while ( g_main_iteration( FALSE ) );
}
