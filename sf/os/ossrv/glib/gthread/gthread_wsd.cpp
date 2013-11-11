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
#include "gthread_wsd.h"
#include <glib.h>


#if EMULATOR
#include <pls.h> // For emulator WSD API 
const TUid KLibgthreadUid3 = {0x10281F3A};     // This is the UID of the library

G_BEGIN_DECLS
extern const GThreadFunctions temp_g_thread_functions_for_glib_use_default;

int InitializeWsd(struct global_struct* g)
{
	memset(g,0,sizeof(struct global_struct));
//	memcpy(&(g->VARIABLE_NAME(g_thread_functions_for_glib_use_default ,gthread_posix)),&g_thread_functions_for_glib_use_default_temp,sizeof(GThreadFunctions));	
	return KErrNone;
}


struct global_struct *Gthread_ImpurePtr()
{
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct global_struct* p  = Pls<struct global_struct>(KLibgthreadUid3, &InitializeWsd);
    return p;	

#else
		
	return NULL;

#endif
}

G_END_DECLS
#endif /* EMULATOR */ 

