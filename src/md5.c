/*
 * md5.c
 *
 * Copyright 2002 Gunter Wambaugh
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

#include <stdio.h>  // Take this out later.
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "md5.h"


generate_md5_return_type generate_md5_sum(const char *file_name_and_path, const char *output_directory)
{
   char sh_argument[PATH_MAX];
   char file_path[PATH_MAX];
   char file_name_only[PATH_MAX];
   int i, j, length, exit_status;
   struct stat file_info;
   generate_md5_return_type return_type;
   

   if ( ( strlen( file_name_and_path ) > ( PATH_MAX + 1 ) ) ||
        ( strlen( output_directory ) > ( PATH_MAX + 1) ) )
      return GENERATE_MD5_OVERFLOW_ERROR;
   
   if ( stat( file_name_and_path, &file_info ) == -1 )
      return GENERATE_MD5_STAT_FAILED;
   
   if ( S_ISREG( file_info.st_mode ) == 0 )
      return GENERATE_MD5_NOT_A_REGULAR_FILE;
   
   if ( stat( output_directory, &file_info ) == -1 )
      return GENERATE_MD5_STAT_FAILED;
   
   if ( S_ISDIR( file_info.st_mode ) == 0 )
      return GENERATE_MD5_NOT_A_DIRECTORY;

   if ( system( NULL ) == 0 )
      return GENERATE_MD5_SYSTEM_SH_NOT_AVAILABLE;
   
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
      Argument:
      md5sum "{file_name_only}" > "{output_directory}{file_name_only}.md5"{NULL}'
   */
  
   strcpy( sh_argument, "md5sum " );
   strcat( sh_argument, "\"" );
   strcat( sh_argument, file_name_only );
   strcat( sh_argument, "\" > \"" );
   strcat( sh_argument, output_directory );
   strcat( sh_argument, file_name_only );
   strcat( sh_argument, ".md5\"" );
   strcat( sh_argument, "\0" );
     
   /* WD = {file_path} */
   if ( chdir( file_path ) == -1 )
      return GENERATE_MD5_CHDIR_FAILED;

   /* Execute command. */
   exit_status = system( sh_argument );
   
   switch ( WEXITSTATUS( exit_status ) )
   {
      case -1:
         
         return_type = GENERATE_MD5_SYSTEM_ERROR;
         break;
      
      case 127:
         
         return_type = GENERATE_MD5_SYSTEM_SH_EXEC_ERROR;
         break;
      
      case 1:  
         
         return_type = GENERATE_MD5_MD5SUM_EXIT_FAILURE;
         break;
      
      case 0:
         
         return_type = GENERATE_MD5_MD5SUM_EXIT_OK;
         break;
      
      default:
      
         return_type = GENERATE_MD5_EXIT_STATUS_UNKNOWN;
         break;
   }
   
   return return_type;
}
             
verify_file_return_type verify_file(const char *file_name_and_path, const char *md5sum_and_path)
{
   char sh_argument[PATH_MAX];
   char file_path[PATH_MAX];
   int i, length, exit_status;
   struct stat file_info;
   verify_file_return_type return_type;

   
   if ( ( strlen( file_name_and_path ) > ( PATH_MAX + 1 ) ) ||
        ( strlen( md5sum_and_path ) > ( PATH_MAX + 1 ) ) )
      return VERIFY_FILE_OVERFLOW_ERROR;
   
   if ( stat( file_name_and_path, &file_info ) == -1 )
      return VERIFY_FILE_STAT_FAILED;

   if ( S_ISREG( file_info.st_mode ) == 0 )
      return VERIFY_FILE_NOT_A_REGULAR_FILE;
   
   if ( stat( md5sum_and_path, &file_info ) == -1 )
      return VERIFY_FILE_STAT_FAILED;
   
   if ( S_ISREG( file_info.st_mode ) == 0 )
      return VERIFY_FILE_NOT_A_REGULAR_FILE;
   
   if ( system( NULL ) == 0 )
      return VERIFY_FILE_SYSTEM_SH_NOT_AVAILABLE;
      
   strcpy( file_path, file_name_and_path );
   
   length = strlen( file_path );
   
   /* Find the root of the file. */
   i = strlen( file_path );
   while ( file_path[i] != '/' )
     i--;
   i++;
   
   file_path[i] = '\0';
     
   /*
      Setup the command-line argument for sh.
      Argument:
      'md5sum --status --check "md5sum_and_path" &> /dev/null{NULL}'
   */
   
   strcpy( sh_argument, "md5sum --status --check \"" );
   strcat( sh_argument, md5sum_and_path );
   strcat( sh_argument, "\" &> /dev/null" );
   strcat( sh_argument, "\0" );

   /* WD = {file_path} */
   if ( chdir( file_path ) == -1 )
      return VERIFY_FILE_CHDIR_FAILED;

   exit_status = system( sh_argument );

   switch ( WEXITSTATUS( exit_status ) )
   {
      case -1:
         
         return_type = VERIFY_FILE_SYSTEM_ERROR;
         break;
      
      case 127:
         
         return_type = VERIFY_FILE_SYSTEM_SH_EXEC_ERROR;
         break;
      
      case 0:  
         
         return_type = VERIFY_FILE_MD5SUM_VERIFY_SUCCESSFUL;
         break;
      
      case 1:
         
         return_type = VERIFY_FILE_MD5SUM_VERIFY_UNSUCCESSFUL;
         break;
      
      default:
      
         return_type = VERIFY_FILE_EXIT_STATUS_UNKNOWN;
         break;
   }
   
   return return_type;
}
