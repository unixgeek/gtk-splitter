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
#include "globals.h"

void write_batchfile(FILE *b, gchar *tmp)
{
   fprintf(b, tmp);
   fflush(b);
}

void writeln_batchfile(FILE *b, gchar *tmp)
{
   fprintf(b, tmp);
   fputc(0x0d, b);
   fputc(0x0a, b);
   fflush(b);
}

void initialize_batchfile(FILE *b, gchar *tmp)
{
   writeln_batchfile(b, "@Echo Off");
   write_batchfile(b, "Echo ");
   writeln_batchfile(b, GTK_SPLITTER_VERSION);
   write_batchfile(b, "Echo Creating ");
   writeln_batchfile(b, tmp);
   write_batchfile(b, "copy /b ");
}

void finalize_batchfile(FILE *b)
{
   writeln_batchfile(b, " > ~combine.tmp");
   writeln_batchfile(b, "erase ~combine.tmp");
   writeln_batchfile(b, "Echo Finished. ");
}

void dosify_filename(gchar *tmp, gushort length)
{
   gushort j;
#if DEBUG
   fprintf(stderr, "batchfile.c:  Truncating filename for DOS.\n");
#endif
   tmp[11] = tmp[length - 2];
   tmp[10] = tmp[length - 3];
   tmp[9] = tmp[length - 4];
   tmp[8] = '.';
   for (j = 0; j <= 7; j++)
     {
       if (tmp[j] == ' ')
         tmp[j] = '_';
     }
   tmp[12] = '\0';
   g_realloc(tmp, 13);
}
