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
#include "split.h"
#include "combine.h"
#include "error.h"

int main(int argc, char *argv[])
{  
   /*Create some widgets to store buttons and such.*/
   GtkWidget *base_box, *box1, *box2, *box3, *box4, *open_button, *units_menu;	
	
   /*session_data is a structure used to store info on the selected file.*/
   session_data *sdata;
   sdata = g_malloc( sizeof(session_data) );
   if (sdata == NULL) 
     {
         fprintf(stderr, "!!! - Critical error: Could not allocate any memory. - !!!\n");
         return 0;
     }

     /*Store the users home directory.*/
   home_dir = getenv("HOME");
   if (home_dir == NULL) 
     {
        fprintf(stderr, "!! - Critical error: Could not determine home directory; "
			    "check environment variables for HOME. - !!!\n");
	return 0;
     }
	
   strcat(home_dir, "/");
	
   //strcpy(version, "gtk-splitter 0.9a - Gunter Wambaugh");
   //version[35] = '\0';
   fprintf(stderr, "%s\n", GTK_SPLITTER_VERSION);

   /*Initialize gtk.*/
   gtk_init(&argc, &argv);
#if DEBUG 
   fprintf(stderr, "\n**************\n");
   fprintf(stderr, "* Debug Info *\n");
   fprintf(stderr, "**************\n");
   fprintf(stderr, "gtk-splitter.c:  Allocated %u bytes for session_data.\n", sizeof(session_data) );
   fprintf(stderr, "gtk-splitter.c:  argc is %d.\n", argc);
   fprintf(stderr, "gtk-splitter.c:  argv[1] is %s.\n", argv[1]);
#endif
       
   /*Create a new window.*/
   sdata->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   /*Set our window size so it is program controlled - and not user controlled.*/
   gtk_window_set_policy(GTK_WINDOW (sdata->window), FALSE, FALSE, TRUE);
	
   /*Create our storage boxes - no spacing.*/
   base_box = gtk_vbox_new(FALSE, 0);
   box1 = gtk_hbox_new(FALSE, 0);
   box2 = gtk_hbox_new(TRUE, 0);
   box3 = gtk_hbox_new(FALSE, 0);
   box4 = gtk_hbox_new(TRUE, 0);

   /*Open button to select a file.*/
   open_button = gtk_button_new_with_label("Open");
   filename_box = gtk_entry_new();
   gtk_entry_set_editable(GTK_ENTRY (filename_box), FALSE);
	
   /*Split and combine radio buttons.*/
   split_button = gtk_radio_button_new_with_label(NULL, "Split");
   combine_button = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (split_button),
               													"Combine");
	
   /*Toggle button to create a dos batch file.*/
   batch_file_button = gtk_check_button_new_with_label("Create DOS batch file.");
	
   /*1 mb = (2^23)/8 = 1048576.  A floppy holds approximately 1.4 
    (or so they say--depends on the FS) so (1.39 * 1048576) = 1457664. */
   size_input_adj = (GtkAdjustment *) gtk_adjustment_new(1457664, 1, G_MAXFLOAT, 1, 5, 5);

   /*We can split up to 4 GB max? 4294967296-1*/

   /*For setting chunk_size.*/
   size_input = gtk_spin_button_new(size_input_adj, 1.0, 2);

   chunk_size_units = gtk_option_menu_new();
   units_menu = gtk_menu_new();
   bytesopt = gtk_menu_item_new_with_label("Bytes");
   kbytesopt = gtk_menu_item_new_with_label("Kilobytes");
   mbytesopt = gtk_menu_item_new_with_label("Megabytes");
   gtk_menu_append(GTK_MENU (units_menu), bytesopt);
   gtk_menu_append(GTK_MENU (units_menu), kbytesopt);
   gtk_menu_append(GTK_MENU (units_menu), mbytesopt);
   gtk_option_menu_set_menu(GTK_OPTION_MENU (chunk_size_units), units_menu);

   start_button = gtk_button_new_with_label("Start");

   gtk_window_set_title(GTK_WINDOW (sdata->window), "gtk-splitter");
   gtk_container_set_border_width(GTK_CONTAINER (sdata->window), 5);

   /*Put our little gui together.*/
   gtk_container_add(GTK_CONTAINER (sdata->window), base_box);
   gtk_box_pack_start(GTK_BOX (base_box), box1, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX (base_box), box2, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX (base_box), box3, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX (base_box), batch_file_button, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX (base_box), box4, TRUE, TRUE, 0);	

   gtk_box_pack_start(GTK_BOX (box1), open_button, TRUE, FALSE, 0);
   gtk_box_pack_start(GTK_BOX (box1), filename_box, TRUE, FALSE, 0);

   gtk_box_pack_start(GTK_BOX (box2), split_button, TRUE, TRUE, 10);
   gtk_box_pack_start(GTK_BOX (box2), combine_button, TRUE, TRUE, 10);

   gtk_box_pack_start(GTK_BOX (box3), size_input, TRUE, TRUE, 0);
   gtk_box_pack_start(GTK_BOX (box3), chunk_size_units, TRUE, TRUE, 0);

   gtk_box_pack_start(GTK_BOX (box4), start_button, TRUE, TRUE, 50);

#if DEBUG
   fprintf(stderr, "gtk-splitter.c:  Initializing the data.\n");
#endif
   /*Initialize our data.*/
   initialize_data(sdata);
	
   /*Different signals that our gui picks up.*/
   /*All callbacks here are defined in callbacks.h*/
   gtk_signal_connect(GTK_OBJECT (sdata->window), "destroy", GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
   gtk_signal_connect(GTK_OBJECT (open_button), "clicked", GTK_SIGNAL_FUNC(choose_file), sdata);
   gtk_signal_connect(GTK_OBJECT (split_button), "clicked", GTK_SIGNAL_FUNC(toggle_split), sdata);
   gtk_signal_connect(GTK_OBJECT (combine_button), "clicked", GTK_SIGNAL_FUNC(toggle_combine), sdata);
   gtk_signal_connect(GTK_OBJECT (size_input_adj), "value_changed", GTK_SIGNAL_FUNC(set_data), sdata);
   gtk_signal_connect(GTK_OBJECT (batch_file_button), "toggled", GTK_SIGNAL_FUNC(toggle_batch), sdata);
   gtk_signal_connect(GTK_OBJECT (start_button), "clicked", GTK_SIGNAL_FUNC(start), sdata);
   gtk_signal_connect(GTK_OBJECT (bytesopt), "activate", GTK_SIGNAL_FUNC(set_bytes), sdata);
   gtk_signal_connect(GTK_OBJECT (kbytesopt), "activate", GTK_SIGNAL_FUNC(set_kbytes), sdata);
   gtk_signal_connect(GTK_OBJECT (mbytesopt), "activate", GTK_SIGNAL_FUNC(set_mbytes), sdata);

#if DEBUG
   fprintf(stderr, "gtk-splitter.c:  Displaying widgets.\n");
#endif

   /*Display the gui on the screen.*/
   gtk_widget_show_all(sdata->window);	

#if DEBUG
   fprintf(stderr, "gtk-splitter.c:  Running the gtk main loop.\n");
#endif
   /*Run the main loop of gtk.*/
   gtk_main();

   /*Free the ram allocated for our session_data.*/
   g_free(sdata);

   return 0;
}
