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
#include <_ansi.h>
#include "gobject_wsd.h"
#include <glib.h>

#if EMULATOR
#include <pls.h> // For emulator WSD API 
const TUid KLibgobjectUid3 = {0x10281F39};     // This is the UID of the library

G_BEGIN_DECLS
extern const GTypeInfo temp_info;
extern const GTypeInfo gobject_info;
extern const GParamSpecTypeInfo temp_pspec_info;
extern const GBSearchConfig temp_g_signal_hlbsa_bconfig;
extern const GBSearchConfig temp_g_class_closure_bconfig;
extern const GSourceCallbackFuncs temp_closure_callback_funcs;
extern const GBSearchConfig gtype_instance_real_class_bconfig;
extern const GBSearchConfig temp_transform_bconfig;

void InitializeGlobalStaticMutexes(struct global_struct* g)
{
	GStaticMutex temp_init_mutex = { NULL, PTHREAD_MUTEX_INITIALIZER };
	GStaticRWLock temp_init_lock = {G_STATIC_MUTEX_INIT, NULL, NULL, 0, FALSE, 0, 0};
	
	//variable initialisation for gobject.c
	memcpy(&(g->VARIABLE_NAME_MACRO(construction_mutex, gobject)), &temp_init_mutex, sizeof(GStaticMutex));
	#ifdef G_ENABLE_DEBUG
	memcpy(&(g->VARIABLE_NAME_MACRO(debug_objects, gobject)), &temp_init_mutex, sizeof(GStaticMutex));
	#endif /* G_ENABLE_DEBUG */
	
	//variable initialisation for gparam.c
	memcpy(&(g->VARIABLE_NAME(init_smutex, g_param_spec_pool_new)), &temp_init_mutex, sizeof(GStaticMutex));
	
	//variable initialisation for gsignal.c
	memcpy(&(g->VARIABLE_NAME_MACRO(g_signal_mutex,gsignal)), &temp_init_mutex, sizeof(GStaticMutex));
	
	//variable initialisation for gtype.c
	memcpy(&(g->VARIABLE_NAME_MACRO(instance_real_class,gtype)), &temp_init_mutex, sizeof(GStaticMutex));
	memcpy(&(g->VARIABLE_NAME_MACRO(type_init_lock,g_type_init_with_debug_flags)), &temp_init_mutex, sizeof(GStaticMutex));
	memcpy(&(g->VARIABLE_NAME(type_rw_lock,gtype)), &temp_init_lock, sizeof(GStaticRWLock));
};

int InitializeWsd(struct global_struct* g)
{
	memset(g,0,sizeof(struct global_struct));
	
	//initialization of genums.c data
	memcpy(&(g->VARIABLE_NAME(info,g_enum_types_init)),&temp_info,sizeof(GTypeInfo));
	
	//initialization of gobject.c data
	memcpy(&(g->VARIABLE_NAME(info,g_object_type_init)),&gobject_info,sizeof(GTypeInfo));
	g->VARIABLE_NAME(floating_flag_handler,gobject) = object_floating_flag_handler;
	
	//initialization of gparamspec.c
	memcpy(&(g->VARIABLE_NAME(pspec_info,g_param_spec_types_init)),&temp_pspec_info,sizeof(GParamSpecTypeInfo));
	
	//initialization of gsignal.c data
	memcpy(&(g->VARIABLE_NAME(g_signal_hlbsa_bconfig,gsignal)),&temp_g_signal_hlbsa_bconfig,sizeof(GBSearchConfig));	
	memcpy(&(g->VARIABLE_NAME(g_class_closure_bconfig,gsignal)),&temp_g_class_closure_bconfig,sizeof(GBSearchConfig));	
	g->VARIABLE_NAME(g_handler_sequential_number ,gsignal) = 1;
	g->VARIABLE_NAME(seq_hook_id ,g_signal_add_emission_hook) = 1;
	
	//initialization of gsourceclosure.c data
	memcpy(&(g->VARIABLE_NAME(closure_callback_funcs,gsourceclosure)),&temp_closure_callback_funcs,sizeof(GSourceCallbackFuncs));
	
	//initialization of gtype.c data
	g->VARIABLE_NAME(static_fundamental_next,gtype) = G_TYPE_RESERVED_USER_FIRST;
	memcpy(&(g->VARIABLE_NAME(instance_real_class_bconfig,gtype)),&gtype_instance_real_class_bconfig,sizeof(GBSearchConfig));	
	(g->VARIABLE_NAME(debug_keys,g_type_init_with_debug_flags)[0]).key = "objects";
	(g->VARIABLE_NAME(debug_keys,g_type_init_with_debug_flags)[0]).value = G_TYPE_DEBUG_OBJECTS ;
	(g->VARIABLE_NAME(debug_keys,g_type_init_with_debug_flags)[1]).key = "signals";
	(g->VARIABLE_NAME(debug_keys,g_type_init_with_debug_flags)[1]).value = G_TYPE_DEBUG_SIGNALS ;
	
	//initialization of gvalue.c data
	memcpy(&(g->VARIABLE_NAME(transform_bconfig,gvalue)),&temp_transform_bconfig,sizeof(GBSearchConfig));	
	
	InitializeGlobalStaticMutexes(g);
	
	return KErrNone;
}

struct global_struct *Gobject_ImpurePtr()
{
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct global_struct* p  = Pls<struct global_struct>(KLibgobjectUid3, &InitializeWsd);
    return p;
    
#else

	return NULL;

#endif
}

G_END_DECLS
#endif /* EMULATOR */ 

