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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "md5.h"
#include "error.h"
#include "globals.h"
#include "progress.h"
#include "dostextfile.h"

gboolean split(GtkWidget *tmp, session_data *data)
{
   /* Progress window stuff. */
   progress_window progress;
   gboolean do_progress;

   /* FILE streams. */
   FILE *in, *out, *batch;

   /* The name of the file to be split (including the path). */
   gchar infile[PATH_MAX];

   /* The name of the file part (including the path). */
   gchar outfile[PATH_MAX]; 
   
   /* The name of the batch file (including the path). 
      This is used when creating a batchfile. */
   gchar batchname_and_path[PATH_MAX];
   
   /* The name of the file part without the path. 
      This is used when creating a batchfile. */
   gchar outfile_only[PATH_MAX];
   
   /* The extension for file parts. */
   gchar ext[] = {"001"};
   
   /* For storing the character read in by fgetc(). */
   gint temp;
   
   /* Various counters. */
   guint number_of_even_files, file_count, number_of_parts;
   
   /* Variables for file sizes and progress tracking. */
   gulong file_size, size_of_leftover_file, byte_count, bytes_read;
   
   struct stat file_info;
   
   /* The size the file should be split into. */
   gulong chunk_size;

   /* The return value of generate_md5_sum( ) */
   generate_md5_exit_status md5_return;

   
   /* Setup the infile string. */
   strcpy( infile, data->file_name_and_path );
   
   /* Setup the outfile string. */
   strcpy( outfile, data->output_directory );
   strcat( outfile, data->file_name_only );
   strcat( outfile, "." );
   strcat( outfile, ext );
  
   /* Setup strings used when creating a batch file. */
   if ( data->create_batchfile )
     {
       /* Setup the outfile_only string.
          If the file name is too long, truncate it.
          Also, convert some characters to '_'. */
       strcpy( outfile_only, data->file_name_only );
       dosify_file_name( outfile_only );
      
       /* Re-setup the outfile string. */
       strcpy( outfile, data->output_directory );
       strcat( outfile, outfile_only );
       strcat( outfile, "." );
       strcat( outfile, ext );

       /* Setup the batchname_and_path string. */ 
       strcpy( batchname_and_path, data->output_directory );
       strcat( batchname_and_path, outfile_only );
       strcat( batchname_and_path, ".bat" ); 
     }
   
   /* Determine chunk_size (bytes). */
   switch ( data->unit )
     {
       case BYTES      :  chunk_size = data->entry;
                          break;
       case KILOBYTES  :  chunk_size = ( data->entry * 1024 );
			                 break;
       case MEGABYTES  :  chunk_size = ( data->entry * 1024 * 1024 );
			                 break;
     }
  
   /* Figure out how many files to split the selected file into. */
   stat( infile, &file_info );
   file_size = file_info.st_size;

   /* Check to see if the file will be evenly split.
      If not, there will be one file with a smaller size. */
   number_of_even_files = ( gint ) file_info.st_size / chunk_size;
   size_of_leftover_file = file_size - ( number_of_even_files * chunk_size );

   /* The number pieces to split the file into. */
   if ( size_of_leftover_file == 0 )
     number_of_parts = number_of_even_files;
   else
     number_of_parts = number_of_even_files + 1;

   /* Make sure we are not trying to split the file into pieces larger than itself. */
   if ( chunk_size >= file_size )
     {
       display_error( "split.c:  Chunk size is greater than or equal to the file size." );
       return FALSE;
     }

   /* Don't create more than 999 files.  Actually, since we are starting at 1, the limit is really
      998 files.  (001 - 999) */
   if ( number_of_parts > 999 )
     {
       display_error( "split.c:  Exceeded maximum number of files (999)." );
       return FALSE;
     }

   /* Decide whether or not a progress window would be beneficiary. */
   if ( file_size <= UPDATE_INTERVAL )
     do_progress = FALSE;
   else
     {
       do_progress = TRUE;
       create_progress_window( &progress, "Split Progress" );
       gtk_widget_show_all( progress.main_window );
       while ( g_main_iteration( FALSE ) );
     }

   /*---Setup a batchfile if so desired.------------------------------------------------------------*/
   if ( data->create_batchfile )
     {
 
       batch = fopen( batchname_and_path, "wb+" );
       if ( batch == NULL )
         {
           display_error( "split.c:  Could not create the batch file." );
           if ( do_progress )
              destroy_progress_window( &progress );
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
       writeln_dostextfile( batch, data->file_name_only );
       write_dostextfile( batch, "Echo Creating " );
       writeln_dostextfile( batch, outfile_only );
       write_dostextfile( batch, "copy /b " );
       /* End of header information */
         
       /* Reserve four spaces for the extension; it will change later. */
       strcat(outfile_only, ".ext"); /* Potential BUG. */
     }
   /*---Done with setting up the batcfhile.-------------------------------------------------------------*/


   /* Create an md5sum of the selected file, if desired. */
   if ( data->verify )
     {
       if ( do_progress )
          progress_window_set_message_text( progress.message, "Generating md5 sum." );
       
       md5_return = generate_md5_sum( data->file_name_and_path, data->output_directory );
       
       /* See if the return was okay first. */
       if ( md5_return != GENERATE_MD5_MD5SUM_EXIT_OK )
         {
           switch ( md5_return )
             {
               case GENERATE_MD5_OVERFLOW_ERROR:
                  
                  display_error( "split.c:  Could not generate md5 sum.  (GENERATE_MD5_OVERFLOW_ERROR)" );
                  break;
               
               case GENERATE_MD5_STAT_FAILED:
                  
                  display_error( "split.c:  Could not generate md5 sum.  (GENERATE_MD5_STAT_FAILED)" );
                  break;
               
               case GENERATE_MD5_NOT_A_REGULAR_FILE:
                  
                  display_error( "split.c:  Could not generate md5 sum.  (GENERATE_MD5_NOT_A_REGULAR_FILE)" );
                  break;
   
               case GENERATE_MD5_NOT_A_DIRECTORY:
                  
                  display_error( "split.c:  Could not generate md5 sum.  (GENERATE_MD5_NOT_A_DIRECTORY)" );
                  break;
               
               case GENERATE_MD5_CHDIR_FAILED:
                  
                  display_error( "split.c:  Could not generate md5 sum.  (GENERATE_MD5_CHDIR_FAILED)" );
                  break;
                  
               case GENERATE_MD5_SYSTEM_ERROR:
                  
                  display_error( "split.c:   Could not generate md5 sum.  (GENERATE_MD5_SYSTEM_ERROR)" );
                  break;
               
               case GENERATE_MD5_SYSTEM_SH_NOT_AVAILABLE:
                  
                  display_error( "split.c:   Could not generate md5 sum.  (GENERATE_MD5_SYSTEM_SH_NOT_AVAILABLE)" ); 
                  break;
               
               case GENERATE_MD5_SYSTEM_SH_EXEC_ERROR:
                 
                  display_error( "split.c:   Could not generate md5 sum.  (GENERATE_MD5_SYSTEM_SH_EXEC_ERROR)" ); 
                  break;
               
               case GENERATE_MD5_MD5SUM_EXIT_OK:
                  
                  break;
               
               case GENERATE_MD5_MD5SUM_EXIT_FAILURE:  
                  
                  display_error( "split.c:   Could not generate md5 sum.  (GENERATE_MD5_MD5SUM_EXIT_FAILURE)" );
                  break;
               
               case GENERATE_MD5_EXIT_STATUS_UNKNOWN:
                  
                  display_error( "split.c:   Could not generate md5 sum.  (GENERATE_MD5_EXIT_STATUS_UNKNOWN)" );
                  break;
             }
         }
     } 
     
   /* Open the selected file. */
   in = fopen( infile, "rb" );
   if ( in == NULL )
     {
       display_error( "split.c:  Could not open the selected file." );
       if ( data->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
          destroy_progress_window( &progress );
       return FALSE;
     }

   /* Open the first outfile. */
   out = fopen( outfile, "wb+" );
   if ( out == NULL )
     {
       display_error( "split.c:  Could not create an output file." );
       fclose( in );
       if ( data->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
          destroy_progress_window( &progress );
       return FALSE;
     }
   
   bytes_read = 0;
   /* The actual split process. */
   for ( file_count = 1; file_count <= number_of_parts; file_count++ )
     {
       if ( do_progress )  /* Display the file name we are creating. */
         progress_window_set_message_text( progress.message, outfile );
       /* The leftover file. (?) */
       if ( ( size_of_leftover_file != 0 ) && ( file_count  == ( number_of_parts ) ) )
         {
           for ( byte_count = 1; ( byte_count <= size_of_leftover_file ); byte_count++ )
             {
               temp = fgetc( in );
               fputc( temp, out );
               bytes_read++;
               if ( ( do_progress ) && ( ( byte_count % UPDATE_INTERVAL ) == 0 ) )
                 {
                   progress_window_set_percentage( progress.current_progress,
                                                 ( ( gfloat ) byte_count ) / 
                                                 ( ( gfloat ) size_of_leftover_file ) );
                   progress_window_set_percentage( progress.total_progress,
                                                 ( ( gfloat ) bytes_read ) /
                                                 ( ( gfloat ) file_size ) );
                 }
             }
         }
       else  /* Even files. (?) */
         {
           for ( byte_count = 1; ( byte_count <= chunk_size ); byte_count++ )
             {
               temp = fgetc( in );
               fputc( temp, out );
               bytes_read++;
               if ( ( do_progress ) && ( ( byte_count % UPDATE_INTERVAL ) == 0 ) )
                 {
                   progress_window_set_percentage( progress.current_progress,
							                            ( ( gfloat ) byte_count ) / 
                                                 ( ( gfloat ) chunk_size ) );
                   progress_window_set_percentage( progress.total_progress,
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

           outfile_only[strlen( outfile_only ) - 3] = '\0';
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
           display_error( "split.c:  Could not close an output file." );
           fclose( in );
           if ( data->create_batchfile )
             {
               fclose( batch );
             }
          if ( do_progress )
             destroy_progress_window( &progress );
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
               display_error( "split.c:  Could not create an output file." );
               fclose( in );
               if ( data->create_batchfile )
                 {
                   fclose( batch );
                 }
               if ( do_progress )
                  destroy_progress_window( &progress );
               return FALSE;
             }
         }
     }
     /* End of split process. */

   /* Close the selected file. */
   if ( fclose( in ) == EOF )
     {
       display_error( "split.c:  Could not close the selected file." );
       if ( data->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
          destroy_progress_window( &progress );
       return TRUE;
     }

   /* Finalize the batch file. */
   if ( data->create_batchfile )
     {
       write_dostextfile( batch, " " );
       outfile_only[strlen( outfile_only ) - 4] = '\0';
       write_dostextfile( batch, outfile_only );
		 
		 /* Write some footer information to the batchfile. */
       writeln_dostextfile( batch, " > ~combine.tmp" );
       writeln_dostextfile( batch, "erase ~combine.tmp" );
       writeln_dostextfile( batch, "Echo Finished." );
       /* End of footer information. */
       
       if ( fclose( batch ) == EOF )
         {
           display_error( "split.c:  Could not close the batch file." );
           if ( do_progress )
              destroy_progress_window( &progress );
           return TRUE;
         }
     }

   /* Free the progress window. */
   if ( do_progress )
     {
       gtk_widget_hide_all( progress.main_window );
       destroy_progress_window( &progress );
     }

   return TRUE;
}
