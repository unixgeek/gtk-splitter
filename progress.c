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
   /*Set up the widgets for the progress window.*/
   pwindow->main_window = gtk_window_new( GTK_WINDOW_DIALOG );
   gtk_window_set_title( GTK_WINDOW( pwindow->main_window ), title );
   pwindow->vbox = gtk_vbox_new( TRUE, 0 );
   pwindow->current_progress = gtk_progress_bar_new();
   pwindow->total_progress = gtk_progress_bar_new();
   pwindow->status = gtk_statusbar_new();

   /*Add the widgets to the dialog.*/
   gtk_container_add( GTK_CONTAINER( pwindow->main_window ), pwindow->vbox );
   gtk_box_pack_start( GTK_BOX( pwindow->vbox ), pwindow->current_progress, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( pwindow->vbox ), pwindow->total_progress, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( pwindow->vbox ), pwindow->status, TRUE, TRUE, 0 );

   /*Set the progress bars to show the percent value in text in addition
     to the colored bar.*/
   gtk_progress_set_show_text( GTK_PROGRESS( pwindow->current_progress ), TRUE );
   gtk_progress_set_show_text( GTK_PROGRESS( pwindow->total_progress ), TRUE );
}

void destroy_progress_window(progress_window *pwindow)
{
   gtk_widget_destroy( pwindow->main_window );
}

void progress_window_set_status_text(GtkWidget *sbar, gchar *text)
{
   gtk_statusbar_push( GTK_STATUSBAR( sbar ), 1, text );
   while ( g_main_iteration( FALSE ) );
}

void progress_window_set_percentage(GtkWidget *p, gfloat percentage)
{
   gtk_progress_set_percentage( GTK_PROGRESS( p ), percentage );
   while ( g_main_iteration( FALSE ) );
}
