/* 
 * batchfile.h
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

#ifndef BATCHFILE_H
#define BATCHFILE_H

#include <stdio.h>
#include <gtk/gtk.h>

void write_batchfile(FILE *, gchar *);  /*Does not include eoln characters.*/

void writeln_batchfile(FILE *, gchar *);

void initialize_batchfile(FILE *, gchar *);

void finalize_batchfile(FILE *);

void dosify_filename(gchar *, gushort);

#endif /* BATCHFILE_H */
