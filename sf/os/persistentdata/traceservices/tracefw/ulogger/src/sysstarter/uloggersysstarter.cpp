// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "uloggersysstarter.h"

namespace Ulogger
{

//**********************************
// CULoggerSysStarter
//**********************************

TInt SysStart()
	{
	RULogger client;
	TInt err = KErrNone;
	err = client.Connect();
	if(err == KErrNone)
		err = client.Start();
	
	client.Close();

	// The System Starter is waiting for us (start_method = EWaitForStart)
	// so we rendez-vous so it can start the next process in the SSC
	RProcess::Rendezvous(KErrNone);
	return KErrNone;
	}

}//namespace


#ifndef __ULOGGER_SYSTEMSTART_UNIT_TEST
GLDEF_C TInt E32Main()
	{ 
	Ulogger::SysStart();
	return(KErrNone);
	}

#endif
