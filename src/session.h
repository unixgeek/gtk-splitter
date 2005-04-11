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
 
#ifndef SESSION_H
#define SESSION_H

#define UPDATE_INTERVAL 32768 

typedef struct
  {
    gfloat entry;
    enum {BYTES, KILOBYTES, MEGABYTES} unit;
    gchar file_name_and_path[PATH_MAX];
    gchar file_name_only[PATH_MAX];
    gchar home_directory[PATH_MAX];
    gchar output_directory[PATH_MAX];
    gboolean verify;
    gboolean split;
    gboolean create_batchfile;    
  } GtkSplitterSessionData;

#endif /* SESSION_H */
