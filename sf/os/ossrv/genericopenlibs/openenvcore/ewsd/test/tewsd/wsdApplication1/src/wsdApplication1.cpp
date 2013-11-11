// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : wsdApplication1.cpp
// Part of     : wsdApplication1 EXE
// Contains the code for the wsdApplication1 EXE
//



#include "commonFramework.h"

_LIT(KStartingWsdApp1,"Starting wsdApplication1 process");
_LIT(KWsdApp2FileName, "wsdApplication2.exe");

// Execution function for wsdApplication1.exe
LOCAL_C void doExampleL()
    {
	console->Printf(KStartingWsdApp1);
	
	// Create a number of processes that each link to a number
	// of library DLLs that have WSD (e.g. libc).
	// Each process will call various library APIs that need
	// to use the WSD of that library
	for (TInt i = 1; i <= 10; ++i)
		{
		RProcess testProcess;
		TBuf<10> cmd;
		User::LeaveIfError(testProcess.Create(KWsdApp2FileName, cmd));
		testProcess.Resume();
		}
	}
