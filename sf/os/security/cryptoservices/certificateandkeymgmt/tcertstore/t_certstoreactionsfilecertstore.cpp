/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_certstoreactionsfilecertstore.h"
#include "t_certstoredefs.h"
#include "t_input.h"

CTestAction* CCreateFileCertStore::NewL(RFs &aFs, 
										CConsoleBase& aConsole, 
										Output& aOut,
										const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}
	
CTestAction* CCreateFileCertStore::NewLC(RFs &aFs, 
										 CConsoleBase& aConsole, 
										 Output& aOut,
										 const TTestActionSpec& aTestActionSpec)	
	{
	CCreateFileCertStore* self = 
		new(ELeave) CCreateFileCertStore(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CCreateFileCertStore::~CCreateFileCertStore()	
	{
	delete iFileName;
	}

void CCreateFileCertStore::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
	case EInit:
		{
		TRAPD(err, HandleEInit());
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, err);
		}
		break;

	case EFinished:
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, aStatus.Int());
		if (aStatus == iExpectedResult)
			{
			iResult = ETrue;
			}
		else
			{
			iResult = EFalse;
			}
		iFinished = ETrue;
		}
		break;
		}
	}

void CCreateFileCertStore::PerformCancel()
	{
	}

void CCreateFileCertStore::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CCreateFileCertStore::Reset()"), 1));
	}

CCreateFileCertStore::CCreateFileCertStore(RFs &aFs, 
										   CConsoleBase& aConsole,
										   Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut), 
iState(EInit), iFs(aFs)
	{
	}

void CCreateFileCertStore::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 fileName = Input::ParseElement(aTestActionSpec.iActionBody, KFileNameStart, 
		KFileNameEnd, pos, err);
	iFileName = HBufC::NewL(fileName.Length());
	iFileName->Des().Copy(fileName);
	
	// Set expected result
	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CCreateFileCertStore::HandleEInit()
	{
	iState = EFinished;
	CreateStoreL(*iFileName, iFs);
	}

void CCreateFileCertStore::DoReportAction()
	{
	}

void CCreateFileCertStore::DoCheckResult(TInt /*aError*/)
	{
	}

void CCreateFileCertStore::CreateStoreL(const TDesC& aFileName, RFs& aFs)
	{
	aFs.MkDirAll(aFileName);
	RFile file;
	User::LeaveIfError(file.Replace(aFs, aFileName, EFileWrite));

	//TCleanupItem deleteFile(CFileCertStore::DeleteFile, this);//store will revert() if a leave occurs
	//CleanupStack::PushL(deleteFile);

	CPermanentFileStore* store = CPermanentFileStore::NewLC(file);
	store->SetTypeL(KPermanentFileStoreLayoutUid);	

	RStoreWriteStream caCertEntryStream;
	TStreamId caCertEntryStreamId = caCertEntryStream.CreateLC(*store);
	caCertEntryStream.WriteInt32L(0);//we have zero ca certs
	caCertEntryStream.CommitL();
	CleanupStack::PopAndDestroy();

	RStoreWriteStream rootStream;
	TStreamId rootId = rootStream.CreateLC(*store);

	rootStream << caCertEntryStreamId;
	rootStream.CommitL();
	CleanupStack::PopAndDestroy();
	
	store->SetRootL(rootId);							
	store->CommitL();							
	CleanupStack::PopAndDestroy();//store
	}


