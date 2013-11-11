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


#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include <asn1enc.h>
#include <asn1dec.h>
#include <x509cert.h>
#include <x509keys.h>
#include <asymmetrickeys.h>

#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
#include <cryptospidef.h>
#include "cryptoasymmetriccipherapi.h"
#include "cryptosignatureapi.h"
#include <cryptospi/cryptoparams.h>
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC && SYMBIAN_ENABLE_SDP_ECC

/*static*/ CTestAction* COpenKey::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = COpenKey::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* COpenKey::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	COpenKey* self = new (ELeave) COpenKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

COpenKey::~COpenKey()
{
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
    delete iSigner;
    delete iDecrypt;
#endif
	iKeys.Close();
}

void COpenKey::PerformAction(TRequestStatus& aStatus)
{
	switch (iState)
	{	
		case EListKeysPreOpen:
		{//	Currently uses the first store, change to check the script for a specific store
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			
			iState = EOpenKey;
		}
		break;
		
		case EOpenKey:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			
			TInt keyCount = iKeys.Count();
			TInt i;
			
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					switch (iType)
						{
					case ERSASign:
						keyStore->Open(*keyInfo, iRSASigner, aStatus);
						break;
					case EDSASign:
						keyStore->Open(*keyInfo, iDSASigner, aStatus);
						break;
					case EDecrypt:
						keyStore->Open(*keyInfo, iDecryptor, aStatus);
						break;
					case EDH:
						keyStore->Open(*keyInfo, iDH, aStatus);
						break;
					#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
					case EECC:
						{
						ASSERT(iHardwareType);
						if(iOperationType == ESigning)
							{
							keyStore->Open(keyInfo->Handle(),iSigner,aStatus);
							}
						if(iOperationType == EDecryption)
							{
							keyStore->Open(keyInfo->Handle(),iDecrypt,aStatus);
							}
						break;
						}
					#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
						}
					break;
					}
				}
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
				
			iState = EFinished;
			
		}
		break;

		case EFinished:
			{
			TInt completionCode = aStatus.Int();
			HBufC* label = 0;
			iKeys.Close();
			if (aStatus.Int() == KErrNone)
				{
				switch (iType)
					{
					case ERSASign:
						if (iRSASigner)
							{
							label = iRSASigner->Label().AllocLC();
							iRSASigner->Release();
							}
						break;
					case EDSASign:
						if (iDSASigner)
							{
							label = iDSASigner->Label().AllocLC();
							iDSASigner->Release();
							}
						break;
					case EDecrypt:
						if (iDecryptor)
							{
							label = iDecryptor->Label().AllocLC();
							iDecryptor->Release();
							}
					case EDH:
						if (iDH)
							{
							label = iDH->Label().AllocLC();
							iDH->Release();
							}
						break;
	#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
					case EECC:
						{
						ASSERT(iHardwareType);
						if(iOperationType == ESigning && iSigner == NULL
								|| iOperationType == EDecryption && iDecrypt == NULL)
							{
							completionCode = KErrGeneral;
							}
						break;
						}
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
				} // switch
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
				if(iHardwareType == 0)
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
					{
					if (*label != *iLabel)
						aStatus = KErrBadName;
					CleanupStack::PopAndDestroy(label);
					}
				}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, completionCode);

			if (aStatus.Int()==iExpectedResult)
				{
				iResult = ETrue;
				}
			else
			{
				iResult = EFalse;
			}
			
			iActionState = EPostrequisite;
		}
		break;
		default:
			ASSERT(EFalse);	
	}

}

void COpenKey::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EOpenKey:
            keystore->CancelList();
            break;

        case EFinished:
            keystore->CancelOpen();
            break;

        default:
            break;
        }
    }

void COpenKey::Reset()
	{
  	iState = EListKeysPreOpen;
    iKeys.Close();
	if (iRSASigner)
		{
		iRSASigner->Release();
		iRSASigner = NULL;
		}
	if (iDSASigner)
		{
		iDSASigner->Release();
		iDSASigner = NULL;
		}
	if (iDecryptor)
		{
		iDecryptor->Release();
		iDecryptor = NULL;
		}
	if (iDH)
		{
		iDH->Release();
		iDH = NULL;
		}
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
    delete iSigner;
    iSigner = NULL;
    delete iDecrypt;
    iDecrypt = NULL;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
	}

void COpenKey::DoReportAction()
{
	_LIT(KDeleting, "Opening...");
	iOut.writeString(KDeleting);
	iOut.writeNewLine();
}

void COpenKey::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Key open success\n");			
			buf.Format(KSuccessful);
			iConsole.Write(buf);
			iOut.writeString(buf);
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Key open failure %d!!!\n");
				buf.Format(KFailed, aError);
				iConsole.Write(buf);
				iOut.writeString(buf);
			}
			else
			{
				_LIT(KFailed, "Key open failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

COpenKey::COpenKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CKeyStoreTestAction(aFs, aConsole, aOut)
{}

void COpenKey::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	TInt pos = 0, err = 0;
	TPtrC8 operationType = Input::ParseElement(aTestActionSpec.iActionBody, KOperationTypeStart, KOperationTypeEnd, pos, err);
	if(operationType.Compare(_L8("sign")) == 0)
		{
		iOperationType = ESigning;
		}
	else if (operationType.Compare(_L8("decrypt")) == 0)
		{
		iOperationType = EDecryption;
		}
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
	
	iState = EListKeysPreOpen;

	}

////////////////////////////////////
// CSign
////////////////////////////////////
/*static*/ CTestAction* CSign::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CSign::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* CSign::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CSign* self = new (ELeave) CSign(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CSign::~CSign()
{
	iKeys.Close();
	delete iReadText;
	delete iExportedPublicKey;
	delete iRSASignature;
	delete iDSASignature;
	delete iHash;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
    delete iSigner;
    delete iSpiSignature;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
}

void CSign::PerformAction(TRequestStatus& aStatus)
{
	// Jump straight to final state if an error occured 
	if (aStatus.Int()!=KErrNone)
		{
		iState=EFinished;
		}
		
	switch (iState)
		{	
		case EListKeysPreOpen:
			{            
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			iState = EOpenKey;

			}
			break;
		
		case EOpenKey:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
            TInt keyCount = iKeys.Count();

            TInt i;
            for (i = 0; i < keyCount; i++)
                {
                CCTKeyInfo* keyInfo = iKeys[i];

                if (keyInfo->Label() == *iLabel)
                    {
                    switch (iType)
                        {
						case ERSASign:
							keyStore->Open(*keyInfo, iRSASigner, aStatus);
							break;
						case EDSASign:
							keyStore->Open(*keyInfo, iDSASigner, aStatus);
							break;
						case EDH:
						case EDecrypt:
							break;	//	Nothing to do, for the compiler
						
						#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
						case EECC:
							{
							iTokenHandle = keyInfo->Handle();
							if(iHardwareType == 0)
								{
								keyStore->Open(iTokenHandle, iSigner, aStatus);
								}
							else
								{
								TRequestStatus* status = &aStatus;
								User::RequestComplete(status, KErrNone);
								}
							}
							break;
						#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
                        }
                    } // if
                if (i == keyCount)
					{
					TRequestStatus* status = &aStatus;
					User::RequestComplete(status, KErrNotFound);
					}		
                }// for
			iState = ESign;
						
		}
		break;

		case ESign:
			{
			switch (iType)
				{
				case ERSASign:
				{
					if (iHash)
					{
					    if (iFailHashDigest)	//	Don't hash it, fail deliberately
							iRSASigner->Sign(*iReadText,iRSASignature,aStatus);
						else  // message gets signed by the keystore
							iRSASigner->SignMessage(*iReadText,iRSASignature,aStatus);
					}
					else
					{
						iRSASigner->Sign(*iReadText,iRSASignature,aStatus);
					}
				}
				break;
				case EDSASign:
				{
					if (iHash)
					{
					    if (iFailHashDigest)	//	Don't hash it, deliberately fail it
							iDSASigner->Sign(*iReadText,iDSASignature,aStatus);
						else  // message gets signed by the keystore
							iDSASigner->SignMessage(*iReadText,iDSASignature,aStatus);
					}
					else
					{
						iDSASigner->Sign(*iReadText,iDSASignature,aStatus);
					}
				}
				break;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
                case EECC:
                    {
                    TRAPD(err, iSpiSignature = CryptoSpi::CCryptoParams::NewL());
                    if(err == KErrNone)
                        {
                        if(iHardwareType)
                            {
                            CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
                            ASSERT(keyStore);   //  Flag it up
                            keyStore->Sign(iTokenHandle, *iReadText, iSpiSignature, aStatus);
                            }
                        else
                            {
                            if(iSigner)
                                {
                                TRAP(err, iSigner->SignL(*iReadText, *iSpiSignature));
                                aStatus = err;
                                }
                            TRequestStatus* status = &aStatus;
                            User::RequestComplete(status, aStatus.Int());
                            }
                        }
                    else
                        {
                        aStatus = err;
                        TRequestStatus* status = &aStatus;
                        User::RequestComplete(status, aStatus.Int());
                        }
                    }
                break;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC

				default:
					ASSERT(EFalse);
			}					
			iState = EExportPublic;					
		}
		break;

	
		case EExportPublic:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);   //  Flag it up              

			TInt keyCount = iKeys.Count();
			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					iExportHandle = keyInfo->Handle();

					switch (iType)
						{
						case ERSASign:
							keyStore->ExportPublic(iExportHandle, iExportedPublicKey,  aStatus);
							break;
						case EDSASign:
							keyStore->ExportPublic(iExportHandle, iExportedPublicKey,  aStatus); 
							break;
						case EDH:
						case EDecrypt:
							break;	//	Nothing to do, for the compiler
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
						case EECC:
							{
							keyStore->ExportPublic(iExportHandle, iExportedPublicKey, aStatus);
							iState = EVerify;
							}
							break;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
						}
					break;
					}
				}
			iState = EVerify;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
			}
			break;

		case EVerify:
			{
			TInt keyCount = iKeys.Count();
			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					iExportHandle = keyInfo->Handle();
					CX509SubjectPublicKeyInfo* ki = NULL;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
					if(iType != EECC)
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
						ki = CX509SubjectPublicKeyInfo::NewLC(*iExportedPublicKey);

					switch (iType)
						{
					case ERSASign:
						{
						TX509KeyFactory factory; 
						CRSAPublicKey* key = factory.RSAPublicKeyL(ki->KeyData());
						CleanupStack::PushL(key);
						
						CRSAPKCS1v15Verifier* verifier = NULL;
						
						if (iHash)	//	Must compare with hash of original data
						{
							verifier = CRSAPKCS1v15Verifier::NewLC(*key);
							iHash->Reset();
							iVerifyResult = verifier->VerifyL(iHash->Final(*iReadText), *iRSASignature);
						}
						else
						{
							verifier = CRSAPKCS1v15Verifier::NewLC(*key);
							iVerifyResult = verifier->VerifyL(*iReadText, *iRSASignature);
						}
												
						_LIT(KReturned, "Returned... ");
						iOut.writeString(KReturned);
						iOut.writeNewLine();

						CleanupStack::PopAndDestroy(2, key);
						}
						break;
					case EDSASign:
						{
						TX509KeyFactory factory;
						CDSAPublicKey* key = factory.DSAPublicKeyL(ki->EncodedParams(), ki->KeyData());
						CleanupStack::PushL(key);
						
						CDSAVerifier* verifier = CDSAVerifier::NewLC(*key);
						if (iHash)	//	Must compare with hash of original value
						{
							iHash->Reset();
							iVerifyResult = verifier->VerifyL(iHash->Final(*iReadText),*iDSASignature);
						}
						else
						{
							iVerifyResult = verifier->VerifyL(*iReadText,*iDSASignature);
						}
						_LIT(KReturned, "Returned... ");
						iOut.writeString(KReturned);
						iOut.writeNewLine();
						CleanupStack::PopAndDestroy(verifier);
						CleanupStack::PopAndDestroy(key);
						}
						break;
				
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
					case EECC:
						{
						iVerifyResult = EFalse;
						if (iSpiSignature && iSpiSignature->IsPresent(CryptoSpi::KEccKeyTypeUid))
							{
							TDesC8* actualSignature = NULL;
							TRAPD(err, actualSignature = const_cast<TDesC8*>(&(iSpiSignature->GetTDesC8L(CryptoSpi::KEccKeyTypeUid))));
							if(err == KErrNone)
							    {
							    if(iExportedPublicKey->Des() == *actualSignature)
							        {
							        iVerifyResult = ETrue;
							        }
							    }
							else
							    {
							    aStatus = err;
							    }
							}
						_LIT(KReturned, "Returned... ");
						iOut.writeString(KReturned);
						iOut.writeNewLine();
						}
					break;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC	
					default:
						ASSERT(EFalse);

						}
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
					if(iType != EECC)
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
						CleanupStack::PopAndDestroy(ki);
					}
				}

			iState = EFinished;
			TRequestStatus* status = &aStatus;
			if (!iVerifyResult)		
				{
				_LIT(KVerifyFail, "**Verify failed**");
				iOut.writeString(KVerifyFail);
				iOut.writeNewLine();

				// Flat verify failed as KErrGeneral
				if (aStatus.Int() == KErrNone)
					{
					aStatus = KErrGeneral;
					}
				}
			User::RequestComplete(status, aStatus.Int());
			}
			break;
		
		case EFinished:
		{
		iActionState = EPostrequisite;
		iResult = (aStatus.Int() == iExpectedResult);
			
			if ((aStatus.Int() != KErrNone)&&(aStatus!=KErrNoMemory))
				{
				_LIT(KExportFail," !Failed when exporting public key! ");
				iOut.writeString(KExportFail);
				}
			
			iKeys.Close();
			switch (iType)
				{
			case ERSASign:
				if (iRSASigner)
					iRSASigner->Release();
				break;
			case EDSASign:
				if (iDSASigner)
					iDSASigner->Release();
				break;
			case EDecrypt:
				if (iDecryptor)
					iDecryptor->Release();
			case EDH:
				if (iDH)
					iDH->Release();
				break;
				}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
		}
		break;
		default:
			ASSERT(EFalse);	
	}
}

void CSign::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EOpenKey:
            keystore->CancelList();
            break;

        case ESign:
            keystore->CancelOpen();
            break;

        case EExportPublic:
			switch (iType)
                {
				case ERSASign:
                    ASSERT(iRSASigner);
                    iRSASigner->CancelSign();
                    break;

                case EDSASign:
                    ASSERT(iDSASigner);
                    iDSASigner->CancelSign();
                    break;

                default:
                    break;
                }
            break;

        case EVerify:            
            keystore->CancelExportPublic();
            break;

        default:
            break;
        }
    }

void CSign::Reset()
	{
	iState = EListKeysPreOpen;
	iKeys.Close();
    if (iRSASigner)
        {
        iRSASigner->Release();
        iRSASigner = NULL;        
        }
    if (iDSASigner)
        {
        iDSASigner->Release();
        iDSASigner = NULL;        
        }
	delete iExportedPublicKey;
	iExportedPublicKey = NULL;
	delete iRSASignature;
	iRSASignature = NULL;
	delete iDSASignature;
	iDSASignature = NULL;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
	delete iSigner;
	iSigner = NULL;
	delete iSpiSignature;
	iSpiSignature = NULL;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
	}

void CSign::DoReportAction()
{
	_LIT(KSigning, "Signing... ");
	iOut.writeString(KSigning);
	iOut.writeNewLine();
}

void CSign::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			iOut.writeNewLine();
			_LIT(KSuccessful, "Sign success\n");			
			buf.Format(KSuccessful);
			iConsole.Write(buf);
			iOut.writeString(buf);
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Sign failure %d!!!\n");
				buf.Format(KFailed, aError);
				iConsole.Write(buf);
				iOut.writeString(buf);
			}
			else
			{
				_LIT(KFailed, "Sign failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

CSign::CSign(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CKeyStoreTestAction(aFs, aConsole, aOut)
{}

void CSign::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	SetDigestSignL(Input::ParseElement(aTestActionSpec.iActionBody, KSignDigestStart));
	SetSignText(Input::ParseElement(aTestActionSpec.iActionBody, KTextStart, KTextEnd));

	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;
	
	iState = EListKeysPreOpen;
		
}

void CSign::SetDigestSignL(const TDesC8& aSignDigestDesc)
{
	TLex8 lexer(aSignDigestDesc);
	TInt digest = 0;
	lexer.Val(digest);

	if (digest > 0)
		iHash = CMessageDigestFactory::NewDigestL(CMessageDigest::ESHA1);

	if (digest==2)
		iFailHashDigest = ETrue;
}

void CSign::SetSignText(const TDesC8& aText)
{
	iReadText = HBufC8::NewMax(aText.Size());
	if (iReadText)
	{
		TPtr8 theText(iReadText->Des());
		theText.FillZ();
		theText.Copy(aText);
	}
}

////////////////////////////////////
// CDecrypt
////////////////////////////////////
/*static*/ CTestAction* CDecrypt::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CDecrypt::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* CDecrypt::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CDecrypt* self = new (ELeave) CDecrypt(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CDecrypt::~CDecrypt()
{
	iKeys.Close();
	delete iReadText;
	delete iPlainText;
	delete iPublic;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
    delete iDecrypt;
#endif
}

void CDecrypt::PerformAction(TRequestStatus& aStatus)
{
	// Jump straight to final state if an error occured 
	if (aStatus.Int()!=KErrNone)
		{
		iState=EFinished;
		}

	switch (iState)
	{	
		case EListKeysPreOpen:
			{//	Currently uses the first store, change to check the script for a specific store
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
				
			iState = EOpenKey;
			}
		break;
		
		case EOpenKey:
		{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up

			TInt keyCount = iKeys.Count();
			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					switch(iType)
						{
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
						case EECC:
						if(iHardwareType == 0)
							{
							keyStore->Open(keyInfo->Handle(), iDecrypt, aStatus);
							}
						else if(iHardwareType == 1 )
							{
							/**
							 * Call the decrypt of hardware directly. iPlainText 
							 * would be populated.
							 */
							keyStore->Decrypt(keyInfo->Handle(), *iReadText, iPlainText, aStatus);
							}
						break;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
					default:
						{
						keyStore->Open(*keyInfo, iDecryptor, aStatus);
						}
					} // switch
					break;
					} // if
				} // for
			
			iState = EExportPublic;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
		}
		break;

		case EExportPublic:
		{
			if (aStatus.Int()!=KErrNone)
				{
				_LIT(KSignFail," !Failed when opening! ");
				iOut.writeString(KSignFail);
				iState=EFinished;
				// need to set it to true so that test is true if some error was expected
				iVerifyResult=ETrue;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				break;
				}

			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up

			TInt keyCount = iKeys.Count();
			TInt i;
			
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					iExportHandle = keyInfo->Handle();
					
					keyStore->ExportPublic(iExportHandle, iPublic,  aStatus);
					break;
					}
				}
			iState = EDecrypt;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
		}
		break;

		case EDecrypt:
			{
			
			if (aStatus.Int()!=KErrNone)
				{
				iState=EFinished;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				break;
				}
			switch(iType)
				{
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
				case EECC:
					{
					if(iHardwareType)
						{
						if(*iPlainText == *iPublic)
							{
							aStatus = KErrNone;
							}
						else
							{
							aStatus = KErrGeneral;
							}
						}
					else
						{
                        aStatus = KErrGeneral;
                        if (iDecrypt)
                            {
                            HBufC8* decryptedText = NULL;
                            TRAPD(err, decryptedText = HBufC8::NewL(iDecrypt->GetMaximumOutputLengthL()));
                            
                            if (err == KErrNone)
                                {
                                CleanupStack::PushL(decryptedText);
                                TPtr8 decryptedTextPtr = decryptedText->Des();

                                TRAP(err, (iDecrypt->ProcessL(*iReadText, decryptedTextPtr)));
                                if(err != KErrNone)
                                    {
                                    aStatus = err;
                                    }
                                if ((err == KErrNone) && (decryptedTextPtr
                                        == iPublic->Des()))
                                    {
                                    aStatus = KErrNone;
                                    }
                                CleanupStack::PopAndDestroy(decryptedText);
                                }
                            else
                                {
                                aStatus = err;
                                }
                            }
                        }
					iState = EFinished;
					TRequestStatus* status = &aStatus;
					User::RequestComplete(status, aStatus.Int());
					}
					break;
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
				default:
					{
					CX509SubjectPublicKeyInfo* ki = 
						CX509SubjectPublicKeyInfo::NewLC(*iPublic);
		
					TX509KeyFactory factory; 
					CRSAPublicKey* key = factory.RSAPublicKeyL(ki->KeyData());
					CleanupStack::PushL(key);
								
				//	Encrypt with public key
					CRSAPKCS1v15Encryptor* encryptor = CRSAPKCS1v15Encryptor::NewLC(*key);
					HBufC8* cipherText = HBufC8::NewLC(encryptor->MaxOutputLength());
					TPtr8 cipherTextPtr = cipherText->Des();
		
					encryptor->EncryptL(*iReadText, cipherTextPtr);
					
				//	Now decrypt again
					iPlainText = HBufC8::NewMaxL(100);
					iPlainTextPtr.Set(iPlainText->Des());
					iDecryptor->Decrypt(*cipherText, iPlainTextPtr, aStatus);
					
					CleanupStack::PopAndDestroy(cipherText);
					CleanupStack::PopAndDestroy(encryptor);
					CleanupStack::PopAndDestroy(key);
					CleanupStack::PopAndDestroy(ki);
					
					iState = EFinished;
					}
				} // switch
		}
		break;

		case EFinished:
			{
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
			if(iType == EECC)
				{
				iActionState = EPostrequisite;
				iResult = (aStatus.Int() == iExpectedResult);
					
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				}
			else
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT && SYMBIAN_ENABLE_SDP_ECC
				{
				if (aStatus == KErrNone && (!iPlainText || 
											*iPlainText != *iReadText))
					{
					aStatus = KErrGeneral;	//	Decryption failed
					}
				
				iActionState = EPostrequisite;
				iResult = (aStatus.Int() == iExpectedResult);
				
				if (iDecryptor)
					{
					iDecryptor->Release();
					}
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				}
		}
		break;
		default:
			ASSERT(EFalse);	
	}

}

void CDecrypt::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EOpenKey:
            keystore->CancelList();
            break;

        case EExportPublic:
            keystore->CancelOpen();
            break;
            
        case EDecrypt:
            keystore->CancelExportPublic();
            break;

        case EFinished:
            ASSERT(iDecryptor);
            iDecryptor->CancelDecrypt();
            break;
            
        default:
            break;
        }
    }

void CDecrypt::Reset()
	{
	iState = EListKeysPreOpen;
	iKeys.Close();
    if (iDecryptor)
        {
        iDecryptor->Release();
        iDecryptor = NULL;
        }
	delete iPlainText;
	iPlainText = NULL;
	delete iPublic;
	iPublic = NULL;
#if (defined(SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT) && defined(SYMBIAN_ENABLE_SDP_ECC))
    delete iDecrypt;
    iDecrypt = NULL;
#endif
	}

void CDecrypt::DoReportAction()
	{
	_LIT(KSigning, "Decrypting... ");
	iOut.writeString(KSigning);
	iOut.writeNewLine();
	}

void CDecrypt::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			iOut.writeNewLine();
			_LIT(KSuccessful, "Decrypt success\n");			
			buf.Format(KSuccessful);
			iConsole.Write(buf);
			iOut.writeString(buf);
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Decrypt failure %d!!!\n");
				buf.Format(KFailed, aError);
				iConsole.Write(buf);
				iOut.writeString(buf);
			}
			else
			{
				_LIT(KFailed, "Decrypt failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

CDecrypt::CDecrypt(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
		:	CKeyStoreTestAction(aFs, aConsole, aOut), iPlainTextPtr(0,0)
{}

void CDecrypt::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	SetSignText(Input::ParseElement(aTestActionSpec.iActionBody, KTextStart));
	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;

	iState = EListKeysPreOpen;
}

void CDecrypt::SetSignText(const TDesC8& aText)
{
	iReadText = HBufC8::NewMax(aText.Size());
	if (iReadText)
	{
		TPtr8 theText(iReadText->Des());
		theText.FillZ();
		theText.Copy(aText);
	}
}

////////////////////////////////////
// CDerive
////////////////////////////////////

/*static*/ CTestAction* CDerive::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CDerive::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* CDerive::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CDerive* self = new (ELeave) CDerive(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CDerive::~CDerive()
	{
	iKeys.Close();
	iN.Close();
	iG.Close();
	if (iDH)
		{
		iDH->Release();
		}
	delete iPublicKey;
	delete iOutput;
	delete iRemote;
	}

void CDerive::PerformAction(TRequestStatus& aStatus)
	{
	// Jump straight to final state if an error occured 
	if (aStatus.Int()!=KErrNone)
		{
		iState=EFinished;
		}
 
	switch (iState)
		{	
		case EListKeysPreOpen:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			
			iState = EOpenKey;
			}
			break;
		
		case EOpenKey:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			
			TInt keyCount = iKeys.Count();

			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					keyStore->Open(*keyInfo, iDH, aStatus);
					break;
					}
				}
			iState = EExportPublic;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
			}
			break;

		case EExportPublic:
			{
			ASSERT(iDH);
			iDH->PublicKey(iN, iG, iPublicKey, aStatus);
			iState = EDerive;
			}
			break;

		case EDerive:
			{
			if (aStatus.Int()!=KErrNone)
				{
				iState=EFinished;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				break;
				}

			ASSERT(iPublicKey);
			iDH->Agree(iRemote->PublicKey(), iOutput, aStatus);
			
			iState = EFinished;
			}
			break;

		case EFinished:
			{
			if (aStatus.Int()==KErrNone)
				{
				CDH* dh = CDH::NewLC(iRemote->PrivateKey());
				const HBufC8* output = dh->AgreeL(*iPublicKey);
				CleanupStack::PopAndDestroy(dh);
			
				if (aStatus == KErrNone && (!iOutput || 
											*iOutput != *output))
					{
					aStatus = KErrGeneral;	//	Agree failed
					}

				delete const_cast<HBufC8*>(output);
				}
			
			iActionState = EPostrequisite;
			iResult = (aStatus.Int() == iExpectedResult);

			if (aStatus != KErrNone)
				{
				_LIT(KSignFail," !Failed when agreeing key!");
				iOut.writeString(KSignFail);
				}

			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			}
			break;
		default:
			ASSERT(EFalse);	
		}
	}

void CDerive::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EOpenKey:
            keystore->CancelList();
            break;

        case EExportPublic:
            keystore->CancelOpen();
            break;
            
        case EDerive:
            keystore->CancelExportPublic();
            break;

        case EFinished:
            ASSERT(iDH);
            iDH->CancelAgreement();
            break;
            
        default:
            break;
        }
    }

void CDerive::Reset()
	{
	iState = EListKeysPreOpen;
	iKeys.Close();
	if (iDH)
		{
		iDH->Release();
		iDH = NULL;
		}
	delete iPublicKey;
	iPublicKey = NULL;
	delete iOutput;
	iOutput = NULL;
	}

void CDerive::DoReportAction()
{
	_LIT(KSigning, "Deriving key... ");
	iOut.writeString(KSigning);
	iOut.writeNewLine();
}

void CDerive::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			iOut.writeNewLine();
			_LIT(KSuccessful, "Derive success\n");			
			buf.Format(KSuccessful);
			iConsole.Write(buf);
			iOut.writeString(buf);
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Derive failure %d!!!\n");
				buf.Format(KFailed, aError);
				iConsole.Write(buf);
				iOut.writeString(buf);
			}
			else
			{
				_LIT(KFailed, "Derive failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

CDerive::CDerive(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
		:	CKeyStoreTestAction(aFs, aConsole, aOut)
{}

void CDerive::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	// Set parameters
	HBufC8* nData = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<n>"));
	CleanupStack::PushL(nData);
	iN = RInteger::NewL(*nData);
	CleanupStack::PopAndDestroy(nData);
	HBufC8* gData = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<g>"));
	CleanupStack::PushL(gData);
	iG = RInteger::NewL(*gData);
	CleanupStack::PopAndDestroy(gData);
	
	// Generate 'remote' key pair
	RInteger n = RInteger::NewL(iN);
	CleanupStack::PushL(n);
	RInteger g = RInteger::NewL(iG);
	CleanupStack::PushL(g);
	iRemote = CDHKeyPair::NewL(n, g);
	CleanupStack::Pop(2); // n, g
	
	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;
	
	iState = EListKeysPreOpen;
}
