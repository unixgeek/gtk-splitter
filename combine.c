/* 
 * combine.c
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
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "globals.h"
#include "callbacks.h"
#include "error.h"
#include "progress.h"

gboolean combine(GtkWidget *tmp, session_data *data)
{
   progress_window *progress;
   gboolean do_progress, done = FALSE;

   FILE *in, *out;
   gint file_count, temp, files_to_combine = 0;
   gulong file_size = 0, byte_count, bytes_read = 0;

   gchar *outfile, *infile, ext[] = {"001"};

   gushort outfile_length, infile_length;

   struct stat file_info;
#if DEBUG
   gushort i;
#endif
 
   stat(data->filename_and_path, &file_info);  /*Not necessary?*/

   /*Setup the outfile.*/
   outfile_length = (strlen(home_dir) + data->f_length);  /*f_length includes space for '\0'.*/
   outfile = g_malloc( outfile_length  * sizeof(gchar) );
   if (outfile == NULL)
     {
       fprintf(stderr, "combine.c:  (outfile) Could not allocate any memory.\n");
       display_error("Could not allocate any memory.", TRUE);
       return FALSE;
     }
   /*Setup the infile.*/
   infile_length = data->fp_length;
   infile = g_malloc( data->fp_length * sizeof(gchar) );
   if (infile == NULL)
     {
       fprintf(stderr, "combine.c:  (infile) Could not allocate any memory.\n");
       display_error("Could not allocate any memory.", TRUE);
       g_free(outfile);
       return FALSE;
     }
   strcpy(infile, data->filename_and_path);
   infile[infile_length - 1] = '\0';
#if DEBUG
   fprintf(stderr, "combine.c:  Allocated %d bytes for infile.\n", infile_length);
   fprintf(stderr, "combine.c:  Allocated %d bytes for outfile.\n", outfile_length);
#endif
   strcpy(outfile, home_dir);
   strcat(outfile, data->filename_only);
   outfile[outfile_length - 5] = '\0';
 
#if DEBUG
    fprintf(stderr, "combine.c:  infile: %s\n", infile);
    fprintf(stderr, "combine.c:  Entire contents of infile:\n\n");
    for (i = 0; i != infile_length; i++)
      fprintf(stderr, "{%c}", infile[i]);
    fprintf(stderr, "\n\n");
    fprintf(stderr, "combine.c:  outfile: %s\n", outfile);
    fprintf(stderr, "combine.c:  Entire contents of outfile:\n\n");
    for (i = 0; i != outfile_length; i++)
      fprintf(stderr, "{%c}", outfile[i]);
    fprintf(stderr, "\n\n");
#endif

   /*Do some pre-combine calcualations.*/
   do
     {
       in = fopen(infile, "rb");
       if (in == NULL)
         done = TRUE;
       else 
         {
           stat(infile, &file_info);
           files_to_combine++;
#if DEBUG
             fprintf(stderr, "combine.c:  File size of %s is %lu\n", infile,
                     file_info.st_size);
#endif
           file_size += file_info.st_size;

           /*Increment the extension.*/
           if (ext[2] != '9')
             ext[2]++;
           else 
             {
               ext[2] = '0';
               if (ext[1] != '9')
                 ext[1]++;
               else 
                 {
               	   ext[1] = '0';
               	   ext[0]++;
               	 }
             }
           
           /*Move on to the next file.*/
           fclose(in);
           infile[strlen(infile) - 3] = '\0';
           strcat(infile, ext);
         } 


     } while (!done);
     /*End of pre-combine calculations.*/

   if (files_to_combine > 999)
     {
       fprintf(stderr, "combine.c:  Exceeded maximum number of files.  (>999)\n");
       display_error("Exceeded maximum number of files.  (>999)", FALSE);
       g_free(infile);
       g_free(outfile);
       return TRUE;
     }	

   strcpy(ext, "001");
   infile[strlen(infile) - 3] = '\0';
   strcat(infile, ext);

#if DEBUG
   fprintf(stderr, "combine.c:  files_to_combine: %d\n", files_to_combine);
   fprintf(stderr, "combine.c:  Combined file should be %lu bytes\n", file_size);
   fprintf(stderr, "combine.c:  Begin of combine process.\n");
#endif

   out = fopen(outfile, "wb+");
   if (out == NULL) 
     {
       fprintf(stderr, "combine.c:  Error creating %s.\n", outfile);
       display_error("Could not open output file.", TRUE);
       g_free(infile);
       g_free(outfile);
       return FALSE;
     }

  
   if (file_size <= (UPDATE_INTERVAL * 4))
     do_progress = FALSE;
   else
     do_progress = TRUE;

   if (do_progress) 
     {
       progress = g_malloc(sizeof(progress_window));
       create_progress_window(progress, "Combine Progress");
       gtk_widget_hide_all(data->window);
     }

   /*Now DO the combine.*/
   for (file_count = 0; file_count != files_to_combine; file_count++)
     {
       if (do_progress)
         gtk_statusbar_push(GTK_STATUSBAR (progress->status), 1, outfile);
       in = fopen(infile, "rb");
       stat(infile, &file_info);
       for (byte_count = 0; byte_count != file_info.st_size; byte_count++) 
         {
           bytes_read++;
           temp = fgetc(in);
           fputc(temp, out);
           if ((do_progress) && ((byte_count % UPDATE_INTERVAL) == 0)) 
             {
               gtk_progress_set_percentage(GTK_PROGRESS (progress->file_progress),
                                          ( (double) byte_count) / ((double) file_info.st_size) );
               gtk_progress_set_percentage(GTK_PROGRESS (progress->total_progress),
                                          ( (double) bytes_read) / ((double) file_size) );
               while (g_main_iteration(FALSE));
             }
         }

       /*Insure that all data is written to disk before we quit.*/
       fflush(out);

       /*Increment the extension.*/
       if (ext[2] != '9')
         ext[2]++;
       else
         {
           ext[2] = '0';
           if (ext[1] != '9')
             ext[1]++;
           else 
             {
               ext[1] = '0';
               ext[0]++;
             }
         }
               	
       /*Move on to the next file.*/
       fclose(in);
       infile[strlen(infile) - 3] = '\0';
       strcat(infile, ext);
     } 
     /*End of combine process.*/

   /*Close our newly combined file.*/
   if (fclose(out) == EOF) 
     {
       fprintf(stderr, "combine.c:  Could not close %s properly.\n", outfile);
       display_error("Could not close the combined file.", TRUE);
       g_free(infile);
       g_free(outfile);
       return FALSE;
     }

   g_free(infile);
   g_free(outfile);

   /*Reset the data.*/
   initialize_data(data);
   if (do_progress) 
     {
       destroy_progress_window(progress);
       g_free(progress);
       gtk_widget_show_all(data->window);
     }
   
#if DEBUG 
     fprintf(stderr, "\ncombine.c:  End of combine process.\n");
     fprintf(stderr, "combine.c:  bytes_read: %lu\n", bytes_read);
#endif

   return TRUE;
}
