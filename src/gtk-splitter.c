/*
 * gtk-splitter.c
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
#include <string.h>
#include <stdlib.h>

#include "globals.h"
#include "callbacks.h"
#include "error.h"
#include "file_selection.h"


int main(int argc, char *argv[])
{
   /*gtk_splitter_window is a struct containing all the widgets
     for the main window of gtk-splitter.*/
   gtk_splitter_window *gtk_s_window;

   /*Allocate memory for the main window.*/
   gtk_s_window = g_malloc( sizeof( gtk_splitter_window ) );
   if ( gtk_s_window  == NULL )
     {
       fprintf( stderr, "!!! - Critical error:  Could not allocate any memory. - !!!\n" );
       return 0;
     }

   /*Allocate memory for the session data.  Session data is a
     struct for storing file name, output directory, etc.
     See globals.h for more information.*/
   gtk_s_window->sdata = g_malloc( sizeof( session_data ) );
   if ( gtk_s_window->sdata == NULL )
     {
       fprintf( stderr, "!!! - Critical error:  Could not allocate any memory. - !!!\n ");
       return 0;
     }

   /* Store the path to the user's home directory. */
   gtk_s_window->sdata->home_dir = getenv( "HOME" );
   if ( gtk_s_window->sdata->home_dir == NULL )
     {
       display_error( "Could not determine home directory.\n"
                      "Check environment variables for $HOME.\n", TRUE );
       return 0;
     }
   strcat( gtk_s_window->sdata->home_dir, "/" );
   
   /* Set the default output directory to the user's home directory. */
   strcpy( gtk_s_window->sdata->output_dir, gtk_s_window->sdata->home_dir );

   printf( "%s-%s Gunter Wambaugh\n", PACKAGE, VERSION );

   /*Initialize gtk.*/
   gtk_init( &argc, &argv );

   /*Create a new top-level window.*/
   gtk_s_window->base_window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
   /*Set our window so its dimensions are program controlled and not user controlled.*/
   gtk_window_set_policy( GTK_WINDOW( gtk_s_window->base_window ), FALSE, FALSE, TRUE );

   /*Create our storage boxes with no spacing.*/
   gtk_s_window->base_box = gtk_vbox_new( FALSE, 0 );
   gtk_s_window->box1 = gtk_hbox_new( FALSE, 0 );
   gtk_s_window->box5 = gtk_hbox_new( FALSE, 0 );
   gtk_s_window->box2 = gtk_hbox_new( TRUE, 0 );
   gtk_s_window->box3 = gtk_hbox_new( TRUE, 0 );
   gtk_s_window->box4 = gtk_hbox_new( TRUE, 0 );

   /*Setup widgets for inputing and displaying the selected
     file and selected output directory.*/
   gtk_s_window->open_button = gtk_button_new_with_label( "Open" );
   gtk_s_window->output_button = gtk_button_new_with_label( "Output" );
   gtk_s_window->filename_box = gtk_entry_new();
   gtk_s_window->output_box = gtk_entry_new();
   gtk_entry_set_editable( GTK_ENTRY( gtk_s_window->filename_box ), FALSE );
   gtk_entry_set_editable( GTK_ENTRY( gtk_s_window->output_box ), FALSE );
   gtk_entry_set_text(GTK_ENTRY( gtk_s_window->output_box ), gtk_s_window->sdata->home_dir );

   /*Split and combine radio buttons.*/
   gtk_s_window->split_button = gtk_radio_button_new_with_label( NULL, "Split" );
   gtk_s_window->combine_button = gtk_radio_button_new_with_label_from_widget(
                                  GTK_RADIO_BUTTON( gtk_s_window->split_button ), "Combine" );

   /*Toggle button to create a DOS batch file.*/
   gtk_s_window->batch_file_button = gtk_check_button_new_with_label(" Create DOS batch file." );

   /*Toggle button to verfiy a file.*/
   gtk_s_window->verify_button = gtk_check_button_new_with_label(" Verify with md5sum.  (beta)" );

   /*1 mb = (2^23)/8 = 1048576.  A floppy holds approximately 1.4
    (or so they say--depends on the FS) so (1.39 * 1048576) = 1457664. */
   gtk_s_window->size_input_adj = GTK_ADJUSTMENT( gtk_adjustment_new( 1457664, 1, G_MAXFLOAT, 1, 5, 5 ) );

   /*We can split up to 4 GB max? 4294967296-1*/

   /*Widgets for inputing the chunk size.*/
   gtk_s_window->size_input = gtk_spin_button_new( gtk_s_window->size_input_adj, 1.0, 2 );
   gtk_s_window->chunk_size_units = gtk_option_menu_new();
   gtk_s_window->units_menu = gtk_menu_new();
   gtk_s_window->bytesopt = gtk_menu_item_new_with_label( "Bytes" );
   gtk_s_window->kbytesopt = gtk_menu_item_new_with_label( "Kilobytes" );
   gtk_s_window->mbytesopt = gtk_menu_item_new_with_label( "Megabytes" );
   gtk_menu_append( GTK_MENU( gtk_s_window->units_menu), gtk_s_window->bytesopt );
   gtk_menu_append( GTK_MENU( gtk_s_window->units_menu), gtk_s_window->kbytesopt );
   gtk_menu_append( GTK_MENU( gtk_s_window->units_menu), gtk_s_window->mbytesopt );
   gtk_option_menu_set_menu( GTK_OPTION_MENU( gtk_s_window->chunk_size_units ), gtk_s_window->units_menu );

   /*The button that starts the split or combine process.*/
   gtk_s_window->start_button = gtk_button_new_with_label( "Start" );

   gtk_window_set_title( GTK_WINDOW( gtk_s_window->base_window ), "gtk-splitter" );
   gtk_container_set_border_width( GTK_CONTAINER( gtk_s_window->base_window ), 5 );

   /*Put our little gui together.*/
   gtk_container_add( GTK_CONTAINER( gtk_s_window->base_window ), gtk_s_window->base_box );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->base_box ), gtk_s_window->box1, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->base_box ), gtk_s_window->box5, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->base_box ), gtk_s_window->box2, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->base_box ), gtk_s_window->box3, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->base_box ), gtk_s_window->verify_button, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->base_box ), gtk_s_window->batch_file_button, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->base_box ), gtk_s_window->box4, TRUE, TRUE, 0 );

   gtk_box_pack_start( GTK_BOX( gtk_s_window->box1 ), gtk_s_window->open_button, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->box1 ), gtk_s_window->filename_box, FALSE, TRUE, 0 );
   gtk_box_set_spacing( GTK_BOX( gtk_s_window->box1 ), 5 );

   gtk_box_pack_start( GTK_BOX( gtk_s_window->box5 ), gtk_s_window->output_button, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->box5 ), gtk_s_window->output_box, FALSE, TRUE, 0 );
   gtk_box_set_spacing( GTK_BOX( gtk_s_window->box5 ), 5 );

   gtk_box_pack_start( GTK_BOX( gtk_s_window->box2 ), gtk_s_window->split_button, TRUE, TRUE, 10 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->box2 ), gtk_s_window->combine_button, TRUE, TRUE, 10 );

   gtk_box_pack_start( GTK_BOX( gtk_s_window->box3 ), gtk_s_window->size_input, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( gtk_s_window->box3 ), gtk_s_window->chunk_size_units, TRUE, TRUE, 0 );

   gtk_box_pack_start( GTK_BOX( gtk_s_window->box4 ), gtk_s_window->start_button, TRUE, TRUE, 50 );
   /*The end of putting the gui together.*/

   /*Initialize our data.*/
   initialize_session_data( gtk_s_window->sdata );
   initialize_splitter_window( gtk_s_window );

   /*Different signals that our gui picks up.*/
   /*Most callbacks here are defined in callbacks.h*/
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->base_window), "destroy",
                       GTK_SIGNAL_FUNC( gtk_main_quit ), NULL);

   gtk_signal_connect( GTK_OBJECT( gtk_s_window->split_button ), "clicked",
                       GTK_SIGNAL_FUNC( toggle_split), gtk_s_window );
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->combine_button ), "clicked",
                       GTK_SIGNAL_FUNC( toggle_combine ), gtk_s_window );
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->size_input_adj ), "value_changed",
                       GTK_SIGNAL_FUNC(set_data), gtk_s_window );
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->start_button ), "clicked",
                       GTK_SIGNAL_FUNC( start ), gtk_s_window );
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->open_button ), "clicked",
                       GTK_SIGNAL_FUNC( choose_file ), gtk_s_window );
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->output_button ), "clicked",
                       GTK_SIGNAL_FUNC( choose_directory ), gtk_s_window );

   gtk_signal_connect( GTK_OBJECT( gtk_s_window->batch_file_button ), "toggled",
                       GTK_SIGNAL_FUNC( toggle_batch ), gtk_s_window->sdata );
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->bytesopt ), "activate",
                       GTK_SIGNAL_FUNC( set_bytes ), gtk_s_window->sdata );
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->kbytesopt ), "activate",
                       GTK_SIGNAL_FUNC( set_kbytes ), gtk_s_window->sdata );
   gtk_signal_connect( GTK_OBJECT( gtk_s_window->mbytesopt ), "activate",
                       GTK_SIGNAL_FUNC( set_mbytes ), gtk_s_window->sdata );
  /*End of callbacks.*/

   /*Display the gui on the screen.*/
   gtk_widget_show_all( gtk_s_window->base_window );

   /*Run the main loop of gtk.*/
   gtk_main();

   /*Free the memory that was allocated.*/
   g_free( gtk_s_window->sdata->home_dir );
   g_free( gtk_s_window->sdata );
   g_free( gtk_s_window );

   return 0;
}
