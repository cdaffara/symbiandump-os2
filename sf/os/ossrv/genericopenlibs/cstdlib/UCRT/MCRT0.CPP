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

GLDEF_C TInt E32Main()
	{     
	CTrapCleanup::New();

	SpawnPosixServerThread();		// arrange for multi-threaded operation

	int argc=0;
	char ** argv=0;
	char ** envp=0;


	__crt0(argc,argv,envp);			// get args & environment from somewhere

	int ret=CALLMAIN(argc, argv, envp);		// go

	// no need to explicitly delete the cleanup stack here as all memory used by
	// the process will be released by RProcess::Terminate(), called from inside exit().

	exit(ret);				// finish with atexit processing

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

// X86GCC uses def files derived from EABI
#if (defined (__EABI__)) || (defined (__X86GCC__))

// standard entrypoint for C runtime, expected by some linkers
// Symbian OS does not currently use this function
extern "C" void __main() {}
#endif
