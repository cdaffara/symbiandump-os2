// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//
// Description:
// EPOC32 version of crt0.c for C programs which always want multi-threaded support
// 

#include <e32std.h>
#include <e32base.h>
#ifdef SYMBIAN_OE_POSIX_SIGNALS
#include <signal.h>
#endif
#include "estlib.h"	// for __crt0

extern "C" int wmain (int argc, wchar_t *argv[], wchar_t *envp[]);

extern "C" IMPORT_C void exit (int ret);

GLDEF_C TInt E32Main()
	{     
	//Create Cleanup Stack
	CTrapCleanup* pCleanup = CTrapCleanup::New();
	if( pCleanup )
		{
		int argc=0;
		wchar_t** wargv=0;
		wchar_t** wenvp=0;
#ifdef SYMBIAN_OE_POSIX_SIGNALS
		sigenable();
#endif
		// get args & environment from somewhere

		//coverity[alloc_fn]

		__crt0(argc,wargv,wenvp);

		//Call user(application)'s main
		int ret = 0;
		
		//coverity[leave_without_push] 
			
		TRAPD(retVal, ret = wmain(argc, wargv, wenvp));

		if(!ret)
			{
			ret = retVal;
			}

		// finish with atexit processing which terminates this process
		exit(ret);
		if (wenvp)
			{
			for (int i = 0; wenvp[i]; ++i)
				{
				User::Free(wenvp[i]);
				}
			User::Free(wenvp);
			}
		delete pCleanup;
		}
	else
		return KErrNoMemory;

	return(KErrNone);
	}

#ifdef __GCC32__

/* stub function inserted into main() by GCC */
#ifdef __MCORE__
extern "C" void __main (void) {}
#else
extern "C" void __gccmain (void) {}
#endif

/* Default GCC entrypoint */
extern "C" TInt _mainCRTStartup (void) 
    {
    extern TInt _E32Startup();
    return _E32Startup();
    }

#endif /* __GCC32__ */
