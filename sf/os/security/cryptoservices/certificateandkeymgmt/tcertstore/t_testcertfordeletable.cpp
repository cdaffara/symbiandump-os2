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


#include "t_testcertfordeletable.h"
#include "t_certstoredefs.h"
#include "t_input.h"
#include "t_certstoreout.h"

#include "t_WritableCSDummies.h"


CTestAction* CTestCertForDeletable::NewL(RFs& aFs,
								   CConsoleBase& aConsole, 
								   Output& aOut, 
								   const TTestActionSpec& aTestActionSpec)
	{
	CTestCertForDeletable* self = new(ELeave) CTestCertForDeletable(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestCertForDeletable::~CTestCertForDeletable()
	{
	delete iCertificate;
	delete iCertificateURL;
	delete iCertificateContent;
	delete iCertificateLabel;
	}

void CTestCertForDeletable::TestSerializationL(	MCTToken& aToken,
												TKeyIdentifier* aIssuerKeyId,
												TKeyIdentifier* aSubjectKeyId, 
												const TInt aCertificateId,
												const TDesC8* aIssuerHash )
	{
	CCTCertInfo* cert = CCTCertInfo::NewLC( *iCertificateLabel,	// const TDesC& 
											iCertificateFormat,	// TCertificateFormat
											iOwnerType, 		// TCertificateOwnerType
											999, 				// TInt aSize | aCert.Length()
											aSubjectKeyId,		// const TKeyIdentifier*
											aIssuerKeyId,		// const TKeyIdentifier* 
											aToken,	 			// MCTToken&
											aCertificateId,		// TInt aCertificateId
											iDeletable,			// TBool
											aIssuerHash );		// const TDesC8* aIssuerHash = NULL	
																
													// cert -> stack
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);							// . fs -> stack

	// this is a name of the file that will be opened as 
	// a stream to test externalize/internalize
	TDriveUnit sysDrive (fs.GetSystemDrive());
	TBuf<24> fileName (sysDrive.Name());
	fileName.Append(_L("\\certinfo.dat"));

	RFileWriteStream write_stream;
	User::LeaveIfError( write_stream.Replace(fs, fileName, EFileWrite) );
	CleanupClosePushL(write_stream);				// . . write_stream -> stack

	cert->ExternalizeL(write_stream);						
	CleanupStack::PopAndDestroy();					// . . write_stream <- pop
		
	// now internalize back from stream
	RFileReadStream read_stream;
	User::LeaveIfError( read_stream.Open(fs, fileName, EFileRead) );
	CleanupClosePushL(read_stream);					// . . read_stream -> stack

	CCTCertInfo* cert_read =
					CCTCertInfo::NewLC(read_stream, aToken); // does InternalizeL()
													// . . . cert_read -> stack
	// log certinfo params just internalized
	iOut.writeString(_L("Cert attributes after internalize:"));
	iOut.writeNewLine();
	WriteFormat( cert_read->CertificateFormat() );
	iOut.writeString(_L("\tiDeletable  = "));
	iDeletable ? iOut.writeString(KTrue) : iOut.writeString(KFalse);
	iOut.writeNewLine();
	iOut.writeNewLine();
			
	// set result
	iResultGood = ETrue; 
	// check deletable flag is persistent
	if ( cert_read->IsDeletable() != iDeletable )
		{
		iResultGood = EFalse; 
		}
	// check format is persistent
	if ( cert_read->CertificateFormat() != iCertificateFormat )
		{
		iResultGood = EFalse; 
		}			
	
	// cleanup
	CleanupStack::PopAndDestroy(cert_read);			// . . . cert_read <- pop
	CleanupStack::PopAndDestroy();					// . . read_stream <- pop
	CleanupStack::PopAndDestroy(&fs); 				// . fs <- pop
	CleanupStack::PopAndDestroy(cert);				// initial certinfo <- pop
	}

void CTestCertForDeletable::PerformAction(TRequestStatus& aStatus)
	{
// 	COMMON PART
	MCTWritableCertStore& ustore = 
			UnifiedCertStore().WritableCertStore(iStoreIndex);
	MCTToken& token = ustore.Token();

	TKeyIdentifier* issuerKeyId = NULL;
	TKeyIdentifier* subjectKeyId = NULL;

	if (iOwnerType != ECACertificate)
		{
		issuerKeyId = & iIssuerKeyId;
		subjectKeyId = & iSubjectKeyId;
		}

	const TInt KCertificateId = 0x00000213;
	_LIT8(KSomeHash, "\x70\xe4\xf4\x54\x5f\x8e\xe6\xf2\xbd\x4e\x76\x2b\x8d\xa1\x83\xd8\xe0\x5d\x4a\x7d");

	// create some certinfo object
	ASSERT(iCertificateLabel);
		
	switch (iState)
		{
		case ETestSerialization:
			{
			iOut.writeString(_L("Checking serialization for a certificate..."));
			iOut.writeNewLine();			

			TRAPD( err, TestSerializationL( token, issuerKeyId, subjectKeyId,
												 KCertificateId, &KSomeHash) );

			TRequestStatus* status = &aStatus;
			if (err != KErrNone )
				{
				// TestSerializationL() did leave
				iResult = EFalse;
				iResultGood = EFalse;
				User::RequestComplete(status, err);
				}
			else
				{
				// TestSerializationL() passed ok
				iResult = ETrue;
				User::RequestComplete(status, aStatus.Int());
				}					
			// set next state
			iState = EFinished;
			}
			break;
		
			
		case ETestNewCSClasswDeletable:			// check mctwritablecertstore
			{
			iOut.writeString(_L("Testing MCTWritableCertStore::Add() w deletable..."));
			iOut.writeNewLine();

			// create a writablestore supporting new Add()
			TNewCSClasswDeletable storeNew;
			
			// call new Add()
			storeNew.Add( *iCertificateLabel, 
						  iCertificateFormat, 
						  iOwnerType,
						  subjectKeyId, 
						  issuerKeyId, 
						  *iCertificateContent,	// this is probably unset 
						  iDeletable,
						  aStatus);
						  
			iResultGood = ETrue; 						  						
			iState = EFinished;
			}
			break;


		case ETestOldCSClasswoDeletable:		// check mctwritablecertstore
			{

			iOut.writeString(_L("Testing new MCTWritableCertStore::Add() on old class..."));
			iOut.writeNewLine();
 
			// create a writablestore that does not have new Add()
			TOldCSClasswoDeletable storeOld;
			
			// call new Add(), expect to get KErrNotSupported
			storeOld.Add( *iCertificateLabel, 
						  iCertificateFormat, 
						  iOwnerType,
						  subjectKeyId, 
						  issuerKeyId, 
						  *iCertificateContent,	// this is probably unset 
						  iDeletable,
						  aStatus);
			iResultGood = ETrue; 						  						
			iState = EFinished;
			
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			}
			break;


		case EFinished:
			{
			
			if (aStatus == KErrNone)
				{
				iResult = ETrue;
				}
			else
			 	if (aStatus == iExpectedResult)
					{
					iResult = ETrue;
					iResultGood = ETrue;
					}
				else
					{
					iResult = EFalse;
					iResultGood = ETrue;
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
			break;
		
		} 	// switch ()

	}  // CTestCertForDeletable::PerformAction()

void CTestCertForDeletable::PerformCancel()
	{
	iState = EFinished;
	MCTWritableCertStore& store = UnifiedCertStore().WritableCertStore(iStoreIndex);
	store.CancelAdd();
	}

void CTestCertForDeletable::AfterOOMFailure()
	{
	}

void CTestCertForDeletable::Reset()
	{
	iState = EFinished;
	}

void CTestCertForDeletable::DoReportAction()
	{
	iOut.writeString(_L("\tLabel = "));
	iOut.writeString(*iCertificateLabel);
	iOut.writeNewLine();
	iOut.writeString(_L("\tOwner type = "));
	WriteOwnerType();
	WriteFormat(iCertificateFormat);
	
	iOut.writeString(_L("\tSubjectKeyId:  "));
	iOut.writeOctetString(iSubjectKeyId);
	iOut.writeNewLine();

	iOut.writeString(_L("\tiDeletable  = "));
	iDeletable ? iOut.writeString(KTrue) : iOut.writeString(KFalse);
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

void CTestCertForDeletable::WriteFormat(TCertificateFormat aFormat)
	{
	iOut.writeString(_L("\tFormat = "));
	switch (aFormat)
		{
		case EX509Certificate:
			iOut.writeString(_L("X.509\n"));
			break;
			
		case EWTLSCertificate:
			iOut.writeString(_L("WTLS\n"));
			break;
			
		case EX968Certificate:
			iOut.writeString(_L("X968\n"));
			break;
			
		case EX509CertificateUrl:
			iOut.writeString(_L("X.509 URL\n"));
			break;
			
		case EWTLSCertificateUrl:
			iOut.writeString(_L("WTLS URL\n"));
			break;
			
		case EX968CertificateUrl:
			iOut.writeString(_L("X968 URL\n"));
			break;
			
		default:
			iOut.writeString(_L("Unknown format\n"));
			break;
		}
	}

void CTestCertForDeletable::WriteOwnerType()
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

CTestCertForDeletable::CTestCertForDeletable(RFs& aFs, CConsoleBase& aConsole,
								 Output& aOut)
: CCertStoreTestAction(aFs, aConsole, aOut), iState(EFinished)
	{
	}

void CTestCertForDeletable::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CCertStoreTestAction::ConstructL(aTestActionSpec);

	SetCertFormatL(Input::ParseElement(aTestActionSpec.iActionBody, KCertFormatStart));
	SetCertOwnerTypeL(Input::ParseElement(aTestActionSpec.iActionBody, KCertOwnerTypeStart));
	SetCertLabelL(Input::ParseElement(aTestActionSpec.iActionBody, KCertLabelStart));
	SetKeyId(iIssuerKeyId, Input::ParseElement(aTestActionSpec.iActionBody, KIssuerKeyStart));
	SetKeyId(iSubjectKeyId, Input::ParseElement(aTestActionSpec.iActionBody, KSubjectKeyStart));
	SetStoreToUse(Input::ParseElement(aTestActionSpec.iActionBody, KStoreToUseStart));
	SetDeletable(Input::ParseElement(aTestActionSpec.iActionBody, KDeletableStart));
	SetSubActionL(Input::ParseElement(aTestActionSpec.iActionBody, KSubActionStart));

	// Setting the expected result
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
	CleanupStack::PopAndDestroy(result);
	}

void CTestCertForDeletable::SetKeyId(TKeyIdentifier& aKeyIdentifier, const TDesC8& aKeyInfo)
	{
	TInt size = aKeyInfo.Length();
	for (TInt i = 0; i < size; i += 2)
		{
		TInt a = (aKeyInfo[i+1] >= 'a') ? (aKeyInfo[i+1] - 'a' + 10) : (aKeyInfo[i+1] - '0');
		TInt b = (aKeyInfo[i] >= 'a') ? (aKeyInfo[i] - 'a' + 10) : (aKeyInfo[i] - '0');
		aKeyIdentifier.Append(a  + b * 16);
		}
	}

void CTestCertForDeletable::SetCertFormatL(const TDesC8& aFormat)
	{
	if (aFormat == KX509)
		{
		iCertificateFormat = EX509Certificate;
		}
	else if (aFormat == KWTLS)
		{
		iCertificateFormat = EWTLSCertificate;
		}
	else if (aFormat == KX968)
		{
		iCertificateFormat = EX968Certificate;
		}
	else if (aFormat == KX509URL)
		{
		iCertificateFormat = EX509CertificateUrl;
		}
	else if (aFormat == KWTLSURL)
		{
		iCertificateFormat = EWTLSCertificateUrl;
		}
	else if (aFormat == KX968URL)
		{
		iCertificateFormat = EX968CertificateUrl;
		}	
	else if (aFormat == KUnknown)
		{
		iCertificateFormat = EUnknownCertificate;
		}
	else
		{
		iOut.write(_L("Unrecognized cert format: "));
		iOut.writeString(aFormat);
		iOut.writeNewLine();		   
		User::Leave(KErrArgument);
		}
	}

void CTestCertForDeletable::SetCertOwnerTypeL(const TDesC8& aOwnerType)
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

void CTestCertForDeletable::SetSubActionL(const TDesC8& aStringVal)
	{
	if (aStringVal == KTestSerialization)
		{
		iState = ETestSerialization;
		}
	else if (aStringVal == KTestNewCSClasswDeletable)
		{
		iState = ETestNewCSClasswDeletable;
		}
	else if (aStringVal == KTestOldCSClasswoDeletable)
		{
		iState = ETestOldCSClasswoDeletable;
		}

	else
		{
		iOut.write(_L("Unknown subaction type: "));
		iOut.writeString(aStringVal);
		iOut.writeNewLine();		   
		User::Leave(KErrArgument);
		}
	}


void CTestCertForDeletable::SetCertLabelL(const TDesC8& aLabel)
	{
	delete iCertificateLabel;
	iCertificateLabel = NULL;
	iCertificateLabel = HBufC::NewL(aLabel.Length());
	TPtr ptr = iCertificateLabel->Des();
	ptr.Copy(aLabel);
	}
	


void CTestCertForDeletable::SetStoreToUse(const TDesC8& aStoreToUse)
	{
	TLex8 lex(aStoreToUse);
	lex.Val(iStoreIndex);
	}
	
void CTestCertForDeletable::SetDeletable(const TDesC8& aStringVal)
	{
	TLex8 lex(aStringVal);
	lex.Val(iDeletable);
	}

void CTestCertForDeletable::SetCertificateContentL(const TDesC8& aFileName)
	{
	TFileName fileName;
	fileName.Copy(aFileName);
	RFs fs; 
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	__ASSERT_DEBUG(!iCertificateContent, User::Panic(_L("CTestCertForDeletable"), 1));
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

void CTestCertForDeletable::ConstructCertL(const TDesC8& aCert)
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
	CleanupStack::PopAndDestroy(2); // fs and certBuf
	}

void CTestCertForDeletable::DoCheckResult(TInt aError)
	{
	if (iFinished)
		{
		if (iResult && !iResultGood)
			{
			iConsole.Write(_L("\ttestcertfordeletable failed\n"));
			iOut.writeString(_L("\ttestcertfordeletable failed"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else if (iResult)
			{
			iConsole.Write(_L("\ttestcertfordeletable passed ok\n"));
			iOut.writeString(_L("\ttestcertfordeletable passed ok"));
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		else 
			{
			iConsole.Write(_L("\tcertinfo couldn't be tested\n"));
			iOut.writeString(_L("\tcertinfo couldn't be tested"));
			iOut.writeNewLine();
			iOut.writeString(_L("\t"));
			iOut.writeError(aError);
			if (aError == KErrBadName)
				{
				iOut.writeString(_L("   - Check that the label is unique"));
				}
			if (aError == KErrAccessDenied)
				{
				iOut.writeString(_L("   - Can't open filestream. Access denied."));
				}
			iOut.writeNewLine();
			iOut.writeNewLine();
			}
		}	
	}
