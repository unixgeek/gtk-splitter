/* 
 * $Id: combine.h,v 1.7 2005/04/15 02:24:08 techgunter Exp $
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

#ifndef COMBINE_H
#define COMBINE_H

#include <gtk/gtk.h>
#include "session.h"

typedef struct
{
  guint number_of_source_files;
  gulong destination_file_size;
  GString *destination_file;
  GArray *source_file_sizes;
  GArray *source_files;
  guint block_size;
} combine_info;

gboolean 
gtk_splitter_combine_files (GtkSplitterSessionData *);

gboolean
get_info (combine_info *, GtkSplitterSessionData *);

void
destroy_info (combine_info * info);

#endif /* COMBINE_H */
