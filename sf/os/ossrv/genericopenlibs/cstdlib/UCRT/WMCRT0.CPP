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
// EPOC32 version of crt0.c for C programs which always want multi-threaded support
// 
//

#include <e32std.h>
#include <e32base.h>
#include <estlib.h>	// for SpawnPosixServerThread()
#include <stdlib.h>

extern "C" int wmain (int argc, wchar_t *argv[], wchar_t *envp[]);

GLDEF_C TInt E32Main()
	{     
	CTrapCleanup::New();

	SpawnPosixServerThread();		// arrange for multi-threaded operation

	int argc=0;
	wchar_t** wargv=0;
	wchar_t** wenvp=0;


	__crt0(argc,wargv,wenvp);			// get args & environment from somewhere

	int ret=wmain(argc, wargv, wenvp);		// go

	// no need to explicitly delete the cleanup stack here as all memory used by
	// the process will be released by RProcess::Terminate(), called from inside exit().

	exit(ret);				// finish with atexit processing

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
