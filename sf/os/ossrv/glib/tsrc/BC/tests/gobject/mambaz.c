/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
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
*
* Description:
*
*/

#include "mambaz.h"

extern GType baz_type;

static void
maman_ibaz_base_init (gpointer g_class)
{
  
}

GType
maman_ibaz_get_type (void)
{
  static GType type = 0;
  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (MamanIbazInterface),
      maman_ibaz_base_init,   /* base_init */
      NULL,   /* base_finalize */
      NULL,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      0,
      0,      /* n_preallocs */
      NULL    /* instance_init */
    };
    type = g_type_register_static (G_TYPE_INTERFACE, "MamanIbaz", &info, 0);
  }
  return type;
}

void maman_ibaz_do_action (MamanIbaz *self)
{
 
  MAMAN_IBAZ_GET_INTERFACE (self)->do_action (self);
}

//that was the interface class

static void baz_do_action (MamanBaz *self)
{
  self->instance_member = 10;
}

void
baz_interface_init (gpointer   g_iface,
                    gpointer   iface_data)
{
  MamanIbazInterface *iface = (MamanIbazInterface *)g_iface;
  iface->do_action = (void (*) (MamanIbaz *self))baz_do_action;
}

void
baz_instance_init (GTypeInstance   *instance,
                   gpointer         g_class)
{
  MamanBaz *self = MAMAN_BAZ(instance);
  self->instance_member = 0xdeadbeaf;
}



