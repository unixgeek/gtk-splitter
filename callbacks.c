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

void setfilename(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gchar *tmp_ptr;
   gushort i=0, j=0, path_only_count=0;

   tmp_ptr = gtk_file_selection_get_filename(GTK_FILE_SELECTION (gsw->file_selector) );
   gsw->sdata->fp_length = strlen(tmp_ptr) + 1;  /*Add in a count for the NULL terminator.*/

   g_free(gsw->sdata->filename_and_path);  /*Free any previous sdata.*/
   
   gsw->sdata->filename_and_path = g_malloc( gsw->sdata->fp_length * sizeof(gchar) );
   if (gsw->sdata->filename_and_path == NULL)
     {
       fprintf(stderr, "callbacks.c:  (filename_and_path) Could not allocate any memory.\n");
       display_error("Could not allocate any memory.", TRUE);
     }

#if DEBUG
   fprintf(stderr, "callbacks.c:  Allocated %d bytes for sdata->filename_and_path.\n", gsw->sdata->fp_length);
#endif
   
   strcpy(gsw->sdata->filename_and_path, tmp_ptr);
   gsw->sdata->filename_and_path[gsw->sdata->fp_length - 1] = '\0';
	
   /*Find out what the file name is, without the path.*/
   path_only_count = gsw->sdata->fp_length - 1;
   j = 0;
   /*Start at the end and count backwards till we find a '/'*/
   while (gsw->sdata->filename_and_path[path_only_count] != '/')
     path_only_count--;
	
   path_only_count++; /*add the count of the last '/' */

   gsw->sdata->f_length = gsw->sdata->fp_length - path_only_count;  /*No +1 is needed, already in fp_length.*/

   g_free(gsw->sdata->filename_only);  /*Free any previous sdata.*/

   gsw->sdata->filename_only = g_malloc( gsw->sdata->f_length * sizeof(gchar));
   if (gsw->sdata->filename_only == NULL)
     {
       fprintf(stderr, "callbacks.c:  (filename_only) Could not allocate any memory.\n");
       display_error("Could not allocate any memory.", TRUE);
     }
   gsw->sdata->filename_only[gsw->sdata->f_length -1] = '\0';
   
#if DEBUG
   fprintf(stderr, "callbacks.c:  Allocated %d bytes for sdata->filename_only.\n", gsw->sdata->f_length);
#endif   
	
   /*Copy the filename only.*/
   for (i = path_only_count; i != gsw->sdata->fp_length; i++) 
     {
       gsw->sdata->filename_only[j] = gsw->sdata->filename_and_path[i];
       j++;
     }
    //sdata->filename_only[j] = '\0';  /*Figure out why this is necessary.*/
   /*Set the filename in the entry box.*/
   gtk_entry_set_text(GTK_ENTRY (gsw->filename_box), gsw->sdata->filename_only);


   /*Set our gui so that all of our buttons active.*/
   gtk_widget_set_sensitive(gsw->split_button, TRUE);
   gtk_widget_set_sensitive(gsw->combine_button, TRUE);
   gtk_widget_set_sensitive(gsw->batch_file_button, TRUE);
   gtk_widget_set_sensitive(gsw->size_input, TRUE);
   gtk_widget_set_sensitive(gsw->chunk_size_units, TRUE);
   gtk_widget_set_sensitive(gsw->start_button, TRUE);

#if DEBUG 
   fprintf(stderr, "callbacks.c:  filename_and_path: %s\n", gsw->sdata->filename_and_path);
   fprintf(stderr, "callbacks.c:  Entire contents of filename_and_path array:\n\n");
   for (i = 0; i != gsw->sdata->fp_length; i++)
     fprintf(stderr, "{%c}", gsw->sdata->filename_and_path[i]);

   fprintf(stderr, "\n\ncallbacks.c:  filename_only: %s\n", gsw->sdata->filename_only);
   fprintf(stderr, "callbacks.c:  Entire contents of filename_only array:\n\n");
   for (i = 0; i != gsw->sdata->f_length; i++)
     fprintf(stderr, "{%c}", gsw->sdata->filename_only[i]);
   fprintf(stderr, "\n\n");
#endif  
}

void choose_file(GtkWidget *tmp, gtk_splitter_window *gsw)
{  
   /*Simple gtk open dialog, copied and pasted from the tutorial.*/
   gsw->file_selector = gtk_file_selection_new("Choose a file.");

   gtk_file_selection_set_filename (GTK_FILE_SELECTION (gsw->file_selector), gsw->sdata->home_dir);
   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION (gsw->file_selector)->ok_button),
	              "clicked", GTK_SIGNAL_FUNC(setfilename), gsw);
   gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION (gsw->file_selector)->ok_button),
	                     "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy),
                             (gpointer) gsw->file_selector);	
   gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION (gsw->file_selector)->cancel_button),
	                     "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy),
                             (gpointer) gsw->file_selector);
   gtk_widget_show(gsw->file_selector);
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

void set_data(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   /*When the value in the spin button is changed, store the value.*/
   gsw->sdata->entry = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON (gsw->size_input) );
}

void initialize_session_data(session_data *data)	
{
   /*Our arrays are cleared in setfilename()*/ 
   /*Split is default action.*/
   data->split = TRUE;
   
   /*Initialize dynamic array information.*/
   data->filename_and_path = NULL;
   data->filename_only = NULL;
   data->fp_length = 0;
   data->f_length = 0;

   /*Set the default chunk_size. (1.44MB)*/
   data->entry = 1457664;
   data->chunk_size = 1457664; 
   
}

void initialize_splitter_window(gtk_splitter_window *gsw)
{
   /*Insure that the split button is clicked.*/
   gtk_button_clicked(GTK_BUTTON (gsw->split_button) );
   gtk_adjustment_set_value(gsw->size_input_adj , 1457664);

   /*Disable all buttons, except for the open button.
     This is so that nothing is done until a file is selected.*/

   /*Don't create a dos batch file by default.*/
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (gsw->batch_file_button), FALSE);
   gsw->sdata->create_batchfile = FALSE;
   gtk_widget_set_sensitive(gsw->split_button, FALSE);
   gtk_widget_set_sensitive(gsw->combine_button, FALSE);
   gtk_widget_set_sensitive(gsw->batch_file_button, FALSE);
   gtk_widget_set_sensitive(gsw->size_input, FALSE);
   gtk_widget_set_sensitive(gsw->chunk_size_units, FALSE);
   gtk_widget_set_sensitive(gsw->start_button, FALSE);
   /*Clear the filename in the entry box.*/
   gtk_entry_set_text(GTK_ENTRY(gsw->filename_box), "");
}

void toggle_split(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gsw->sdata->split = TRUE;
   /*Enable buttons related to the split action.*/
   gtk_widget_set_sensitive(gsw->batch_file_button, TRUE);
   gtk_widget_set_sensitive(gsw->size_input, TRUE);
   gtk_widget_set_sensitive(gsw->chunk_size_units, TRUE);
}

void toggle_combine(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gsw->sdata->split = FALSE;
   /*Disable buttons not related to the combine action.*/
   gtk_widget_set_sensitive(gsw->batch_file_button, FALSE);
   gtk_widget_set_sensitive(gsw->size_input, FALSE);
   gtk_widget_set_sensitive(gsw->chunk_size_units, FALSE);
}

void toggle_batch(GtkWidget *tmp, session_data *data)
{
   data->create_batchfile = !data->create_batchfile;
}

void start(GtkWidget *tmp, gtk_splitter_window *gsw)
{
   gboolean do_initialization;

   gtk_widget_hide_all(gsw->base_window);

   if (gsw->sdata->split)
     do_initialization = split(tmp, gsw->sdata);
   else
     do_initialization = combine(tmp, gsw->sdata);

   /*Reset the data.*/
   if (do_initialization)  
     {   
       initialize_session_data(gsw->sdata);
       initialize_splitter_window(gsw);
     }   
   
   gtk_widget_show_all(gsw->base_window);

#if DEBUG
   if (do_initialization)
     fprintf(stderr, "callbacks.c:  split/combine returned stable.\n");
   else
     fprintf(stderr, "callbacks.c:  split/combine returned unstable.\n");
#endif
}
