// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include "D32Assert.h"
#include "cn_main.h"
#include "cn_cmdparse.h"
#include "cn_util.h"
#include "cn_proc.h"

/**
It will be used for parsed command line arguments.
@internalComponent
*/
static TCmdLinePrm TheCmdLinePrm;

/**
DoMainL() parses the command line, fills TheCmdLinePrm struct, creates the appropriate
command line execution object and calls its RunL() method.
@internalComponent
*/
static void DoMainL()
	{
	RFs fileSession;
	__LEAVE_IF_ERROR(fileSession.Connect());
	CleanupClosePushL(fileSession);

	CCommandLineArguments* cmdLineParser = CCommandLineArguments::NewLC();

	TSPConvUtil::ParseL(*cmdLineParser, TheCmdLinePrm);
	TSPConvUtil::ConstructBinFileNameL(TheCmdLinePrm.iBinFile);

	CCmdProcessor* proc = TCmdProcessorFactory::NewLC(fileSession, TheCmdLinePrm);
	proc->RunL();

	CleanupStack::PopAndDestroy(3, &fileSession);//cmdLineParser, fileSession, proc
	}

/**
The main function of DbSpConv tool, which can be used to load and parse a text policy file
and store it in a binary policy file, which can be used by the DBMS server.
*/
TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	__ASSERT_ALWAYS(trapCleanup != NULL, User::Invariant());
	
	TRAPD(err, ::DoMainL());

	if(err != KErrNone)
		{
		_LIT(KText, "Security policy tool failed. Error code = %d\n");
		TSPConvUtil::Print(KText, err);
		}
	else
		{
		_LIT(KText, "\"%S\"->\"%S\" : OK!\n");
		TSPConvUtil::Print(KText, TheCmdLinePrm.iTxtFile, TheCmdLinePrm.iBinFile);
		}

	delete trapCleanup;

	__UHEAP_MARKEND;

	return err;
    }
