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
#include <limits.h>
#include "md5.h"


void create_sum(const char *file_name_and_path, const char *output_directory)
{
   char sh_argument[PATH_MAX];
   char file_path[PATH_MAX];
   char file_name_only[PATH_MAX];
   int i, j, length;

   strcpy( file_path, file_name_and_path );
   
   length = strlen( file_path );
   
   /* Find the root of the file. */
   i = strlen( file_path );
   while ( file_path[i] != '/' )
     i--;
   i++;
   
   /* Copy the file name from file_path into file_name_only. */
   for ( j = 0; i != length; j++, i++ )
     {
       file_name_only[j] = file_path[i];
       file_path[i] = '\0';
     }
   file_name_only[j] = '\0';

     
   /*
      Setup the command-line argument for sh.
      Full Command: 
      chdir {file_path}
      md5sum "{file_name_only}" > "{output_directory}{filename_only}.md5"{NULL}
   */
  
   strcpy( sh_argument, "md5sum " );
   strcat( sh_argument, "\"" );
   strcat( sh_argument, file_name_only );
   strcat( sh_argument, "\" > \"" );
   strcat( sh_argument, output_directory );
   strcat( sh_argument, file_name_only );
   strcat( sh_argument, ".md5\"" );
   strcat( sh_argument, "\0" );
     
   chdir( file_path );
   execl( "/bin/sh", "sh", "-c", sh_argument, NULL);
}
             
int verify_file(const char *file_name_and_path )
{
   char sh_argument[PATH_MAX];
   char file_path[PATH_MAX];
   char file_name_only[PATH_MAX];
   int i, j, length, exit_status;
   
   strcpy( file_path, file_name_and_path );
   
   length = strlen( file_path );
   
   /* Find the root of the file. */
   i = strlen( file_path );
   while ( file_path[i] != '/' )
     i--;
   i++;
   
   /* Copy the file name from file_path into file_name_only. */
   for ( j = 0; i != length; j++, i++ )
     {
       file_name_only[j] = file_path[i];
       file_path[i] = '\0';
     }
   file_name_only[j] = '\0';

   /*
      Setup the sh_argument to execute.
      Full Command: 
      chdir {root_of_open_file}
      md5sum --status -c "{filename_only}.md5"{NULL}
   */
     
   strcpy( sh_argument, "md5sum --status -c \"" );
   strcat( sh_argument, file_name_only );
   strcat( sh_argument, ".md5\"" );
   strcat( sh_argument, "\0" );
   
   chdir( file_path );
   exit_status = system( sh_argument );
       
   return ( exit_status );  
}
