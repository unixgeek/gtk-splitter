/*
 * $Id: batch.c,v 1.1 2005/04/22 19:41:12 techgunter Exp $
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
 * rem gtk-splitter-2.2
 * rem
 * rem Tested on Windows 2000.  
 * rem 
 * rem If this works on other versions, or if you have a patch to make this work
 * rem on other versions, then send email to techgunter@yahoo.com.
 * rem
 * echo Creating DESTINATION...
 * copy /b +"SOURCE.001"+"SOURCE.002"+"SOURCE.003" DESTINATION > NUL
 * if %errorlevel% equ 0 goto okay
 * 
 * :error
 * echo Failed
 * goto quit
 * 
 * :okay
 * echo Ok
 * 
 * :quit
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
    
  fprintf (batch, "@echo off\n");
  fprintf (batch, "rem\n");
  fprintf (batch, "rem %s-%s\n", PACKAGE, VERSION);
  fprintf (batch, "rem\n");
  fprintf (batch, "rem Tested on Windows 2000\n");
  fprintf (batch, "rem\n");
  fprintf (batch, "rem If this works on other versions, or if you have a "
                  "patch to make this work\n");
  fprintf (batch, "rem on other versions, then send email to "
                  "techgunter@yahoo.com.\n");
  fprintf (batch, "rem\n");
  fprintf (batch, "echo Creating %s...\n", source);
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
  fprintf (batch, " \"%s\" > NUL\n", source);
  fprintf (batch, "if %%errorlevel%% equ 0 goto okay\n\n");
  fprintf (batch, ":error\n");
  fprintf (batch, "echo Failed\n");
  fprintf (batch, "goto quit\n\n");
  fprintf (batch, ":okay\n");
  fprintf (batch, "echo Ok\n\n");
  fprintf (batch, ":quit\n");
  
  fflush (batch);
  fclose (batch);
  
  g_free (destination);
  g_free (source);
}
