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
#include "error.h"
#include "progress.h"
#include "md5.h"

gboolean combine(GtkWidget *tmp, session_data *data)
{
   /* Progress window stuff. */
   progress_window progress;
   gboolean do_progress, done;
   
   /* File streams. */
   FILE *in, *out;

   /* Various counters. */
   guint file_count, temp, files_to_combine;
   gulong combined_file_size, byte_count, bytes_read;
   
   /* The name of the first file part (including the path). */
   gchar infile[PATH_MAX];
   
   /* The name of the combined file. */
   gchar outfile[PATH_MAX];
   
   /* The extension for file parts. */
   gchar ext[] = {"001"};
   
   struct stat file_info;


   /* Setup the infile string. */
   strcpy( infile, data->file_name_and_path );
   
   /* Setup the outfile string. */
   strcpy( outfile, data->output_directory );
   strcat( outfile, data->file_name_only );
   
   /* The outfile to be created is the file_name_only minus the '.00x' extension.
      (i.e. the end of outfile might be:  [.] [0] [0] [1].) */ 
   outfile[strlen( outfile ) - 4] = '\0';

   /* Do some pre-combine calcualations. */
   files_to_combine = 0;
   combined_file_size = 0;
   done = FALSE;
   do
     {
       in = fopen( infile, "rb" );
       /* If the file cannot be opened, assume it does not exist.
          Iterate through the extensions.  (.001, .002, .003, ...) */
       if ( in == NULL )
         done = TRUE;
       else
         {
           files_to_combine++;

           stat( infile, &file_info );
           combined_file_size += file_info.st_size;

           /* Increment the extension. */
           if ( ext[2] != '9' )
             ext[2]++;
           else
             {
               ext[2] = '0';
               if ( ext[1] != '9' )
                 ext[1]++;
               else
                 {
                   ext[1] = '0';
                   ext[0]++;
                 }
             }

           /* Move on to the next file. */
           fclose( in );
           infile[strlen( infile ) - 3] = '\0';
           strcat( infile, ext );
         }

     } while ( !done );
     /* End of pre-combine calculations.
        We now know how many files there are to be combined, and what the size
        of the combined file should be. */

   /* Make sure the files don't exceed our limit. */
   if ( files_to_combine > 999 )
     {
       display_error( "combine.c:  Exceeded maximum number of files (999).", FALSE );
       return FALSE;
     }

   /* Reset the extension counter and the infile. */
   strcpy( ext, "001" );
   infile[strlen( infile ) - 3] = '\0';
   strcat( infile, ext );

   /* Open a file to combine the other files to. */
   out = fopen( outfile, "wb+" );
   if ( out == NULL )
     {
       display_error( "combine.c:  Could not create an output file.", TRUE );
       return FALSE;
     }

   /* Decide whether or not a progress window would be beneficiary. */
   if ( combined_file_size <= UPDATE_INTERVAL )
     do_progress = FALSE;
   else
     {
       do_progress = TRUE;
       create_progress_window( &progress, "Combine Progress" );
       gtk_widget_show_all( progress.main_window );
       while ( g_main_iteration( FALSE ) );
     }

   /* Now DO the combine. */
   bytes_read = 0;
   for (file_count = 0; file_count != files_to_combine; file_count++)
     {
       if ( do_progress )
         progress_window_set_status_text( progress.status, outfile );
       in = fopen( infile, "rb" );
       if ( in == NULL )
         {
           display_error( "combine.c:  Could not open one of the files to be combined.", FALSE );
           if ( do_progress )
              destroy_progress_window( &progress );
           fclose( out );
           return FALSE;
         }
       stat( infile, &file_info );
       for ( byte_count = 0; byte_count != file_info.st_size; byte_count++ )
         {
           bytes_read++;
           temp = fgetc( in );
           fputc( temp, out );
           if ( ( do_progress ) && ( ( byte_count % UPDATE_INTERVAL ) == 0 ) )
             {
               progress_window_set_percentage( progress.current_progress,
                                               ( ( gfloat ) byte_count ) / ( ( gfloat ) file_info.st_size ) );
               progress_window_set_percentage( progress.total_progress,
                                               ( ( gfloat ) bytes_read ) / ( ( gfloat ) combined_file_size ) );
             }
         }

       /* Insure that all data is written to disk before we close. */
       fflush( out );

       /* Increment the extension. */
       if ( ext[2] != '9' )
         ext[2]++;
       else
         {
           ext[2] = '0';
           if ( ext[1] != '9' )
             ext[1]++;
           else
             {
               ext[1] = '0';
               ext[0]++;
             }
         }

       /* Move on to the next file. */
       if ( fclose( in ) == EOF )
         {
           display_error( "combine.c:  Could not open one of the files to be combined.", FALSE );
           if ( do_progress )
              destroy_progress_window( &progress );
           fclose( out );
           return FALSE;
         }
         
       infile[strlen( infile ) - 3] = '\0';
       strcat( infile, ext );
     }
     /* End of the combine process. */

   /* Close our newly combined file. */
   if ( fclose( out ) == EOF )
     {
       display_error( "combine.c:  Could not close the combined file.", FALSE );
       if ( do_progress )
          destroy_progress_window( &progress );
       return TRUE;
     }
     
   /* Verify if desired. */
   if ( data->verify )
     {
       //if ( do_progress ) 
         //progress_window_set_status_text( progress->status, "Verifying file..." );
       
       //if ( verify_file( outfile, data->fp_length ) != 0 )
          //display_error( "\ncombine.c:  File verification failed!\n", FALSE );
     }
   
   if ( do_progress )
     {
       gtk_widget_hide_all( progress.main_window );
       destroy_progress_window( &progress );
     }

   return TRUE;
}
