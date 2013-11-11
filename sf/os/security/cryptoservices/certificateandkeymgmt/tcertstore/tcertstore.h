/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/
 
#ifndef __TNAMES_H
#define __TNAMES_H

//#include <e32test.h>
#include <e32cons.h>

// public
LOCAL_D CConsoleBase* console; // write all your messages to this
LOCAL_C void doExampleL(); // code this function for the real example
// private
LOCAL_C void callExampleL(); // initialize with cleanup stack, then do example

GLDEF_C TInt E32Main() // main function called by E32
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,callExampleL()); // more initialization, then do example

	__ASSERT_ALWAYS(!error,User::Panic(_L("CERT"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return 0; // and return
    }

LOCAL_C void callExampleL() // initialize and call example code under cleanup stack
    {
	console=Console::NewL(_L("Test code"),
		TSize(KDefaultConsWidth,KDefaultConsHeight));
	CleanupStack::PushL(console);
	TRAPD(error,doExampleL()); // perform example function
	if (error) console->Printf(_L("failed: leave code=%d"), error);
	else console->Printf(_L("ok"));
	console->Printf(_L(" [press any key]"));
	console->Getch(); // get and ignore character
	CleanupStack::PopAndDestroy(); // close console
    }

#endif
