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

#include "t_certstoreactionsclient.h"
#include "t_certstoredefs.h"
#include "t_certstoreout.h"
#include "t_input.h"

CTestAction* CInitCertificateAppInfoManager::NewL(RFs &aFs, 
												  CConsoleBase& aConsole, 
												  Output& aOut,
												  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}
	
CTestAction* CInitCertificateAppInfoManager::NewLC(RFs &aFs, 
												   CConsoleBase& aConsole, 
												   Output& aOut,
												   const TTestActionSpec& aTestActionSpec)	
	{
	CInitCertificateAppInfoManager* self = 
		new(ELeave) CInitCertificateAppInfoManager(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CInitCertificateAppInfoManager::~CInitCertificateAppInfoManager()	
	{
	}

void CInitCertificateAppInfoManager::PerformAction(TRequestStatus& aStatus)
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
		if (aStatus == KErrNoMemory)
			{	
			iState = EInit;
			}
		else
			{
			iFinished = ETrue;
			}
		}
		break;
		}
	}

void CInitCertificateAppInfoManager::PerformCancel()
	{
	}

void CInitCertificateAppInfoManager::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CInitCertificateAppInfoManager::Reset()"), 1));
	}

CInitCertificateAppInfoManager::CInitCertificateAppInfoManager(RFs &aFs, 
															   CConsoleBase& aConsole,
															   Output& aOut)

: CCertStoreTestAction(aFs, aConsole, aOut), iState(EInit), iFs(aFs)
	{
	}

void CInitCertificateAppInfoManager::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	
	// Set expected result
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CInitCertificateAppInfoManager::HandleEInit()
	{
	iState = EFinished;
	InitTheCertificateAppInfoManagerL();	
	}

void CInitCertificateAppInfoManager::DoReportAction()
	{
	iOut.writeString(_L("Creating CertificateAppInfoManager..."));
	iOut.writeNewLine();
	}

void CInitCertificateAppInfoManager::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iConsole.Write(_L("\tcertificate app info manager created successfully\n"));
			iOut.writeString(_L("\tcertificate app info manager created successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Write(_L("\tcertificate app info manager creation failed\n"));
			iOut.writeString(_L("\tcertificate app info manager creation failed"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

CTestAction* CDeleteCertificateAppInfoManager::NewL(RFs& aFs,
													CConsoleBase& aConsole, 
													Output& aOut,
													const TTestActionSpec& aTestActionSpec)
	{
	CDeleteCertificateAppInfoManager* self = 
		new(ELeave) CDeleteCertificateAppInfoManager(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CDeleteCertificateAppInfoManager::~CDeleteCertificateAppInfoManager()
	{
	}

CDeleteCertificateAppInfoManager::CDeleteCertificateAppInfoManager(RFs& aFs, CConsoleBase& aConsole,
																   Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut), iState(EDelete)
	{
	}

void CDeleteCertificateAppInfoManager::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CDeleteCertificateAppInfoManager::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EDelete:
			{
			DeleteTheCertificateAppInfoManager();
			iState = EFinished;
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, KErrNone);
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
			if (aStatus == KErrNoMemory)
				{	
				iState = EDelete;
				}
			else
				{
				iFinished = ETrue;
				}
			}
			break;
		}
	}

void CDeleteCertificateAppInfoManager::PerformCancel()
	{
	}

void CDeleteCertificateAppInfoManager::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CDeleteCertificateAppInfoManager::Reset()"), 1));
	}

void CDeleteCertificateAppInfoManager::DoReportAction()
	{
	iOut.writeString(_L("Deleting CertificateAppInfoManager..."));
	iOut.writeNewLine();
	}

void CDeleteCertificateAppInfoManager::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iConsole.Write(_L("\tcertificate app info manager deleted successfully\n"));
			iOut.writeString(_L("\tcertificate app info manager deleted successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Write(_L("\tcertificate app info manager deletion failed\n"));
			iOut.writeString(_L("\tcertificate app info manager deletion failed"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

CTestAction* CAddClient::NewL(RFs& aFs,
							  CConsoleBase& aConsole, 
							  Output& aOut,
							  const TTestActionSpec& aTestActionSpec)
	{
	CAddClient* self = new(ELeave) CAddClient(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CAddClient::~CAddClient()
	{
	delete iClientInfo;
	}

void CAddClient::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EAdd:
			{
			TInt err = KErrNotReady;
			CCertificateAppInfoManager* appManager = NULL;
			appManager = TheCertificateAppInfoManager();
			if (appManager)
			{
				TRAP(err, appManager->AddL(*iClientInfo));
			}
			
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, err);
			iState = EFinished;
			}
			break;

		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if ( (aStatus == iExpectedResult) || (aStatus == KErrAlreadyExists) )
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			if (aStatus == KErrNoMemory)
				{	
				iState = EAdd;
				}
			else
				{
				iFinished = ETrue;
				}
			}
			break;
		}
	}

void CAddClient::PerformCancel()
	{
	}

void CAddClient::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CAddClient::Reset()"), 1));
	}

CAddClient::CAddClient(RFs& aFs, CConsoleBase& aConsole, 
					   Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut), iState(EAdd)
	{
	}

void CAddClient::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	TInt err = KErrNone;
	TInt pos = 0;
	const TDesC8& name = Input::ParseElement(aTestActionSpec.iActionBody, KNameStart, KNameEnd,
		pos, err);
	const TDesC8& uid = Input::ParseElement(aTestActionSpec.iActionBody, KUIDStart, KUIDEnd,
		pos, err);
	TLex8 lex(uid);
	TUid u;
	lex.Val(u.iUid);
	TName n;
	n.Copy(name);
	iClientInfo = new(ELeave) TCertificateAppInfo(u, n);

	// Set expected result
	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CAddClient::DoReportAction()
	{
	iOut.writeString(_L("Adding client..."));
	iOut.writeNewLine();
	}

void CAddClient::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iConsole.Write(_L("\tClient added successfully\n"));
			iOut.writeString(_L("\tClient added successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Write(_L("\tClient added successfully\n"));
			iOut.writeString(_L("\tClient added successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}

CTestAction* CGetClients::NewL(RFs& aFs,
							   CConsoleBase& aConsole, 
							   Output& aOut,
							   const TTestActionSpec& aTestActionSpec)
	{
	CGetClients* self = new(ELeave) CGetClients(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CGetClients::~CGetClients()
	{
	// We own iExpectedClients but not iClients
	iExpectedClients.Close();
	}

void CGetClients::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EGet:
			{
			TRequestStatus* status = &aStatus;
			CCertificateAppInfoManager* appManager = NULL;
			appManager = TheCertificateAppInfoManager();
			if (appManager)
			{
				iClients = &(appManager->Applications());
				User::RequestComplete(status, KErrNone);
			}
			else
				User::RequestComplete(status, KErrNotReady);			
			
			iState = EFinished;
			}
			break;

		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
				{
				// Check that we have the expected uids
				TInt iEnd = iClients->Count();
				if (iEnd == iExpectedClients.Count())
					{
					TInt i = 0;
					for (i = 0; i < iEnd; i++)
						{
						if ((*iClients)[i].Id() != iExpectedClients[i])
							{
							break;
							}
						}
					if (i == iEnd)
						{
						iResult = ETrue;
						}
					else
						{
						iResult = EFalse;
						}
					}
				else
					{
					iResult = EFalse;
					}
				}
			else
				{
				iResult = EFalse;
				}
			if (aStatus == KErrNoMemory)
				{	
				iState = EGet;
				}
			else
				{
				iFinished = ETrue;
				}
			}
			break;
		}
	}

void CGetClients::PerformCancel()
	{
	}

void CGetClients::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CGetClients::Reset()"), 1));
	}

CGetClients::CGetClients(RFs& aFs, CConsoleBase& aConsole, 
						 Output& aOut)
	: CCertStoreTestAction(aFs, aConsole, aOut), 
	  iState(EGet)
	{
	}

void CGetClients::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	TInt err = KErrNone;
	TInt pos = 0;
	
	// Set expected result
	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	
	const TDesC8& uidsString = Input::ParseElement(aTestActionSpec.iActionResult, KUIDStart, KUIDEnd,
												   pos, err);
	TLex8 lex(uidsString);

	err = KErrNone;
	while (err == KErrNone)
		{
		TUid uid;
		err = lex.Val(uid.iUid);
		if (err == KErrNone)
			{	
			lex.SkipSpace();
			User::LeaveIfError(iExpectedClients.Append(uid));
			}
		}
	}

void CGetClients::DoReportAction()
	{
	iOut.writeString(_L("Getting client..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tExpected clients :"));
	TInt iEnd = iExpectedClients.Count();
	for (TInt i = 0; i < iEnd; i++)
		{
		iOut.writeString(_L(" "));
		iOut.writeNum(iExpectedClients[i].iUid);
		}
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CGetClients::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		iConsole.Printf(_L("\tClients:\n"));
		iOut.writeString(_L("\tClients: "));
		iOut.writeNewLine();
		iOut.writeString(_L("\t\t"));
		TInt count = iClients->Count();
		for (TInt i = 0; i < count; i++)
			{
			iConsole.Printf(_L("%D "), (*iClients)[i].Id());
			iOut.writeNum((*iClients)[i].Id().iUid);
			iOut.writeString(_L(" "));
			}
		iOut.writeNewLine();
		if (aError == KErrNone)
			{
			iConsole.Printf(_L("\tClients retrieved successfully\n"));
			iOut.writeString(_L("\tClients retrieved successfully"));
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Printf(_L("\tClients retrieved failed\n"));
			iOut.writeString(_L("\tClients retrieved failed"));
			iOut.writeNewLine();
			}
		iOut.writeNewLine();
		}
	}

CTestAction* CRemoveClient::NewL(RFs& aFs,
								 CConsoleBase& aConsole,
								 Output& aOut,
								 const TTestActionSpec& aTestActionSpec)
	{
	CRemoveClient* self = new(ELeave) CRemoveClient(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CRemoveClient::~CRemoveClient()
	{
	}

void CRemoveClient::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case ERemove:
			{
			TInt err = KErrNotReady;
			CCertificateAppInfoManager* appManager = NULL;
			appManager = TheCertificateAppInfoManager();
			if (appManager)
			{
				TRAP(err, appManager->RemoveL(iUid));
			}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, err);
			iState = EFinished;
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
			if (aStatus == KErrNoMemory)
				{	
				iState = ERemove;
				}
			else
				{
				iFinished = ETrue;
				}
			}
			break;
		}
	}

void CRemoveClient::PerformCancel()
	{
	}

void CRemoveClient::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CRemoveClient::Reset()"), 1));
	}

CRemoveClient::CRemoveClient(RFs& aFs, CConsoleBase& aConsole, 
							 Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut), iState(ERemove)
	{
	}

void CRemoveClient::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);
	TInt err = KErrNone;
	TInt pos = 0;
	const TDesC8& uid = Input::ParseElement(aTestActionSpec.iActionBody, KUIDStart, KUIDEnd,
		pos, err);
	TLex8 lex(uid);
	lex.Val(iUid.iUid);
	
	// Set expected result
	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CRemoveClient::DoReportAction()
	{
	iOut.writeString(_L("Removing client..."));
	iOut.writeNewLine();
	}

void CRemoveClient::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone)
			{
			iConsole.Write(_L("\tClient removed successfully\n"));
			iOut.writeString(_L("\tClient removed successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else if (aError == KErrNotFound)
			{
			iConsole.Write(_L("\tClient not found\n"));
			iOut.writeString(_L("\tClient not found"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}
	}
