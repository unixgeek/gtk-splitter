/* 
 * dostextfile.h
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

#ifndef DOSTEXTFILE_H
#define DOSTEXTFILE_H

#include <stdio.h>

/* write_dostextfile() and writeln_dostextfile() assume that the FILE
   is opened for writing in BINARY mode and the array is NULL terminated. */

/* Write text to a file without the eoln characters. */
void write_dostextfile (FILE *, char *);

/* Write text to a file with the eoln characters. */
void writeln_dostextfile (FILE *, char *);

/* Convert a file name to a dos file name.
   This function assumes that the array is NULL terminated and at least 13 characters
   long--12 characters for the file name and 1 character for the NULL terminator. */
void dosify_file_name (char *);

#endif /* DOSTEXTFILE_H */
