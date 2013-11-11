/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


 
#ifndef _LIBDBUS_GLIB_WSD_H
#define _LIBDBUS_GLIB_WSD_H



#include <dbus.h>
#include <dbus-threads.h>
#include <glib.h>
#include <glib/gtypes.h>
#include <glib-object.h>
#include <glib/gthread.h>
#include <glibconfig.h>
#include "libdbus_glib_wsd_macros.h"


#if EMULATOR
#ifdef __cplusplus
extern "C" 
{
#endif

struct libdbus_glib_global_struct
{
	  VARIABLE_DECL(server_slot,s,dbus_gmain,dbus_int32_t)
	  VARIABLE_DECL(specialized_containers,s,dbus_gtype_specialized,GHashTable *)
      VARIABLE_DECL(marshal_table,s,dbus_gobject,GHashTable *)
      VARIABLE_DECL(error_metadata,s,dbus_gobject,GData *)
      VARIABLE_DECL(globals_lock,s,dbus_gobject,GStaticRWLock )
      VARIABLE_DECL(g_proxy_manager_slot,s,dbus_gproxy,int)
      VARIABLE_DECL(parent_class,s,dbus_gproxy,void *)
      VARIABLE_DECL_ARRAY(signals,s,dbus_gproxy,guint,2)  //array
      VARIABLE_DECL(connection_g_proxy_lock,s,dbus_gproxy,GStaticMutex)
      
      VARIABLE_DECL(_dbus_gmain_connection_slot,g,dbus_gmain,dbus_int32_t)  //global
      
      
      VARIABLE_DECL(quark,s,dbus_gtype_specialized,GQuark)
      VARIABLE_DECL(quark,s,dbus_glib,GQuark)
      VARIABLE_DECL(our_type,p,dbus_glib,GType)
      VARIABLE_DECL(our_type,q,dbus_glib,GType)
      VARIABLE_DECL(our_type,r,dbus_glib,GType)
      VARIABLE_DECL(our_type,s,dbus_glib,GType)
      VARIABLE_DECL(quark,s,dbus_gobject,GQuark)
      VARIABLE_DECL(object_type,s,dbus_gproxy,GType)
      VARIABLE_DECL(quark,s,dbus_gvalue,GQuark)
      VARIABLE_DECL(types_initialized,s,dbus_gvalue,gboolean)
      VARIABLE_DECL(type_id,s,dbus_gvalue,GType)
      VARIABLE_DECL(our_type,s,dbus_gidl,GType)
            
    /*END-global vars*/
 
    };

struct libdbus_glib_global_struct * libdbus_glib_ImpurePtr();
int libdbus_glib_Init(struct libdbus_glib_global_struct *);
#ifdef __cplusplus
}
#endif
#endif //EMULATOR
#endif //header guard

