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

 


#include "libdbus_wsd_solution.h" 

#ifdef EMULATOR

#include <e32std.h> 
#include <pls.h> // For emulator WSD API 

const TUid KLibDbusUid3 = {0x20010153};     // This is the UID of the library

struct libdbus_global_struct *libdbus_ImpurePtr()
{
	  
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct libdbus_global_struct* p  = Pls<struct libdbus_global_struct>(KLibDbusUid3, &libdbus_Init);
  	return p;

#else

	return NULL;	

#endif

	  
	  
}
#ifdef DBUS_BUILD_TESTS

extern "C"
{
void debug_finalize (DBusServer *server);
void debug_disconnect (DBusServer *server);
}
#endif //DBUS_BUILD_TESTS

int libdbus_Init(libdbus_global_struct *g)
{
#if defined(__WINSCW__) || defined(__WINS__)
	if(g)
	{
    	g->GET_DBUS_WSD_VAR_NAME(bus_data_slot,dbus_bus,s)= -1;
		g->GET_DBUS_WSD_VAR_NAME(activation_bus_type,dbus_bus,s)= DBUS_BUS_STARTER;
		g->GET_DBUS_WSD_VAR_NAME(initialized,dbus_bus,s)= FALSE;
		g->GET_DBUS_WSD_VAR_NAME(_dbus_modify_sigpipe,dbus_connection,s)= TRUE;
		g->GET_DBUS_WSD_VAR_NAME(shared_connections,dbus_connection,s)=NULL;
		g->GET_DBUS_WSD_VAR_NAME(warn_initted,dbus_internal,s)=FALSE;
		g->GET_DBUS_WSD_VAR_NAME(fatal_warnings,dbus_internal,s)=FALSE;
		g->GET_DBUS_WSD_VAR_NAME(fatal_warnings_on_check_failed,dbus_internal,s)= TRUE;
		g->GET_DBUS_WSD_VAR_NAME(verbose_initted,dbus_internals,s)= FALSE;
		g->GET_DBUS_WSD_VAR_NAME(verbose,dbus_internals,s)= TRUE;
		g->GET_DBUS_WSD_VAR_NAME(machine_uuid_initialized_generation,dbus_internals,s)=0;
		g->GET_DBUS_WSD_VAR_NAME(need_pid,dbus_internals,s)=TRUE;
		g->GET_DBUS_WSD_VAR_NAME(_dbus_current_generation,dbus_memory,g)=1;

		g->GET_DBUS_WSD_VAR_NAME(notify_user_data_slot,dbus_pending_call,s)=-1;
		g->GET_DBUS_WSD_VAR_NAME(message_cache_count,dbus_message,s)=0;
		g->GET_DBUS_WSD_VAR_NAME(message_cache_shutdown_registered,dbus_message,s)=FALSE;
		g->GET_DBUS_WSD_VAR_NAME(thread_init_generation,dbus_threads,s)=0;
		g->GET_DBUS_WSD_VAR_NAME(uninitialized_mutex_list,dbus_threads,s)= NULL;
		g->GET_DBUS_WSD_VAR_NAME(uninitialized_condvar_list,dbus_threads,s)= NULL;
		g->GET_DBUS_WSD_VAR_NAME(list_pool,dbus_list,s)=NULL;
		g->GET_DBUS_WSD_VAR_NAME(registered_globals,dbus_memory,s)=NULL;
		g->GET_DBUS_WSD_VAR_NAME(database_locked,dbus_userdb,s)=FALSE;
		g->GET_DBUS_WSD_VAR_NAME(system_db,dbus_userdb,s)=NULL;

		memset(&(g->GET_DBUS_WSD_VAR_NAME(message_cache_array,dbus_message,s)),0,5*sizeof(DBusMessage *));	  	  	
		memset(&(g->GET_DBUS_WSD_VAR_NAME(bus_connections,dbus_bus,s)),0,3*sizeof(DBusConnection *));	  
		memset(&(g->GET_DBUS_WSD_VAR_NAME(bus_connection_addresses,dbus_bus,s)),0,3*sizeof(char*));
		memset(&(g->GET_DBUS_WSD_VAR_NAME(thread_functions,dbus_threads,s)),0,19*sizeof(DBusThreadFunctions));	  
#ifdef DBUS_BUILD_TESTS
		g->GET_DBUS_WSD_VAR_NAME(fail_nth,dbus_memory,s) = -1;
		g->GET_DBUS_WSD_VAR_NAME(fail_alloc_counter,dbus_memory,s) = _DBUS_INT_MAX;
		g->GET_DBUS_WSD_VAR_NAME(n_failures_per_failure,dbus_memory,s) = 1;

		DBusServerVTable temp_vtable = {
		  debug_finalize,
		  debug_disconnect
		};

		memcpy(&(g->GET_DBUS_WSD_VAR_NAME(debug_vtable,dbus_server_debug_pipe,s)),&temp_vtable,sizeof(DBusServerVTable));
#endif //DBUS_BUILD_TESTS	 

		return 0;
	  }
	  return 1;
#else

	return 1;

#endif		  
}

extern "C" EXPORT_C void _dbus_wsd_reset()
{
	struct libdbus_global_struct* p  = Pls<struct libdbus_global_struct>(KLibDbusUid3, &libdbus_Init);	
	if(p)
	{
		memset(p,0,sizeof(struct libdbus_global_struct));
		libdbus_Init(p);
	}
}

#endif // EMULATOR


