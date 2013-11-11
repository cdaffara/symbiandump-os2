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



#ifndef __GTHREAD_WSD_H__
#define __GTHREAD_WSD_H__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))

#include <_ansi.h>
#include <sys/cdefs.h>
#include <gthread.h>

/* ------------------------ MACRO DEFINATIONS -------------------------*/
#ifndef EMULATOR
#define EMULATOR (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#endif /* EMULATOR */

#define VARIABLE_NAME(var,filename) s_##filename##_##var

#define FUNCTION_NAME(var,filename) _s_##filename##_##var

#define VARIABLE_DECL(var,filename,datatype)\
datatype VARIABLE_NAME(var,filename);

#define VARIABLE_DECL_ARRAY(name,filename,datatype,size) \
datatype VARIABLE_NAME(name,filename)[size];

#define RETURN_VAL(var,filename) (Gthread_ImpurePtr()->VARIABLE_NAME(var,filename))

#define PLS(var,filename,type) \
type *FUNCTION_NAME(var,filename)() \
{ \
return &RETURN_VAL(var,filename);\
}

#define PLS_ARRAY(var,filename,type) \
type *FUNCTION_NAME(var,filename)() \
{ \
return RETURN_VAL(var,filename);\
}
/* ------------------------ END MACRO DEFINATIONS -------------------------*/

struct global_struct
{
	VARIABLE_DECL(thread_system_already_initialized,gthread_impl,gboolean)
	VARIABLE_DECL_ARRAY(g_thread_priority_map,gthread_impl,gint,G_THREAD_PRIORITY_URGENT + 1)
	
	#ifdef G_ENABLE_DEBUG
	VARIABLE_DECL(posix_check_cmd_prio_warned,gthread_posix,gboolean)
	#endif /* G_ENABLE_DEBUG */
	
	VARIABLE_DECL(zero_thread,gthread_impl,GSystemThread)
	VARIABLE_DECL(priority_normal_value,gthread_posix,gint)
	VARIABLE_DECL(g_thread_min_stack_size ,gthread_posix,gulong)
	VARIABLE_DECL(g_thread_functions_for_glib_use_default ,gthread_posix,GThreadFunctions)
};

__BEGIN_DECLS
struct global_struct *Gthread_ImpurePtr();
__END_DECLS
#endif 

#endif /* __GTHREAD_WSD_H__ */