/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/
 
#include "tcertrequeststep.h"
#include <f32file.h> 
#include <e32std.h>

_LIT(KKeyLabel, "new pkcs10 test key"); 

// CertRequest tester active.
CPKCS10TesterActive::CPKCS10TesterActive( CTestExecuteLogger& aLogger ) : 
   CActive( EPriorityStandard ),
   iLogger( aLogger )
	{
	CActiveScheduler::Add( this );
	User::LeaveIfError(iFs.Connect());
	}

CPKCS10TesterActive::~CPKCS10TesterActive()
	{
   	delete iDN;
	iDN=NULL;	 
	delete iCertRequest;
	iCertRequest=NULL;			
 	delete iOutputASN1Encoding;	
 	iOutputASN1Encoding=NULL; 			
 	delete iSecondOutputASN1Encoding;
 	iSecondOutputASN1Encoding=NULL;
 	if (iKeyInfo)
 	{
		iKeyInfo->Release();
		iKeyInfo=NULL;	 			
 	}
	delete iKeyData;
	iKeyData=NULL;
	if(iKeyStore) // debug.
	{
	    iKeyStore->Cancel();
		delete iKeyStore;
		iKeyStore=NULL;	
	}
	iFs.Close ();
	}
	
void CPKCS10TesterActive::DeleteAllKeysL()
	{
	// Delete Key store.
	INFO_PRINTF1(_L("deleting store keys"));
  	iKeyStore = CUnifiedKeyStore::NewL(iFs);
  	CleanupStack::PushL(iKeyStore); 
  	iKeyStore->Initialize(iStatus);  
	
	iState = EDeleteAllInit;
	SetActive();
   	CActiveScheduler::Start();
   	   	
   	iKeys.Close();
   	CleanupStack::Pop(iKeyStore);
   	delete iKeyStore;
	iKeyStore = NULL;	
	
	}

TVerdict CPKCS10TesterActive::doActiveCertRequestL(CCertificateRequestStep* aStep)
	{
	
	iTestSuccess= EFail;
	INFO_PRINTF1(_L("Active tester for Cert Request started. "));
	iStepPointer = aStep;
		
    DeleteAllKeysL();
	
    INFO_PRINTF1(_L("initialising keystore"));
    
	// Initialise Key store.
  	iKeyStore = CUnifiedKeyStore::NewL(iFs);
  	CleanupStack::PushL(iKeyStore); 
  	iKeyStore->Initialize(iStatus);  
	iState = EInitKeyStore; 
	SetActive();
   	CActiveScheduler::Start();
   	
   	// After encoding was produced it checks correctness
    if(iRunError==KErrNone)
     {
	     if( !(iStepPointer->iOOMCondition) &&verifyCertReqEncodingL()!=EPass  )
		 {
			  iTestSuccess= EFail;
		 }
		 else    
		 {
		 	  iTestSuccess= EPass;
		 }

		 
		 if( iTestSuccess && iStepPointer->iGenerateSecondRequest) // if cert was reused.
		 {
			// compare encoding of first and second request.
			if((iOutputASN1Encoding->Compare(iSecondOutputASN1Encoding->Des())) == 0)
			{
		 		INFO_PRINTF1(_L("Reuse verified"));
		 	}
			else
		 	{
		 		iTestSuccess= EFail;
		 		//iStepPointerSetTestStepResult(EFail);
		 		INFO_PRINTF1(_L("New output encoding is not what is expected"));		
		 	} 
		 }
     }
  	
   	CleanupStack::Pop(iKeyStore);  
    return iTestSuccess;
}
	
TInt CPKCS10TesterActive::RunError(TInt aError)
	{
	iRunError =aError;
	iKeyStore->Cancel();
	if(iCertRequest)
	{
		iCertRequest->Cancel();	
	}
	CActiveScheduler::Stop();
	return KErrNone;
	
	}
	
void CPKCS10TesterActive::RunL()
	{
	iRunError =KErrNone;
	
 	User::LeaveIfError(iStatus.Int());
   
	switch(iState)
		{
		
		case EDeleteAllInit:
			INFO_PRINTF1(_L("  listing existing keys\n"));
			iKeyStore->List(iKeys, iKeyFilter, iStatus);
			iState = EDeleteAllDelete;
			SetActive();
			break;
			
		case EDeleteAllDelete:
			if (iKeys.Count() == 0)
				{
				// key log is empty
				iKeys.Close();
				CActiveScheduler::Stop();
				break;
				}
			
			INFO_PRINTF1(_L("  deleting key\n"));
			iKeyStore->DeleteKey(*iKeys[0], iStatus);
			iState = EDeleteAllDelete;
			SetActive();
			iKeys[0]->Release();
			iKeys.Remove(0);
			break;
		
		case EInitKeyStore:
			{
	  		INFO_PRINTF1(_L("Importing keys"));
 			TFileName filename;
 			filename = iStepPointer->iPrivateKey;
			RFile file;
			User::LeaveIfError(file.Open(iFs,filename,EFileRead));
			CleanupClosePushL(file);
			TInt size;
			User::LeaveIfError(file.Size(size));
			iKeyData = HBufC8::NewMaxL(size);
			TPtr8 keyPtr = iKeyData->Des();
			User::LeaveIfError(file.Read(keyPtr));			
			CleanupStack::PopAndDestroy(); // file

			TTime start(0.0); 
			TTime end(0.0); 
			
			// Assumes only one keystore
			// Check parameters!
		 	ASSERT(iKeyInfo == NULL);      
			iKeyStore->ImportKey(0, *(iKeyData), EPKCS15UsageSign, KKeyLabel,0, start, end,iKeyInfo, iStatus);
	  		iState = EImportKey;
	  		SetActive();
			break;
			}
	  	case EImportKey:
			{
			INFO_PRINTF1(_L("Setting security policy for new stored key"));
			TSecureId secureId(0x101f7784); // Application secure ID 
			TSecurityPolicy securePolicy(secureId,ECapabilityReadUserData);
			iKeyStore->SetUsePolicy(iKeyInfo->Handle(),securePolicy,iStatus);
			iState = EKeyPolicy;
 			SetActive();
 			break;
		   	}
		case EKeyPolicy:
			{
			iAttrCollection=CPKCS10Attributes::NewL();
 			CleanupStack::PushL(iAttrCollection); 
			INFO_PRINTF1(_L("Adding generic attributes"));		
 			AddGenericAttributesL();
 			INFO_PRINTF1(_L("Adding Challenge password"));
 			AddChallengePasswordL();
 			INFO_PRINTF1(_L("Adding V3 extensions"));
 			AddV3ExtensionsL();
 			INFO_PRINTF1(_L("Generating distinguished name"));
 			iDN=MakeDistinguishedNameL();
 			CleanupStack::PushL(iDN);
   	 		INFO_PRINTF1(_L("Generating cert request"));
   	 		iCertRequest=CPKCS10Request::NewL(*iDN,*iKeyInfo,iAttrCollection);
     		CleanupStack::PushL(iCertRequest);
     		INFO_PRINTF1(_L("Setting digest algorithm"));
			TAlgorithmId digestAlgo=iStepPointer->ConvertNameToDigestId(iStepPointer->iDigestAlg);
			iCertRequest->SetDigestAlgL(digestAlgo);
 			INFO_PRINTF1(_L("Requesting cert request encoding"));
 	  		
 	  		// Clean up
 	  		CleanupStack::Pop(iCertRequest);
 			CleanupStack::Pop(iDN);
    	 	CleanupStack::Pop(iAttrCollection);
    	  	iAttrCollection=NULL;  
			iOutputASN1Encoding=NULL;
			iCertRequest->CreateEncoding(iOutputASN1Encoding,iStatus);
			iState=EGenerateCertRequest; 
	 		SetActive();     
	  		break;	
			}
		case EGenerateCertRequest:
			{ 
			
			// Use to debug encoding 			
  			// iStepPointer->OutputEncodingToFileL(iOutputASN1Encoding->Des()); //debug
			// Used for cert request reuse cases  
			if(iStepPointer->iGenerateSecondRequest)
				{       
						INFO_PRINTF1(_L("Reusing instance of CPKCS10Request"));
				        if(iStepPointer->iRepopulateDataRequest)
				        {
				         	iAttrCollection=CPKCS10Attributes::NewL();
				            CleanupStack::PushL(iAttrCollection); 
				        	AddGenericAttributesL();
				 			AddChallengePasswordL();
				 			AddV3ExtensionsL();
				 			// deletes previous value of iDN.
				 			delete iDN;
				 			iDN=MakeDistinguishedNameL();
				 			CleanupStack::PushL(iDN);
				   	 		TAlgorithmId digestAlgo2=iStepPointer->ConvertNameToDigestId(iStepPointer->iDigestAlg);
							// Repopulates data.
							CleanupStack::PushL(iCertRequest);
							iCertRequest->SetDigestAlgL(digestAlgo2);
							iCertRequest->SetDistinguishedNameL(*iDN);
							iCertRequest->SetAttributes(iAttrCollection);
							iCertRequest->SetKeyInfoL(*iKeyInfo);
							// Clean up
							CleanupStack::Pop(iCertRequest);
							CleanupStack::Pop(iDN);
    						CleanupStack::Pop(iAttrCollection);
    						iAttrCollection=NULL;		
				        }
				        INFO_PRINTF1(_L("Launches second cert request"));
				        iSecondOutputASN1Encoding=NULL;
				        iCertRequest->CreateEncoding(iSecondOutputASN1Encoding,iStatus);
				        iState=EGenerateSecondCertRequest;
				}
				else
				{
					 // if no reuse case delete keys and prepare for final state
						INFO_PRINTF1(_L("Deleting key"));
 		 				iKeyStore->DeleteKey(*iKeyInfo, iStatus);
 		 				iState=EDeleteKey;
				}
			
 		  	SetActive();  
 		 	break;	
			}
		case EGenerateSecondCertRequest:
			{
			INFO_PRINTF1(_L("Deleting key"));
 		 	iKeyStore->DeleteKey(*iKeyInfo,iStatus);
 		 	iState=EDeleteKey;
 		 	SetActive();
 		 	break;
			}
		case EDeleteKey:
			{
		    iKeyInfo->Release();  
			iKeyInfo = NULL;
		 	CActiveScheduler::Stop();
 		    break;	
			}
 		default:
			{
		  	INFO_PRINTF1(_L("Cert Request Active tester: State corrupted."));
			User::Leave(KErrCorrupt);
			}
 		} 
 		
   	return; 
}

	
CCertificateRequestStep::~CCertificateRequestStep()
/**
 * Destructor
 */
	{   
 		delete iActiveObjTest;
 		delete iSched;
	}

CCertificateRequestStep::CCertificateRequestStep()
{
	SetTestStepName(KCertificateRequestStep);
}

TVerdict CCertificateRequestStep::doTestStepPreambleL()
{
	__UHEAP_MARK;	
	User::LeaveIfError (iFs.Connect());
	
	// initializes data.
           
	// Read values form config file 
	GetIntFromConfig(ConfigSection(), _L("Expected_error"), iExpectedError);	
    GetStringFromConfig(ConfigSection(), _L("DN_country"), iDN_country);
    GetStringFromConfig(ConfigSection(), _L("DN_state"), iDN_state);
    GetStringFromConfig(ConfigSection(), _L("DN_locality"), iDN_locality);
    GetStringFromConfig(ConfigSection(), _L("DN_organization"), iDN_organization);
    GetStringFromConfig(ConfigSection(), _L("DN_unit"), iDN_unit);
    GetStringFromConfig(ConfigSection(), _L("DN_common"), iDN_common);
    GetStringFromConfig(ConfigSection(), _L("DN_email"), iDN_email);
    GetStringFromConfig(ConfigSection(), _L("PrivateKey"),iPrivateKey);
    GetStringFromConfig(ConfigSection(), _L("OPENSSL_certreq"),iOPENSSLCertReq);
    GetStringFromConfig(ConfigSection(), _L("KeyAlg"),iKeyAlg);
    GetStringFromConfig(ConfigSection(), _L("ChallengePassword"),iChallengePassword);
    GetStringFromConfig(ConfigSection(), _L("DigestAlg"),iDigestAlg);
    GetBoolFromConfig(ConfigSection(), _L("OOMCondition"),iOOMCondition);
    GetBoolFromConfig(ConfigSection(), _L("GenerateSecondRequest"),iGenerateSecondRequest);
    GetBoolFromConfig(ConfigSection(), _L("RepopulateDataRequest"),iRepopulateDataRequest);
    GetIntFromConfig(ConfigSection(), _L("ElemCertReqCount"),iElemCertReqCount);
    GetIntFromConfig(ConfigSection(), _L("ElemCertInfoCount"),iElemCertInfoCount);
    GetIntFromConfig(ConfigSection(), _L("CertReqVer"),iCertReqVer);
    GetIntFromConfig(ConfigSection(), _L("ElemSubPubKeytInfoCount"),iElemSubPubKeytInfoCount);
    GetIntFromConfig(ConfigSection(), _L("ElemKeyAlgIdenCount"),iElemKeyAlgIdenCount);
    GetIntFromConfig(ConfigSection(), _L("ElemSigAlgIdenCount"),iElemSigAlgIdenCount);
    GetIntFromConfig(ConfigSection(), _L("Attribute_count"),iAttribute_count );
    
    // Read generic Attributes (ARRAY).
    TInt index(0);
	TName fGenericAttrOID;
	fGenericAttrOID.Format(_L("Attribute_OID_%d"), index);
	TName fGenericAttrValue;
	fGenericAttrValue.Format(_L("Attribute_value_%d"), index); 
	
	TPtrC genericAttrOIDName;
	TPtrC genericAttrValueName;
    
   	while (GetStringFromConfig(ConfigSection(), fGenericAttrOID, genericAttrOIDName)
			&& GetStringFromConfig(ConfigSection(), fGenericAttrValue, genericAttrValueName))
	{
	   
	    iArrayGenAttrOID.AppendL(genericAttrOIDName);
	    iArrayGenAttrValue.AppendL(genericAttrValueName);
		index++;
		fGenericAttrOID.Format(_L("Attribute_OID_%d"), index);
		fGenericAttrValue.Format(_L("Attribute_value_%d"), index);
		
	}	
		
	// Read the v3 extension attributes (Array)
	index=0;
    TName fV3AttrOID;
	fV3AttrOID.Format(_L("V3_Extension_OID_%d"), index);
	TName fV3AttrCritical;
	fV3AttrCritical.Format(_L("V3_Extension_Critical_%d"), index); 
	TName fV3AttrValue;
	fV3AttrValue.Format(_L("V3_Extension_Value_%d"), index); 

	TPtrC v3AttrOID;
	TBool v3AttrCritical;
	TPtrC v3AttrValue;
	
   	while (GetStringFromConfig(ConfigSection(), fV3AttrOID, v3AttrOID)
			&& GetBoolFromConfig(ConfigSection(), fV3AttrCritical, v3AttrCritical)
			&& GetStringFromConfig(ConfigSection(), fV3AttrValue, v3AttrValue))
		{
	    iArrayV3AttrOID.AppendL(v3AttrOID);
	    iArrayV3AttrCritical.AppendL(v3AttrCritical);
	    iArrayV3AttrValue.AppendL(v3AttrValue);
		index++;
		fV3AttrOID.Format(_L("V3_Extension_OID_%d"), index);
		fV3AttrCritical.Format(_L("V3_Extension_Critical_%d"), index); 
		fV3AttrValue.Format(_L("V3_Extension_Value_%d"), index); 
		}	
		
	SetTestStepResult(EPass);
	return TestStepResult();
}


TVerdict CCertificateRequestStep::doTestStepL()
{
	if (!iOOMCondition)
		{
		 doTestL(); 
		}
	else
		{
 		return doOOMTestL();
	    }	

   	 return TestStepResult();
}

TVerdict CCertificateRequestStep::doOOMTestL()
{
	TVerdict verdict = EFail;
 	TInt countAfter = 0;
	TInt countBefore = 0;
 	for (TInt oomCount = 0; ; oomCount++)
 		{
 		INFO_PRINTF2(_L("\n ==== Number of memory allocations %d ===="), oomCount);
 		verdict = EFail;
 		__UHEAP_RESET;
 		__UHEAP_SETFAIL(RHeap::EDeterministic, oomCount);
 		countBefore = User::CountAllocCells();
 		TRAPD(error, doTestL());// ----> This is the actual test that runs under OOM conditions.
 		countAfter = User::CountAllocCells();
 		__UHEAP_RESET;
 		if (error != KErrNoMemory)  
 			{
 			verdict = EPass;
 			INFO_PRINTF2(_L("OOM Status %d"),error);
			INFO_PRINTF1(_L("Test outcome : Passed"));
 			break;
 			}
 		else
 			{
 			if (countBefore != countAfter)
 				{
 				INFO_PRINTF2(_L("OOM Status %d"),error);
 				INFO_PRINTF2(_L("OOM Failed at %d"), oomCount);
 				SetTestStepResult(verdict);  
 				break;
 				}
 			}
 		INFO_PRINTF2(_L("OOM Failed Point status %d"), error);
		}
	INFO_PRINTF3(_L("Heap alloc count ok: %d final vs %d initial"), countAfter,countBefore);
 	SetTestStepResult(verdict);
 	if (verdict==EFail)
	 	{
 		User::Leave(KErrGeneral);	 		
	 	}	 	
 	return verdict;
	}	
	


	
void CCertificateRequestStep::doTestL()
{
    
    iSched=new(ELeave) CActiveScheduler; 
    CleanupStack::PushL(iSched);  
	CActiveScheduler::Install(iSched);
	
	
	iActiveObjTest = new (ELeave) CPKCS10TesterActive(Logger());
	CleanupStack::PushL(iActiveObjTest);

	if (iActiveObjTest->doActiveCertRequestL(this) != EPass)
	 {
	 	SetTestStepResult(EFail);
	 	INFO_PRINTF1(_L("Verification FAIL."));
	 	// To keep happy out of memory test.
	 	User::Leave(KErrNoMemory);

	 }
	 else
	 {
	 	INFO_PRINTF1(_L("Verification PASS."));	
	 }

	 CleanupStack::PopAndDestroy(iActiveObjTest);
	 iActiveObjTest = NULL;
	 CleanupStack::PopAndDestroy(iSched);
	 iSched=NULL;
	 
 }

TVerdict CPKCS10TesterActive::verifyCertReqEncodingL()
{   
    TVerdict certReqCheck= EPass;
	INFO_PRINTF1(_L("Verifiying cert request encoding"));
	TInt pos(0);
	 
	CArrayPtrFlat<TASN1DecGeneric>* certReq= TASN1DecSequence().DecodeDERLC(*iOutputASN1Encoding,pos);
	// Verifies Number of elements in cert request.
	if(certReq->Count() != iStepPointer->iElemCertReqCount)
	{
		INFO_PRINTF1(_L("VERIFICATION FAILED: Incorrect number of elements in cert request"));
		certReqCheck= EFail;
	}
	
	CArrayPtrFlat<TASN1DecGeneric>* certReqInfo=TASN1DecSequence().DecodeDERLC(*certReq->At(0));  
	// Verifies Number of elements in cert request info.
	if(certReqInfo->Count() != iStepPointer->iElemCertInfoCount)
	{
		INFO_PRINTF1(_L("VERIFICATION FAILED: Incorrect number of elements in cert request info"));
		certReqCheck= EFail;
	}
		  
	TASN1DecInteger decInt;
	TInt version = decInt.DecodeDERShortL(*certReqInfo->At(0));
	// Verifies expected version in cert request info.
	if(version != iStepPointer->iCertReqVer)
	{
		INFO_PRINTF1(_L("VERIFICATION FAILED: Incorrect version in cert request info"));
		certReqCheck= EFail;
	}

	pos = 0;
	CX500DistinguishedName* dn = CX500DistinguishedName::NewLC(certReqInfo->At(1)->Encoding());
	// Verifies distinguished name.
	if(!(iDN->ExactMatchL(*dn)))
		{
		INFO_PRINTF1(_L("VERIFICATION FAILED: Incorrect distinguished name encoding"));
		certReqCheck= EFail;
	}
	
	CArrayPtrFlat<TASN1DecGeneric>* subjPubKeyInfo = TASN1DecSequence().DecodeDERLC(*certReqInfo->At(2));
	// Verifies number of elements in public key info.
	if( iStepPointer->iElemSubPubKeytInfoCount != subjPubKeyInfo->Count())
	{
		INFO_PRINTF1(_L("VERIFICATION FAILED: Incorrect number of elements in public key info."));
		certReqCheck= EFail;
	}
	
	CArrayPtrFlat<TASN1DecGeneric>* keyAlg = TASN1DecSequence().DecodeDERLC(*subjPubKeyInfo->At(0));
	HBufC* keyAlgOid = TASN1DecObjectIdentifier().DecodeDERL(*keyAlg->At(0));
	CleanupStack::PushL(keyAlgOid);
	// Verifies number of elements in public key algorithm identifier.
	if( keyAlg->Count() != iStepPointer->iElemKeyAlgIdenCount)
	{
		INFO_PRINTF1(_L("VERIFICATION FAILED: Incorrect number of elements in public key algorithm identifier."));
		certReqCheck= EFail;
	}

    HBufC8* pubKeyData = TASN1DecBitString().ExtractOctetStringL(*subjPubKeyInfo->At(1));
	CleanupStack::PushL(pubKeyData);
    // Verifies number of elements in signature algorithm identifier.
	CArrayPtrFlat<TASN1DecGeneric>* sigAlg = TASN1DecSequence().DecodeDERLC(*certReq->At(1));
	
	if( sigAlg->Count() != iStepPointer->iElemSigAlgIdenCount)
	{
		INFO_PRINTF1(_L("VERIFICATION FAILED: Incorrect number of elements in signature algorithm identifier."));
		certReqCheck= EFail;
	}
	
	
	HBufC* sigAlgOid = TASN1DecObjectIdentifier().DecodeDERL(*sigAlg->At(0));
	CleanupStack::PushL(sigAlgOid);
	
	HBufC8* signature = TASN1DecBitString().ExtractOctetStringL(*certReq->At(2));
	CleanupStack::PushL(signature);
	
	CMessageDigest* digest = NULL;
	
	switch (iStepPointer->ConvertNameToDigestId(iStepPointer->iDigestAlg))
		{
		case ESHA1:
			digest = CSHA1::NewL();
			break;

		case EMD2:
			digest = CMD2::NewL();
			break;

		case EMD5:
			digest = CMD5::NewL();
			break;

		default:
				User::Leave(KErrCorrupt);
		}
	CleanupStack::PushL(digest);
	 
	if (iStepPointer->ConvertNameToEKeyAlgorithm(iStepPointer->iKeyAlg) == CCTKeyInfo::ERSA)
	{	
	    // Verifies key.
		if(*keyAlgOid != KRSA)
		{
			INFO_PRINTF1(_L("VERIFICATION FAILED: Expecting RSA key."));
			certReqCheck= EFail;
		}
				
		// Verifies digest.
		switch  (iStepPointer->ConvertNameToDigestId(iStepPointer->iDigestAlg))
		{
			case ESHA1:
			{
				if(*sigAlgOid != KSHA1WithRSA)
				{
					INFO_PRINTF1(_L("VERIFICATION FAILED: Expecting RSA with SHA1 signature."));
					certReqCheck= EFail;
				}
			}
			break;
			case EMD2:
			{
				if(*sigAlgOid != KMD2WithRSA)
				{
					INFO_PRINTF1(_L("VERIFICATION FAILED: Expecting RSA with MD2 signature."));
					certReqCheck= EFail;
				}
			}
			break;
			case EMD5:
			{
				if(*sigAlgOid != KMD5WithRSA)
				{
					INFO_PRINTF1(_L("VERIFICATION FAILED: Expecting RSA with MD5 signature."));
					certReqCheck= EFail;
				}
			}
			break;
			default:
			{
				INFO_PRINTF1(_L("VERIFICATION FAILED: Unrecognised signature algorithm."));
				User::Leave(KErrCorrupt);
			}
			break;
		}
		        
        // Checks RSA signature.
        // There are doubts about the validity of the method below
 /*		CRSAPublicKey* pubKey = TX509KeyFactory().RSAPublicKeyL(*pubKeyData);
		CleanupStack::PushL(pubKey);
		
		RInteger sigInt = RInteger::NewL(*signature);
		CleanupStack::PushL(sigInt);
		CRSASignature* sig = CRSASignature::NewL(sigInt);
		CleanupStack::Pop(); // sigInt
		CleanupStack::PushL(sig);
		CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewLC(*pubKey);
		digest->Update(certReq->At(0)->Encoding());
		
		if(!(verifier->VerifyL(digest->Final(),*sig)))
		{
			INFO_PRINTF1(_L("VERIFICATION FAILED: RSA Signature verification failed."));
			User::Leave(KErrGeneral);
		}   
		CleanupStack::PopAndDestroy(verifier);	
		CleanupStack::PopAndDestroy(sig);
		CleanupStack::PopAndDestroy(pubKey);   */
	}
	else if (iStepPointer->ConvertNameToEKeyAlgorithm(iStepPointer->iKeyAlg)== CCTKeyInfo::EDSA)
	{
				// Verifies key 
		if(*keyAlgOid != KDSA)
		{
				certReqCheck= EFail;
		}
		INFO_PRINTF1(_L("DSA key algorithm OID CORRECT"));
		// Verifies digest
	    if(*sigAlgOid != KDSAWithSHA1)
		{
				certReqCheck= EFail;
		}
		INFO_PRINTF1(_L("Signature algorithm OID CORRECT"));
		
		CDSAParameters* params = TX509KeyFactory().DSAParametersL(keyAlg->At(1)->Encoding());
		CleanupStack::PushL(params);
		CDSAPublicKey* pubKey = TX509KeyFactory().DSAPublicKeyL(*params, *pubKeyData);
		CleanupStack::PushL(pubKey);

		// Test sig
		CDSASignature* sig = TX509KeyFactory().DSASignatureL(*signature);
		CleanupStack::PushL(sig);

		CDSAVerifier* verifier = CDSAVerifier::NewLC(*pubKey);
		digest->Update(certReq->At(0)->Encoding());
		// Verifies signature.
		if(!(verifier->VerifyL(digest->Final(),*sig)))
		{
				certReqCheck= EFail;
		}
			
		CleanupStack::PopAndDestroy(verifier);
		CleanupStack::PopAndDestroy(sig);
		CleanupStack::PopAndDestroy(pubKey);
		CleanupStack::PopAndDestroy(params);
	}
	else
	{
		INFO_PRINTF1(_L("VERIFICATION FAILED: Invalid key algorithm."));
		certReqCheck= EFail;
	}
    
    // Verifies number of attributes.
    CArrayPtrFlat<TASN1DecGeneric>* attrSet = TASN1DecSet().DecodeDERLC(*certReqInfo->At(3));
    
    if(attrSet->Count() != iStepPointer->iAttribute_count)
	{
			INFO_PRINTF1(_L("VERIFICATION FAILED: Number of attributes incorrect"));
			certReqCheck= EFail;
	}
		
	// makes binary compare if key is not DSA.
 	if (iStepPointer->ConvertNameToEKeyAlgorithm(iStepPointer->iKeyAlg) != CCTKeyInfo::EDSA)  // Do not compare if we have DSA signatures, these are not deterministic!
	{
		if(!(CompareRequestToOPENSSLReqL()))
		{
				INFO_PRINTF1(_L("VERIFICATION FAILED: Binary compare with OPENSSL cert request does not match"));
				certReqCheck= EFail;
		}
	}
	else
	{
		INFO_PRINTF1(_L("No binary compare becuase is a DSA cert req."));	
	}   
     
    INFO_PRINTF1(_L("Verification completed."));
   
    // pop and destroy: attrSet, digest, signature, sigAlgOid, sigAlg, pubKeyData
    // keyAlgOid, keyAlg, elmSubjPubKeyInfo, dnChecker, certReqInfo, certReqASN1
    CleanupStack::PopAndDestroy(12,certReq);
	return certReqCheck;	
}

TVerdict CCertificateRequestStep::doTestStepPostambleL()
{
 	iArrayGenAttrOID.Close();
 	iArrayGenAttrValue.Close();
	iArrayV3AttrOID.Close();
	iArrayV3AttrCritical.Close();
	iArrayV3AttrValue.Close();
	__UHEAP_MARKEND;
	
	return TestStepResult();
}

void cleanuparray(TAny* aArray)
{
	CArrayPtrFlat<CX520AttributeTypeAndValue>* array=(CArrayPtrFlat<CX520AttributeTypeAndValue>*)aArray;
	array->ResetAndDestroy();
	delete array;
}
	
CX500DistinguishedName* CPKCS10TesterActive::MakeDistinguishedNameL()
{
    CArrayPtrFlat<CX520AttributeTypeAndValue>* array = new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue>(7);
	TCleanupItem cleanup(cleanuparray, array);	
	CleanupStack::PushL(cleanup);
	array->SetReserveL(7);
	
	HBufC8 *converter = HBufC8::NewMaxLC(iStepPointer->iDN_common.Length());
    converter->Des().Copy(iStepPointer->iDN_common);	
	CX520AttributeTypeAndValue* commonName = CX520AttributeTypeAndValue::NewLC(ECommonName,*converter);
	array->AppendL(commonName);
	CleanupStack::Pop(commonName);
	CleanupStack::PopAndDestroy(converter);
	
	converter = HBufC8::NewMaxLC(iStepPointer->iDN_country.Length());
    converter->Des().Copy(iStepPointer->iDN_country);
	CX520AttributeTypeAndValue* country = CX520AttributeTypeAndValue::NewLC(ECountryName,*converter);
	array->AppendL(country);
	CleanupStack::Pop(country);
	CleanupStack::PopAndDestroy(converter);
	
	converter = HBufC8::NewMaxLC(iStepPointer->iDN_locality.Length());
    converter->Des().Copy(iStepPointer->iDN_locality);
	CX520AttributeTypeAndValue* locality = CX520AttributeTypeAndValue::NewLC(ELocalityName,*converter);
	array->AppendL(locality);
	CleanupStack::Pop(locality);
	CleanupStack::PopAndDestroy(converter);
	
	converter = HBufC8::NewMaxLC(iStepPointer->iDN_state.Length());
    converter->Des().Copy(iStepPointer->iDN_state);
	CX520AttributeTypeAndValue* province = CX520AttributeTypeAndValue::NewLC(EStateOrProvinceName,*converter);
	array->AppendL(province);
	CleanupStack::Pop(province);
	CleanupStack::PopAndDestroy(converter);
		
	converter = HBufC8::NewMaxLC(iStepPointer->iDN_organization.Length());
    converter->Des().Copy(iStepPointer->iDN_organization);
	CX520AttributeTypeAndValue* org = CX520AttributeTypeAndValue::NewLC(EOrganizationName,*converter);
	array->AppendL(org);
	CleanupStack::Pop(org);
	CleanupStack::PopAndDestroy(converter);
	
	converter = HBufC8::NewMaxLC(iStepPointer->iDN_unit.Length());
    converter->Des().Copy(iStepPointer->iDN_unit);
	CX520AttributeTypeAndValue* unit = CX520AttributeTypeAndValue::NewLC(EOrganizationalUnitName,*converter);
	array->AppendL(unit);
	CleanupStack::Pop(unit);
	CleanupStack::PopAndDestroy(converter);
	//delete converter;
	converter = HBufC8::NewMaxLC(iStepPointer->iDN_email.Length());
    converter->Des().Copy(iStepPointer->iDN_email);	
	CX520AttributeTypeAndValue* email = CX520AttributeTypeAndValue::NewLC(EPKCS9EmailAddress,*converter);
	array->AppendL(email);
	CleanupStack::Pop(email);
	CleanupStack::PopAndDestroy(converter);
    
	CX500DistinguishedName* dn = CX500DistinguishedName::NewL(*array);
	
	CleanupStack::PopAndDestroy(); //array

	return dn;
}

void CPKCS10TesterActive::AddGenericAttributesL()
{
	TInt index;
 	TInt numberGenAttr;
 	CPKCS10Attribute* genericAttr;
 	CASN1EncPrintableString* attrString;
 			
 	// Add generic attributes.
 	numberGenAttr= iStepPointer->iArrayGenAttrOID.Count() ;
 			
 	index=0;
 	HBufC8 *converter;
 	while(numberGenAttr>index)
 	{   
 	  	converter = HBufC8::NewMaxLC(iStepPointer->iArrayGenAttrValue[index].Length());
        converter->Des().Copy(iStepPointer->iArrayGenAttrValue[index]);
 	    attrString=CASN1EncPrintableString::NewLC(*converter);
 	    genericAttr= CPKCS10Attribute::NewL(iStepPointer->iArrayGenAttrOID[index],attrString);
 	    CleanupStack::Pop(attrString);  
 	    CleanupStack::PushL(genericAttr);
 	    iAttrCollection->AddPKCSAttributeL(genericAttr);
 	    CleanupStack::Pop(genericAttr);
 		CleanupStack::PopAndDestroy(converter);
 		index++;  
 	}
 	
 	if(numberGenAttr>0)
 	{
 		INFO_PRINTF1(_L("Generic attributes not found nor added"));	
 	}		
}

void CPKCS10TesterActive::AddChallengePasswordL()
{
     if(iStepPointer->iChallengePassword.Length()>0)
   {
      HBufC8 *passwordString = HBufC8::NewMaxLC(iStepPointer->iChallengePassword.Length());
	  passwordString->Des().Copy(iStepPointer->iChallengePassword);
   	  CPKCS9ChallengePasswordAttr* challengePassword = CPKCS9ChallengePasswordAttr::NewL(*passwordString);
   	  CleanupStack::PopAndDestroy(passwordString);
   	  CleanupStack::PushL(challengePassword);
   	  iAttrCollection->AddPKCSAttributeL(challengePassword);
   	  CleanupStack::Pop(challengePassword);
   }
   else
   {
   	   	INFO_PRINTF1(_L("Challenge Password not found or added"));
   }
}

void CPKCS10TesterActive::AddV3ExtensionsL()
{
	TInt index;
 	TInt numV3ExtensionAttr;
 	CX509CertExtension* v3ExtensionAttr;
 	HBufC8* rawExtensionValue;
 			
 	// Add generic attributes.
 	numV3ExtensionAttr= iStepPointer->iArrayV3AttrOID.Count() ;
 			
 	index=0;
   	
 	while(numV3ExtensionAttr>index)
	{
	
 		TFileName filename;
		filename = iStepPointer->iArrayV3AttrValue[index];
		RFile file;
		User::LeaveIfError(file.Open(iFs,filename,EFileRead|EFileStream));
		CleanupClosePushL(file);
		TInt size;
		User::LeaveIfError(file.Size(size));
		rawExtensionValue = HBufC8::NewMaxL(size);
		CleanupStack::PushL(rawExtensionValue);
		TPtr8 extValuePtr = rawExtensionValue->Des();
		
		User::LeaveIfError(file.Read(extValuePtr));
				
		v3ExtensionAttr= CX509CertExtension::NewLC(iStepPointer->iArrayV3AttrOID[index], 
											 iStepPointer->iArrayV3AttrCritical[index],
											 extValuePtr);  
	
		if(index==0) // creates a new  CPKCS9ExtensionRequestAttr object.
		{
			iV3ExtensionsCollection = CPKCS9ExtensionRequestAttr::NewL(*v3ExtensionAttr);	
		}
		else // adds extension to existing CPKCS9ExtensionRequestAttr.
		{
			iV3ExtensionsCollection->AddExtensionL(*v3ExtensionAttr);	
		}
		CleanupStack::PopAndDestroy(v3ExtensionAttr);
		CleanupStack::PopAndDestroy(); // rawExtensionValue			
		CleanupStack::PopAndDestroy(); // file
		index++;
	}
	
	if(numV3ExtensionAttr>0)
	{
		// Add extension attributes to collection of attributes.
		iAttrCollection->AddPKCSAttributeL(iV3ExtensionsCollection);	
	}
		else
	{
		INFO_PRINTF1(_L("Extension requests not found nor added"));
	}

}

TAlgorithmId CCertificateRequestStep::ConvertNameToDigestId(const TDesC& aName)
{
	if (aName.Compare(_L("SHA1"))==0)
		{
		return ESHA1;	
		}
	else if (aName.Compare(_L("MD2"))==0)
			{
			return EMD2;
			}
		 else if (aName.Compare(_L("MD5"))==0)
			{
			return EMD5;
			}
			else
			 { //invalid algorithm
			 return TAlgorithmId(7);	
			 }
}
TInt  CCertificateRequestStep::ConvertNameToEKeyAlgorithm(const TDesC& aName)
{
	if (aName.Compare(_L("RSA"))==0)
			{
			return CCTKeyInfo::ERSA;
			}
			else if (aName.Compare(_L("DSA"))==0)
			{
				return CCTKeyInfo::EDSA;
			}
				else
				 { //invalid algorithm
					 return 7;	
				 }

}
	
void CCertificateRequestStep::OutputEncodingToFileL(const TDesC8& aEncoding)
{
	INFO_PRINTF1(_L("Writting encoding to file"));
	
	_LIT(KPath, "c:\\tpkcs10\\myresults\\");
	TInt err=iFs.MkDir(KPath);
	if (err!=KErrNone && err!=KErrAlreadyExists)
	{
		User::Leave(err);	 
	}

	_LIT(KExtension, ".der");
	TFileName rName;
	rName.Append(KPath);
	rName.Append(ConfigSection());
	rName.Append(KExtension);
	rName.LowerCase();
		
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Replace(iFs, rName, EFileWrite | EFileStream));
	User::LeaveIfError(file.Write(aEncoding));
	CleanupStack::PopAndDestroy(&file);
}
	
	
TBool CPKCS10TesterActive::CompareRequestToOPENSSLReqL()
{
	RFile file;
	TFileName fileName;
	fileName = iStepPointer->iOPENSSLCertReq;
	User::LeaveIfError(file.Open(iFs, fileName, EFileRead));
	CleanupClosePushL(file);
	TInt size;
	User::LeaveIfError(file.Size(size));
	HBufC8* buf = HBufC8::NewMaxLC(size);
	TPtr8 ptr = buf->Des();
	User::LeaveIfError(file.Read(ptr));
	TBool result = *iOutputASN1Encoding == *buf;
	CleanupStack::PopAndDestroy(2); // buf, file
	return result;
}

TBool CCertificateRequestStep::IsMatchingEncodingL(CASN1EncBase* aASN1Enc1, CASN1EncBase* aASN1Enc2)
	{
	TBool result = EFalse;
	
	// Check the length first
	TInt lenEnc1 = aASN1Enc1->LengthDER();
	TInt lenEnc2 = aASN1Enc2->LengthDER();
	if (lenEnc1 == lenEnc2)
		{
		// Get the encoding and compare them
		HBufC8* enc1Buf = HBufC8::NewMaxLC(lenEnc1);
		HBufC8* enc2Buf = HBufC8::NewMaxLC(lenEnc2);
		TPtr8 enc1Ptr(enc1Buf->Des());
		TPtr8 enc2Ptr(enc2Buf->Des());
		TUint pos1 = 0, pos2 = 0;

		aASN1Enc1->WriteDERL(enc1Ptr, pos1);
		aASN1Enc2->WriteDERL(enc2Ptr, pos2);

		result = (*enc1Buf == *enc2Buf);
		CleanupStack::PopAndDestroy(2, enc1Buf);
		}
	else
		{
		result = EFalse;
		}

	return result;
	}
