// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EPOC32 version of crt0.c for pure C programs, adapted to cause the full Eikon
// environment to come into being and hence only used with WINS.
// 
//

#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>
#include <estlib.h>
#include <estw32.h>
#include <stdlib.h>

extern "C" int main (int argc, char *argv[], char *envp[]);

#ifndef EKA2
IMPORT_C void RegisterWsExe(const TDesC &aName);
#endif

GLDEF_C TInt E32Main()
	{     
	CTrapCleanup::New();

	RWin32Stream::StartServer();	// arrange for access to Win32 stdin/stdout/stderr
	SpawnPosixServerThread();	// arrange for multi-threaded operation

	int argc=0;
	char** argv=0;
	char** envp=0;

	__crt0(argc,argv,envp);			// get args & environment from somewhere

#ifndef EKA2
	// Cause the graphical Window Server to come into existence
	RSemaphore sem;
	sem.CreateGlobal(_L("WsExeSem"),0);
	RegisterWsExe(sem.FullName());
#endif

	int ret=main(argc, argv, envp);		// go

	// no need to explicitly delete the cleanup stack here as all memory used by
	// the process will be released by RProcess::Terminate(), called from inside exit().

	exit(ret);	// to get atexit processing, eventually terminates this process

	return(KErrNone);
	}
