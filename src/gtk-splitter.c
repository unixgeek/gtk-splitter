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
#include <glib.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "globals.h"
#include "callbacks.h"
#include "interface.h"
#include "file_selection.h"

int main( int argc, char *argv[] )
{
   /* Used to automatically detect whether a file name read from
      the command-line should be split or combined. */
   char ext[4];
   char *ptr;
   char resolved_name[PATH_MAX];
   
   GtkSplitterWindow *main_window;

   /* Initialize gtk. */
   gtk_init( &argc, &argv );

   main_window = gtk_splitter_main_window_new( );

   /* Initialize session data. */
   initialize_session_data( main_window->session_data );
   initialize_splitter_window( main_window );
    
   g_print( "%s-%s Gunter Wambaugh\n", PACKAGE, VERSION );

   /* Display the gui on the screen. */
   gtk_widget_show_all( main_window->base_window );
   
   /* Determine whether a file name read from
      the command-line should be split or combined. */
   if ( argc == 2 )
     { 
       ptr = realpath( argv[1], resolved_name );
        
       if ( ptr != NULL )
         {
           set_file_name( main_window, resolved_name );

           if ( strlen( resolved_name ) >= 3 )
             {
               /* Copy the file's extension. */
               ext[0] = resolved_name[strlen( resolved_name ) - 3];
               ext[1] = resolved_name[strlen( resolved_name ) - 2];
               ext[2] = resolved_name[strlen( resolved_name ) - 1];
               ext[3] = '\0';
               /* Check to see if the extension is '001'. */
               if ( strcmp( ext, "001" ) == 0 )
                 {
                   /* Start with the combine button selected. */
                   gtk_button_clicked( GTK_BUTTON( main_window->combine_button ) );
                 } 
             }
         }
     }

   /* Run the main loop of gtk. */
   gtk_main( );

   gtk_splitter_main_window_destroy( main_window );
   
   return 0;
}
