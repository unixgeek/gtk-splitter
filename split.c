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

#include "dostextfile.h"
#include "globals.h"
#include "error.h"
#include "progress.h"

gboolean split(GtkWidget *tmp, session_data *data)
{
   /*Progress window stuff.*/
   progress_window *progress;
   gboolean do_progress;

   /*FILE streams.*/
   FILE *in, *out, *batch;
   
   /*Various character array pointers.*/
   gchar *infile, *outfile, *batchname_and_path, *outfile_only, ext[] = {"001"};
  
   /*Array length variables.*/
   gushort infile_length, outfile_length, bp_length, outfile_only_length;

   /*For storing the character read in by fgetc().*/
   gint temp;

   /*Various counters.*/
   guint number_of_even_files, file_count, files_to_split;

   /*Variables for file sizes and progress tracking.*/
   gulong file_size, size_of_leftover_file, byte_count, bytes_read;
   struct stat file_info;

   bytes_read = 0;
  
   /*Determine the unit of measurement to evaluate the chunk_size.
     chunk_size should probably be moved from the struct to a local variable.*/
   switch ( data->unit )
     {
       case BYTES      :  data->chunk_size = data->entry;
                          break;
       case KILOBYTES  :  data->chunk_size = ( data->entry * 1024 );
			                 break;
       case MEGABYTES  :  data->chunk_size = ( data->entry * 1024 * 1024 );
			                 break;
     }

   /*Set up the infile array.*/
   infile_length = data->fp_length;
   infile = g_malloc( infile_length * sizeof( gchar ) );
   if ( infile == NULL )
     {
       display_error( "\nsplit.c:  Could allocate memory for infile string.\n", TRUE );
       return FALSE;
     }
   strcpy( infile, data->filename_and_path );
   infile[infile_length - 1] = '\0';
   /*infile is setup.*/

   /*Figure out how many files to split the selected file into.*/
   stat( infile, &file_info );
   file_size = file_info.st_size;
   number_of_even_files = ( gint ) file_info.st_size/data->chunk_size;
   size_of_leftover_file = file_size - ( number_of_even_files * data->chunk_size );

   if ( size_of_leftover_file == 0 )
     files_to_split = number_of_even_files;
   else
     files_to_split = number_of_even_files + 1;

   /*Make sure we are not trying to split the file into pieces larger than itself.*/
   if ( data->chunk_size >= file_size )
     {
       display_error( "\nsplit.c:  Chunk size is greater than or equal to the file size.\n", FALSE );
       g_free( infile );
       return FALSE;
     }


   /*Don't create more than 999 files.  Actually, since we are starting at 1, the limit is really
     998 files.  (001 - 099)*/
   if ( files_to_split > 999 )
     {
       display_error( "\nsplit.c:  Exceeded maximum number of files (999).\n", FALSE );
       g_free( infile );
       return FALSE;
     }

   /*Decide whether or not a progress window would be beneficiary.*/
   if ( file_size <= UPDATE_INTERVAL )
     do_progress = FALSE;
   else
     {
       do_progress = TRUE;
       progress = g_malloc( sizeof ( progress_window ) );
       if ( progress == NULL )
         {
           display_error( "\nsplit.c:  Could not allocate memory for a progress window.\n", FALSE );
           /*Try to go on without it.*/
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
       outfile_only_length = data->f_length + 4;  /*f_length includes space for '\0'.*/
       outfile_only = g_malloc( outfile_only_length * sizeof( gchar ) );
       if ( outfile_only == NULL )
         {
           display_error( "\nsplit.c:  Could not allocate memory for outfile_only string.\n", TRUE );
           if ( do_progress )
             {
               destroy_progress_window( progress );
               g_free( progress );
             }
           g_free( infile );
           return FALSE;
         }

       /*If the file name is too long, truncate it.*/
       if ( ( data->f_length - 1 ) > 12 )
         {
           dosify_filename( data->filename_only );
			  data->f_length = 13;
			  g_realloc( data->filename_only, data->f_length );
         }
       /*Convert spaces to '_'.*/

       strcpy( outfile_only, data->filename_only );
       /*Reserve four spaces for the extension; it will change later.*/
       strcat(outfile_only, ".ext");
       bp_length = data->f_length + strlen( data->output_dir ) + 4;  /*f_length includes space for '\0'.*/
       batchname_and_path = g_malloc( bp_length * sizeof( gchar ) );
       if ( batchname_and_path == NULL )
         {
           display_error( "\nsplit.c:  Could not allocate memory for batchname_and_path string.\n", TRUE );
           g_free( outfile_only );
           if ( do_progress )
             {
               destroy_progress_window( progress );
               g_free( progress );
             }
           g_free( infile );
           return FALSE;
         }

       strcpy( batchname_and_path, data->output_dir );
       strcat( batchname_and_path, data->filename_only );
       strcat( batchname_and_path, ".bat");

       batch = fopen( batchname_and_path, "w+" );
       if ( batch == NULL )
         {
           display_error( "\nsplit.c:  Could not create the batch file.\n", TRUE );
           g_free( batchname_and_path );
           g_free( outfile_only );
           if ( do_progress )
             {
               destroy_progress_window( progress );
               g_free( progress );
             }
           g_free( infile );
           return FALSE;
	      }
       /*Write some header information to the batchfile.*/
       writeln_dostextfile( batch, "@Echo Off" );
       write_dostextfile( batch, "Echo " );
       writeln_dostextfile( batch, GTK_SPLITTER_VERSION );
       write_dostextfile( batch, "Echo Creating " );
       writeln_dostextfile( batch, data->filename_only );
       write_dostextfile( batch, "copy /b " );
       /*End of header information*/
     }
   /*---Done with setting up the batcfhile.-------------------------------------------------------------*/


   /*Setup the outfile array.*/
   outfile_length = data->f_length + strlen( data->output_dir ) + 4 ;  /*f_length includes space for '\0'.*/
   outfile = g_malloc( outfile_length * sizeof( char ) );
   if ( outfile == NULL )
     {
       display_error( "\nsplit.c:: Could not allocate memory for outfile string.\n", TRUE );
       if ( data->create_batchfile )
         {
           fclose( batch );
           g_free( batchname_and_path );
           g_free( outfile_only );
         }
       if ( do_progress )
         {
           destroy_progress_window( progress );
           g_free( progress );
         }
       g_free( infile );
       return FALSE;
     }
   strcpy( outfile, data->output_dir );
   strcat( outfile, data->filename_only );
   strcat( outfile, "." );
   strcat( outfile, ext );
   /*outfile is setup.*/

   /*Open the selected file.*/
   in = fopen( infile, "rb" );
   if ( in == NULL )
     {
       display_error( "\nsplit.c:  Could not open the selected file.\n", TRUE );
       g_free( outfile );
       if ( data->create_batchfile )
         {
           fclose( batch );
           g_free( batchname_and_path );
           g_free( outfile_only );
         }
       if ( do_progress )
         {
           destroy_progress_window( progress );
           g_free( progress );
         }
       g_free( infile );
       return FALSE;
     }

   /*Open the first outfile.*/
   out = fopen( outfile, "wb+" );
   if ( out == NULL )
     {
       display_error( "\nsplit.c:  Could not create an output file.\n", FALSE );
       fclose( in );
       g_free( outfile );
       if ( data->create_batchfile )
         {
           fclose( batch );
           g_free( batchname_and_path );
           g_free( outfile_only );
         }
       if ( do_progress )
         {
           destroy_progress_window( progress );
           g_free( progress );
         }
       g_free( infile );
       return FALSE;
     }

   /*The actual split process.*/
   for ( file_count = 1; file_count <= files_to_split; file_count++ )
     {
       if ( do_progress )  /*Display the file name we are creating.*/
         progress_window_set_status_text( progress->status, outfile );
       /*The leftover file. (?)*/
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
       else  /*Even files. (?)*/
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

       /*Copy the outfile name to the batch file.*/
       if ( data->create_batchfile )
         {
           if ( strcmp( ext, "001" ) != 0 )
             write_dostextfile( batch, "+" );

           outfile_only[strlen( outfile_only ) - 4] = '\0';
           strcat( outfile_only, "." );
           strcat( outfile_only, ext );
           write_dostextfile( batch, outfile_only );
         }

       /*Increment the extension.
         Remember, the extension looks like this: [.] [0] [1] [2]*/
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

       /*Insure that all data is written to disk before we quit.*/
       fflush( out );
       /*Close the split file.*/
       if ( fclose( out ) == EOF )
         {
           display_error( "\nsplit.c:  Could not close an output file.\n", TRUE );
           fclose( in );
           g_free( outfile );
           if ( data->create_batchfile )
             {
               fclose( batch );
               g_free( batchname_and_path );
               g_free( outfile_only );
             }
          if ( do_progress )
            {
              destroy_progress_window( progress );
              g_free( progress );
            }
          g_free( infile );
          return FALSE;
         }

       /*Start a new outfile.*/
       if ( bytes_read != file_size )
         {
            /*Setup a new outfile.*/
           outfile[strlen( outfile ) - 3] = '\0';
           strcat( outfile, ext );

           out = fopen( outfile, "wb+" );
           if ( out == NULL )
             {
               display_error( "\nsplit.c:  Could not create an output file.\n", TRUE );
               fclose( in );
               g_free( outfile );
               if ( data->create_batchfile )
                 {
                   fclose( batch );
                   g_free( batchname_and_path );
                   g_free( outfile_only );
                 }
               if ( do_progress )
                 {
                   destroy_progress_window( progress );
                   g_free( progress );
                 }
               g_free( infile );
               return FALSE;
             }
         }
     }
     /*End of split process.*/

   /*Close the selected file.*/
   if ( fclose( in ) == EOF )
     {
       display_error( "\nsplit.c:  Could not close the selected file.\n", FALSE );
       g_free( outfile );
       if ( data->create_batchfile )
         {
           fclose( batch );
           g_free( batchname_and_path );
           g_free( outfile_only );
         }
       if ( do_progress )
         {
           destroy_progress_window( progress );
           g_free( progress );
         }
       g_free( infile );
       return TRUE;
     }

   /*Finalize the batch file.*/
   if ( data->create_batchfile )
     {
       write_dostextfile( batch, " " );
       write_dostextfile( batch, data->filename_only );
		 
		 /*Write some footer information to the batchfile.*/
       writeln_dostextfile( batch, " > ~combine.tmp" );
       writeln_dostextfile( batch, "erase ~combine.tmp" );
       writeln_dostextfile( batch, "Echo Finished." );
       /*End of footer information.*/
       
       /*Free the memory we allocated.*/
		 g_free( batchname_and_path );
       g_free( outfile_only );
       if ( fclose( batch ) == EOF )
         {
           display_error( "\nsplit.c:  Could not close the batch file.\n", FALSE );
           g_free( outfile );
           g_free( outfile_only );
           if ( do_progress )
             {
               destroy_progress_window( progress );
               g_free( progress );
             }
           g_free( infile );
           return TRUE;
         }
     }

   /*Free the memory we allocated.*/
   g_free( infile );
   g_free( outfile );

   /*Free the progress window.*/
   if ( do_progress )
     {
       gtk_widget_hide_all( progress->main_window );
       destroy_progress_window( progress );
       g_free( progress );
     }

   return TRUE;
}
