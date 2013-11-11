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

 


#include <e32std.h> 
#include "libdbus_glib_wsd_solution.h"
#include <pls.h> // For emulator WSD API 

const TUid KLibGlibDbusUid3 = {0x20010156};     // This is the UID of the library

struct libdbus_glib_global_struct *libdbus_glib_ImpurePtr()
{
	  
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct libdbus_glib_global_struct* p  = Pls<struct libdbus_glib_global_struct>(KLibGlibDbusUid3, &libdbus_glib_Init);
	return p;

#else

	return NULL;	

#endif

}

int libdbus_glib_Init(libdbus_glib_global_struct *g)
{
	  if(g)
	  {
	      g->GET_DBUS_WSD_VAR_NAME(server_slot,dbus_gmain,s)= -1;
		  g->GET_DBUS_WSD_VAR_NAME(specialized_containers,dbus_gtype_specialized,s)= NULL;
		  g->GET_DBUS_WSD_VAR_NAME(marshal_table,dbus_gobject,s)= NULL;
		  g->GET_DBUS_WSD_VAR_NAME(error_metadata,dbus_gobject,s)= NULL;
		
		 // g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s) = G_STATIC_RW_LOCK_INIT;
		   g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).mutex.runtime_mutex =NULL; ///*G_STATIC_MUTEX_INIT*/ {NULL,{-3,NULL,0}};
          g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).mutex.static_mutex.iState = _ENeedsNormalInit;
          g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).mutex.static_mutex.iPtr = NULL;
          g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).mutex.static_mutex.iReentry = 0;
           g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).read_cond=NULL;
            g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).write_cond=NULL;
             g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).read_counter=0;
              g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).have_writer=FALSE;
               g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).want_to_read=0;
                g->GET_DBUS_WSD_VAR_NAME(globals_lock,dbus_gobject,s).want_to_write=0;
          
         
		  
		  g->GET_DBUS_WSD_VAR_NAME(g_proxy_manager_slot,dbus_gproxy,s)=-1;
          g->GET_DBUS_WSD_VAR_NAME(parent_class,dbus_gproxy,s)=NULL;
          
          memset(&(g->GET_DBUS_WSD_VAR_NAME(signals,dbus_gproxy,s)),0,2*sizeof(guint));	  	  	
          g->GET_DBUS_WSD_VAR_NAME(connection_g_proxy_lock,dbus_gproxy,s).runtime_mutex =NULL; ///*G_STATIC_MUTEX_INIT*/ {NULL,{-3,NULL,0}};
          g->GET_DBUS_WSD_VAR_NAME(connection_g_proxy_lock,dbus_gproxy,s).static_mutex.iState = _ENeedsNormalInit;
          g->GET_DBUS_WSD_VAR_NAME(connection_g_proxy_lock,dbus_gproxy,s).static_mutex.iPtr = NULL;
          g->GET_DBUS_WSD_VAR_NAME(connection_g_proxy_lock,dbus_gproxy,s).static_mutex.iReentry = 0;
         
          g->GET_DBUS_WSD_VAR_NAME(_dbus_gmain_connection_slot,dbus_gmain,g)= -1;
          
          
          g->GET_DBUS_WSD_VAR_NAME(quark,dbus_gtype_specialized,s)=0;
          g->GET_DBUS_WSD_VAR_NAME(quark,dbus_glib,s)=0;
          g->GET_DBUS_WSD_VAR_NAME(our_type,dbus_glib,p)=0;
          g->GET_DBUS_WSD_VAR_NAME(our_type,dbus_glib,q)=0;
          g->GET_DBUS_WSD_VAR_NAME(our_type,dbus_glib,r)=0;
          g->GET_DBUS_WSD_VAR_NAME(our_type,dbus_glib,s)=0;
          g->GET_DBUS_WSD_VAR_NAME(quark,dbus_gobject,s)=0;
          g->GET_DBUS_WSD_VAR_NAME(object_type,dbus_gproxy,s)= 0;
          g->GET_DBUS_WSD_VAR_NAME(quark,dbus_gvalue,s)= 0;
          g->GET_DBUS_WSD_VAR_NAME(types_initialized,dbus_gvalue,s)=0;
          g->GET_DBUS_WSD_VAR_NAME(type_id,dbus_gvalue,s)=0;
		  g->GET_DBUS_WSD_VAR_NAME(our_type,dbus_gidl,s)=0;
         
			return 0;	
	  }
	  return 1;
}
