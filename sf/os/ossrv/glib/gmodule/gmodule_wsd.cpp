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
#include <glib.h>
#include "gmodule_wsd.h"
#include <stdlib.h>
#include <gthread.h>

#if EMULATOR

#include <pls.h> // For emulator WSD API 
const TUid KLibgmoduleUid3 = {0x10281F3B};     // This is the UID of the library

G_BEGIN_DECLS

int InitializeWsd(struct global_struct* g)
{
	memset(g,0,sizeof(struct global_struct));
	
	GStaticRecMutex temp_init_mutex = G_STATIC_REC_MUTEX_INIT;
	memcpy(&(g->VARIABLE_NAME(g_module_global_lock, gmodule)), &temp_init_mutex, sizeof(GStaticMutex));
	return KErrNone;
}

struct global_struct *Gmodule_ImpurePtr()
{
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct global_struct* p  = Pls<struct global_struct>(KLibgmoduleUid3, &InitializeWsd);
    return p;
    
#else
	
	return NULL;

#endif

}


G_END_DECLS
#endif /* EMULATOR */ 

