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


#include "omxxmltestrunner.h"
#include <test/testexecutelogger.h>

COmxXmlTestRunner* COmxXmlTestRunner::NewL(CTestExecuteLogger& aLogger)
	{
	COmxXmlTestRunner* self = new(ELeave) COmxXmlTestRunner(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

COmxXmlTestRunner::COmxXmlTestRunner(CTestExecuteLogger& aLogger):
iTestExecuteLogger(aLogger)
	{
	}

void COmxXmlTestRunner::ConstructL()
	{
	iScript = COmxXmlScript::NewL(*this);
	}

COmxXmlTestRunner::~COmxXmlTestRunner()
	{
	delete iFilename;
	delete iScript;
	}

void COmxXmlTestRunner::SetFilenameL(const TDesC& aFilename)
	{
	iFilename = aFilename.AllocL();
	}

void COmxXmlTestRunner::RunTestL(const TDesC& aSectionName)
	{
	// TODO check for memory leaks
	// TODO how to fail test block without causing E32USER-CBase 47 panic
	iScript->RunScriptL(*iFilename, aSectionName);
	}

void COmxXmlTestRunner::Log(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity, const TDes& aMessage)
	{
	// TEF Severity values are not the same as OMX Script
	TLogSeverity tefSeverity;
	switch(aSeverity)
		{
	case EOmxScriptSevErr:
		tefSeverity = ESevrErr;
		break;
	case EOmxScriptSevWarn:
		tefSeverity = ESevrWarn;
		break;
	case EOmxScriptSevInfo:
		tefSeverity = ESevrInfo;
		break;
	default:
		tefSeverity = ESevrAll;
		}
	
	iTestExecuteLogger.LogExtra(aFile, aLine, tefSeverity, aMessage);
	}
