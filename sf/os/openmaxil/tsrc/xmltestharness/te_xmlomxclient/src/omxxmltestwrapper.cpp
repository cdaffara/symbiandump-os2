/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include "omxxmltestwrapper.h"
#include "omxxmltestrunner.h"

// SCRIPT commands
_LIT(KNewCmd, "New");
_LIT(KRunTestCmd, "RunTest");

COmxXmlTestWrapper* COmxXmlTestWrapper::NewL()
	{
	COmxXmlTestWrapper* self = new(ELeave) COmxXmlTestWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

COmxXmlTestWrapper::COmxXmlTestWrapper()
	{
	
	}

void COmxXmlTestWrapper::ConstructL()
	{	
	iActiveCallback = CActiveCallback::NewL(*this);
	}

COmxXmlTestWrapper::~COmxXmlTestWrapper()
	{
	delete iTestRunner;
	delete iActiveCallback;
	}

TBool COmxXmlTestWrapper::DoCommandL(const TTEFFunction& aCommand,
		         				const TTEFSectionName& aSection,
		         				const TInt /*aAsyncErrorIndex*/)
	{
	TBool ret = ETrue;
	TInt err = KErrNone;
	if(aCommand == KNewCmd())
		{
		// XML file name is specified in aSection
		TRAP(err, DoNewCmdL(aSection));
		}
	else if(aCommand == KRunTestCmd())
		{
		// XML section name is specified in aSection
		TRAP(err, DoRunTestCmdL(aSection));
		}
	else
		{
		ERR_PRINTF2(_L("Unrecognized command %S"), &aCommand);
		ret = EFalse;
		}
	
	if(err != KErrNone)
	    {
        ret = EFalse;
	    }
	
	return ret;
	}

TAny* COmxXmlTestWrapper::GetObject()
	{
	return iTestRunner;
	}

void COmxXmlTestWrapper::DoNewCmdL(const TDesC& aFilename)
	{
	COmxXmlTestRunner* newRunner = COmxXmlTestRunner::NewL(Logger());
	CleanupStack::PushL(newRunner);
	newRunner->SetFilenameL(aFilename);
	CleanupStack::Pop(newRunner);
	delete iTestRunner;
	iTestRunner = newRunner;
	}

void COmxXmlTestWrapper::DoRunTestCmdL(const TDesC& aSectionName)
	{
	iTestRunner->RunTestL(aSectionName);
	}

void COmxXmlTestWrapper::RunL(CActive* aActive, TInt aIndex)
	{
	TInt err = aActive->iStatus.Int();
	SetAsyncError(aIndex, err);
	DecOutstanding();
	}
