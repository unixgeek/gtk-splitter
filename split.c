/* 
 * split.c
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
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "batchfile.h"
#include "globals.h"
#include "callbacks.h"
#include "error.h"
#include "progress.h"

gboolean split(GtkWidget *tmp, session_data *data)
{
   progress_window *progress;
   gboolean do_progress;

   /*File streams.*/
   FILE *in, *out, *batch;

   gchar *infile, *outfile, *batchname_and_path, *outfile_only, ext[] = {"001"};
   gushort infile_length, outfile_length, bp_length, outfile_only_length;
   gint temp;

   /*Needed by the stat function.*/
   gint number_of_even_files, file_count, files_to_split;
   gulong file_size, size_of_leftover_file, byte_count, bytes_read = 0;

   struct stat file_info;

   /*Doing this in callbacks doesn't work...(?)*/
   switch (data->unit) 
     {
       case BYTES   :  data->chunk_size = data->entry;
                       break;
       case KBYTES	:  data->chunk_size = (data->entry * 1024);
			   break;
       case MBYTES  :  data->chunk_size = (data->entry * 1024 * 1024);
			   break;
       default		:  break;
     }

   infile_length = data->fp_length;
   infile = g_malloc( infile_length * sizeof(gchar) );
   if (infile == NULL)
     {
       fprintf(stderr, "split.c:  Could allocate memory for infile string.\n");
       display_error("\nsplit.c:  Could allocate memory for infile string.\n", TRUE);
       return FALSE;
     }
   strcpy(infile, data->filename_and_path);
   infile[infile_length - 1] = '\0';
   
   stat(infile, &file_info);
   file_size = file_info.st_size;
   number_of_even_files = (gint) file_info.st_size/data->chunk_size;
   size_of_leftover_file = file_size - (number_of_even_files * data->chunk_size);

   if (data->chunk_size >= file_size) 
     {
       fprintf(stderr, "split.c:  Chunk size is greater than or equal to the file size.\n");
       display_error("\nsplit.c:  Chunk size is greater than or equal to the file size.\n", FALSE);
       g_free(infile);
       return FALSE;
     }

   if (size_of_leftover_file == 0)
     files_to_split = number_of_even_files;
   else
     files_to_split = number_of_even_files + 1;

   if (files_to_split > 999)
     {
       fprintf(stderr, "split.c:  Exceeded maximum number of files (999).\n");
       display_error("\nsplit.c:  Exceeded maximum number of files (999).\n", FALSE);
       g_free(infile);
       return FALSE;
     }

   if (file_size <= (UPDATE_INTERVAL * 4))
     do_progress = FALSE;
   else
     do_progress = TRUE;

   if (do_progress) 
     {
       progress = g_malloc(sizeof(progress_window));
       if (progress == NULL)
         {
           fprintf(stderr, "split.c:  Could not allocate memory for a progress window.\n");
           display_error("\nsplit.c:  Could not allocate memory for a progress window.\n", FALSE);
           /*Try to go on without it.*/
           do_progress = FALSE;
         }
       else
         create_progress_window(progress, "Split Progress");
     }
   
   /*BATCH FILE FOR DOS*/
   /*Do we need to create a batch file?*/
   if (data->create_batchfile) 
     {
       outfile_only_length = data->f_length + 4;  /*f_length includes space for '\0'.*/
       outfile_only = g_malloc( outfile_only_length * sizeof(gchar) );
       if (outfile_only == NULL)
         {
           fprintf(stderr, "split.c:  Could not allocate memory for outfile_only string.\n");
           display_error("\nsplit.c:  Could not allocate memory for outfile_only string.\n", TRUE);
           if (do_progress)
             {
               destroy_progress_window(progress);
               g_free(progress);
             }
           g_free(infile);
           return FALSE;
         }

       if ((data->f_length - 1) > 12)
         {
           dosify_filename(data->filename_only, data->f_length);
           data->f_length = 13;
         }
       strcpy(outfile_only, data->filename_only);
       /*Reserve four spaces for the extension; it will change later.*/
       strcat(outfile_only, ".ext");
       bp_length = data->f_length + strlen(data->output_dir) + 4;  /*f_length includes space for '\0'.*/
       batchname_and_path = g_malloc( bp_length * sizeof(gchar));  
       if (batchname_and_path == NULL)
         {
           fprintf(stderr, "split.c:  Could not allocate memory for batchname_and_path string.\n");
           display_error("\nsplit.c:  Could not allocate memory for batchname_and_path string.\n", TRUE);
           g_free(outfile_only);
           if (do_progress)
             {
               destroy_progress_window(progress);
               g_free(progress);
             }
           g_free(infile);
           return FALSE;
         }

       strcpy(batchname_and_path, data->output_dir);
       strcat(batchname_and_path, data->filename_only);
       strcat(batchname_and_path, ".bat");

       batch = fopen(batchname_and_path, "w+");
       if (batch == NULL) 
         {
           fprintf(stderr, "aplit.c:  Could not create the batch file.\n");
           display_error("\nsplit.c:  Could not create the batch file.\n", TRUE);
           g_free(batchname_and_path);
           g_free(outfile_only);
           if (do_progress)
             {
               destroy_progress_window(progress);
               g_free(progress);
             }           
           g_free(infile);
           return FALSE;
	 }

       initialize_batchfile(batch, data->filename_only);
     }
   /*END OF BATCH FILE*/
   
   /*Setup the outfile.*/
   outfile_length = data->f_length + strlen(data->output_dir) + 4 ;  /*f_length includes space for '\0'.*/
   outfile = g_malloc( outfile_length * sizeof(char) );
   if (outfile == NULL)
     {
       fprintf(stderr, "split.c:: Could not allocate memory for outfile string.\n");
       display_error("\nsplit.c:: Could not allocate memory for outfile string.\n", TRUE);
       if (data->create_batchfile)
         {
           fclose(batch);
           g_free(batchname_and_path);
           g_free(outfile_only);
         }       
       if (do_progress)
         {
           destroy_progress_window(progress);
           g_free(progress);
         }
       g_free(infile);
       return FALSE;
     }

   strcpy(outfile, data->output_dir);
   strcat(outfile, data->filename_only);
   strcat(outfile, ".");
   strcat(outfile, ext);

   /*Open the selected file.*/
   in = fopen(infile, "rb");
   if (in == NULL) 
     {
       fprintf(stderr, "split.c:  Could not open the selected file.\n");
       display_error("\nsplit.c:  Could not open the selected file.\n", TRUE);
       g_free(outfile);
       if (data->create_batchfile)
         {
           fclose(batch);
           g_free(batchname_and_path);
           g_free(outfile_only);
         }       
       if (do_progress)
         {
           destroy_progress_window(progress);
           g_free(progress);
         }
       g_free(infile);
       return FALSE;
     }

   /*Open the first outfile.*/
   out = fopen(outfile, "wb+");
   if (out == NULL) 
     {
       fprintf(stderr, "split.c:  Could not create an output file.\n");
       display_error("\nsplit.c:  Could not create an output file.\n", FALSE);
       fclose(in);
       g_free(outfile);
       if (data->create_batchfile)
         {
           fclose(batch);
           g_free(batchname_and_path);
           g_free(outfile_only);
         }       
       if (do_progress)
         {
           destroy_progress_window(progress);
           g_free(progress);
         }
       g_free(infile);
       return FALSE;
     }

   /*The actual split process.*/
   for (file_count = 1; file_count <= files_to_split; file_count++) 
     {
       if (do_progress)  /*Display the file we are creating.*/
         gtk_statusbar_push(GTK_STATUSBAR (progress->status), 1, outfile);
       if ( (size_of_leftover_file != 0) && (file_count  == (files_to_split) ) ) 
         {
           for (byte_count = 1; (byte_count <= size_of_leftover_file); byte_count++) 
             {
               temp = fgetc(in);
               fputc(temp, out);
               bytes_read++;
               if ((do_progress) && ((byte_count % UPDATE_INTERVAL) == 0)) 
                 {
                   gtk_progress_set_percentage(GTK_PROGRESS (progress->current_progress), 
                                              ((double) byte_count) / ((double) size_of_leftover_file));
                   gtk_progress_set_percentage(GTK_PROGRESS (progress->total_progress),
                                              ((double) bytes_read) / ((double) file_size));
                   while (g_main_iteration(FALSE));
                 }
             }  
         }
       else 
         {
           for (byte_count = 1; (byte_count <= data->chunk_size); byte_count++)
             {
               temp = fgetc(in);
               fputc(temp, out);
               bytes_read++;
               if ((do_progress) && ((byte_count % UPDATE_INTERVAL) == 0)) 
                 {
                   gtk_progress_set_percentage(GTK_PROGRESS (progress->current_progress),
							          ( (double) byte_count) / ( (double) data->chunk_size) );
                   gtk_progress_set_percentage(GTK_PROGRESS (progress->total_progress),
							          ( (double) bytes_read) / ( (double) file_size) );
                   while (g_main_iteration(FALSE));
                 }
             }
         }

       /*Copy the outfile name to the batch file.*/
       if (data->create_batchfile) 
         {
           if (strcmp(ext, "001") != 0)
             write_batchfile(batch, "+");
           outfile_only[strlen(outfile_only) - 4] = '\0';
           strcat(outfile_only, ".");
           strcat(outfile_only, ext);
           write_batchfile(batch, outfile_only);
         }

       /*Increment the extension.
         Remember, the extension looks like this: .[0][1][2]*/
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

       /*Insure that all data is written to disk before we quit.*/
       fflush(out);
       if (fclose(out) == EOF)
         {
           fprintf(stderr, "split.c:  Could not close an output file.\n");
           display_error("\nsplit.c:  Could not close an output file.\n", TRUE);
           fclose(in);
           g_free(outfile);
           if (data->create_batchfile)
             {
               fclose(batch);
               g_free(batchname_and_path);
               g_free(outfile_only);
             }       
          if (do_progress)
            {
              destroy_progress_window(progress);
              g_free(progress);
            }
          g_free(infile);
          return FALSE;
         }

       /*Close the outfile and start a new one.*/
       if (bytes_read != file_size) 
         {
            /*Setup a new outfile.*/
           outfile[strlen(outfile) - 3] = '\0';
           strcat(outfile, ext);
           
           out = fopen(outfile, "wb+");
           if (out == NULL) 
             {
               fprintf(stderr, "split.c:  Could not create an output file.");
               display_error("\nsplit.c:  Could not create an output file.\n", TRUE);
               fclose(in);
               g_free(outfile);
               if (data->create_batchfile)
                 {
                   fclose(batch);
                   g_free(batchname_and_path);
                   g_free(outfile_only);
                 }       
               if (do_progress)
                 {
                   destroy_progress_window(progress);
                   g_free(progress);
                 }
               g_free(infile);
               return FALSE;
             }
         }
     }
     /*End of split process.*/
    
     if (fclose(in) == EOF) 
         {
           fprintf(stderr, "split.c:  Could not close the selected file.\n");
           display_error("\nsplit.c:  Could not close the selected file.\n", FALSE);
           g_free(outfile);
           if (data->create_batchfile)
             {
               fclose(batch);
               g_free(batchname_and_path);
               g_free(outfile_only);
             }       
           if (do_progress)
             {
               destroy_progress_window(progress);
               g_free(progress);
             }
           g_free(infile);
           return TRUE;
         }

   if (data->create_batchfile) 
     {
       write_batchfile(batch, " ");
       write_batchfile(batch, data->filename_only);
       finalize_batchfile(batch); 
       g_free(batchname_and_path);
       g_free(outfile_only);
       if (fclose(batch) == EOF) 
         {
           fprintf(stderr, "split.c:  Could not close the batch file.\n");
           display_error("\nsplit.c:  Could not close the batch file.\n", FALSE);
           g_free(outfile);
           g_free(outfile_only);
           if (do_progress)
             {
               destroy_progress_window(progress);
               g_free(progress);
             }
           g_free(infile);
           return TRUE;
         }
     }
  

   g_free(infile);
   g_free(outfile);
	
   /*Hide the status window and unhide our gui.*/
   if (do_progress) 
     {
       destroy_progress_window(progress);
       g_free(progress);
     }

   return TRUE;
}
