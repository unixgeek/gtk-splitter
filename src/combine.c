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
   progress_window *progress;
   gboolean do_progress, done;
   FILE *in, *out;
   guint file_count, temp, files_to_combine;
   gulong file_size, byte_count, bytes_read;
   gchar *outfile, *infile, ext[] = {"001"};
   gushort outfile_length, infile_length;
   struct stat file_info;

   files_to_combine = 0;
   file_size = 0;
   bytes_read = 0;

   /*Setup the outfile.*/
   outfile_length = ( strlen( data->output_dir ) + data->f_length );  /*f_length includes space for '\0'.*/
   outfile = g_malloc( outfile_length  * sizeof( gchar ) );
   if ( outfile == NULL )
     {
       display_error( "\ncombine.c:  Could not allocate memory for outfile string.\n", TRUE );
       return FALSE;
     }
   strcpy( outfile, data->output_dir );
   strcat( outfile, data->filename_only );
   /*The outfile to be created is the filename_only minus the '.00x' extension.
     NOTE:  outfile_length contains an extra count for the '\0' at the end.
     (i.e. the end of outfile might be:  [.] [0] [0] [1] [\0].)*/ 
   outfile[outfile_length - 5] = '\0';
   /*Outfile is ready.*/

   /*Setup the infile.*/
   infile_length = data->fp_length;
   infile = g_malloc( data->fp_length * sizeof( gchar ) );
   if ( infile == NULL )
     {
       display_error( "\ncombine.c:  Could not allocate memory for infile string.\n", TRUE );
       g_free( outfile );
       return FALSE;
     }
   strcpy( infile, data->filename_and_path );
   infile[infile_length - 1] = '\0';
   /*Infile is ready.*/

   /*Do some pre-combine calcualations.*/
   done = FALSE;
   do
     {
       in = fopen( infile, "rb" );
       /*If the file cannot be opened, assume it does not exist.
         Iterate through the extensions.  (.001, .002, .003, ...)*/
       if ( in == NULL )
         done = TRUE;
       else
         {
           files_to_combine++;

           stat(infile, &file_info);
           file_size += file_info.st_size;

           /*Increment the extension.*/
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

           /*Move on to the next file.*/
           fclose( in );
           infile[strlen(infile) - 3] = '\0';
           strcat( infile, ext );
         }

     } while ( !done );
     /*End of pre-combine calculations.
       We now know how many files there are to be combined, and what the size
       of the combined file should be.*/

   /*Make sure the files don't exceed our limit.*/
   if ( files_to_combine > 999 )
     {
       display_error( "\ncombine.c:  Exceeded maximum number of files (999).\n", FALSE );
       g_free( infile );
       g_free( outfile );
       return FALSE;
     }

   /*Reset the extension counter and the infile.*/
   strcpy( ext, "001" );
   infile[strlen( infile ) - 3] = '\0';
   strcat( infile, ext );

   /*Open a file to combine the other files to.*/
   out = fopen( outfile, "wb+" );
   if ( out == NULL )
     {
       display_error( "\ncombine.c:  Could not create an output file.\n", TRUE );
       g_free( infile );
       g_free( outfile );
       return FALSE;
     }

   /*Decide whether or not a progress window would be beneficiary.*/
   if ( file_size <= UPDATE_INTERVAL )
     do_progress = FALSE;
   else
     {
       do_progress = TRUE;
       progress = g_malloc( sizeof( progress_window ) );
       if ( progress == NULL )
         {
           display_error( "\ncombine.c:  Could not allocate memory for a progress window.\n", FALSE );
           /*Try to go on without it.*/
           do_progress = FALSE;
         }
       else
         {
           create_progress_window( progress, "Combine Progress" );
           gtk_widget_show_all( progress->main_window );
           while ( g_main_iteration( FALSE ) );
         }
     }

   /*Now DO the combine.*/
   for (file_count = 0; file_count != files_to_combine; file_count++)
     {
       if ( do_progress )
         progress_window_set_status_text( progress->status, outfile );
       in = fopen( infile, "rb" );
       if ( in == NULL )
         {
           display_error( "\ncombine.c:  Could not open one of the files to be combined.\n", FALSE );
           if ( do_progress )
             {
               destroy_progress_window( progress );
               g_free( progress );
             }
           fclose( out );
           g_free( infile );
           g_free( outfile );
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
               progress_window_set_percentage( progress->current_progress,
                                               ( ( gfloat ) byte_count ) / ( ( gfloat ) file_info.st_size ) );
               progress_window_set_percentage( progress->total_progress,
                                               ( ( gfloat ) bytes_read ) / ( ( gfloat ) file_size ) );
             }
         }

       /*Insure that all data is written to disk before we quit.*/
       fflush( out );

       /*Increment the extension.*/
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

       /*Move on to the next file.*/
       if ( fclose( in ) == EOF )
         {
           display_error( "\ncombine.c:  Could not open one of the files to be combined.\n", FALSE );
           if ( do_progress )
             {
               destroy_progress_window( progress );
               g_free( progress );
             }
           fclose( out );
           g_free( infile );
           g_free( outfile );
           return FALSE;
         }
       infile[strlen( infile ) - 3] = '\0';
       strcat( infile, ext );
     }
     /*End of the combine process.*/

   /*Close our newly combined file.*/
   if ( fclose( out ) == EOF )
     {
       display_error( "\ncombine.c:  Could not close the combined file.\n", FALSE );
       if ( do_progress )
         {
           destroy_progress_window( progress );
           g_free( progress );
         }
       g_free( infile );
       g_free( outfile );
       return TRUE;
     }
     
   /*Verify by md5sum if desired.*/
   if ( data->verify )
     {
       if ( do_progress ) 
         progress_window_set_status_text( progress->status, "Verifying file..." );
       
       if ( verify_file( outfile, data->fp_length ) != 0 )
          display_error( "\ncombine.c:  File verification failed!\n", FALSE );
     }
   
   /*Free the memory we allocated.*/
   g_free( infile );
   g_free( outfile );

   if ( do_progress )
     {
       gtk_widget_hide_all( progress->main_window );
       destroy_progress_window( progress );
       g_free( progress );
     }

   return TRUE;
}
