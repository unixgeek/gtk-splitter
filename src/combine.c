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
 
#ifdef HAVE_LIBMHASH
#include <mhash.h>
#endif
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/stat.h>
#include "error.h"
#include "globals.h"
#include "progress.h"
#include "interface.h"

gboolean gtk_splitter_combine_files( GtkSplitterSessionData *data )
{
   /* Progress window stuff. */
   ProgressWindow *progress_window = NULL;
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

#ifdef HAVE_LIBMHASH
   /* For mhash. */
   unsigned char hash[16];
   int hashchar;
   MHASH thread;
   gchar md5_file[PATH_MAX];
   gboolean verified;
   FILE *md5;
   unsigned char hashtemp[3] = {'\0'};
   unsigned char new_hash[33] = {'\0'};
   unsigned char given_hash[33] = {'\0'};
   int i;
#endif
   
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
       display_error( "combine.c:  Exceeded maximum number of files (999)." );
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
       display_error( "combine.c:  Could not create an output file." );
       return FALSE;
     }

   /* Decide whether or not a progress window would be beneficiary. */
   if ( combined_file_size <= UPDATE_INTERVAL )
     do_progress = FALSE;
   else
     {
       do_progress = TRUE;
       progress_window = progress_window_new( );
       if ( progress_window == NULL )
          do_progress = FALSE;
       else
       {
          gtk_window_set_title( GTK_WINDOW( progress_window->base_window ), "Combine Progress" );
          gtk_widget_show_all( progress_window->base_window );
          while ( g_main_iteration( FALSE ) );
       }
     }

   /* Now DO the combine. */
   bytes_read = 0;
   for ( file_count = 0; file_count != files_to_combine; file_count++ )
     {
       if ( do_progress )
         gtk_label_set_text( GTK_LABEL( progress_window->message ), infile );
       in = fopen( infile, "rb" );
       if ( in == NULL )
         {
           display_error( "combine.c:  Could not open one of the files to be combined." );
           if ( do_progress )
              progress_window_destroy( progress_window );
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
               gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->current_progress ),
                                               ( ( gfloat ) byte_count ) / ( ( gfloat ) file_info.st_size ) );
               gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->total_progress ),
                                               ( ( gfloat ) bytes_read ) / ( ( gfloat ) combined_file_size ) );
               while ( g_main_iteration( FALSE ) );
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
           display_error( "combine.c:  Could not open one of the files to be combined." );
           if ( do_progress )
              progress_window_destroy( progress_window );
           fclose( out );
           return FALSE;
         }
         
       infile[strlen( infile ) - 3] = '\0';
       strcat( infile, ext );
     }
     /* End of the combine process. */

#ifdef HAVE_LIBMHASH
   /* Verify if desired. */
   if ( data->verify )
     {
       verified = TRUE;
       hashchar = '\0';
       if ( do_progress )
          gtk_label_set_text( GTK_LABEL( progress_window->message ), "Verifying file..." );
     
       thread = mhash_init( MHASH_MD5 );
       
       if ( thread == MHASH_FAILED )
          display_error( "combine.c:  Error generating md5 sum." );
       else
       {
          bytes_read = 0;
          fseek( out, 0, SEEK_SET );
          while ( fread( &temp, 1, 1, out ) == 1 ) 
          {
            mhash( thread, &temp, 1 );
            bytes_read++;
            if ( ( do_progress ) && ( ( bytes_read % UPDATE_INTERVAL ) == 0 ) )  
            {
               gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->current_progress ),
                                                ( ( gfloat ) bytes_read ) /
                                                ( ( gfloat ) combined_file_size ) );
               gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progress_window->total_progress ),
                                                ( ( gfloat ) bytes_read ) /
                                                ( ( gfloat ) combined_file_size ) ); 
                while ( g_main_iteration( FALSE ) );  
             }
          }
          
          mhash_deinit( thread, hash );
          for ( i = 0; i < mhash_get_block_size( MHASH_MD5 ); i++ ) 
          {
             sprintf( hashtemp, "%.2x", hash[i] );
             strcat( new_hash, hashtemp );
          }
                  
          infile[strlen( infile ) - 4] = '\0';
          sprintf( md5_file, "%s.md5", infile );
          md5 = fopen( md5_file, "r" );
          
          if ( md5 != NULL )
          {
             while ( hashchar != EOF )
             {
                hashchar = fgetc( md5 );
                if ( hashchar != EOF )
                   strcat( given_hash, ( char * ) &hashchar );
             }
             fclose( md5 );

             if ( strcmp( new_hash, given_hash ) != 0 )
                 verified = FALSE;
             display_verification( verified );
         }
         else
         {
             display_error( "split.c:  Error reading md5 sum from file." );
         }
      }       
   }    
#endif
   
   /* Close our newly combined file. */
   if ( fclose( out ) == EOF )
     {
       display_error( "combine.c:  Could not close the combined file." );
       if ( do_progress )
          progress_window_destroy( progress_window );
       return TRUE;
     }   
     
   if ( do_progress )
     {
       gtk_widget_hide_all( progress_window->base_window );
       progress_window_destroy( progress_window );
     }

   return TRUE;
}
