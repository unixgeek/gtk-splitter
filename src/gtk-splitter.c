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
   /* Used to automatically detect whether a file name read from
      the command-line should be split or combined. */
   gchar ext[4];
   
   /* gtk_splitter_window is a struct containing all the widgets
      for the main window of gtk-splitter. */
   gtk_splitter_window main_window;

   /* Store the path to the user's home directory. */
   main_window.my_session_data.home_directory = getenv( "HOME" );
   if ( main_window.my_session_data.home_directory == NULL )
     {
       display_error( "Could not determine home directory.\n"
                      "Check environment variables for $HOME.\n", TRUE );
       return 0;
     }
   strcat( main_window.my_session_data.home_directory, "/" );
   
   /* Set the default output directory to the user's home directory. */
   strcpy( main_window.my_session_data.output_directory, main_window.my_session_data.home_directory );

   printf( "%s-%s Gunter Wambaugh\n", PACKAGE, VERSION );

   /* Initialize gtk. */
   gtk_init( &argc, &argv );

   /* Create a new top-level window. */
   main_window.base_window = gtk_window_new( GTK_WINDOW_TOPLEVEL );

   /* Set the window so its dimensions are program controlled and not user controlled. */
   gtk_window_set_policy( GTK_WINDOW( main_window.base_window ), FALSE, FALSE, TRUE );

   /* Create storage boxes with no spacing. */
   main_window.base_box = gtk_vbox_new( FALSE, 0 );
   main_window.box1 = gtk_hbox_new( FALSE, 0 );
   main_window.box5 = gtk_hbox_new( FALSE, 0 );
   main_window.box2 = gtk_hbox_new( TRUE, 0 );
   main_window.box3 = gtk_hbox_new( TRUE, 0 );
   main_window.box4 = gtk_hbox_new( TRUE, 0 );

   /* Setup widgets for inputing and displaying the selected
      file and selected output directory. */
   main_window.open_button = gtk_button_new_with_label( "Open" );
   main_window.output_button = gtk_button_new_with_label( "Output" );
   main_window.file_name_box = gtk_entry_new( );
   main_window.output_box = gtk_entry_new( );
   gtk_entry_set_editable( GTK_ENTRY( main_window.file_name_box ), FALSE );
   gtk_entry_set_editable( GTK_ENTRY( main_window.output_box ), FALSE );
   gtk_entry_set_text(GTK_ENTRY( main_window.output_box ), main_window.my_session_data.home_directory );

   /* Radio buttons for the split and combine options. */
   main_window.split_button = gtk_radio_button_new_with_label( NULL, "Split" );
   main_window.combine_button = gtk_radio_button_new_with_label_from_widget(
                                GTK_RADIO_BUTTON( main_window.split_button ), "Combine" );

   /* Toggle button for the DOS batch file option. */
   main_window.batch_file_button = gtk_check_button_new_with_label(" Create DOS batch file" );

   /* Toggle button for the verfiy file option. */
   main_window.verify_button = gtk_check_button_new_with_label(" Verify" );

   /* 1 mb = (2^23)/8 = 1048576.  A floppy holds approximately 1.4
      (or so they say--depends on the FS) so (1.39 * 1048576) = 1457664. */
   main_window.size_input_adj = GTK_ADJUSTMENT( gtk_adjustment_new( 1457664, 1, G_MAXFLOAT, 1, 5, 5 ) );

   /* We can split up to 4 GB max? 4294967296-1 */

   /* Widgets for inputing the chunk size. */
   main_window.size_input = gtk_spin_button_new( main_window.size_input_adj, 1.0, 2 );
   main_window.chunk_size_units = gtk_option_menu_new();
   main_window.units_menu = gtk_menu_new();
   main_window.unit_bytes = gtk_menu_item_new_with_label( "Bytes" );
   main_window.unit_kilobytes = gtk_menu_item_new_with_label( "Kilobytes" );
   main_window.unit_megabytes = gtk_menu_item_new_with_label( "Megabytes" );
   gtk_menu_append( GTK_MENU( main_window.units_menu), main_window.unit_bytes );
   gtk_menu_append( GTK_MENU( main_window.units_menu), main_window.unit_kilobytes );
   gtk_menu_append( GTK_MENU( main_window.units_menu), main_window.unit_megabytes );
   gtk_option_menu_set_menu( GTK_OPTION_MENU( main_window.chunk_size_units ), main_window.units_menu );

   /* The button that starts the split or combine process. */
   main_window.start_button = gtk_button_new_with_label( "Start" );

   gtk_window_set_title( GTK_WINDOW( main_window.base_window ), "gtk-splitter" );
   gtk_container_set_border_width( GTK_CONTAINER( main_window.base_window ), 5 );

   /* Put the gui together. */
   gtk_container_add( GTK_CONTAINER( main_window.base_window ), main_window.base_box );
   gtk_box_pack_start( GTK_BOX( main_window.base_box ), main_window.box1, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.base_box ), main_window.box5, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.base_box ), main_window.box2, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.base_box ), main_window.box3, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.base_box ), main_window.verify_button, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.base_box ), main_window.batch_file_button, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.base_box ), main_window.box4, TRUE, TRUE, 0 );

   gtk_box_pack_start( GTK_BOX( main_window.box1 ), main_window.open_button, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.box1 ), main_window.file_name_box, FALSE, TRUE, 0 );
   gtk_box_set_spacing( GTK_BOX( main_window.box1 ), 5 );

   gtk_box_pack_start( GTK_BOX( main_window.box5 ), main_window.output_button, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.box5 ), main_window.output_box, FALSE, TRUE, 0 );
   gtk_box_set_spacing( GTK_BOX( main_window.box5 ), 5 );

   gtk_box_pack_start( GTK_BOX( main_window.box2 ), main_window.split_button, TRUE, TRUE, 10 );
   gtk_box_pack_start( GTK_BOX( main_window.box2 ), main_window.combine_button, TRUE, TRUE, 10 );

   gtk_box_pack_start( GTK_BOX( main_window.box3 ), main_window.size_input, TRUE, TRUE, 0 );
   gtk_box_pack_start( GTK_BOX( main_window.box3 ), main_window.chunk_size_units, TRUE, TRUE, 0 );

   gtk_box_pack_start( GTK_BOX( main_window.box4 ), main_window.start_button, TRUE, TRUE, 50 );
   /* The end of putting the gui together. */

   /* Initialize session data. */
   initialize_session_data( &main_window.my_session_data );
   initialize_splitter_window( &main_window );

   /* Different signals the gui listens for.*/
   /* Most callbacks listed here are defined in callbacks.h */
   gtk_signal_connect( GTK_OBJECT( main_window.base_window), "destroy",
                       GTK_SIGNAL_FUNC( gtk_main_quit ), NULL);

   gtk_signal_connect( GTK_OBJECT( main_window.split_button ), "clicked",
                       GTK_SIGNAL_FUNC( toggle_split), &main_window );
   gtk_signal_connect( GTK_OBJECT( main_window.combine_button ), "clicked",
                       GTK_SIGNAL_FUNC( toggle_combine ), &main_window );
   gtk_signal_connect( GTK_OBJECT( main_window.size_input_adj ), "value_changed",
                       GTK_SIGNAL_FUNC(set_data), &main_window );
   gtk_signal_connect( GTK_OBJECT( main_window.start_button ), "clicked",
                       GTK_SIGNAL_FUNC( start ), &main_window );
   gtk_signal_connect( GTK_OBJECT( main_window.open_button ), "clicked",
                       GTK_SIGNAL_FUNC( get_file_name_dialog ), &main_window );
   gtk_signal_connect( GTK_OBJECT( main_window.output_button ), "clicked",
                       GTK_SIGNAL_FUNC( get_directory_name_dialog ), &main_window );

   gtk_signal_connect( GTK_OBJECT( main_window.batch_file_button ), "toggled",
                       GTK_SIGNAL_FUNC( toggle_batch ), &main_window.my_session_data );
                       
   gtk_signal_connect( GTK_OBJECT( main_window.unit_bytes ), "activate",
                       GTK_SIGNAL_FUNC( set_unit_bytes ), &main_window.my_session_data );
   gtk_signal_connect( GTK_OBJECT( main_window.unit_kilobytes ), "activate",
                       GTK_SIGNAL_FUNC( set_unit_kilobytes ), &main_window.my_session_data );
   gtk_signal_connect( GTK_OBJECT( main_window.unit_megabytes ), "activate",
                       GTK_SIGNAL_FUNC( set_unit_megabytes ), &main_window.my_session_data );
   /* End of callbacks. */

   /* Display the gui on the screen. */
   gtk_widget_show_all( main_window.base_window );
   
   /* Determine whether a file name read from
      the command-line should be split or combined. */
   if ( argc == 2 )
     { 
       set_file_name( &main_window, argv[1] );

       if ( strlen( argv[1] ) >= 3 )
         {
           /* Copy the file's extension. */
           ext[0] = argv[1][strlen( argv[1] ) - 3];
           ext[1] = argv[1][strlen( argv[1] ) - 2];
           ext[2] = argv[1][strlen( argv[1] ) - 1];
           ext[3] = '\0';
           /* Check to see if the extension is '001'. */
           if ( strcmp( ext, "001" ) == 0 )
             {
               /* Start with the combine button selected. */
               gtk_button_clicked( GTK_BUTTON( main_window.combine_button ) );
             } 
         }
     }


   /* Run the main loop of gtk. */
   gtk_main();

   return 0;
}
