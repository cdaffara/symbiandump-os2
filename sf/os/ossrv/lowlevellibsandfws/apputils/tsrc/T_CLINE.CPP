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

#include <e32cons.h>
#include <bacline.h>

CConsoleBase* console;

void WriteArgsL()
	{
	CCommandLineArguments* args=CCommandLineArguments::NewLC();
	for (TInt i=0; i<args->Count(); i++)
		{
		TPtrC argumentPrt(args->Arg(i));
		console->Printf(_L("arg %d == %S\n"), i, &argumentPrt);
		}
	CleanupStack::PopAndDestroy();
	}

void TestReaderL()
	{
	console=Console::NewL(_L("Command-line arguments reader test"),TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);
	WriteArgsL();
	//The following two lines were removed to support the automated test for T_CLINEPARENT
	//console->Printf(_L("Press any key to exit:"));
	//console->Getch();
	CleanupStack::PopAndDestroy(console);
	}

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK; // mark heap state
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,TestReaderL()); // do most stuff under cleanup stack
	__ASSERT_ALWAYS(!error,User::Invariant());
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND; // check no memory leak
	return 0; // and return
    }

