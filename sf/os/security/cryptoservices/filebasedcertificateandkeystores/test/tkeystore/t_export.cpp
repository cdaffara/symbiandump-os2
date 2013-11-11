/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
  
#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include <asn1enc.h>
#include <asn1dec.h>
#include <x509cert.h>
#include <x509keys.h>
#include <pbe.h>
#include <pbedata.h>
#include <securityerr.h>


const TInt KKeyStoreEmpty = -1199;
const TInt KDefaultSaltSize = 16;
const TInt KDefaultIVSize = 8;
const TInt KDefaultIterations = 2048;

/** The block size for the default PBE cipher */
const TUint KDESBlockBytes = 8; 

/////////////////////////////////////////////////////////////////////////////////
// CExportKey
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CExportKey::NewL(RFs& aFs, 
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CExportKey::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

CTestAction* CExportKey::NewLC(RFs& aFs,
							CConsoleBase& aConsole, 
							Output& aOut,
							const TTestActionSpec& aTestActionSpec)
{
	CExportKey* self = new (ELeave) CExportKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CExportKey::~CExportKey()
{
	iKeys.Close();
	if (iPbeParams)
		{
		delete iPbeParams;
		}
}

CExportKey::CExportKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CImportKey(aFs, aConsole, aOut), iEncrypted(EFalse)
{
 	iState = EExportKey;
}
 

void CExportKey::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;
	SetKeyDataFile(Input::ParseElement(aTestActionSpec.iActionBody, KExportDataFile, KExportDataFileEnd, pos, err));
	SetKeyEncrypted(Input::ParseElement(aTestActionSpec.iActionBody, KExportEncrypted, KExportEncryptedEnd, pos, err));

	// PBE parameters
	TPtrC8 cipher = Input::ParseElement(aTestActionSpec.iActionBody, KExportEncryptedCipher, KExportEncryptedCipherEnd, pos, err);	

	if (cipher != _L8("")) 
		{
		SetPBEParametersL(cipher, KNullDesC8, KNullDesC8, -1);		
		}
	else 
		{
		// Create default PBE paramenters
		CSystemRandom* rand = CSystemRandom::NewLC();
		HBufC8* saltc = HBufC8::NewMaxLC(KDefaultSaltSize);           
		TPtr8 salt(saltc->Des());
		salt.FillZ(); 
		TRAPD(err, rand->GenerateBytesL(salt));
		if((err != KErrNone) && (err != KErrNotSecure))
			User::Leave(err);

		HBufC8* ivc = HBufC8::NewMaxLC(KDESBlockBytes);          
	
		TPtr8 iv(ivc->Des());
		iv.FillZ(); 
		TRAP(err, rand->GenerateBytesL(iv));
		if((err != KErrNone) && (err != KErrNotSecure))
			User::Leave(err);

		ASSERT(!iPbeParams);
		iPbeParams = CPBEncryptParms::NewL(ECipherDES_CBC, salt, iv, 2048); 
        
		CleanupStack::PopAndDestroy(3, rand);     // rand, saltc, ivc
		}
	}

void CExportKey::SetKeyEncrypted(const TDesC8& aDes)
{
	TLex8 lex(aDes);
	TInt encrypted = 0;
	lex.Val(encrypted);
	if (encrypted > 0)
		iEncrypted = ETrue;
	else
		iEncrypted = EFalse;
}

void 	CExportKey::SetPBEParametersL(const TDesC8& aCipher, const TDesC8& aSalt, const TDesC8& aIV, const TInt aIteration)
	{
	TPBECipher myCipher = ECipherDES_CBC;  // Default to DES
	TInt iterations = KDefaultIterations;
	HBufC8* salt = NULL;
	HBufC8* iv = NULL;
	
	if (aCipher == _L8("ECipherRC2_CBC_40"))
		{
		myCipher = ECipherRC2_CBC_40;
		}
	if (aCipher == _L8("ECipherRC2_CBC_128"))
		{
		myCipher = ECipherRC2_CBC_128;
		}
	if (aCipher == _L8("ECipherRC2_CBC_40_16"))
		{
		myCipher = ECipherRC2_CBC_40_16;
		}
	if (aCipher == _L8("ECipherRC2_CBC_128_16"))
		{
		myCipher = ECipherRC2_CBC_128_16;
		}
	if (aCipher == _L8("ECipher3DES_CBC"))
		{
		myCipher = ECipher3DES_CBC;
 		}
	if (aCipher == _L8("ECipherDES_CBC"))
		{
		myCipher = ECipherDES_CBC;
 		}
	if (aCipher == _L8("ECipherAES_CBC_256"))
		{
		myCipher = ECipherAES_CBC_256;
 		}
	if (aCipher == _L8("ECipherAES_CBC_192"))
		{
		myCipher = ECipherAES_CBC_192;
 		}
	if (aCipher == _L8("ECipherAES_CBC_128"))
		{
		myCipher = ECipherAES_CBC_128;
 		}

	if (aSalt == KNullDesC8) 
 		{
 		salt = HBufC8::NewMaxLC(KDefaultSaltSize);
 		}
	else 
		{
		salt = aSalt.AllocLC();
		}

 	if (aIV == KNullDesC8) 
 		{
 		iv = HBufC8::NewMaxLC(KDefaultIVSize);
 		}
	else
		{
		iv = aIV.AllocLC();
		}

 	if (aIteration == -1)
 		{
 		iterations = KDefaultIterations; 
 		} 		
	else
		{
		iterations = aIteration;
		}	

	iPbeParams = CPBEncryptParms::NewL(myCipher, *salt, *iv, iterations); 

	CleanupStack::PopAndDestroy(2, salt); //salt, iv
	}

void CExportKey::SetKeyDataFile(const TDesC8& aDes)
{
//	First the private data path for this test	
	_LIT(KRamDrive, "\\tkeystore\\data\\");
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	iKeyDataFile = sysDrive.Name();
	iKeyDataFile.Append(KRamDrive);
	
	TFileName buf;
	buf.FillZ();
	buf.Copy(aDes);
	
//	Now the filename itself
	iKeyDataFile.Append(buf);
}


void CExportKey::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
	case EExportKey:
		{
		CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);

		iFilter.iUsage = EPKCS15UsageAll;
		keyStore->List(iKeys, iFilter, aStatus);
				
		iState = EListing;
		}
		
		break;
	case EListing:
		{
		switch(aStatus.Int())
			{				
			case KErrNone:
				{

				TInt keyIndex = -1; // The index of the key we want to export
		
				if (iKeys.Count() == 0)
					{
					iConsole.Printf(_L("keyStore empty!!"));
					iState = EFinished;
					TRequestStatus *status = &aStatus;
					User::RequestComplete(status, KKeyStoreEmpty);			
					break;
					}
			
				// Select the key with the label we want!
				for (TInt j = 0; j < iKeys.Count(); j++)
					{
					if (iKeys[j]->Label() == *iLabel) 
						{
						keyIndex = j;
						break;
						}
					}
		
				if (keyIndex == -1)
					{
					// Not found
					iState = EFinished;
					TRequestStatus *status = &aStatus;
					User::RequestComplete(status, KErrNotFound);
					break;
					}
			
				CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
				
				if (iEncrypted) 
					{
					ASSERT(iPbeParams);
					keyStore->ExportEncryptedKey(iKeys[keyIndex]->Handle(), *iPbeParams, iKeyData, aStatus);	
					}
				else 
					{
					keyStore->ExportKey(iKeys[keyIndex]->Handle(), iKeyData, aStatus);				
					}
	
				iState = EIntermediate;
				}
				break;

			default:
				{
				iConsole.Printf(_L("keyStore->ExportKey returned: %d\n"), aStatus.Int());
				iState = EFinished;
				TRequestStatus *status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				}
				break;
			}
		break;	
		}
		
	case EIntermediate:
		{
		switch(aStatus.Int())
			{				
			case KErrNone:
				{	
					iState = EFinished;            // All is well let's end this
					TRAPD(err, WriteToFileL());                  // Write exported key to file
					TRequestStatus *status = &aStatus;
					User::RequestComplete(status, err);
				}
				break;
			
			default:
				{
				iConsole.Printf(_L("keyStore->ExportKey returned: %d\n"), aStatus.Int());
				iState = EFinished;
				TRequestStatus *status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				}
			break;			
			}
		break;
		}

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

void CExportKey::WriteToFileL()
	{
	RFs theFs;
	CleanupClosePushL(theFs);
	User::LeaveIfError(theFs.Connect());
		
	RFile file;
	TInt r = file.Replace(theFs, iKeyDataFile, EFileWrite);

	User::LeaveIfError(r);

	CleanupClosePushL(file);
	
	file.Write(*iKeyData);

	file.Close();
	theFs.Close();	
	CleanupStack::PopAndDestroy(2, &theFs);
	}

void CExportKey::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EListing:
            keystore->CancelList();
            break;

        case EIntermediate:
			if (iEncrypted)
				{
				keystore->CancelExportEncryptedKey();
				}
			else
				{
				keystore->CancelExportKey();
				}
            break;

        default:
            break;
        }
    }

void CExportKey::Reset()
	{
	iState = EExportKey;
	iKeys.Close();
	}

void CExportKey::DoReportAction()
{
	_LIT(KExporting, "Exporting key...");
	iOut.writeString(KExporting);
	TPtr theLabel(iLabel->Des());
	iOut.writeString(theLabel);
	iOut.writeNewLine();
}


void CExportKey::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Key exported successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Key export failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}
			else
			{
				_LIT(KFailed, "Key export failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
			iOut.writeNewLine();
		}
	}
}
