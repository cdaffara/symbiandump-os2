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
#include "estlib.h"

#ifdef __ARMCC__
__asm int CallMain(int argc, char *argv[], char *envp[])
{
       import main
       code32
       b main
}
#define CALLMAIN(argc, argv, envp) CallMain(argc, argv, envp)
#else
extern "C" int main (int argc, char *argv[], char *envp[]);
#define CALLMAIN(argc, argv, envp) main(argc, argv, envp)
#endif

// Dummy function to handle GCCE toolchain problem

extern "C" GLDEF_C int __GccGlueInit()
    {
    return 0;
    }

extern "C" IMPORT_C void exit (int ret);

GLDEF_C TInt E32Main()
	{     
	//Create Cleanup Stack
	CTrapCleanup* pCleanup = CTrapCleanup::New();

	if( pCleanup )
		{
		int argc=0;
		char ** argv=0;
		char ** envp=0;
#ifdef SYMBIAN_OE_POSIX_SIGNALS
		sigenable();
#endif
		// get args & environment from somewhere
		//coverity[alloc_fn]

		__crt0(argc,argv,envp);

		//Call user(application)'s main
		int ret = 0;

		//coverity[leave_without_push]

		TRAPD(retVal, ret = CALLMAIN(argc, argv, envp));

		if(!ret)
			{
			ret = retVal;
			}
		// finish with atexit processing which terminates this process
		exit(ret);
		if (envp)
			{
			for (int i = 0; envp[i]; ++i)
				{
				User::Free(envp[i]);
				}
			User::Free(envp);
			}
		
		delete pCleanup;
		}
	else
		return KErrNoMemory;

	return(KErrNone);
	}


#if defined __GCC32__ && !defined __X86GCC__

/* stub function inserted into main() by GCC */
extern "C" void __gccmain (void) {}

/* Default GCC entrypoint */
extern "C" TInt _mainCRTStartup (void) 
    {
    extern TInt _E32Startup();
    return _E32Startup();
    }

#endif /* __GCC32__ */

#if defined __EABI__ || defined __X86GCC__

// standard entrypoint for C runtime, expected by some linkers
// Symbian OS does not currently use this function
extern "C" void __main() {}
#endif
