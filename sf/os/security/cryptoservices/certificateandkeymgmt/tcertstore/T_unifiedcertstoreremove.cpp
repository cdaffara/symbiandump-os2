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

#include "t_unifiedcertstoreremove.h"
#include "t_certstoredefs.h"
#include "t_input.h"
#include "t_certstoreout.h"
#include <ccertattributefilter.h>


/////////////////////////////////////////////////////////////////////////////////
//CDeleteCertificate
/////////////////////////////////////////////////////////////////////////////////


CTestAction* CDeleteCertificate::NewL(RFs& aFs,
										CConsoleBase& aConsole, 
										Output& aOut,
										const TTestActionSpec& aTestActionSpec)
	{
	CDeleteCertificate* self = new(ELeave) CDeleteCertificate(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CDeleteCertificate::CDeleteCertificate(RFs& aFs, CConsoleBase& aConsole, 
									   Output& aOut)
: CSubscriberAction(aFs, aConsole, aOut), iState(EIdle),iCertDeleteType(NULL)
	{
	}

void CDeleteCertificate::PerformAction(TRequestStatus& aStatus)
	{
	if (aStatus != KErrNone)
		{
		iState = EFinished;
		}
	
	switch (iState)
		{
		case EIdle:
			//get the certs
			if (iCertInfos.Count() > 0)
				{
				iCertInfos.Close();	//	JCS added for multiple delete case
				}

			iState = EGettingCerts;
			//get the certs
			GetCerts(aStatus);
			break;
			
		case EGettingCerts:
			{
 			if (iNotificationSubscribed)
 				{
 				if (!iNotifier)
 					{
	 				iNotifier = CCertStoreChangeNotifier::NewL(iNotifierFlag);
 					iNotifier->StartNotification();			
 					}
 				iState = ECheckNotification;
 				}
 			else
 				{
 				iState = EFinished;
 				}
			DeleteCert(aStatus);
			}
			break;

 		case ECheckNotification:
 			{
 			iState = EFinished;
 			if (iNotifierFlag)
	 			{
 				TRequestStatus* status = &aStatus;
 				User::RequestComplete(status, KErrNone);
	 			}
			else
				{
				iNotifier->SetCompleteStatus(&aStatus);
				}			
 			}

 			break;

		case EFinished:
			{
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
				iState = EIdle;
				}
			else
				{
				iFinished = ETrue;
				}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			}
			break;
			
		default:
			User::Invariant();
		}
	}

void CDeleteCertificate::PerformCancel()
	{
	switch (iState)
		{
		case EGettingCerts:
			CertStore().CancelList();
			break;
		case ECheckNotification:
		case EIdle:
		case EFinished:
			// We /may/ be deleting a cert in these states - fortunately cancel is lenient
			UnifiedCertStore().CancelRemove();
			break;

		default:
			User::Invariant();
		}
	}

void CDeleteCertificate::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CSubscriberAction::ConstructL(aTestActionSpec);

	iFilter = CCertAttributeFilter::NewL();
	TInt pos = 0;
	TInt err1 = KErrNone;
	
	TPtrC8 pLabel1 = Input::ParseElement(aTestActionSpec.iActionBody, KCertTypeStart, KCertTypeEnd, pos, err1);
	
	if(err1 == KErrNone)
		{
		iCertDeleteType = HBufC8::NewL(pLabel1.Size());
		*iCertDeleteType = pLabel1;
		}
		
	pos=0;
	TInt err2 = KErrNone;
	TPtrC8 pLabel2 = Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart, KCertLabelEnd, pos, err2);
	if (err2 != KErrNone)
		{
		User::Leave(err2);
		}
	
	iCertLabel.Copy(pLabel2);
	if (iCertLabel.Compare(_L("*.*"))==0)	//	delete all certs
	{//	Check that the flag has been set
		TPtrC8 pDeleteAll = Input::ParseElement(aTestActionSpec.iActionBody, KDeleteAllStart, KDeleteAllEnd, pos, err1);
		if (pDeleteAll!=KNullDesC8)
		{
			TLex8 lexer(pDeleteAll);
			TInt boolVal = 0;
			lexer.Val(boolVal);
			iDeleteAllCerts = (boolVal > 0) ? ETrue : EFalse;
		}
	}
	else if( err1 != KErrNone && err1 != KErrNotFound)
		{
		User::Leave(err1);
		}

	// Set expected result
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CDeleteCertificate::GetCerts(TRequestStatus& aStatus)
	{
	CertStore().List(iCertInfos, *iFilter, aStatus);
	}

CDeleteCertificate::~CDeleteCertificate()
	{
	delete iCertDeleteType;
	iCertInfos.Close();
	delete iFilter;
	}

void CDeleteCertificate::DeleteCert(TRequestStatus& aStatus)
	{
	TInt ix = KErrNotFound;
	TInt count = iCertInfos.Count();
	for (TInt i = count-1; i >= 0 ; i--)
		{
		if (iDeleteAllCerts)
			{//	Just delete the first one we come across
			if (i>=0)					//	Still some certificates to delete
				{							//	Set the state back to continue
				iState = EIdle;	
				}

			ix = i;
			break;
			}
		//	Only delete if there's a label match and the certificate is of specified type
		else if (iCertInfos[i]->Label() == iCertLabel && CheckCertType(*iCertInfos[i]) )
			{
			ix = i;
			break;
			}
		}
	
	TRequestStatus* status = &aStatus;
	if (ix == KErrNotFound)
		{
		TInt result = KErrNone;
		
		if(iDeleteAllCerts) 
			result = KErrNone; 
		else if(iCertDeleteType == NULL)
			result = KErrNotSupported;
		else
			result = KErrNotFound;
		
		User::RequestComplete(status, result);
		}	
	else
		{
		iCertInfoToDelete = iCertInfos[ix];
		UnifiedCertStore().Remove(*iCertInfos[ix], aStatus);
		}
	}

TBool CDeleteCertificate::CheckCertType( CCTCertInfo& aCertInfo )
	{
	if(iCertDeleteType == NULL)
		return EFalse;
	
	TBool returnValue = EFalse;
	switch(aCertInfo.CertificateOwnerType())
		{
		case ECACertificate:
			if(iCertDeleteType->Compare(KCACert) == 0 )
				returnValue = ETrue;
			break;
		case EUserCertificate:
			if(iCertDeleteType->Compare(KUserCert) == 0 )
				returnValue = ETrue;
			break;
		case EPeerCertificate:
			if(iCertDeleteType->Compare(KPeerCert) == 0 )
				returnValue = ETrue;
			break;
		}
	return returnValue;
	}

void CDeleteCertificate::DoReportAction()
	{
	iOut.writeString(_L("Deleting certificate..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(iCertLabel);
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CDeleteCertificate::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone && iCertInfoToDelete)
			{
			switch (iCertInfoToDelete->CertificateOwnerType())
				{
			case ECACertificate:
				iConsole.Write(_L("\tCA certificate deleted successfully\n"));
				iOut.writeString(_L("\tCA certificate deleted successfully"));
				break;

			case EUserCertificate:
				iConsole.Write(_L("\tUser certificate deleted successfully\n"));
				iOut.writeString(_L("\tUser certificate deleted successfully"));
				break;
			
			case EPeerCertificate:
				iConsole.Write(_L("\tPeer certificate deleted successfully\n"));
				iOut.writeString(_L("\tPeer certificate deleted successfully"));
				break;
			default:
				iConsole.Write(_L("\tUnknown owner certificate deleted successfully\n"));
				iOut.writeString(_L("\tUnknown owner certificate deleted successfully"));
				break;
				}
			}
		iOut.writeNewLine();
		iOut.writeNewLine();
		}
	}

void CDeleteCertificate::Reset()
	{
	iState = EIdle;
	iCertInfos.Close();
	}


/////////////////////////////////////////////////////////////////////////////////
//CDeleteRetrieveCertificate
/////////////////////////////////////////////////////////////////////////////////
CDeleteRetrieveCertificate::CDeleteRetrieveCertificate(RFs& aFs, CConsoleBase& aConsole, 
													   Output& aOut)
: CSubscriberAction(aFs, aConsole, aOut), iState(EIdle)
	{
	}

void CDeleteRetrieveCertificate::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	switch (iState)
		{
		case EIdle:
			//get the certs
			iState = EDeletingCert;
			//get the certs
			GetCerts(aStatus);
			if (aStatus == KErrNoMemory)
				{
				iState = EFinished;
				}
			break;

		case EDeletingCert:
			{
 			if (iNotificationSubscribed)
 				{
 				iNotifier = CCertStoreChangeNotifier::NewL(iNotifierFlag);
 				iNotifier->StartNotification();
 				iState = ECheckNotification;
 				}
 			else
 				{
 				iState = EGettingCerts;
 				}
			//delete the cert
			DeleteCert(aStatus);
			break;
			}

 		case ECheckNotification:
 			{
 			TInt ret=KErrNone;
 			if (!iNotifierFlag)
 				{
 				ret=KErrGeneral;
 				}
 			if (ret!=KErrNone)
 				{
 				iState = EFinished;	
 				}
 			else
 				{
 				iState = EGettingCerts;
 				}
 			TRequestStatus* status = &aStatus;
 			User::RequestComplete(status, ret);
 			}
 			break;
		case EGettingCerts:
			{
			iState = EFinished;
			//get the cert
			RetrieveCerts(aStatus);
			break;
			}
		case EFinished:
			{
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
				iState = EIdle;
				}
			else
				{
				iFinished = ETrue;
				}
			User::RequestComplete(status, aStatus.Int());
			}
			break;
		default:
			{
			//some kind of error behaviour
			}
		}
	}

void CDeleteRetrieveCertificate::PerformCancel()
	{
	}

void CDeleteRetrieveCertificate::Reset()
	{
	__ASSERT_DEBUG(EFalse, User::Panic(_L("CDeleteRetrieveCertificate::Reset()"), 1));
	}

////////////////////////////////////////////////////////////////////////////////
//CDeleteCACertificate
//////////////////////////////////////////////////////////////////////////////////

CTestAction* CDeleteRetrieveCACertificate::NewL(RFs& aFs,
												CConsoleBase& aConsole, 
												Output& aOut,
												const TTestActionSpec& aTestActionSpec)
	{
	CDeleteRetrieveCACertificate* self = 
		new(ELeave) CDeleteRetrieveCACertificate(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CDeleteRetrieveCACertificate::CDeleteRetrieveCACertificate(RFs& aFs, CConsoleBase& aConsole, 
														   Output& aOut)
	:CDeleteRetrieveCertificate(aFs, aConsole, aOut)
	{
	}

void CDeleteRetrieveCACertificate::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CSubscriberAction::ConstructL(aTestActionSpec);
	iCertlisted = new(ELeave) RPointerArray<HBufC8>;
	iFilter = CCertAttributeFilter::NewL();
	TInt pos = 0;
	TInt err = KErrNone;
	TPtrC8 pLabel = Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart, KCertLabelEnd, pos, err);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	iCertLabel.Copy(pLabel);

	// Set expected result
	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CDeleteRetrieveCACertificate::GetCerts(TRequestStatus& aStatus)
	{
	CertStore().List(iCertInfos, *iFilter, aStatus);
	}

void CDeleteRetrieveCACertificate::RetrieveCerts(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt ix = KErrNotFound;
	TInt count = iCertInfos.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iCertInfos[i]->Label() == iCertLabel)
			{
			ix = i;
			break;
			}
		}
	if (ix == KErrNotFound)
		{
		User::RequestComplete(status, ix);
		}	
	else
		{
		iCertInfoToRetrieve = iCertInfos[ix];

		HBufC8* buf = HBufC8::NewLC(iCertInfoToRetrieve->Size());
		User::LeaveIfError(iCertlisted->Append(buf));
		CleanupStack::Pop();//buf
		__ASSERT_DEBUG(!iBuf, User::Panic(_L("CListCACertificates"), 1));
		iBuf = new(ELeave) TPtr8(buf->Des());
		CertStore().Retrieve(*iCertInfoToRetrieve, *iBuf, aStatus);
		}
	}

CDeleteRetrieveCACertificate::~CDeleteRetrieveCACertificate()
	{
	iCertInfos.Close();
	delete iFilter;
	if (iCertlisted)
		{
		iCertlisted->ResetAndDestroy();
		delete iCertlisted;
		}
	delete iBuf;
	}
	
void CDeleteRetrieveCACertificate::DeleteCert(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt ix = KErrNotFound;
	TInt count = iCertInfos.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iCertInfos[i]->Label() == iCertLabel)
			{
			ix = i;
			break;
			}
		}
	if (ix == KErrNotFound)
		{
		User::RequestComplete(status, ix);
		}	
	else
		{
		iCertInfoToDelete = iCertInfos[ix];
		UnifiedCertStore().Remove(*iCertInfoToDelete, aStatus);
		}
	}

void CDeleteRetrieveCACertificate::DoReportAction()
	{
	iOut.writeString(_L("Deleting certificate..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(iCertLabel);
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CDeleteRetrieveCACertificate::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone && iResult)
			{
			switch (iCertInfoToDelete->CertificateOwnerType())
				{
			case ECACertificate:
				iConsole.Write(_L("\tCA certificate deleted successfully\n"));
				iOut.writeString(_L("\tCA certificate deleted successfully"));
				break;

			case EUserCertificate:
				iConsole.Write(_L("\tUser certificate deleted successfully\n"));
				iOut.writeString(_L("\tUser certificate deleted successfully"));
				break;

			default:
				iConsole.Write(_L("\tUnknown owner certificate deleted successfully\n"));
				iOut.writeString(_L("\tUnknown owner certificate deleted successfully"));
				break;
				}
			}
		else
			{
			if (iResult)
				{
				iConsole.Write(_L("\tCA OK\n"));
				iOut.writeString(_L("\tCA OK"));
				}
			}
		iOut.writeNewLine();
		iOut.writeNewLine();
		}
	}

////////////////////////////////////////////////////////////////////////////////
//CDeleteUserCertificate
//////////////////////////////////////////////////////////////////////////////////
CTestAction* CDeleteRetrieveUserCertificate::NewL(RFs& aFs, 
												  CConsoleBase& aConsole,
												  Output& aOut, 
												  const TTestActionSpec& aTestActionSpec)
	{
	CDeleteRetrieveUserCertificate* self = 
		new(ELeave) CDeleteRetrieveUserCertificate(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CDeleteRetrieveUserCertificate::CDeleteRetrieveUserCertificate(RFs& aFs, CConsoleBase& aConsole, 
															   Output& aOut)
	:CDeleteRetrieveCertificate(aFs, aConsole, aOut)
	{
	}

void CDeleteRetrieveUserCertificate::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CSubscriberAction::ConstructL(aTestActionSpec);
	iCertlisted = new(ELeave) RPointerArray<HBufC8>;
	iFilter = CCertAttributeFilter::NewL();
	TInt pos = 0;
	TInt err = KErrNone;
	TPtrC8 pLabel = Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart, KCertLabelEnd, pos, err);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	iCertLabel.Copy(pLabel);

	// Set expected result
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CDeleteRetrieveUserCertificate::GetCerts(TRequestStatus& aStatus)
	{
	CertStore().List(iCertInfos, *iFilter, aStatus);
	}

void CDeleteRetrieveUserCertificate::RetrieveCerts(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt ix = KErrNotFound;
	TInt count = iCertInfos.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iCertInfos[i]->Label() == iCertLabel)
			{
			ix = i;
			break;
			}
		}
	if (ix == KErrNotFound)
		{
		User::RequestComplete(status, ix);
		}	
	else
		{
		iCertInfoToRetrieve = iCertInfos[ix];

		HBufC8* buf = HBufC8::NewLC(iCertInfoToRetrieve->Size());
		User::LeaveIfError(iCertlisted->Append(buf));
		CleanupStack::Pop();//buf
		//__ASSERT_DEBUG(!iBuf, User::Panic(_L("CListCACertificates"), 1));
		iBuf = new(ELeave) TPtr8(buf->Des());
		CertStore().Retrieve(*iCertInfoToRetrieve, *iBuf, aStatus);
		}
	}

CDeleteRetrieveUserCertificate::~CDeleteRetrieveUserCertificate()
	{
	iCertInfos.Close();
	delete iFilter;
	if (iCertlisted)
		{
		iCertlisted->ResetAndDestroy();
		delete iCertlisted;
		}
	delete iBuf;
	}
	
void CDeleteRetrieveUserCertificate::DeleteCert(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	TInt ix = KErrNotFound;
	TInt count = iCertInfos.Count();
	for (TInt i = 0; i < count; i++)
		{
		if (iCertInfos[i]->Label() == iCertLabel)
			{
			ix = i;
			break;
			}
		}
	if (ix == KErrNotFound)
		{
		User::RequestComplete(status, ix);
		}	
	else
		{
		CCTCertInfo* certInfo = iCertInfos[ix];
		UnifiedCertStore().Remove(*certInfo, aStatus);
		}
	}

void CDeleteRetrieveUserCertificate::DoReportAction()
	{
	iOut.writeString(_L("Deleting user certificate..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(iCertLabel);
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CDeleteRetrieveUserCertificate::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (aError == KErrNone && iResult)
			{
			iConsole.Write(_L("\tuser cert deleted successfully\n"));
			iOut.writeString(_L("\tuser cert deleted successfully"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else
			{
			if (iResult)
				{
				iConsole.Write(_L("\tnot retrieved but expected\n"));
				iOut.writeString(_L("\tCA OK"));
				}
			else
				{
				iConsole.Write(_L("\tuser cert deleted successfully\n"));
				iOut.writeString(_L("\tuser cert deleted successfully"));
				iOut.writeNewLine();
				iOut.writeNewLine();
				}
			}
		}
	}
