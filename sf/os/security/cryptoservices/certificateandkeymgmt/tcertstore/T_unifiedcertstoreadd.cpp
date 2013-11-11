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
*
*/


#include "t_unifiedcertstoreadd.h"
#include "t_certstoredefs.h"
#include "t_input.h"
#include "t_certstoreout.h"
#include <mctwritablecertstore.h>

CTestAction* CAddCertificate::NewL(RFs& aFs,
								   CConsoleBase& aConsole, 
								   Output& aOut, 
								   const TTestActionSpec& aTestActionSpec)
	{
	CAddCertificate* self = new(ELeave) CAddCertificate(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CAddCertificate::~CAddCertificate()
	{
	delete iCertificate;
	delete iCertificateURL;
	delete iCertificateContent;
	delete iCertificateLabel;
	}

void CAddCertificate::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EAdding:
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
			MCTWritableCertStore& store = UnifiedCertStore().WritableCertStore(iStoreIndex);
			
			TKeyIdentifier* issuerKeyId = &iIssuerKeyId;
			TKeyIdentifier* subjectKeyId = &iSubjectKeyId;
			
			ASSERT(iCertificateLabel);
			
			// Use the Add() with Deletable param if Deletable flag present in test data
			if (iDeletableFlagPresent)
				{
				store.Add(*iCertificateLabel, iCertificateFormat, iOwnerType,
						  subjectKeyId, issuerKeyId, *iCertificateContent, 
						  iDeletable, aStatus);
				}
			// otherwise, use the original Add()
			else 
				{
				store.Add(*iCertificateLabel, iCertificateFormat, iOwnerType,
						  subjectKeyId, issuerKeyId, *iCertificateContent, aStatus);
				}
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

            if (aStatus != KErrNoMemory)
                {
                iFinished = ETrue;
                }
            
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			}
			break;

		default:
			User::Invariant();
			break;
		}
	}

void CAddCertificate::PerformCancel()
	{
	switch (iState)
		{
	case ECheckNotification:
	case EFinished:	
		{
		MCTWritableCertStore& store = UnifiedCertStore().WritableCertStore(iStoreIndex);
		store.CancelAdd();
		break;
		}
	default:
		break;
		}
	}

void CAddCertificate::AfterOOMFailure()
	{
	}

void CAddCertificate::Reset()
	{
	iState = EAdding;
	}

void CAddCertificate::DoReportAction()
	{
	iOut.writeString(_L("Adding certificate..."));
	iOut.writeNewLine();
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(*iCertificateLabel);
	iOut.writeNewLine();
	iOut.writeString(_L("\tOwner type = "));
	WriteOwnerType();
	WriteFormat();
	iOut.writeString(_L("\tSubjectKeyId:  "));
	iOut.writeOctetString(iSubjectKeyId);
	iOut.writeNewLine();
	iOut.writeString(_L("\tDeletable = "));
	iDeletable ? iOut.writeString(KTrue) : iOut.writeString(KFalse);
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CAddCertificate::WriteFormat()
	{
	iOut.writeString(_L("\tFormat = "));
	switch (iCertificateFormat)
		{
		case EX509Certificate:
			iOut.writeString(_L("X.509\n"));
			break;
			
		case EWTLSCertificate:
			iOut.writeString(_L("WTLS\n"));
			break;
			
		case EX509CertificateUrl:
			iOut.writeString(_L("X.509 URL\n"));
			break;
			
		case EWTLSCertificateUrl:
			iOut.writeString(_L("WTLS URL\n"));
			break;
			
		default:
			iOut.writeString(_L("Unknown format\n"));
			break;
		}
	}

void CAddCertificate::WriteOwnerType()
	{
	switch (iOwnerType)
		{
		case ECACertificate:
			iOut.writeString(_L("CA\n"));
			break;
			
		case EUserCertificate:
			iOut.writeString(_L("User"));
			break;
			
		case EPeerCertificate:
			iOut.writeString(_L("Peer"));
			break;

		default:
			iOut.writeString(_L("Unknown"));
			break;
		}
	}
	
CAddCertificate::CAddCertificate(RFs& aFs, CConsoleBase& aConsole,
								 Output& aOut)
: CSubscriberAction(aFs, aConsole, aOut), iState(EAdding), 
  iDeletable(ETrue), iDeletableFlagPresent(EFalse)
	{
	}

void CAddCertificate::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CSubscriberAction::ConstructL(aTestActionSpec);

	SetCertFormatL(Input::ParseElement(aTestActionSpec.iActionBody, KCertFormatStart));
	SetCertOwnerTypeL(Input::ParseElement(aTestActionSpec.iActionBody, KCertOwnerTypeStart));
	SetCertLabelL(Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart));
	SetKeyId(iIssuerKeyId, Input::ParseElement(aTestActionSpec.iActionBody, KIssuerKeyStart));
	SetKeyId(iSubjectKeyId, Input::ParseElement(aTestActionSpec.iActionBody, KSubjectKeyStart));
	SetStoreToUse(Input::ParseElement(aTestActionSpec.iActionBody, KStoreToUseStart));

	TPtrC8 certFileOrURL = Input::ParseElement(aTestActionSpec.iActionBody, KCertFileStart);

	SetCertificateContentL(certFileOrURL);

	if (iCertificateFormat == EX509CertificateUrl ||
		iCertificateFormat == EWTLSCertificateUrl)
		{
		iCertificateURL = certFileOrURL.AllocL();
		}
	else
		{
		ConstructCertL(certFileOrURL);
		}
		
	// check for a possible deletable flag value for the certificate
	TInt err = KErrNone;
	TInt pos = 0;
	const TDesC8& deletableStr = Input::ParseElement(aTestActionSpec.iActionBody, 
														KDeletableStart,
														KDeletableEnd,
														pos,
														err);

	// set the deletable attribute if a value was found for the certificate
	if (err == KErrNone)
		{
		SetDeletable(deletableStr);
		}

	// Setting the expected result
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CAddCertificate::SetKeyId(TKeyIdentifier& aKeyIdentifier, const TDesC8& aKeyInfo)
	{
	TInt size = aKeyInfo.Length();
	for (TInt i = 0; i < size; i += 2)
		{
		TInt a = (aKeyInfo[i+1] >= 'a') ? (aKeyInfo[i+1] - 'a' + 10) : (aKeyInfo[i+1] - '0');
		TInt b = (aKeyInfo[i] >= 'a') ? (aKeyInfo[i] - 'a' + 10) : (aKeyInfo[i] - '0');
		aKeyIdentifier.Append(a  + b * 16);
		}
	}

void CAddCertificate::SetCertFormatL(const TDesC8& aFormat)
	{
	if (aFormat == KX509)
		{
		iCertificateFormat = EX509Certificate;
		}
	else if (aFormat == KWTLS)
		{
		iCertificateFormat = EWTLSCertificate;
		}
	else if (aFormat == KX509URL)
		{
		iCertificateFormat = EX509CertificateUrl;
		}
	else if (aFormat == KWTLSURL)
		{
		iCertificateFormat = EWTLSCertificateUrl;
		}
	else if (aFormat == KUnknown)
		{
		iCertificateFormat = EUnknownCertificate;
		}
	else
		{
		iOut.write(_L("Unknown cert format: "));
		iOut.writeString(aFormat);
		iOut.writeNewLine();		   
		User::Leave(KErrArgument);
		}
	}

void CAddCertificate::SetCertOwnerTypeL(const TDesC8& aOwnerType)
	{
	if (aOwnerType == KCACert)
		{
		iOwnerType = ECACertificate;
		}
	else if (aOwnerType == KUserCert)
		{
		iOwnerType = EUserCertificate;
		}
	else if (aOwnerType == KPeerCert)
		{
		iOwnerType = EPeerCertificate;
		}
	else if (aOwnerType == KUnknown)
		{
		// set dummy bogus owner type
		iOwnerType = static_cast<TCertificateOwnerType>(EPeerCertificate + 1);
		}
	else
		{
		iOut.write(_L("Unknown cert owner type: "));
		iOut.writeString(aOwnerType);
		iOut.writeNewLine();		   
		User::Leave(KErrArgument);
		}
	}

void CAddCertificate::SetCertLabelL(const TDesC8& aLabel)
	{
	delete iCertificateLabel;
	iCertificateLabel = NULL;
	iCertificateLabel = HBufC::NewL(aLabel.Length());
	TPtr ptr = iCertificateLabel->Des();
	ptr.Copy(aLabel);
	}

void CAddCertificate::SetStoreToUse(const TDesC8& aStoreToUse)
	{
	TLex8 lex(aStoreToUse);
	lex.Val(iStoreIndex);
	}

void CAddCertificate::SetCertificateContentL(const TDesC8& aFileName)
	{
	TFileName fileName;
	fileName.Copy(aFileName);
	RFs fs; 
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	__ASSERT_DEBUG(!iCertificateContent, User::Panic(_L("CAddCertificate"), 1));
	TRAPD(err, iCertificateContent = Input::ReadFileL(fileName, fs));
	if (err != KErrNone)
		{
		iConsole.Printf(_L("Error reading file : "));
		iConsole.Printf(fileName);
		iConsole.Printf(_L("\n"));
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy();	// fs
	}

void CAddCertificate::SetDeletable(const TDesC8& aDeletable)
	{
	iDeletableFlagPresent = ETrue;
	if (aDeletable.Compare(KTrue)==0)
		{
		iDeletable = ETrue;
		}
	else
		{
		iDeletable = EFalse;
		}
	}

void CAddCertificate::ConstructCertL(const TDesC8& aCert)
	{
	TFileName filename;
	filename.Copy(aCert);
	RFs fs; 
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	HBufC8* certBuf = 0;
	TRAPD(err, certBuf = Input::ReadFileL(filename, fs));
	if (err != KErrNone)
		{
		iConsole.Printf(_L("Error reading file : "));
		iConsole.Printf(filename);
		iConsole.Printf(_L("\n"));
		User::Leave(err);
		}
	CleanupStack::PushL(certBuf);
	switch (iCertificateFormat)
		{
		case EX509Certificate:
			iCertificate = CX509Certificate::NewL(*certBuf);
			break;
			
		case EWTLSCertificate:
			iCertificate = CWTLSCertificate::NewL(*certBuf);
			break;
			
		default:
			// Unknown format - do nothing
			break;
		}
	CleanupStack::PopAndDestroy(2);
	}

void CAddCertificate::DoCheckResult(TInt aError)
	{
	
	if (iFinished)
		{
		if (iResult )
			{
			if (iExpectedResult == KErrNone )
				{
				iConsole.Write(_L("\tcertificate added successfully\n"));
				iOut.writeString(_L("\tcertificate added successfully"));
				}
			else	
				{
				iConsole.Write(_L("\tcertificate not added.\n"));
				iOut.writeString(_L("\tcertificate not added."));
				}
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else if( !iResult)
			{
			if(iExpectedResult == KErrNone )
				{
				iConsole.Write(_L("\tcertificate not added\n"));
				iOut.writeString(_L("\tcertificate not added"));	
				}
			else
				{
				iConsole.Write(_L("\tcertificate should not be added\n"));
				iOut.writeString(_L("\tcertificate should not be added"));
				}
			iOut.writeNewLine();
			iOut.writeString(_L("\t"));
			iOut.writeError(aError);
			if (aError == KErrBadName)
				{
				iOut.writeString(_L(" Check that the label is unique"));
				}
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}	
	}
//////////////////////////////////////////////////////////
//	Key import, from keystore for adding user certificates
//////////////////////////////////////////////////////////

CTestAction* CImportKey::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CImportKey::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CImportKey::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CImportKey* self = new (ELeave) CImportKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CImportKey::~CImportKey()
{
	delete iLabel;
	delete iKeyData;
	if (iKeyInfo)
		{
		iKeyInfo->Release();
		}
	delete iUnifiedKeyStore;
	iFs.Close();
}

CImportKey::CImportKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertStoreTestAction(aFs, aConsole, aOut),
	iState(EInitialise)
{}


void CImportKey::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	User::LeaveIfError(iFs.Connect());

	CCertStoreTestAction::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;
	SetKeyDataFileL(Input::ParseElement(aTestActionSpec.iActionBody, KImportDataFile, KImportDataFileEnd, pos, err));
	for (;SetKeyUsage(Input::ParseElement(aTestActionSpec.iActionBody, KKeyUsageStart, KKeyUsageEnd, pos, err));)
		;
	
	SetKeyLabel(Input::ParseElement(aTestActionSpec.iActionBody, KKeyLabelStart, KKeyLabelEnd, pos, err));
	for (;SetKeyAccessType(Input::ParseElement(aTestActionSpec.iActionBody, KKeyAccessTypeStart, KKeyAccessTypeEnd, pos, err));)
		;
	
	SetKeyPassphrase(Input::ParseElement(aTestActionSpec.iActionBody, KKeyPassphraseStart, KKeyPassphraseEnd, pos, err));
		
	pos = 0;
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
}

TBool CImportKey::SetKeyUsage(const TDesC8& aKeyUsage)
	{
	TBool ret = ETrue;
	if (aKeyUsage.Compare(KAllKeyUsages)==0)
		iUsage = EPKCS15UsageAll;
	else if (aKeyUsage.Compare(KAllKeyUsagesButNR)==0)
		iUsage |= (TKeyUsagePKCS15)(EPKCS15UsageSign) |
			(TKeyUsagePKCS15)(EPKCS15UsageSignRecover) |
			(TKeyUsagePKCS15)(EPKCS15UsageDecrypt);
	else if (aKeyUsage.Compare(KDSAUsage)==0)
		iUsage |= (TKeyUsagePKCS15)(EPKCS15UsageSign) |
			(TKeyUsagePKCS15)(EPKCS15UsageSignRecover);
	else if (aKeyUsage.Compare(KDerive)==0)
		iUsage |= EPKCS15UsageDerive;
	else if (aKeyUsage.Compare(KSign)==0)
		iUsage |= EPKCS15UsageSign;
	else if (aKeyUsage.Compare(KSignRecover)==0)
		iUsage |= EPKCS15UsageSignRecover;
	else if (aKeyUsage.Compare(KDecrypt)==0)
		iUsage |= EPKCS15UsageDecrypt;
	else if (aKeyUsage.Compare(KNR)==0)
		iUsage |= EPKCS15UsageNonRepudiation;
	else if (aKeyUsage.Compare(KEncipherAndSign)==0)
		iUsage |= (TKeyUsagePKCS15)(EPKCS15UsageSign) |
			(TKeyUsagePKCS15)(EPKCS15UsageSignRecover) |
			(TKeyUsagePKCS15)(EPKCS15UsageUnwrap);
	else
		ret = EFalse;
	return ret;
	}

void CImportKey::SetKeyLabel(const TDesC8& aKeyLabel)
{
	iLabel = HBufC::NewMax(aKeyLabel.Size());
	if (iLabel)
	{
		TPtr theLabel(iLabel->Des());
		theLabel.FillZ();
		theLabel.Copy(aKeyLabel);
	}
}


void CImportKey::SetKeyPassphrase(const TDesC8& aPassphrase)
	{
	// If the passphrase is empty, then use "clanger" by default.
	_LIT8(KDefaultPassphrase, "clanger");
	TPtrC8 phrase(KDefaultPassphrase());
	if (aPassphrase.Length())
		{
		phrase.Set(aPassphrase);
		}


	RFs fs;
	RFile file;
	fs.Connect();
	
	// Write the passphrase straight to the file.
	TDriveUnit sysDrive (fs.GetSystemDrive());
	TBuf<24> fileName (sysDrive.Name());
	fileName.Append(_L("\\password.txt"));
	
	file.Replace(fs, fileName, EFileWrite);
	file.Write(phrase);
	file.Close();
	fs.Close();
	}

void CImportKey::SetKeyDataFileL(const TDesC8& aDes)
{	
//	Now the filename itself
	TFileName fileName;
	fileName.FillZ();
	fileName.Copy(aDes);
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> buf(sysDrive.Name());
	buf.Append(_L("\\tcertstore\\data\\"));
	buf.Append(fileName);
		
	RFile file;
	TInt r = file.Open(iFs, buf, EFileRead);
	if ( (r==KErrNotFound) || (r==KErrPathNotFound) )
	{//	Not on c:, try z:
		buf[0] = 'z';
		r = file.Open(iFs, buf, EFileRead);
	}

	User::LeaveIfError(r);

	CleanupClosePushL(file);

	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));

	if (fileSize > 0)
	{
		iKeyData = HBufC8::NewMaxL(fileSize);	
		TPtr8 data(iKeyData->Des());
		data.FillZ();
		User::LeaveIfError(file.Read(data, fileSize));
		CleanupStack::Pop(1);
	}

	file.Close();
}

TBool CImportKey::SetKeyAccessType(const TDesC8& aKeyAccessType)
	{
	TBool ret = ETrue;
	if (aKeyAccessType.Compare(KExtractable)==0)
		{
		iAccessType |= CCTKeyInfo::EExtractable;
		}
	else if (aKeyAccessType.Compare(KSensitive)==0)
		{
		iAccessType |= CCTKeyInfo::ESensitive;
		}
	else if (aKeyAccessType.Compare(KAlwaysSensitive)==0)
		{
		iAccessType |= CCTKeyInfo::EAlwaysSensitive;
		}
	else if (aKeyAccessType.Compare(KNeverExtractable)==0)
		{
		iAccessType |= CCTKeyInfo::ENeverExtractable;
		}
	else if (aKeyAccessType.Compare(KLocal)==0)
		{
		iAccessType |= CCTKeyInfo::ELocal;
		}
	else
		ret = EFalse;
	return ret;
	}

void CImportKey::PerformAction(TRequestStatus& aStatus)
{
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	switch (iState)
	{
	case EInitialise:
		{
		if (iKeyInfo != NULL)
			{
			iKeyInfo->Release();
			iKeyInfo = NULL;
			}

		// Delete t_secdlg files - this will then always answer "clanger" for the passphrase
		    
		TInt result;
		TBuf<24> datFile(sysDrive.Name());
		datFile.Append(_L("\\t_secdlg_in.dat"));
		result = iFs.Delete(datFile);
		
		if (result != KErrNone && result != KErrNotFound)
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, result);
			return;
			}
			
		datFile.Copy(sysDrive.Name());
		datFile.Append(_L("\\t_secdlg_out.dat"));
		result = iFs.Delete(datFile);
		
		if (result != KErrNone && result != KErrNotFound)
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, result);
			return;
			}
		
		TRAP(result, iUnifiedKeyStore = CUnifiedKeyStore::NewL(iFs));
		if ( (result==KErrNone) && (iUnifiedKeyStore) )
			{
				iUnifiedKeyStore->Initialize(aStatus);
				iState = EImportKey;
			}
		else
			{
				aStatus = result;
				iState = EFinished;
			}
		}
		break;
	case EImportKey:
		{
			if (KErrNone==aStatus.Int())
			{
			//	Currently uses the first store, change to check the script for a specific store
				iUnifiedKeyStore->ImportKey(0, iKeyData->Des(), iUsage, *iLabel, iAccessType,
											TTime(0), TTime(0), iKeyInfo, aStatus);			
			}
			else
				{
				// Errors get passed to next state
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				}

			iState = EFinished;
		}
		
		break;
		
		case EFinished:
		{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if ( (aStatus == iExpectedResult) || (aStatus==KErrAlreadyExists) )
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

void CImportKey::PerformCancel()
{//	To do when test harness cancel comes back.  Currently cancel testing
//	is performed in RunL with a set of flags and a separate active object
}

void CImportKey::Reset()
{}

void CImportKey::DoReportAction()
{
	_LIT(KImporting, "Importing key from keystore...");
	iOut.writeString(KImporting);
	TPtr theLabel(iLabel->Des());
	iOut.writeString(theLabel);
	iOut.writeNewLine();
}


void CImportKey::DoCheckResult(TInt aError)
{
	
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Key imported successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
		else
		{
			if ( (aError!=iExpectedResult) && (aError!=KErrAlreadyExists) )
			{
				_LIT(KFailed, "!!!Key import failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}
			
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
	}
}

