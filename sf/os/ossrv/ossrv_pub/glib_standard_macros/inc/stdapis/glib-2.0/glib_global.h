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



#define g_thread_functions_for_glib_use (*_g_thread_functions_for_glib_use())
#define g_thread_use_default_impl (*_g_thread_use_default_impl())
#define g_threads_got_initialized (*_g_threads_got_initialized()) 
#define g_idle_funcs (*_g_idle_funcs())
#define g_timeout_funcs (*_g_timeout_funcs())
#define g_child_watch_funcs (*_g_child_watch_funcs())
#define g_io_watch_funcs (*_g_io_watch_funcs())
#define _g_debug_flags (*__g_debug_flags())
#define _g_debug_initialized (*__g_debug_initialized())
#define glib_mem_profiler_table (*_glib_mem_profiler_table())
#define g_mem_gc_friendly (*_g_mem_gc_friendly())
#define g_ascii_table (*_g_ascii_table())
#define g_utf8_skip (*_g_utf8_skip())
#define glib_binary_age (*_glib_binary_age())
#define glib_interface_age (*_glib_interface_age())
#define glib_major_version (*_glib_major_version())
#define glib_micro_version (*_glib_micro_version())
#define glib_minor_version (*_glib_minor_version())
#define glib_on_error_halt (*_glib_on_error_halt())
