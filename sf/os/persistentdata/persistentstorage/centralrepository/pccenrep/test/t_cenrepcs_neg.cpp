// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <centralrepository.h>
#include <e32test.h>
#include <f32file.h>
#include "../../test/t_cenrep_helper.h"

using namespace NCentralRepositoryConstants;

_LIT(KCentralRepositoryServerName, "Centralrepositorysrv");

enum TestMode{
	ETxt,
	ECre,
	EAutoCre,
	EAutoTxt
};

RTest TheTest(_L("Central Repository Tests"));
RFs TheFs;

const TUid uid={0x000001ff};
const TUid uidcre={0x00001FFF};

_LIT(KPrivateTemplateCreOutFile,"c:\\private\\10202be9\\persists\\00001fff.cre");
_LIT(KPrivateTemplateTxtOutFile,"c:\\private\\10202be9\\persists\\000001ff.cre");

//DEFINED IN THE COMMON CPP FILE
extern TInt CopyFile(const TDesC& aSource, const TDesC& aTarget);
extern void NegativeTestingL(TUid aUid,const TDesC& aInFile,const TDesC& aOutFile,TUint aTestMode);

void SetupEnv(const TDesC& /**aInFilePath*/,const TDesC& aOutFilePath,TUint aTestMode)
	{
	if (aTestMode==EAutoCre || aTestMode==EAutoTxt)
		TheFs.Delete(aOutFilePath);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4044
@SYMTestCaseDesc		Client-Server-side CentralRepository negtive test
@SYMTestPriority		High
@SYMTestActions			Giving invalid input, checking error code returned.
@SYMTestExpectedResults Test must not fail
@SYMPREQ				PREQ2111
*/						   
LOCAL_C void MainL()
	{
	//Note these files are equivalent, the cre being generated from txt
	TheTest.Start(_L("Repository Negative testing"));
	
	KillProcess(KCentralRepositoryServerName);

	User::LeaveIfError(TheFs.Connect());

	//CRE exists only
	TheTest.Next(_L("CRE Negative Testing"));
	NegativeTestingL(uidcre,KNullDesC(),KPrivateTemplateCreOutFile(),EAutoCre);

	//TXT exists only
	TheTest.Next(_L("TXT Negative Testing"));	
	NegativeTestingL(uid,KNullDesC(),KPrivateTemplateTxtOutFile(),EAutoTxt);			

	TheFs.Close();
	TheTest.End();
	TheTest.Close();
	}

TInt E32Main()
	{	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;

	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);
	
	delete cleanup;
	__UHEAP_MARKEND;

	return 0;
	}
