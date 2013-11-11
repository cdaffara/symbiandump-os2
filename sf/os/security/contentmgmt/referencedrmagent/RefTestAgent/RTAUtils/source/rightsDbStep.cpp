/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* archivestep.cpp
*
*/


#include <f32file.h>
#include <test/testexecutelog.h>
#include <apmstd.h>
#include <caf/caf.h>
#include "rightsDbStep.h"
#include "rtarights.h"

using namespace ReferenceTestAgent;

CClearRightsDb::~CClearRightsDb()
	{
	}

CClearRightsDb::CClearRightsDb(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KClearRightsDb);
	}


TVerdict CClearRightsDb::doTestStepL()
	{
	SetTestStepResult(EFail);

	__UHEAP_MARK;	
	
	INFO_PRINTF1(_L("Clearing Rights Database"));

	RRtaRights rightsServer;
	User::LeaveIfError(rightsServer.Connect());
	CleanupClosePushL(rightsServer);
	
	rightsServer.ClearAllRightsL();
	
	CleanupStack::PopAndDestroy(&rightsServer);

	SetTestStepResult(EPass);

	__UHEAP_MARKEND;

	return TestStepResult();
	}	


CDumpRightsDb::~CDumpRightsDb()
	{
	}

CDumpRightsDb::CDumpRightsDb(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KDumpRightsDb);
	}


TVerdict CDumpRightsDb::doTestStepL()
	{
	SetTestStepResult(EFail);

	TPtrC fileName;
	GetStringFromConfig(ConfigSection(),_L("Filename"),fileName);

	__UHEAP_MARK;	
	
	INFO_PRINTF2(_L("Dumping rights database to file: %S "), &fileName);

	RRtaRights rightsServer;
	User::LeaveIfError(rightsServer.Connect());
	CleanupClosePushL(rightsServer);
	
	rightsServer.DumpRightsDatabaseL(fileName);
	
	CleanupStack::PopAndDestroy(&rightsServer);

	SetTestStepResult(EPass);

	__UHEAP_MARKEND;
	
	return TestStepResult();
	}	

//==================================
CLoadOldDb::~CLoadOldDb()
	{
	}

CLoadOldDb::CLoadOldDb(CRTAUtilsServer& aParent) : iParent(aParent)
	{
	SetTestStepName(KLoadOldDb);
	}

TVerdict CLoadOldDb::doTestStepL()
	{
	SetTestStepResult(EFail);

	TPtrC dbFileName;
	GetStringFromConfig(ConfigSection(),_L("DbFilename"), dbFileName);

	TPtrC txtDumpFileName;
	GetStringFromConfig(ConfigSection(),_L("TxtDumpFilename"), txtDumpFileName);

	TInt minDumpSize;
	GetIntFromConfig(ConfigSection(),_L("MinDumpSize"), minDumpSize);

	__UHEAP_MARK;	
	
	INFO_PRINTF2(_L("Reload rights database from file: %S "), &dbFileName);

	RRtaRights rightsServer;
	User::LeaveIfError(rightsServer.Connect());
	CleanupClosePushL(rightsServer);
	
	rightsServer.ReloadRightsDatabaseL(dbFileName);
	rightsServer.DumpRightsDatabaseL(txtDumpFileName);
	
	CleanupStack::PopAndDestroy(&rightsServer);

	// Check the size of the text dump. It should be 1657 bytes. We
	// consider the test pass if size is bigger than 1000 bytes.
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	TEntry entry;
	User::LeaveIfError(fs.Entry(txtDumpFileName, entry));
	CleanupStack::PopAndDestroy(&fs);

	TESTL(entry.iSize > minDumpSize);

	SetTestStepResult(EPass);

	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
