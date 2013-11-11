// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// E32Main(), DoMainL() implementations
// 
//

#include <e32std.h>
#include <e32debug.h>
#include "CentRepConvTool.h"
#include "srvres.h"

//
// DoMainL() get command line args and pass them to CCentRepConv.
//
static void DoMainL()
	{
	TInt nPushed = 0;

	// create and install the active scheduler so that 
	// CacheManager can add itself to the scheduler.
	// *** but do not CActiveScheduler::Start() ***
	CActiveScheduler* s=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	nPushed++;
	CActiveScheduler::Install(s);

	TServerResources::InitialiseL(); // CentRep server static data
	CleanupStack::PushL(TCleanupItem(TCleanupOperation(&TServerResources::Close), NULL));
	nPushed++;

	HBufC* cmdLine = HBufC::NewL(User::CommandLineLength());
	CleanupStack::PushL(cmdLine);
	nPushed++;

	TPtr cmdLinePtr(cmdLine->Des());
	User::CommandLine(cmdLinePtr);

	CCentRepConvTool* convTool = CCentRepConvTool::NewL(*cmdLine,
		TServerResources::iFs, ETrue);
	CleanupStack::PushL(convTool);
	nPushed++;

	convTool->ProcessCmdL();

	CleanupStack::PopAndDestroy(nPushed);
	}

//
// The main function of CentRepConv tool which converts repository files
// from text format to binary and vice versa.
// Repositories in binary format load faster. CentRep clients are
// strongly encouraged to convert their text init files to binary
// to speed up access to their settings.
//
TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	__ASSERT_ALWAYS(trapCleanup != NULL, User::Invariant());
	
	TRAPD(err, ::DoMainL());

	if (err != KErrNone)
		{
		_LIT(KText, "CentRepConv tool failed. Error code = %d");
		RDebug::Print(KText, err);
		}

	delete trapCleanup;

	__UHEAP_MARKEND;

	return err;
    }
