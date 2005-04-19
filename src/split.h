/* 
 * $Id: split.h,v 1.7 2005/04/15 02:24:09 techgunter Exp $
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

#ifndef SPLIT_H
#define SPLIT_H

#include <gtk/gtk.h>
#include "session.h"

typedef struct
{
  guint number_of_destination_files;
  gulong source_file_size;
  GString *source_file;
  GArray *destination_file_sizes;
  GArray *destination_files;
  guint block_size;
} split_info;

/* Split the selected file. */
gboolean
gtk_splitter_split_file (GtkSplitterSessionData *);

gboolean
gtk_splitter_get_split_info (split_info *, GtkSplitterSessionData *);

void
gtk_splitter_destroy_split_info (split_info *);

#endif /* SPLIT_H */
