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
#include <sys/stat.h>
#include <string.h>

#include "dostextfile.h"
#include "globals.h"
#include "error.h"
#include "progress.h"
#include "md5.h"

gboolean split(GtkWidget *tmp, session_data *data)
{
   /* Progress window stuff. */
   progress_window *progress;
   gboolean do_progress;

   /* FILE streams. */
   FILE *in, *out, *batch;
   
   /* Various character array pointers. */
   gchar infile[PATH_MAX], outfile[PATH_MAX], batchname_and_path[PATH_MAX], outfile_only[PATH_MAX], original_name[PATH_MAX], ext[] = {"001"};
  
   /* For storing the character read in by fgetc(). */
   gint temp;

   /* Various counters. */
   guint number_of_even_files, file_count, files_to_split;

   /* Variables for file sizes and progress tracking. */
   gulong file_size, size_of_leftover_file, byte_count, bytes_read;
   struct stat file_info;

   bytes_read = 0;
  
   /* Determine the unit of measurement to evaluate the chunk_size.
      FIX ME:  chunk_size should probably be moved from the struct to a local variable. */
   switch ( data->unit )
     {
       case BYTES      :  data->chunk_size = data->entry;
                          break;
       case KILOBYTES  :  data->chunk_size = ( data->entry * 1024 );
			                 break;
       case MEGABYTES  :  data->chunk_size = ( data->entry * 1024 * 1024 );
			                 break;
     }

   /* Set up the infile array. */
   strcpy( infile, data->file_name_and_path );
   
   /* Figure out how many files to split the selected file into. */
   stat( infile, &file_info );
   file_size = file_info.st_size;
   number_of_even_files = ( gint ) file_info.st_size/data->chunk_size;
   size_of_leftover_file = file_size - ( number_of_even_files * data->chunk_size );

   if ( size_of_leftover_file == 0 )
     files_to_split = number_of_even_files;
   else
     files_to_split = number_of_even_files + 1;

   /* Make sure we are not trying to split the file into pieces larger than itself. */
   if ( data->chunk_size >= file_size )
     {
       display_error( "\nsplit.c:  Chunk size is greater than or equal to the file size.\n", FALSE );
       return FALSE;
     }


   /* Don't create more than 999 files.  Actually, since we are starting at 1, the limit is really
      998 files.  (001 - 999) */
   if ( files_to_split > 999 )
     {
       display_error( "\nsplit.c:  Exceeded maximum number of files (999).\n", FALSE );
       return FALSE;
     }

   /* Decide whether or not a progress window would be beneficiary. */
   if ( file_size <= UPDATE_INTERVAL )
     do_progress = FALSE;
   else
     {
       do_progress = TRUE;
       progress = g_malloc( sizeof ( progress_window ) );
       if ( progress == NULL )
         {
           display_error( "\nsplit.c:  Could not allocate memory for a progress window.\n", FALSE );
           /* Try to go on without it. */
           do_progress = FALSE;
         }
       else
         {
           create_progress_window( progress, "Split Progress" );
           gtk_widget_show_all( progress->main_window );
           while ( g_main_iteration( FALSE ) );
         }
     }

   /*---Setup a batchfile if so desired.------------------------------------------------------------*/
   if ( data->create_batchfile )
     {
 
       strcpy( original_name, data->file_name_only );

       /* If the file name is too long, truncate it.
          Also, convert some characters to '_'. */
       dosify_file_name( data->file_name_only );

       /* dosify_filename() will truncate the file name if it is longer than 12 characters. */
       if ( strlen( data->file_name_only ) > 12 )
         {
			  data->file_name_only[13] = '\0';
         }

       strcpy( outfile_only, data->file_name_only );
       /* Reserve four spaces for the extension; it will change later. */
       strcat(outfile_only, ".ext");
       
       strcpy( batchname_and_path, data->output_directory );
       strcat( batchname_and_path, data->file_name_only );
       strcat( batchname_and_path, ".bat" );

       batch = fopen( batchname_and_path, "wb+" );
       if ( batch == NULL )
         {
           display_error( "\nsplit.c:  Could not create the batch file.\n", TRUE );
           if ( do_progress )
             {
               destroy_progress_window( progress );
               g_free( progress );
             }
           return FALSE;
	      }
       /* Write some header information to the batchfile. */
       writeln_dostextfile( batch, "@Echo Off" );
       write_dostextfile( batch, "Echo " );
       write_dostextfile( batch, PACKAGE );
       write_dostextfile( batch, "-" );
       write_dostextfile( batch, VERSION );
       writeln_dostextfile( batch, " Gunter Wambaugh" );
       writeln_dostextfile( batch, "Echo NOTE:  The file name may have been modified to ensure compatibility with the DOS copy utility.");
       write_dostextfile( batch, "Echo Original File Name:  " );
       writeln_dostextfile( batch, original_name );
       write_dostextfile( batch, "Echo Creating " );
       writeln_dostextfile( batch, data->file_name_only );
       write_dostextfile( batch, "copy /b " );
       /* End of header information */
     }
   /*---Done with setting up the batcfhile.-------------------------------------------------------------*/


   /* Setup the outfile array. */
   strcpy( outfile, data->output_directory );
   strcat( outfile, data->file_name_only );
   strcat( outfile, "." );
   strcat( outfile, ext );

   /*Create an md5sum of the selected file, if desired.*/
   //if ( data->verify )
     //{
       //if (do_progress )
         //progress_window_set_status_text( progress->status, "Generating MD5 checksum.." );
       
       //create_sum( data->file_name_and_path, data->fp_length, data->output_directory );
     //} 
     
   /* Open the selected file. */
   in = fopen( infile, "rb" );
   if ( in == NULL )
     {
       display_error( "\nsplit.c:  Could not open the selected file.\n", TRUE );
       if ( data->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
         {
           destroy_progress_window( progress );
           g_free( progress );
         }
       return FALSE;
     }

   /* Open the first outfile. */
   out = fopen( outfile, "wb+" );
   if ( out == NULL )
     {
       display_error( "\nsplit.c:  Could not create an output file.\n", FALSE );
       fclose( in );
       if ( data->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
         {
           destroy_progress_window( progress );
           g_free( progress );
         }
       return FALSE;
     }

   /* The actual split process. */
   for ( file_count = 1; file_count <= files_to_split; file_count++ )
     {
       if ( do_progress )  /* Display the file name we are creating. */
         progress_window_set_status_text( progress->status, outfile );
       /* The leftover file. (?) */
       if ( ( size_of_leftover_file != 0 ) && ( file_count  == ( files_to_split ) ) )
         {
           for ( byte_count = 1; ( byte_count <= size_of_leftover_file ); byte_count++ )
             {
               temp = fgetc( in );
               fputc( temp, out );
               bytes_read++;
               if ( ( do_progress ) && ( ( byte_count % UPDATE_INTERVAL ) == 0 ) )
                 {
                   progress_window_set_percentage( progress->current_progress,
                                                 ( ( gfloat ) byte_count ) / 
                                                 ( ( gfloat ) size_of_leftover_file ) );
                   progress_window_set_percentage( progress->total_progress,
                                                 ( ( gfloat ) bytes_read ) /
                                                 ( ( gfloat ) file_size ) );
                 }
             }
         }
       else  /* Even files. (?) */
         {
           for ( byte_count = 1; ( byte_count <= data->chunk_size ); byte_count++ )
             {
               temp = fgetc( in );
               fputc( temp, out );
               bytes_read++;
               if ( ( do_progress ) && ( ( byte_count % UPDATE_INTERVAL ) == 0 ) )
                 {
                   progress_window_set_percentage( progress->current_progress,
							                            ( ( gfloat ) byte_count ) / 
                                                 ( ( gfloat ) data->chunk_size ) );
                   progress_window_set_percentage( progress->total_progress,
							                            ( ( gfloat ) bytes_read ) /
                                                 ( ( gfloat ) file_size ) );
                 }
             }
         }

       /* Copy the outfile name to the batch file. */
       if ( data->create_batchfile )
         {
           if ( strcmp( ext, "001" ) != 0 )
             write_dostextfile( batch, "+" );

           outfile_only[strlen( outfile_only ) - 4] = '\0';
           strcat( outfile_only, "." );
           strcat( outfile_only, ext );
           write_dostextfile( batch, outfile_only );
         }

       /* Increment the extension.
          Remember, the extension looks like this: [.] [0] [1] [2] */
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

       /* Insure that all data is written to disk before we quit. */
       fflush( out );
       /* Close the split file. */
       if ( fclose( out ) == EOF )
         {
           display_error( "\nsplit.c:  Could not close an output file.\n", TRUE );
           fclose( in );
           if ( data->create_batchfile )
             {
               fclose( batch );
             }
          if ( do_progress )
            {
               destroy_progress_window( progress );
               g_free( progress );
            }
          return FALSE;
         }

       /* Start a new outfile. */
       if ( bytes_read != file_size )
         {
            /* Setup a new outfile. */
           outfile[strlen( outfile ) - 3] = '\0';
           strcat( outfile, ext );

           out = fopen( outfile, "wb+" );
           if ( out == NULL )
             {
               display_error( "\nsplit.c:  Could not create an output file.\n", TRUE );
               fclose( in );
               if ( data->create_batchfile )
                 {
                   fclose( batch );
                 }
               if ( do_progress )
                 {
                   destroy_progress_window( progress );
                   g_free( progress );
                 }
               return FALSE;
             }
         }
     }
     /* End of split process. */

   /* Close the selected file. */
   if ( fclose( in ) == EOF )
     {
       display_error( "\nsplit.c:  Could not close the selected file.\n", FALSE );
       if ( data->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
         {
           destroy_progress_window( progress );
           g_free( progress );
         }
       return TRUE;
     }

   /* Finalize the batch file. */
   if ( data->create_batchfile )
     {
       write_dostextfile( batch, " " );
       write_dostextfile( batch, data->file_name_only );
		 
		 /* Write some footer information to the batchfile. */
       writeln_dostextfile( batch, " > ~combine.tmp" );
       writeln_dostextfile( batch, "erase ~combine.tmp" );
       writeln_dostextfile( batch, "Echo Finished." );
       /* End of footer information. */
       
       if ( fclose( batch ) == EOF )
         {
           display_error( "\nsplit.c:  Could not close the batch file.\n", FALSE );
           if ( do_progress )
             {
               destroy_progress_window( progress );
               g_free( progress );
             }
           return TRUE;
         }
     }

   /* Free the progress window. */
   if ( do_progress )
     {
       gtk_widget_hide_all( progress->main_window );
       destroy_progress_window( progress );
       g_free( progress );
     }

   return TRUE;
}
