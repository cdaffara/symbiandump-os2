/* GLIB - Library of useful routines for C programming
 * gmappedfile.h: Simplified wrapper around the mmap function
 *
 * Copyright 2005 Matthias Clasen
 * Portions copyright (c) 2006 Nokia Corporation.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef __G_MAPPED_FILE_H__
#define __G_MAPPED_FILE_H__

#include <_ansi.h>
#include <glib/gerror.h>

G_BEGIN_DECLS

typedef struct _GMappedFile GMappedFile;

IMPORT_C GMappedFile *g_mapped_file_new          (const gchar  *filename,
				         gboolean      writable,
				         GError      **error) G_GNUC_MALLOC;
IMPORT_C gsize        g_mapped_file_get_length   (GMappedFile  *file);
IMPORT_C gchar       *g_mapped_file_get_contents (GMappedFile  *file);
IMPORT_C void         g_mapped_file_free         (GMappedFile  *file);

G_END_DECLS

#endif /* __G_MAPPED_FILE_H__ */
