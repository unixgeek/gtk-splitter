/*
 * md5.c
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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "md5.h"

void create_sum(const char *filename_and_path, const int fp_length, const char *output_dir)
{
   char *command;
   char *root_of_open_file;
   char *filename_only;
   int i, j;
   
   root_of_open_file = malloc( fp_length * sizeof( char ) );
   filename_only = malloc( fp_length * sizeof( char ) );
  
   strcpy( root_of_open_file, filename_and_path );
   strcpy( filename_only, filename_and_path );
   
   /* Determine the root of the open file and the file name. */
   i = fp_length - 1;
   while ( root_of_open_file[i] != '/' )
     i--;
   i++;
   for ( j = 0; i < ( fp_length ); j++, i++ )
     {
       filename_only[j] = root_of_open_file[i];
       root_of_open_file[i] = '\0';
     }
   filename_only[j] = '\0';
   
   /*
      Setup the command to execute.
      Full Command: 
      chdir {root_of_open_file}
      md5sum "{filename_only}" > "{output_dir}{filename_only}.md5"{NULL}
   */
  
   command = malloc( ( 8 + strlen( filename_only ) + 5 + strlen( output_dir ) + strlen( filename_only ) + 6 ) * sizeof( char ) );
   strcpy( command, "md5sum \"" );
   strcat( command, filename_only );
   strcat( command, "\" > \"" );
   strcat( command, output_dir );
   strcat( command, filename_only );
   strcat( command, ".md5\"" );
   strcat( command, "\0" );
     
   chdir( root_of_open_file );
   system( command );
  
   free( root_of_open_file );
   free( filename_only );
   free( command );
}
             
int verify_file(const char *filename_and_path, const int fp_length)
{
   char *command;
   char *root_of_open_file;
   char *filename_only;
   int i, j, exit_status;
   
   root_of_open_file = malloc( fp_length * sizeof( char ) );
   filename_only = malloc( fp_length * sizeof( char ) );
  
   strcpy( root_of_open_file, filename_and_path );
   strcpy( filename_only, filename_and_path );
   
   /* Determine the root of the open file and the file name. */
   i = fp_length - 1;
   while ( root_of_open_file[i] != '/' )
     i--;
   i++;
   for ( j = 0; i < ( fp_length ); j++, i++ )
     {
       filename_only[j] = root_of_open_file[i];
       root_of_open_file[i] = '\0';
     }
     
   filename_only[j] = '\0';

   /*
      Setup the command to execute.
      Full Command: 
      chdir {root_of_open_file}
      md5sum --status -c "{filename_only}.md5"{NULL}
   */
     
   command = malloc( ( 20 + strlen( filename_only ) + 6 ) * sizeof( char ) );
   strcpy( command, "md5sum --status -c \"" );
   strcat( command, filename_only );
   strcat( command, ".md5\"" );
   strcat( command, "\0" );
   
   chdir( root_of_open_file );
   exit_status = system( command );
     
   free( root_of_open_file );
   free( filename_only );
   free( command );
     
   return ( exit_status );  
}
