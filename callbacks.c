/* 
 * callbacks.c
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

#include <stdio.h>

#include <gtk/gtk.h>
#include <string.h>

#include "callbacks.h"
#include "split.h"
#include "combine.h"
#include "error.h"

void setfilename(GtkWidget *tmp, session_data *data)
{
   gchar *tmp_ptr;
   gushort i=0, j=0, path_only_count=0;

   tmp_ptr = gtk_file_selection_get_filename(GTK_FILE_SELECTION (file_selector) );
   data->fp_length = strlen(tmp_ptr) + 1;  /*Add in a count for the NULL terminator.*/

   g_free(data->filename_and_path);  /*Free any previous data.*/
   
   data->filename_and_path = g_malloc( data->fp_length * sizeof(gchar) );
   if (data->filename_and_path == NULL)
     {
       fprintf(stderr, "callbacks.c:  (filename_and_path) Could not allocate any memory.\n");
       display_error("Could not allocate any memory.", TRUE);
     }

#if DEBUG
   fprintf(stderr, "callbacks.c:  Allocated %d bytes for data->filename_and_path.\n", data->fp_length);
#endif
   
   strcpy(data->filename_and_path, tmp_ptr);
   data->filename_and_path[data->fp_length - 1] = '\0';
	
   /*Find out what the file name is, without the path.*/
   path_only_count = data->fp_length - 1;
   j = 0;
   /*Start at the end and count backwards till we find a '/'*/
   while (data->filename_and_path[path_only_count] != '/')
     path_only_count--;
	
   path_only_count++; /*add the count of the last '/' */

   data->f_length = data->fp_length - path_only_count;  /*No +1 is needed, already in fp_length.*/

   g_free(data->filename_only);  /*Free any previous data.*/

   data->filename_only = g_malloc( data->f_length * sizeof(gchar));
   if (data->filename_only == NULL)
     {
       fprintf(stderr, "callbacks.c:  (filename_only) Could not allocate any memory.\n");
       display_error("Could not allocate any memory.", TRUE);
     }
   data->filename_only[data->f_length -1] = '\0';
   
#if DEBUG
   fprintf(stderr, "callbacks.c:  Allocated %d bytes for data->filename_only.\n", data->f_length);
#endif   
	
   /*Copy the filename only.*/
   for (i = path_only_count; i != data->fp_length; i++) 
     {
       data->filename_only[j] = data->filename_and_path[i];
       j++;
     }
    //data->filename_only[j] = '\0';  /*Figure out why this is necessary.*/
   /*Set the filename in the entry box.*/
   gtk_entry_set_text(GTK_ENTRY (filename_box), data->filename_only);


   /*Set our gui so that all of our buttons active.*/
   gtk_widget_set_sensitive(split_button, TRUE);
   gtk_widget_set_sensitive(combine_button, TRUE);
   gtk_widget_set_sensitive(batch_file_button, TRUE);
   gtk_widget_set_sensitive(size_input, TRUE);
   gtk_widget_set_sensitive(chunk_size_units, TRUE);
   gtk_widget_set_sensitive(start_button, TRUE);

#if DEBUG 
   fprintf(stderr, "callbacks.c:  filename_and_path: %s\n", data->filename_and_path);
   fprintf(stderr, "callbacks.c:  Entire contents of filename_and_path array:\n\n");
   for (i = 0; i != data->fp_length; i++)
     fprintf(stderr, "{%c}", data->filename_and_path[i]);

   fprintf(stderr, "\n\ncallbacks.c:  filename_only: %s\n", data->filename_only);
   fprintf(stderr, "callbacks.c:  Entire contents of filename_only array:\n\n");
   for (i = 0; i != data->f_length; i++)
     fprintf(stderr, "{%c}", data->filename_only[i]);
   fprintf(stderr, "\n\n");
#endif  
}

void choose_file(GtkWidget *tmp, session_data *data)
{  
   /*Simple gtk open dialog, copied and pasted from the tutorial.*/
   file_selector = gtk_file_selection_new("Choose a file.");	
   gtk_file_selection_set_filename (GTK_FILE_SELECTION (file_selector), home_dir);
   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION (file_selector)->ok_button),
	              "clicked", GTK_SIGNAL_FUNC(setfilename), data);
   gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION (file_selector)->ok_button),
	                     "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy),
                             (gpointer) file_selector);	
   gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION (file_selector)->cancel_button),
	                     "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy),
                             (gpointer) file_selector);
   gtk_widget_show(file_selector);
}

void set_bytes(GtkWidget *tmp, session_data *data)
{
   data->unit = BYTES;
}

void set_kbytes(GtkWidget *tmp, session_data *data)
{
   data->unit = KBYTES;
}

void set_mbytes(GtkWidget *tmp, session_data *data)
{
   data->unit = MBYTES;
}

void set_data(GtkWidget *tmp, session_data *data)
{
   /*When the value in the spin button is changed, store the value.*/
   data->entry = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON (size_input) );
}

void initialize_data(session_data *data)	
{
   /*Our arrays are cleared in setfilename()*/ 
   /*Split is default action.*/
   data->split = TRUE;

   /*Insure that the split button is clicked.*/
   gtk_button_clicked(GTK_BUTTON (split_button) );
   
   /*Initialize dynamic array information.*/
   data->filename_and_path = NULL;
   data->filename_only = NULL;
   data->fp_length = 0;
   data->f_length = 0;

   /*Set the default chunk_size. (1.44MB)*/
   data->entry = 1457664;
   data->chunk_size = 1457664; 
   gtk_adjustment_set_value(size_input_adj , 1457664);

   /*Disable all buttons, except for the open button.
     This is so that nothing is done until a file is selected.*/

   /*Don't create a dos batch file by default.*/
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (batch_file_button), FALSE);
   data->create_batchfile = FALSE;
   gtk_widget_set_sensitive(split_button, FALSE);
   gtk_widget_set_sensitive(combine_button, FALSE);
   gtk_widget_set_sensitive(batch_file_button, FALSE);
   gtk_widget_set_sensitive(size_input, FALSE);
   gtk_widget_set_sensitive(chunk_size_units, FALSE);
   gtk_widget_set_sensitive(start_button, FALSE);
   /*Clear the filename in the entry box.*/
   gtk_entry_set_text(GTK_ENTRY(filename_box), "");
}

void toggle_split(GtkWidget *tmp, session_data *data)
{
   data->split = TRUE;
   /*Enable buttons related to the split action.*/
   gtk_widget_set_sensitive(batch_file_button, TRUE);
   gtk_widget_set_sensitive(size_input, TRUE);
   gtk_widget_set_sensitive(chunk_size_units, TRUE);
}

void toggle_combine(GtkWidget *tmp, session_data *data)
{
   data->split = FALSE;
   /*Disable buttons not related to the combine action.*/
   gtk_widget_set_sensitive(batch_file_button, FALSE);
   gtk_widget_set_sensitive(size_input, FALSE);
   gtk_widget_set_sensitive(chunk_size_units, FALSE);
}

void toggle_batch(GtkWidget *tmp, session_data *data)
{
   data->create_batchfile = !data->create_batchfile;
}

void start(GtkWidget *tmp, session_data *data)
{
   gboolean stable;

   if (data->split)
     stable = split(tmp, data);
   else
     stable = combine(tmp, data);
#if DEBUG
   if (stable)
     fprintf(stderr, "callbacks.c:  split/combine returned stable.\n");
   else
     fprintf(stderr, "callbacks.c:  split/combine returned unstable.\n");
#endif
   /*if (!stable)
     gtk_main_quit();*/
}
