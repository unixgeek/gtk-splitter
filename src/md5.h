/*
 * md5.h
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

#ifndef MD5_H
#define MD5_H

typedef enum 
{
   GENERATE_MD5_OVERFLOW_ERROR,
   GENERATE_MD5_STAT_FAILED,
   GENERATE_MD5_NOT_A_REGULAR_FILE,
   GENERATE_MD5_NOT_A_DIRECTORY,
   GENERATE_MD5_CHDIR_FAILED,
   GENERATE_MD5_SYSTEM_FORK_FAILED, 
   GENERATE_MD5_SYSTEM_SH_NOT_FOUND, 
   GENERATE_MD5_MD5SUM_EXIT_OK,
   GENERATE_MD5_MD5SUM_EXIT_FAILURE,
   GENERATE_MD5_EXIT_STATUS_UNKNOWN
} generate_md5_exit_status;

typedef enum 
{
   VERIFY_FILE_OVERFLOW_ERROR,
   VERIFY_FILE_STAT_FAILED,
   VERIFY_FILE_NOT_A_REGULAR_FILE,
   VERIFY_FILE_CHDIR_FAILED,
   VERIFY_FILE_SYSTEM_FORK_FAILED, 
   VERIFY_FILE_SYSTEM_SH_NOT_FOUND, 
   VERIFY_FILE_MD5SUM_VERIFY_SUCCESSFUL,
   VERIFY_FILE_MD5SUM_VERIFY_UNSUCCESSFUL,
   VERIFY_FILE_EXIT_STATUS_UNKNOWN
} verify_file_exit_status;

generate_md5_exit_status generate_md5_sum(const char *file_name_and_path, const char *output_directory);

verify_file_exit_status verify_file(const char *file_name_and_path, const char *location_of_md5sum);

#endif /* MD5_H */
