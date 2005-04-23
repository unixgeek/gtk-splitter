/*
 * $Id: split.c,v 1.41 2005/04/22 19:48:51 techgunter Exp $
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
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include "interface.h"
#include "session.h"
#include "window.h"
#include "progress.h"
#include "split.h"
#include "shell.h"
#include "batch.h"

void
gtk_splitter_split_file_test (split_info info)
{
  GString *destination;
  gulong size;
  int i;
  
  g_print ("blocksize: %d\n", info.block_size);
  g_print ("%s (%li) -> %i\n", info.source_file->str, 
           info.source_file_size,
           info.number_of_destination_files);
  
  g_print ("shell script: %s\n", info.shell_file->str);
  g_print ("batch script: %s\n", info.batch_file->str);
  
  for (i = 0; i != info.number_of_destination_files; i++)
    {
      destination = g_array_index (info.destination_files, GString *, i);
      size = g_array_index (info.destination_file_sizes, gulong, i);
       
      g_print ("%d) %s (%ld)\n", i + 1, destination->str, size);
    }
}

gboolean
gtk_splitter_split_file (GtkSplitterSessionData * data)
{
  ProgressWindow *progress_window = NULL;
  split_info info;
  GString *destination;
  gchar *buffer = NULL;
  gchar *basename = NULL;
  gulong size;
  gulong bytes;
  gulong total_bytes;
  gulong bytes_fread;
  gulong bytes_fwrite;
  
  int i;
  FILE *in, *out;
  
  gtk_splitter_get_split_info(&info, data);
  
  gtk_splitter_split_file_test (info);
  
  /* Make sure the files don't exceed our limit. */
  if (info.number_of_destination_files > 999)
    {
      display_error ("Exceeded maximum number of files (999).");
      return FALSE;
    }
  
  if (data->create_shell_script)
    create_shell_script (&info);
  
  if (data->create_batch_script)
    create_batch_script (&info);
  
  /* Open the source file. */
  in = fopen (info.source_file->str, "rb");
  if (in == NULL)
    {
      display_error ("Could not create an output file.");
      return FALSE;
    }
    
  progress_window = progress_window_new ();
  if (progress_window == NULL)
    {
      display_error ("Could not create a progress window.");
      fclose (in);
      return FALSE;
    }
    
  gtk_window_set_title (GTK_WINDOW (progress_window->base_window),
                        "Split Progress");
  gtk_widget_show_all (progress_window->base_window);
  while (g_main_context_iteration (NULL, FALSE));

  buffer = g_malloc (info.block_size * sizeof (gchar));
  if (buffer == NULL)
    {
      display_error ("Could not allocate buffer.");
      progress_window_destroy (progress_window);
      fclose (in);
      return FALSE;
    }
    
  total_bytes = 0;
  for (i = 0; i != info.number_of_destination_files; i++)
    {
      
      if (progress_window->cancelled)
        {
          i = info.number_of_destination_files;
          break;
        }
        
      destination = g_array_index (info.destination_files, GString *, i);
      size = g_array_index (info.destination_file_sizes, gulong, i);
      
      /* Free the prior instance. */
      g_free (basename);
      
      /* Set the text to the basename of the current file. */
      basename = g_path_get_basename (destination->str);
      gtk_label_set_text (GTK_LABEL (progress_window->message), basename);
      
      /* Open the split file. */
      out = fopen (destination->str, "wb+");
      if (out == NULL)
        {
          display_error ("Could not create an output file.");
          g_free (buffer);
          g_free (basename);
          progress_window_destroy (progress_window);
          fclose (in);
          return FALSE;
        }
      
      bytes = 0;
      while ((bytes != size) && (!progress_window->cancelled))
        {
          bytes_fread = fread (buffer, sizeof (gchar), info.block_size, in);
          
          if ((bytes_fread + bytes) > size)
            {
                bytes_fread = size - bytes;
                
                bytes_fwrite = fwrite (buffer, sizeof (gchar),
                    bytes_fread, out);
                bytes += bytes_fread;
                total_bytes += bytes_fread;
                
                /* Re-position the file pointer. */
                //fseek (in, - (size - bytes), SEEK_CUR);
                fseek (in, total_bytes, SEEK_SET);
            }
          else
            {
              bytes_fwrite = fwrite (buffer, sizeof (gchar), bytes_fread, out);
              bytes += bytes_fread;
              total_bytes += bytes_fread;
            }
            
          gtk_progress_bar_set_fraction (
              GTK_PROGRESS_BAR (progress_window->current_progress),
              ((gfloat) bytes) / ((gfloat) size));
              
          gtk_progress_bar_set_fraction (
              GTK_PROGRESS_BAR (progress_window->total_progress),
              ((gfloat) total_bytes) / ((gfloat) info.source_file_size));
              
          while (g_main_context_iteration (NULL, FALSE));
        }
        
      /* Insure that all data is written to disk before we close. */
      fflush (out);

      /* Move on to the next file. */
      if (fclose (out) == EOF)
        {
          display_error
            ("Could not close one of the files.");
          g_free (buffer);
          g_free (basename);
          progress_window_destroy (progress_window);
          fclose (in);
          return FALSE;
        }
    }
  
  g_free (buffer);
  g_free (basename);
  
  if (fclose (in) == EOF)
    {
      display_error ("Could not close the combined file.");
      progress_window_destroy (progress_window);
      return TRUE;
    }
  
  gtk_widget_hide_all (progress_window->base_window);
  progress_window_destroy (progress_window);
  gtk_splitter_destroy_split_info (&info);
  
  if (progress_window->cancelled)
    {
      return FALSE;
    }
     
  return TRUE;
}

gboolean
gtk_splitter_get_split_info (split_info * info, 
  GtkSplitterSessionData * data)
{
  struct stat file_info;
  struct statfs file_fs_info;
  GString *destination;
  GString *current_file;
  GString *extension;
  gulong target_size;
  gulong remaining_size;
  guint i;
  
  /* Set the source file. */
  info->source_file = g_string_new (data->file_name_and_path);
  
  /* Create an array for the file sizes. */
  info->destination_file_sizes = g_array_new (FALSE, FALSE, sizeof (gulong));
  
  /* Create an array for the destination filenames. */
  info->destination_files = g_array_new (FALSE, FALSE, sizeof (GString *));
  
  /* Get the block size. */
  if (statfs (info->source_file->str, &file_fs_info) == -1)
    {
      display_error ("Could not stat file.");
      return FALSE;
    }    
  info->block_size = file_fs_info.f_bsize;
  
  /* Get the source file size. */
  if (g_stat (info->source_file->str, &file_info) == -1)
    {
      display_error ("Could not stat file.");
      return FALSE;
    }
  info->source_file_size = file_info.st_size;
  
  /* Get the target size. */
  switch (data->unit) 
  {
    case BYTES:
      target_size = (gulong) data->entry;
      break;
    case KILOBYTES:
      target_size = (gulong) (data->entry * 1024);
      break;
    case MEGABYTES:
      target_size = (gulong) (data->entry * 1048576);
      break;
  }
  
  /* Set the shell script filename. */
  info->shell_file = g_string_new (data->output_directory);
  info->shell_file = g_string_append (info->shell_file, G_DIR_SEPARATOR_S);
  info->shell_file = g_string_append (info->shell_file, data->file_name_only);
  info->shell_file = g_string_append (info->shell_file, ".sh");
  
  /* Set the batch script filename. */
  info->batch_file = g_string_new (data->output_directory);
  info->batch_file = g_string_append (info->batch_file, G_DIR_SEPARATOR_S);
  info->batch_file = g_string_append (info->batch_file, data->file_name_only);
  info->batch_file = g_string_append (info->batch_file, ".bat");
  
  /* Determine the number of destination files. */
  info->number_of_destination_files = info->source_file_size / target_size;;
  
  remaining_size = info->source_file_size % target_size;
  if (remaining_size != 0)
    info->number_of_destination_files++;
    
  extension = g_string_new ("001");
  
  destination = g_string_new (data->output_directory);
  destination = g_string_append (destination, G_DIR_SEPARATOR_S);
  destination = g_string_append (destination, data->file_name_only);
  destination = g_string_append (destination, ".");
  destination = g_string_append (destination, extension->str);
  
  for (i = 0; i != info->number_of_destination_files; i++)
    {
      /* 
       * Add to the array of source file sizes. 
       * If this is the last file, use the remaining size if it isn't zero.
       */
      if ((i == info->number_of_destination_files - 1) 
        && (remaining_size != 0))
        {
          g_array_append_val (info->destination_file_sizes, remaining_size);
        }
      else
        {
          g_array_append_val (info->destination_file_sizes, target_size);
        }
      
      /* Next destination file. */
      destination = g_string_truncate (destination, destination->len - 3);
      destination = g_string_append (destination, extension->str);
      
      /* Add to the array of destination files. */
      current_file = g_string_new (destination->str);
      g_array_append_val (info->destination_files, current_file);
      
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
    }
    
  return TRUE;
}

void
gtk_splitter_destroy_split_info (split_info * info)
{
  GString *destination;
  int i;
  
  /* Free all the GStrings. */
  for (i = 0; i != info->number_of_destination_files; i++)
    {
      destination = g_array_index (info->destination_files, GString *, i);
      
      g_string_free (destination, TRUE);
    }
  
  g_string_free (info->source_file, TRUE);
  g_string_free (info->shell_file, TRUE);
  g_string_free (info->batch_file, TRUE);
  
  /* Free the GArrays. */
  g_array_free (info->destination_file_sizes, TRUE);
  g_array_free (info->destination_files, TRUE);
}
