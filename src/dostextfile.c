/*
 * $Id$
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
#include <string.h>
#include "dostextfile.h"

/* write_dostextfile() and writeln_dostextfile() assume that the FILE
   is opened for writing in BINARY mode and 'data' is NULL terminated. */

/* Write a string to the file pointed to by b without the eoln. */
void
write_dostextfile (FILE * b, char *data)
{
  fprintf (b, data);
  fflush (b);
}

/* Write a string to the file pointed to by b with the eoln. */
void
writeln_dostextfile (FILE * b, char *data)
{
  fprintf (b, data);
  fputc (0x0d, b);
  fputc (0x0a, b);
  fflush (b);
}

/* Convert a file name to a dos file name.
   This function assumes that the array is NULL terminated and at least 13 characters
   long--12 characters for the file name and 1 character for the NULL terminator. */
void
dosify_file_name (char *file_name)
{
  unsigned short i, length;
  char ext[3];

  length = strlen (file_name);

  /*Truncate the file name if it is longer than 12 characters. */
  if (length > 12)
    {

      /* Preserve the original file name's extension.
         The extension must be copied into temporary storage first
         to keep from overwriting itself in the original array. */
      ext[0] = file_name[length - 3];
      ext[1] = file_name[length - 2];
      ext[2] = file_name[length - 1];

      file_name[8] = '.';
      file_name[9] = ext[0];
      file_name[10] = ext[1];
      file_name[11] = ext[2];
      file_name[12] = '\0';
    }

  /* Convert some characters to underscores. */
  for (i = 0; i <= 7; i++)
    {                           /* {'*' '+' ' ' '|' '\' '=' '?' '[' ']' ';' ':' '"' '<' '>' ','} */
      if ((file_name[i] == '*') || (file_name[i] == '+')
          || (file_name[i] == ' ') || (file_name[i] == '|')
          || (file_name[i] == '\\') || (file_name[i] == '=')
          || (file_name[i] == '?') || (file_name[i] == '[')
          || (file_name[i] == ']') || (file_name[i] == ';')
          || (file_name[i] == ':') || (file_name[i] == '"')
          || (file_name[i] == '<') || (file_name[i] == '>')
          || (file_name[i] == ','))
        file_name[i] = '_';
    }
}
