/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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




/**
 @file 
*/
#ifndef __TESTEXECUTEINTERFACE_H__
#define __TESTEXECUTEINTERFACE_H__

#include <e32cons.h>

struct TScriptTests;

namespace TestExecuteInterface
	{
	// The test code's WinsMain() should call this to kick off
	// the server thread for wins/eka1. Usage is:
	// EXPORT_C TInt WinsMain()
	//     {
	//     return TTestExecuteInterface::StartWinsEka1Thread(E32Main);
	//     }
	IMPORT_C TInt StartWinsEka1Thread(TInt (*aE32Main)(), 
		const TDesC& aServerName);

	// Client calls this to perform the tests instead of
	// CTestSetup::CreateAndRunTestsL	
	IMPORT_C void StartWithTestExecuteL(TScriptTests theTestTypes[],
		const TDesC& aServerName);
	}

#endif
