/*
 * batchfile.c
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
#include "batchfile.h"

/*Write a string to the file pointed to by b without the eoln.*/
void write_batchfile(FILE *b, gchar *tmp)
{
   fprintf( b, tmp );
   fflush( b );
}

/*Write a string to the file pointed to by b with the eoln.*/
void writeln_batchfile(FILE *b, gchar *tmp)
{
   fprintf( b, tmp );
   fputc( 0x0d, b );
   fputc( 0x0a, b );
   fflush( b );
}

/*Modify the file name so that it is compatible with DOS.*/
void dosify_filename(gchar *tmp, gushort length)
{
   gushort i;

   /*Preserve the original file name's extension.*/
   tmp[11] = tmp[length - 2];
   tmp[10] = tmp[length - 3];
   tmp[9] = tmp[length - 4];
   tmp[8] = '.';

   /*Truncate the file name and convert spaces to underscores.*/
   for ( i = 0; i <= 7; i++ )
     {
       if ( tmp[i] == ' ' )
         tmp[i] = '_';
     }
   tmp[12] = '\0';
}
