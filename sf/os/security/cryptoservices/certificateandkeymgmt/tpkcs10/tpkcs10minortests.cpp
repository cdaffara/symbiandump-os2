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
* Implementation of reuse test cases
*
*/


#include "tpkcs10minortests.h"

_LIT(KKeyLabel, "new pkcs10 test key"); 

// Panics
_LIT(KPanicReuseTests, "PKCS10ReuseTests");
TInt EPanicInvalidTestData = 1;

void CPKCS10ReuseAttributeStep::doTestL()
	{
	// Set it to pass initially
	SetTestStepResult(EPass);

	//////////////////////////////////////////////////////////////////////////////
	// Test for reuse case of CPKCS10Attribute class
 	TInt numAttr = iArrayGenAttrOID.Count();
 	__ASSERT_ALWAYS((numAttr > 0), User::Panic(KPanicReuseTests, EPanicInvalidTestData));

	HBufC8 *string = HBufC8::NewMaxLC(iArrayGenAttrValue[0].Length());
	string->Des().Copy(iArrayGenAttrValue[0]);

	CASN1EncOctetString* attrString1 = CASN1EncOctetString::NewLC(*string);

	CPKCS10Attribute *genericAttr = CPKCS10Attribute::NewLC(iArrayGenAttrOID[0], attrString1);

	// Get the encoding
	CASN1EncBase* attrEncoding1 = genericAttr->GetEncodingLC();

	// Reset and reuse the class
	CASN1EncOctetString* attrString2 = CASN1EncOctetString::NewLC(*string);
	genericAttr->ResetL(iArrayGenAttrOID[0], attrString2);
	CleanupStack::Pop(attrString2);
	CASN1EncBase* attrEncoding2 = genericAttr->GetEncodingLC();

	// Compare the encodings
	TBool result = IsMatchingEncodingL(attrEncoding1, attrEncoding2);

	CleanupStack::PopAndDestroy(3, genericAttr);	// attrEncoding1, attrEncoding2
	CleanupStack::Pop(attrString1);
	CleanupStack::PopAndDestroy(string);

	if (result == EFalse)
		{
		SetTestStepResult(EFail);
		}
	}

void CPKCS10ReuseChallengePasswordAttrStep::doTestL()
	{
	// Set it to pass initially
	SetTestStepResult(EPass);

	//////////////////////////////////////////////////////////////////////////////
	// Test for reuse case of CPKCS9ChallengePasswordAttr class
	HBufC8 *passwordString = HBufC8::NewMaxLC(iChallengePassword.Length());
	passwordString->Des().Copy(iChallengePassword);

	CPKCS9ChallengePasswordAttr *chPasswordAttr = CPKCS9ChallengePasswordAttr::NewLC(*passwordString);

	// Get the encoding
	CASN1EncBase* attrEncoding1 = chPasswordAttr->GetEncodingLC();

	// Reset and reuse the class
	chPasswordAttr->ResetL(*passwordString);
	CASN1EncBase* attrEncoding2 = chPasswordAttr->GetEncodingLC();

	// Compare the encodings
	TBool result = IsMatchingEncodingL(attrEncoding1, attrEncoding2);

	CleanupStack::PopAndDestroy(4, passwordString);	// chPasswordAttr, attrEncoding1, attrEncoding2

	if (result == EFalse)
		{
		SetTestStepResult(EFail);
		}
	}

void CPKCS10ReuseExtensionAttrStep::doTestL()
	{
	// Set it to pass initially
	SetTestStepResult(EPass);

	//////////////////////////////////////////////////////////////////////////////
	// Test for reuse case of CPKCS10Attribute class
	HBufC8 *extValue1 = HBufC8::NewMaxLC(iArrayV3AttrValue[0].Length());
	extValue1->Des().Copy(iArrayV3AttrValue[0]);
	CX509CertExtension *v3ExtAttr1 = CX509CertExtension::NewLC(iArrayV3AttrOID[0], 
												 iArrayV3AttrCritical[0],
												 *extValue1);

	HBufC8 *extValue2 = HBufC8::NewMaxLC(iArrayV3AttrValue[1].Length());
	extValue2->Des().Copy(iArrayV3AttrValue[1]);
	CX509CertExtension *v3ExtAttr2 = CX509CertExtension::NewLC(iArrayV3AttrOID[1], 
												 iArrayV3AttrCritical[1],
												 *extValue2);

	CPKCS9ExtensionRequestAttr *extReqAttr = CPKCS9ExtensionRequestAttr::NewLC(*v3ExtAttr1);
	extReqAttr->AddExtensionL(*v3ExtAttr2);

	// Get the encoding
	CASN1EncBase* attrEncoding1 = extReqAttr->GetEncodingLC();

	// Reset and reuse the class
	extReqAttr->ResetL(*v3ExtAttr1);
	extReqAttr->AddExtensionL(*v3ExtAttr2);
	CASN1EncBase* attrEncoding2 = extReqAttr->GetEncodingLC();

	// Compare the encodings
	TBool result = IsMatchingEncodingL(attrEncoding1, attrEncoding2);

	CleanupStack::PopAndDestroy(7, extValue1);	// v3ExtAttr1, extValue2, v3ExtAttr2, extReqAttr, attrEncoding1, attrEncoding2

	if (result == EFalse)
		{
		SetTestStepResult(EFail);
		}
	}
	
CPKCS10TesterActiveCancelRequestA::CPKCS10TesterActiveCancelRequestA( CTestExecuteLogger& aLogger ) : 
   CPKCS10TesterActive( aLogger )
	{
	}
	
void CPKCS10CancelRequestAStep::doTestL()
{
    
    iSched=new(ELeave) CActiveScheduler; 
    CleanupStack::PushL(iSched);  
	CActiveScheduler::Install(iSched);
	
	iActiveObjTest = new (ELeave) CPKCS10TesterActiveCancelRequestA(Logger());
	CleanupStack::PushL(iActiveObjTest);

	if (iActiveObjTest->doActiveCertRequestL(this) != EPass)
	 {
	 	SetTestStepResult(EFail);
	 }
	 
	 CleanupStack::PopAndDestroy(iActiveObjTest);
	 iActiveObjTest = NULL;
	 CleanupStack::PopAndDestroy(iSched);
	 iSched=NULL;
	 
 }

	
TVerdict CPKCS10TesterActiveCancelRequestA::doActiveCertRequestL(CCertificateRequestStep* aStep)
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
    if(iRunError!=KErrCancel)
     {
	     iTestSuccess= EFail;
	 }
	 else
	 {
	 	iTestSuccess= EPass;
	 }
  	
   	CleanupStack::Pop(iKeyStore);  
    return iTestSuccess;
}

void CPKCS10TesterActiveCancelRequestA::RunL()
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
			
			iCertRequest->Cancel();
		
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
