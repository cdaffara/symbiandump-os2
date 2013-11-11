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
#include "glibbackend_wsd.h"
#include "glibbackendinternal.h"

#if EMULATOR
#include "pls.h" // For emulator WSD API 
const TUid KGlibbackendUid3 = {0x10282414};     // This is the UID of the library

int InitializeWsd(struct global_struct* g)
{
	g->VARIABLE_NAME(key_once,lowmem) = PTHREAD_ONCE_INIT;
	InitGLIBHeap(g->_iPrivateHeap);
	return KErrNone;
}

struct global_struct *Glibbackend_ImpurePtr()
{
#if defined(__WINSCW__) || defined(__WINS__)

	// Access the PLS of this process
	struct global_struct* p  = Pls<struct global_struct>(KGlibbackendUid3, &InitializeWsd);
    return p;

#else
		
	return NULL;

#endif

}

//Implementation of private heap for glib
#ifdef __WINSCW__
RHeap* PrivateHeap()
	{
	return (Glibbackend_ImpurePtr()->_iPrivateHeap);
	}
#define iPrivateHeap	(PrivateHeap())
#else
RHeap* 			iPrivateHeap;
#endif // WINSCW


#endif /* EMULATOR */ 

