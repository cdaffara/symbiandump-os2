// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tbackenddesblocks.cpp
// 
//

#include <e32svr.h>
#include "tbackenddes.h"
#include "sysif.h"
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdapis/netinet/in.h>
#include <stdapis/arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <semaphore.h>

#ifdef __EPOC32__
#define BACKEND_ORDINAL_NUMBER	7
#else
#define BACKEND_ORDINAL_NUMBER	6
#endif

//Utility functions
void CTestBackendDes::ReadIntParam(TInt &aInt)
{
	_LIT( Kparam, "Param%d" );
     TBuf<8> pNameBuf;
     TPtrC string;
     pNameBuf.Format(Kparam,++iParamCnt);
	 TBool res = GetIntFromConfig(ConfigSection(), pNameBuf, aInt);
	 if(!res)
	 	{
	 	_LIT(Kerr , "Unable to retrieve integer parameter") ;
	 	INFO_PRINTF1(Kerr);
	  	}
	 return;
}

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CTestLibcbackend::CLSIDesTest
// -----------------------------------------------------------------------------
//
TInt CTestBackendDes::CLSIDesTest()
    {
	int retVal = 0;
	RLibrary lBackendLib;
	TLibraryFunction backendFn = NULL;
	CLocalSystemInterface* lClsi = 0;
	lClsi = lClsi;
	typedef CLocalSystemInterface* (*fnBackend)();
	
	fnBackend backendFnActual = NULL;
	
	_LIT(KInfo, "Loading backend.dll") ;
	INFO_PRINTF1(KInfo);
	retVal = lBackendLib.Load(_L("backend.dll"));
	if(retVal!=KErrNone)
		{
		_LIT(Kerr , "Unable to load backend.dll") ;
		INFO_PRINTF1(Kerr);
		return retVal;
		}
	
	backendFn = lBackendLib.Lookup(BACKEND_ORDINAL_NUMBER);
	if(backendFn == NULL)
		{
		_LIT(Kerr , "Unable to lookup Backend()") ;
		INFO_PRINTF1(Kerr);
		}
	else
		{
		_LIT(KInfo4, "Invoking Backend()") ;
		INFO_PRINTF1(KInfo4);
		backendFnActual = (fnBackend)backendFn;
		lClsi = backendFnActual();
		}
	
	_LIT(KInfo2, "Closing backend.dll") ;
	INFO_PRINTF1(KInfo2);	
	lBackendLib.Close();
	_LIT(KInfo3, "backend.dll unloaded successfully") ;
	INFO_PRINTF1(KInfo3);

	return retVal;
    }
	
//  End of File
