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

#ifdef HAVE_LIBMHASH
#include <mhash.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "interface.h"
#include "globals.h"
#include "progress.h"
#include "dostextfile.h"

gboolean gtk_splitter_split_file( GtkSplitterSessionData *gssd )
{
   /* Progress window stuff. */
   ProgressWindow *progress_window = NULL;
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

#ifdef HAVE_LIBMHASH
   /* For mhash. */
   unsigned char hash[16];
   MHASH thread;
   gchar md5_file[PATH_MAX];
   FILE *md5;
   int i;
#endif
   
   /* Setup the infile string. */
   g_stpcpy( infile, gssd->file_name_and_path );
   
   /* Setup the outfile string. */
   sprintf( outfile, "%s/%s.%s", gssd->output_directory, gssd->file_name_only, ext );
  
   /* Setup strings used when creating a batch file. */
   if ( gssd->create_batchfile )
     {
       /* Setup the outfile_only string.
          If the file name is too long, truncate it.
          Also, convert some characters to '_'. */
       g_stpcpy( outfile_only, gssd->file_name_only );
       dosify_file_name( outfile_only );
                
      /* Re-setup the outfile string. */
       sprintf( outfile, "%s/%s.%s", gssd->output_directory, outfile_only, ext );

       /* Setup the batchname_and_path string. */ 
       sprintf( batchname_and_path, "%s/%s.bat", gssd->output_directory, outfile_only ); 
     }

   /* Determine chunk_size (bytes). */
   switch ( gssd->unit )
     {
       case BYTES      :  chunk_size = gssd->entry;
                          break;
       case KILOBYTES  :  chunk_size = ( gssd->entry * 1024 );
			              break;
       case MEGABYTES  :  chunk_size = ( gssd->entry * 1024 * 1024 );
			              break;
       default         :  chunk_size = gssd->entry;
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
       progress_window = progress_window_new( );
       if ( progress_window == NULL )
          do_progress = FALSE;
       else
       {
          gtk_window_set_title( GTK_WINDOW( progress_window->base_window ), "Split Progress" );
          gtk_widget_show_all( progress_window->base_window );
          while ( g_main_iteration( FALSE ) );
       }
     }

   batch = NULL;
   /*---Setup a batchfile if so desired.------------------------------------------------------------*/
   if ( gssd->create_batchfile )
     {
 
       batch = fopen( batchname_and_path, "wb+" );
       if ( batch == NULL )
         {
           display_error( "split.c:  Could not create the batch file." );
           if ( do_progress )
              progress_window_destroy( progress_window );
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
       writeln_dostextfile( batch, gssd->file_name_only );
       write_dostextfile( batch, "Echo Creating " );
       writeln_dostextfile( batch, outfile_only );
       write_dostextfile( batch, "copy /b " );
       /* End of header information */
          
       strcat( outfile_only, ".ext" ); /* Potential BUG. */
         
          }
   /*---Done with setting up the batcfhile.-------------------------------------------------------------*/


   /* Open the selected file. */
   in = fopen( infile, "rb" );
   if ( in == NULL )
     {
       display_error( "split.c:  Could not open the selected file." );
       if ( gssd->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
          progress_window_destroy( progress_window );
       return FALSE;
     }

   /* Open the first outfile. */
   out = fopen( outfile, "wb+" );
   if ( out == NULL )
     {
       display_error( "split.c:  Could not create an output file." );
       fclose( in );
       if ( gssd->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
          progress_window_destroy( progress_window );
       return FALSE;
     }
   
   bytes_read = 0;
   /* The actual split process. */
   for ( file_count = 1; file_count <= number_of_parts; file_count++ )
     {
       if ( do_progress )  /* Display the file name we are creating. */
         gtk_label_set_text( GTK_LABEL( progress_window->message ), outfile );
while ( g_main_iteration( FALSE ) );       
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
                   gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->current_progress ),
                                                ( ( gfloat ) byte_count ) / 
                                                ( ( gfloat ) size_of_leftover_file ) );
                   gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->total_progress ),
                                                ( ( gfloat ) bytes_read ) /
                                                ( ( gfloat ) file_size ) );
                   while ( g_main_iteration( FALSE ) );
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
                   gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->current_progress ),
							                      ( ( gfloat ) byte_count ) / 
                                                  ( ( gfloat ) chunk_size ) );
                   gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->total_progress ),
							                      ( ( gfloat ) bytes_read ) /
                                                  ( ( gfloat ) file_size ) );
                   while ( g_main_iteration( FALSE ) );  
                 }
             }
         }

       /* Copy the outfile name to the batch file. */
       if ( gssd->create_batchfile )
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
           if ( gssd->create_batchfile )
             {
               fclose( batch );
             }
          if ( do_progress )
             progress_window_destroy( progress_window );
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
               if ( gssd->create_batchfile )
                 {
                   fclose( batch );
                 }
               if ( do_progress )
                  progress_window_destroy( progress_window );
               return FALSE;
             }
         }
     }
   /* End of split process. */

#ifdef HAVE_LIBMHASH     
   /* Create an md5sum of the selected file, if desired. */
   if ( gssd->verify )
     {
       if ( do_progress )
          gtk_label_set_text( GTK_LABEL( progress_window->message ), "Generating md5 sum..." );
     
       thread = mhash_init( MHASH_MD5 );
       
       if ( thread == MHASH_FAILED )
          display_error( "split.c:  Error generating md5 sum." );
       else
       {
          bytes_read = 0;
          fseek( in, 0, SEEK_SET );
          while ( fread( &temp, 1, 1, in ) == 1 ) 
          {
            mhash( thread, &temp, 1 );
            bytes_read++;
            if ( ( do_progress ) && ( ( bytes_read % UPDATE_INTERVAL ) == 0 ) )  
            {
               gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->current_progress ),
                                                ( ( gfloat ) bytes_read ) /
                                                ( ( gfloat ) file_size ) );
               gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->total_progress ),
                                                ( ( gfloat ) bytes_read ) /
                                                ( ( gfloat ) file_size ) );
               while ( g_main_iteration( FALSE ) );  
            }
          }
          
          mhash_deinit( thread, hash );
          outfile[strlen( outfile ) - 4] = '\0';
          sprintf( md5_file, "%s.md5", outfile );
          md5 = fopen( md5_file, "w+" );
          
          if ( md5 != NULL )
          {
             //fwrite( hash, sizeof( gchar ), mhash_get_block_size( MHASH_MD5 ), md5 );
             for ( i = 0; i < mhash_get_block_size ( MHASH_MD5 ); i++ ) 
                fprintf( md5, "%.2x", hash[i] );
             fflush( md5 );
             fclose( md5 );
         }
         else
         {
            display_error( "split.c:  Error writing md5 sum to file." );
         }
       }
     }
 #endif
     
   /* Close the selected file. */
   if ( fclose( in ) == EOF )
     {
       display_error( "split.c:  Could not close the selected file." );
       if ( gssd->create_batchfile )
         {
           fclose( batch );
         }
       if ( do_progress )
          progress_window_destroy( progress_window );
       return TRUE;
     }

   /* Finalize the batch file. */
   if ( gssd->create_batchfile )
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
              progress_window_destroy( progress_window );
           return TRUE;
         }
     }

   /* Free the progress window. */
   if ( do_progress )
     {
       gtk_widget_hide_all( progress_window->base_window );
       progress_window_destroy( progress_window );
     }

   return TRUE;
}
