/*
 * $Id: combine.c,v 1.29 2005/04/22 15:57:21 techgunter Exp $
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

#ifdef HAVE_LIBMHASH
#include <mhash.h>
#endif
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include "session.h"
#include "window.h"
#include "progress.h"
#include "interface.h"
#include "combine.h"

void
gtk_splitter_combine_files_test (combine_info info)
{
  GString *source;
  gulong size;
  int i;
  
  g_print ("blocksize: %d\n", info.block_size);
  g_print ("%i->%s (%li)\n", info.number_of_source_files, 
           info.destination_file->str,
           info.destination_file_size);
  
  for (i = 0; i != info.number_of_source_files; i++)
    {
      source = g_array_index (info.source_files, GString *, i);
      size = g_array_index (info.source_file_sizes, gulong, i);
       
      g_print ("%d) %s (%ld)\n", i + 1, source->str, size);
    }
}

gboolean
gtk_splitter_combine_files (GtkSplitterSessionData * data)
{
  ProgressWindow *progress_window = NULL;
  combine_info info;
  GString *source;
  gchar *buffer = NULL;
  gchar *basename = NULL;
  gulong size;
  gulong bytes;
  gulong total_bytes;
  gulong bytes_fread;
  gulong bytes_fwrite;
  
  int i;
  FILE *in, *out;
  
  gtk_splitter_get_combine_info(&info, data);
  
  gtk_splitter_combine_files_test (info);
  
  /* Make sure the files don't exceed our limit. */
  if (info.number_of_source_files > 999)
    {
      display_error ("combine.c:  Exceeded maximum number of files (999).");
      return FALSE;
    }

  /* Open a file to combine the other files to. */
  out = fopen (info.destination_file->str, "wb+");
  if (out == NULL)
    {
      display_error ("combine.c:  Could not create an output file.");
      return FALSE;
    }

  progress_window = progress_window_new ();
  if (progress_window == NULL)
    {
      display_error ("combine.c:  Could not create a progress window.");
      fclose (out);
      return FALSE;
    }

  gtk_window_set_title (GTK_WINDOW (progress_window->base_window),
                        "Combine Progress");
  gtk_widget_show_all (progress_window->base_window);
  while (g_main_context_iteration (NULL, FALSE));

  buffer = g_malloc (info.block_size * sizeof (gchar));
  
  total_bytes = 0;
  for (i = 0; i != info.number_of_source_files; i++)
    {
      if (progress_window->cancelled)
        {
          i = info.number_of_source_files;
          break;
        }
        
      source = g_array_index (info.source_files, GString *, i);
      size = g_array_index (info.source_file_sizes, gulong, i);
      
      /* Free the prior instance. */
      g_free (basename);
      
      /* Set the text to the basename of the current file. */
      basename = g_path_get_basename (source->str);
      gtk_label_set_text (GTK_LABEL (progress_window->message), basename);
      
      in = fopen (source->str, "rb");
      if (in == NULL)
        {
          display_error
            ("combine.c:  Could not open one of the files to be combined.");
          g_free (buffer);
          g_free (basename);
          progress_window_destroy (progress_window);
          fclose (out);
          return FALSE;
        }
      
      bytes = 0;
      bytes_fread = 0;
      bytes_fwrite = 0;
      while ((!feof (in)) && (!progress_window->cancelled))
        {
          bytes_fread = fread (buffer, sizeof (gchar), info.block_size, in);
          bytes_fwrite = fwrite (buffer, sizeof (gchar), bytes_fread, out);
          bytes += bytes_fread;
          total_bytes += bytes_fread;
          
          gtk_progress_bar_set_fraction (
              GTK_PROGRESS_BAR (progress_window->current_progress),
              ((gfloat) bytes) / ((gfloat) size));
              
          gtk_progress_bar_set_fraction (
              GTK_PROGRESS_BAR (progress_window->total_progress),
              ((gfloat) total_bytes) / ((gfloat) info.destination_file_size));
              
          while (g_main_context_iteration (NULL, FALSE));
        }

      /* Insure that all data is written to disk before we close. */
      fflush (out);

      /* Move on to the next file. */
      if (fclose (in) == EOF)
        {
          display_error
            ("combine.c:  Could not open one of the files to be combined.");
          g_free (buffer);
          g_free (basename);
          progress_window_destroy (progress_window);
          fclose (out);
          return FALSE;
        }
    }
  /* End of the combine process. */

  g_free (buffer);
  g_free (basename);
  
    /* Close our newly combined file. */
  if (fclose (out) == EOF)
    {
      display_error ("combine.c:  Could not close the combined file.");
      progress_window_destroy (progress_window);
      return TRUE;
    }

  gtk_widget_hide_all (progress_window->base_window);
  progress_window_destroy (progress_window);
  gtk_splitter_destroy_combine_info (&info);
  
  if (progress_window->cancelled)
    {
      return FALSE;
    }
    
  return TRUE;
}

gboolean
gtk_splitter_get_combine_info (combine_info * info, 
  GtkSplitterSessionData * data)
{
  struct stat file_info;
  struct statfs file_fs_info;
  GString *source;
  GString *current_file;
  GString *extension;
  FILE *test;
  
  /* Set the destination file. */
  info->destination_file = g_string_new (data->output_directory);
  info->destination_file = g_string_append_c (info->destination_file, '/');
  info->destination_file = g_string_append (info->destination_file,
                                            data->file_name_only);
  info->destination_file = g_string_truncate (info->destination_file,
                                              info->destination_file->len - 4);
  
  /* Create an array for the file sizes. */
  info->source_file_sizes = g_array_new (FALSE, FALSE, sizeof (gulong));
  
  /* Create an array for the source filenames. */
  info->source_files = g_array_new (FALSE, FALSE, sizeof (GString *));
  
  /* Setup the input file. */
  extension = g_string_new ("001");
  source = g_string_new (data->file_name_and_path);
  
  /* Should already end in 001. */
  source = g_string_truncate (source, source->len - 3);
  source = g_string_append (source, extension->str);
  
  info->number_of_source_files = 0;
  info->destination_file_size = 0;
  
  if (statfs (source->str, &file_fs_info) == -1)
    {
      display_error ("combine.c:  Could not stat file.");
      return FALSE;
    }
  info->block_size = file_fs_info.f_bsize;
  
  while ((test = fopen (source->str, "rb")) != NULL)
    {
      fclose (test);
      
      info->number_of_source_files++;

      /* Add to the array of source files. */
      current_file = g_string_new (source->str);
      g_array_append_val (info->source_files, current_file);
      
      if (g_stat (source->str, &file_info) == -1)
        {
          display_error ("combine.c:  Could not stat file.");
          return FALSE;
        }
      
      info->destination_file_size += file_info.st_size;
            
      /* Add to the array of source file sizes. */
      g_array_append_val (info->source_file_sizes, file_info.st_size);
      
      /* Increment the extension. */
      if (extension->str[2] != '9')
        extension->str[2]++;
      else
        {
          extension->str[2] = '0';
          if (extension->str[1] != '9')
            extension->str[1]++;
          else
            {
              extension->str[1] = '0';
              extension->str[0]++;
            }
        }
      
      /* Next file to open. */
      source = g_string_truncate (source, source->len - 3);
      source = g_string_append (source, extension->str);
    }
    
  return TRUE;
}

void
gtk_splitter_destroy_combine_info (combine_info * info)
{
  GString *source;
  int i;
  
  /* Free all the GStrings. */
  for (i = 0; i != info->number_of_source_files; i++)
    {
      source = g_array_index (info->source_files, GString *, i);
      
      g_string_free (source, TRUE);
    }
    
  /* Free the GArrays. */
  g_array_free (info->source_file_sizes, TRUE);
  g_array_free (info->source_files, TRUE);
}
