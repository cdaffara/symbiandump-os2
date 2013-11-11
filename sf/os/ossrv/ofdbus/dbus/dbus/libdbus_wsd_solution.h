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

 


 
#ifndef __LIBDBUS_WSD_SOLUTION_H__
#define __LIBDBUS_WSD_SOLUTION_H__
#include "libdbus_wsd_macros.h"
#include <dbus/dbus.h>
#include "dbus/dbus-hash.h"
#include "dbus/dbus-internals.h"
#include "dbus/dbus-mempool.h"
#include "dbus/dbus-dataslot.h"
#include "dbus/dbus-userdb.h"
#include "dbus/dbus-sysdeps.h"
#include "dbus/dbus-string.h"
#include "dbus/dbus-threads.h"
#include "dbus-server-protected.h"

#if EMULATOR
#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct ShutdownClosure ShutdownClosure;

/**
 * This struct represents a function to be called on shutdown.
 */
struct ShutdownClosure
{
  ShutdownClosure *next;     /**< Next ShutdownClosure */
  DBusShutdownFunction func; /**< Function to call */
  void *data;                /**< Data for function */
};

struct libdbus_global_struct
{
	  VARIABLE_DECL(bus_data_slot,s,dbus_bus,dbus_int32_t)
	  VARIABLE_DECL(activation_bus_type,s,dbus_bus,DBusBusType)
      VARIABLE_DECL(initialized,s,dbus_bus,dbus_bool_t)
      VARIABLE_DECL(_dbus_modify_sigpipe,s,dbus_connection,dbus_bool_t)
      VARIABLE_DECL(shared_connections,s,dbus_connection,DBusHashTable *)
      
      VARIABLE_DECL(warn_initted,s,dbus_internal,dbus_bool_t)
      VARIABLE_DECL(fatal_warnings,s,dbus_internal,dbus_bool_t)
      VARIABLE_DECL(fatal_warnings_on_check_failed,s,dbus_internal,dbus_bool_t)
      
      VARIABLE_DECL(verbose_initted,s,dbus_internals,dbus_bool_t)
      VARIABLE_DECL(verbose,s,dbus_internals,dbus_bool_t)
      VARIABLE_DECL(machine_uuid_initialized_generation,s,dbus_internals,int)
      VARIABLE_DECL(machine_uuid,s,dbus_internals,DBusGUID)
      VARIABLE_DECL(need_pid,s,dbus_internals,dbus_bool_t)
      VARIABLE_DECL(message_cache_count,s,dbus_message,int)
      VARIABLE_DECL(message_cache_shutdown_registered,s,dbus_message,dbus_bool_t)
      VARIABLE_DECL(notify_user_data_slot,s,dbus_pending_call,dbus_int32_t)
      VARIABLE_DECL(thread_init_generation,s,dbus_threads,int)
      VARIABLE_DECL(uninitialized_mutex_list,s,dbus_threads,DBusList*)
      VARIABLE_DECL(uninitialized_condvar_list,s,dbus_threads,DBusList*)
      VARIABLE_DECL(list_pool,s,dbus_list,DBusMemPool *)
      VARIABLE_DECL(registered_globals,s,dbus_memory,ShutdownClosure *)
      VARIABLE_DECL(slot_allocator,s,dbus_connection,DBusDataSlotAllocator)
      VARIABLE_DECL(slot_allocator,s,dbus_message,DBusDataSlotAllocator)
      VARIABLE_DECL(slot_allocator,s,dbus_pending_call,DBusDataSlotAllocator)
      VARIABLE_DECL(slot_allocator,s,dbus_server,DBusDataSlotAllocator)
      
      VARIABLE_DECL(database_locked,s,dbus_userdb,dbus_bool_t)
      VARIABLE_DECL(system_db,s,dbus_userdb,DBusUserDatabase*)
      VARIABLE_DECL(process_username,s,dbus_userdb,DBusString)
      VARIABLE_DECL(process_homedir,s,dbus_userdb,DBusString)
      VARIABLE_DECL(tmpdir,s,dbus_sysdeps_unix,const char*)

      VARIABLE_DECL_ARRAY(thread_functions,s,dbus_threads,DBusThreadFunctions,19)
      
      VARIABLE_DECL_ARRAY(message_cache_array,s,dbus_message,DBusMessage *,5)
      VARIABLE_DECL_ARRAY(bus_connections,s,dbus_bus,DBusConnection *,3)
      VARIABLE_DECL_ARRAY(bus_connection_addresses,s,dbus_bus,char *,3)
      
      VARIABLE_DECL(_dbus_current_generation,g,dbus_memory,int)
      
      VARIABLE_DECL(_dbus_lock_bus,g,dbus_bus,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_bus_datas,g,dbus_bus,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_shared_connections,g,dbus_connection,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_message_cache,g,dbus_message,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_message_slots,g,dbus_message,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_pending_call_slots,g,dbus_pending_call,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_server_slots,g,dbus_server,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_machine_uuid,g,dbus_internals,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_list,g,dbus_list,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_shutdown_funcs,g,dbus_memory,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_atomic,g,dbus_sysdeps,DBusMutex *)
      
      VARIABLE_DECL(_dbus_lock_win_fds,g,dbus_sysdeps,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_sid_atom_cache,g,dbus_sysdeps,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_connection_slots,g,dbus_connection,DBusMutex *)
      VARIABLE_DECL(_dbus_lock_system_users,g,dbus_userdb,DBusMutex *)

#ifdef DBUS_BUILD_TESTS

	VARIABLE_DECL(server_pipe_hash,s,dbus_server_debug_pipe,DBusHashTable *)
	VARIABLE_DECL(server_pipe_hash_refcount,s,dbus_server_debug_pipe,int)

	VARIABLE_DECL(already_warned,s,_dbus_keyring_new_homedir,dbus_bool_t )

	  VARIABLE_DECL(free_counter,s,dbus_dataslot,int)

	VARIABLE_DECL(debug_initialized,s,dbus_memory,dbus_bool_t)
	VARIABLE_DECL(fail_nth,s,dbus_memory,int)
	VARIABLE_DECL(fail_size,s,dbus_memory,size_t)
	VARIABLE_DECL(fail_alloc_counter,s,dbus_memory,int)
	VARIABLE_DECL(n_failures_per_failure,s,dbus_memory,int)
	VARIABLE_DECL(n_failures_this_failure,s,dbus_memory,int)
	VARIABLE_DECL(guards,s,dbus_memory,dbus_bool_t)
	VARIABLE_DECL(disable_mem_pools,s,dbus_memory,dbus_bool_t)
	VARIABLE_DECL(backtrace_on_fail_alloc,s,dbus_memory,dbus_bool_t)
	VARIABLE_DECL(n_blocks_outstanding,s,dbus_memory,DBusAtomic)

	VARIABLE_DECL(debug_vtable,s,dbus_server_debug_pipe,DBusServerVTable)
#endif //DBUS_BUILD_TESTS	

	  /*END-global vars*/ 
    };

struct libdbus_global_struct *libdbus_ImpurePtr();
int libdbus_Init(struct libdbus_global_struct *);

#ifdef __cplusplus
}
#endif

#endif //EMULATOR
#endif //__LIBDBUS_WSD_SOLUTION_H__

