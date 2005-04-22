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
#include "split.h"
#include "interface.h"

/*
 * #!/bin/sh
 * #
 * # PACKAGE VERSION
 * #
 * echo "Creating DESTINATION..."
 * cat SOURCE1 SOURCE2 SOURCE3 ... 2> /dev/null > DESTINATION
 * if [ "$?" -ne "0"]; then
 *     echo "Failed"
 *     exit 1
 * else
 *     echo "Ok"
 * fi
 * 
 * exit 0
 * 
 */

void
create_shell_script (split_info * info)
{
  FILE *shell;
  GString *destination_string;
  gchar *destination = NULL;
  gchar *source = NULL;
  int i;
  
  shell = fopen (info->shell_file->str, "w+");
  if (shell == NULL)
    {
      display_error ("Could not create the shell script");
      return;
    }
  
  source = g_path_get_basename (info->source_file->str);
    
  fprintf (shell, "#!/bin/sh\n");
  fprintf (shell, "#\n");
  fprintf (shell, "# %s-%s\n", PACKAGE, VERSION);
  fprintf (shell, "#\n");
  fprintf (shell, "echo \"Creating %s...\"\n", source);
  fprintf (shell, "cat ");
  for (i = 0; i != info->number_of_destination_files; i++)
    {
      g_free (destination);
      
      destination_string = g_array_index (info->destination_files, 
                                          GString *, i);
      destination = g_path_get_basename (destination_string->str);
      
      fprintf (shell, "%s ", destination);
    }
  fprintf (shell, "2> /dev/null > %s\n", source);
  fprintf (shell, "if [ \"$?\" -ne \"0\" ]; then\n");
  fprintf (shell, "    echo \"Failed\"\n");
  fprintf (shell, "    exit 1\n");
  fprintf (shell, "else\n");
  fprintf (shell, "    echo \"Ok\"\n");
  fprintf (shell, "fi\n\n");
  fprintf (shell, "exit 0\n\n");
  
  fflush (shell);
  fclose (shell);
  
  g_free (destination);
  g_free (source);
}
