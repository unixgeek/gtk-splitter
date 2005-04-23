/*
 * $Id: batch.c,v 1.2 2005/04/23 14:20:26 techgunter Exp $
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
#include "split.h"
#include "interface.h"

/*
 * @echo off
 * rem
 * rem PACKAGE-VERSION
 * rem
 * echo Creating DESTINATION...
 * copy /b +"SOURCE.001"+"SOURCE.002"+"SOURCE.003" DESTINATION
 */

void
create_batch_script (split_info * info)
{
  FILE *batch;
  GString *destination_string;
  gchar *destination = NULL;
  gchar *source = NULL;
  int i;
  
  batch = fopen (info->batch_file->str, "w+");
  if (batch == NULL)
    {
      display_error ("Could not create the batch script");
      return;
    }
  
  source = g_path_get_basename (info->source_file->str);
    
  fprintf (batch, "@echo off\r\n");
  fprintf (batch, "rem\r\n");
  fprintf (batch, "rem %s-%s\r\n", PACKAGE, VERSION);
  fprintf (batch, "rem\r\n");
  fprintf (batch, "echo Creating %s...\r\n", source);
  fprintf (batch, "copy /b ");
  for (i = 0; i != info->number_of_destination_files; i++)
    {
      g_free (destination);
      
      destination_string = g_array_index (info->destination_files, 
                                          GString *, i);
      destination = g_path_get_basename (destination_string->str);
      
      if (i != 0)
        fprintf (batch, "+\"%s\"", destination);
      else
        fprintf (batch, "\"%s\"", destination);
    }
  fprintf (batch, " \"%s\"\r\n", source);
  
  fflush (batch);
  fclose (batch);
  
  g_free (destination);
  g_free (source);
}
