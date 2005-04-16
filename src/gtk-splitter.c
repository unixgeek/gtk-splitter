/*
 * $Id: gtk-splitter.c,v 1.24 2005/04/15 02:24:08 techgunter Exp $
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
#include <glib.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "window.h"
#include "session.h"
#include "callbacks.h"
#include "interface.h"
#include "file_selection.h"

int
main (int argc, char *argv[])
{
  GtkSplitterWindow *main_window;

  /* Initialize gtk. */
  gtk_init (&argc, &argv);

  main_window = gtk_splitter_main_window_new ();

  /* Initialize session data. */
  initialize_session_data (main_window->session_data);
  initialize_splitter_window (main_window);

  /* Display the gui on the screen. */
  gtk_widget_show_all (main_window->base_window);

  /* Run the main loop of gtk. */
  gtk_main ();

  gtk_splitter_main_window_destroy (main_window);

  return 0;
}
