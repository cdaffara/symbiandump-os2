/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tpkcs7step.h"
#include <test/testexecutelog.h>
#include "pkcs7signedobject.h"
#include <pkcs7contentinfo_v2.h>
#include <pkcs7digestinfo.h>
#include <pkcs7encrypteddataobject.h>
#include <pkixcertchain.h>
#include <pbe.h>
#include <pbedata.h>
#include <asn1dec.h>

#include "validate.h"

CTPKCS7CorruptTest::CTPKCS7CorruptTest()
	{
	SetTestStepName(KTPKCS7CorruptTest);
	}

CTPKCS7ValidTest::CTPKCS7ValidTest()
	{
	SetTestStepName(KTPKCS7ValidTest);
	}

CTPKCS7ContentTest::CTPKCS7ContentTest()
	{
	SetTestStepName(KTPKCS7ContentTest);
	}

CTPKCS7CertificateTest::CTPKCS7CertificateTest()
	{
	SetTestStepName(KTPKCS7CertificateTest);
	}

CTPKCS7SignerTest::CTPKCS7SignerTest()
	{
	SetTestStepName(KTPKCS7SignerTest);
	}
CTPKCS7EncryptedDataTest::CTPKCS7EncryptedDataTest()
	{
	SetTestStepName(KTPKCS7EncryptedDataTest);
	}

CTPKCS7DigestInfoTest::CTPKCS7DigestInfoTest()
	{
	SetTestStepName(KTPKCS7DigestInfoTest);
	}
	
CTPKCS7EncryptedDataCorruptTest::CTPKCS7EncryptedDataCorruptTest()
	{
	SetTestStepName(KTPKCS7EncryptedDataCorruptTest);
	}
	
CTPKCS7DigestInfoCorruptTest::CTPKCS7DigestInfoCorruptTest()
	{
	SetTestStepName(KTPKCS7DigestInfoCorruptTest);
	}

CTPKCS7OOMTest::CTPKCS7OOMTest()
	{
	SetTestStepName(KTPKCS7OOMTest);
	}

TVerdict CTPKCS7OOMTest::doTestStepL()
	{
	TVerdict verdict = EFail;
 	TInt countAfter = 0;
	TInt countBefore = 0;
 	for (TInt oomCount = 0; ; oomCount++)
 		{
 		__UHEAP_RESET;
 		__UHEAP_SETFAIL(RHeap::EDeterministic, oomCount);
 		countBefore = User::CountAllocCells();
 		TRAPD(error, doTestOOML());
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
 				verdict = EFail;
 				break;
 				}
 			}
 		INFO_PRINTF2(_L("OOM Failed Point status %d"), error);
		}
	INFO_PRINTF3(_L("Heap alloc count ok: %d final vs %d initial"), countAfter,countBefore);
 	SetTestStepResult(verdict);
 	return verdict;
	}

void CTPKCS7OOMTest::doTestOOML()
	{
	TInt OOMTest;
	GetIntFromConfig(ConfigSection(),_L("OOMTestNo"),OOMTest);
	__UHEAP_MARK;
			
	if (OOMTest == 1)
		{
		CPKCS7ContentInfo* theTest = NULL;
		theTest = CPKCS7ContentInfo::NewL(iRawData->Des());
		delete theTest;
		}
	// Digest Info
	else if (OOMTest == 2)
		{
		CPKCS7DigestInfo* theTest0 = NULL;
		theTest0 = CPKCS7DigestInfo::NewL(iRawData->Des());
		delete theTest0;
		}
	// Encrypted Data
	else if (OOMTest == 3)
		{
		CPKCS7ContentInfo* theTest1 = NULL;
		theTest1 = CPKCS7ContentInfo::NewL(iRawData->Des());
			
		CleanupStack::PushL(theTest1);
		CPKCS7EncryptedDataObject* theTest2 = NULL;
		theTest2 = CPKCS7EncryptedDataObject::NewL(*theTest1);
				
		CleanupStack::PopAndDestroy(theTest1);
		delete theTest2;
		}
	
	__UHEAP_MARKEND;
}

//	Step 1	Corrupt
TVerdict CTPKCS7CorruptTest::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	TInt reason;
	if (GetIntFromConfig(ConfigSection(),_L("Reason"),reason) == EFalse)
		{
		reason = KErrNone;
		}
	
	TInt err;
	
    //Use CPKCS7ContentInfo class in order to obtain the ContentType and ContentData.
    //Depending on the ContentType the corresponding Object of that ContentType is created.
	CPKCS7ContentInfo * contentInfo = NULL;
	TRAP (err, contentInfo = CPKCS7ContentInfo::NewL(iRawData->Des()));

	//Checks if the file is empty, Corrupt
	if (err == reason)
		{
		delete contentInfo;
		SetTestStepResult(EPass);
		INFO_PRINTF3(_L("Expected result %d, got %d"), reason, err);
		return TestStepResult();
		}
	
	if( err == KErrNone)
		{
		//ContentType is SignedData
		if( contentInfo->ContentType() == KPkcs7SignedData)
			{
			CPKCS7SignedObject * p7 = NULL;
			TRAP (err, p7 = CPKCS7SignedObject::NewL(*contentInfo));
			delete p7;
			}
		}
	delete contentInfo;

	if (err != reason)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("Expected result %d, got %d"), reason, err);
		}
	//Not corrupt , repeat files
	return TestStepResult();
	}
	
	
TVerdict CTPKCS7ValidTest::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	__UHEAP_MARK;
	TBool expectedValid;
	if (GetBoolFromConfig(ConfigSection(),_L("IsValid"), expectedValid) == EFalse)
		{
		expectedValid = ETrue;
		}
	
	TInt err;

	CPKCS7ContentInfo * contentInfo = NULL;
	TRAP (err, contentInfo = CPKCS7ContentInfo::NewL(iRawData->Des()));
	
	if(err == KErrNone)
		{
		CPKCS7SignedObject * p7 = NULL;
		if( contentInfo->ContentType() == KPkcs7SignedData)
			{
			TRAP (err, p7 = CPKCS7SignedObject::NewL(*contentInfo));
			//expired, and the case where certificate chain root is not on the device	
			if (!expectedValid)
				{
				if (err != KErrNone)
					{
					SetTestStepResult(EPass);
					INFO_PRINTF2(_L("Got %d building PKCS7 object"), err);
					return TestStepResult();
					}
				}

			if (err != KErrNone)
				{
				SetTestStepResult(EFail);
				INFO_PRINTF2(_L("Got %d building PKCS7 object"), err);
				}
			else
				{
				CleanupStack::PushL (p7);
		
				const RPointerArray<CPKCS7SignerInfo>& signers = p7->SignerInfo();
				TBool isValid = EFalse;
				HBufC8* certificateEncoding = NULL;
				if(!p7->ValidateSignerL(*signers[0], certificateEncoding))
					{
					INFO_PRINTF1(_L("Couldn't validate signer"));
					}
				else
					{
					CActiveScheduler* sched = NULL;
					if (CActiveScheduler::Current() == NULL)
						{
						INFO_PRINTF1(_L("Installing scheduler"));
						sched = new (ELeave) CActiveScheduler();
						CleanupStack::PushL (sched);
						__UHEAP_MARK;
						CActiveScheduler::Install (sched);
						}
					RPointerArray<CX509Certificate> roots (&iRootCertificate, 1);
					CPKIXCertChain * chain = CPKIXCertChain::NewLC(iFs, *certificateEncoding, roots);
					TTime tm;
					_LIT(KDateCorrect1,"20040801:"); 
					TBuf <24> theDate(KDateCorrect1); 
					TInt err=tm.Set(theDate); 
					if(err)
						{
						tm.HomeTime();
						}
					CPKIXValidationResult* result = CPKIXValidationResult::NewLC();
					CTPKCS7Validator* validator = new (ELeave) CTPKCS7Validator (chain, result, &tm);
					CleanupStack::PushL (validator);
					validator->doValidate ();
					sched->Start ();
					if (result->Error().iReason == EValidatedOK)
						{
						isValid = ETrue;
						INFO_PRINTF1(_L("Validation success"));
						}
					else
						{
						INFO_PRINTF2(_L("Validation failed: %d"), result->Error().iReason);
						}
            				
					CleanupStack::PopAndDestroy(validator);
					CleanupStack::PopAndDestroy(result);
					CleanupStack::PopAndDestroy(chain);
					if (sched)
						{
						CActiveScheduler::Install (NULL);
						CleanupStack::PopAndDestroy (sched);
						}
					}
				if (certificateEncoding)
					{
					CleanupStack::PopAndDestroy(certificateEncoding);
					}
				CleanupStack::PopAndDestroy (p7);
				if (isValid != expectedValid)
					{
					SetTestStepResult(EFail);
					if (expectedValid)
						{
						INFO_PRINTF1(_L("Expected valid, got invalid"));
						}
					else
						{
						INFO_PRINTF1(_L("Expected invalid, got valid"));
						}
					}
				}

			}
		}
	else
		{
		SetTestStepResult(EFail);
		}

	
	__UHEAP_MARKEND;
	return TestStepResult();
	}
	
	
//	Step 3	Content
TVerdict CTPKCS7ContentTest::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	TBool checkType;
	TBool checkLength;
	TBool checkContent;
	TInt expectedType;
	TInt expectedLength;
	TPtrC expectedString;
	HBufC8 * expectedContent = NULL;

	checkType    = GetIntFromConfig   (ConfigSection(),_L("ContentType"),   expectedType);
	checkLength  = GetIntFromConfig   (ConfigSection(),_L("ContentLength"), expectedLength);
	checkContent = GetStringFromConfig(ConfigSection(),_L("ContentString"), expectedString);

	if (checkContent)
		{
		if (expectedString.Length() & 1)
			{
			SetTestStepResult(ETestSuiteError);
			INFO_PRINTF1(_L("ContentString is not a multiple of 2 long"));
			checkContent = EFalse;
			}
		else
			{
			// workaround to parse the expectedstring 
			expectedContent = HBufC8::NewLC (expectedString.Length()/2);
			TPtr8 des = expectedContent->Des ();
			for (TInt i = 0; i < expectedString.Length()/2; i++)
				{
				TInt top = expectedString[2*i];
				TInt low = expectedString[2*i+1];
				if (top >= 0x61) top -= (0x61-10);
				else top -= 0x30;
				if (low >= 0x61) low -= (0x61-10);
				else low -= 0x30;
				des.Append (top*16+low);
				}
			}
		}
	TInt err;

    CPKCS7ContentInfo * contentInfo = NULL;
	TRAP (err, contentInfo = CPKCS7ContentInfo::NewL(iRawData->Des()));
    if(err == KErrNone)
		{
		CPKCS7SignedObject * p7 = NULL;
		if( contentInfo->ContentType() == 2)
			{
			TRAPD (err, p7 = CPKCS7SignedObject::NewL(*contentInfo));

			if (err != KErrNone)
				{
				SetTestStepResult(EFail);
				INFO_PRINTF2(_L("Got %d building PKCS7 object"), err);
				}
			else
				{
				CleanupStack::PushL (p7);
				const CPKCS7ContentInfo& p7info = p7->ContentInfo ();
				if (checkType)
					{
					if (p7info.ContentType() != expectedType)
						{
						SetTestStepResult(EFail);
						INFO_PRINTF3(_L("Expected ContentType %d, got %d"), expectedType, p7info.ContentType());
						}
					}
				const TDesC8& content = p7info.ContentData();
		
				if (checkLength)
					{
					if (content.Length() != expectedLength)
						{
						SetTestStepResult(EFail);
						INFO_PRINTF3(_L("Expected ContentLength %d, got %d"), expectedLength, content.Length());
						}
					}
				if (checkContent)
					{
					if (content.Length() != expectedContent->Length())
						{
						SetTestStepResult(EFail);
						INFO_PRINTF3(_L("Expected ContentString length %d does not correspond to PKCS7 data length %d"),
						expectedContent->Length(), content.Length());
						}
					else
						{
						if (expectedContent->Compare(content) != 0)
							{
							SetTestStepResult(EFail);
							INFO_PRINTF1(_L("Expected ContentString does not match PKCS7 content"));
							}
						}
					}
				CleanupStack::PopAndDestroy (p7);
				}

			}
		}
	else
		{
		SetTestStepResult(EFail);
		}

		if (expectedContent)
			{
			CleanupStack::PopAndDestroy (expectedContent);
			}

		if (!checkType && !checkLength && !checkContent && (TestStepResult()==EPass))
			{
			SetTestStepResult(EInconclusive);
			INFO_PRINTF1(_L("No test performed in this step"));
			}

	return TestStepResult();
	}

//	Step 4	Certificate
TVerdict CTPKCS7CertificateTest::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	TBool checkCount;
	TInt expectedCount;

	checkCount = GetIntFromConfig (ConfigSection(),_L("CertificateCount"), expectedCount);
	
	TInt err;
	

	CPKCS7ContentInfo * contentInfo = NULL;
	TRAP (err, contentInfo = CPKCS7ContentInfo::NewL(iRawData->Des()));
    
	if(err == KErrNone)
		{
		CPKCS7SignedObject * p7 = NULL;
		if( contentInfo->ContentType() == 2)
			{
			TRAP (err, p7 = CPKCS7SignedObject::NewL(*contentInfo));
			
			if (err != KErrNone)
				{
				SetTestStepResult(EFail);
				INFO_PRINTF2(_L("Got %d building PKCS7 object"), err);
				}
			else
				{
				CleanupStack::PushL (p7);
				const RPointerArray<CPKCS7ExtendedCertificateOrCertificate>& certificates = p7->Certificates();
				if (checkCount)
					{
					if (certificates.Count() != expectedCount)
						{
						SetTestStepResult(EFail);
						INFO_PRINTF3(_L("Expected %d certificates, got %d"), expectedCount, certificates.Count());
						}
					}
				CleanupStack::PopAndDestroy (p7);
				}

			}
		}
	else
		{
		SetTestStepResult(EFail);
		}
		
	if (!checkCount && (TestStepResult()==EPass))
		{
		SetTestStepResult(EInconclusive);
		INFO_PRINTF1(_L("No test performed in this step"));
		}

	return TestStepResult();
	}

//	Step 5	Signer
TVerdict CTPKCS7SignerTest::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	TBool checkCount;
	TInt expectedCount;

	checkCount = GetIntFromConfig (ConfigSection(),_L("SignerCount"), expectedCount);

	TInt err;

	CPKCS7ContentInfo * contentInfo = NULL;
	TRAP (err, contentInfo = CPKCS7ContentInfo::NewL(iRawData->Des()));
    if(err == KErrNone)
		{
		CPKCS7SignedObject * p7 = NULL;
		if( contentInfo->ContentType() == 2)
			{
			TRAPD (err, p7 = CPKCS7SignedObject::NewL(*contentInfo));

			if (err != KErrNone)
				{
				SetTestStepResult(EFail);
				INFO_PRINTF2(_L("Got %d building PKCS7 object"), err);
				}
			else
				{
				CleanupStack::PushL (p7);
				const RPointerArray<CPKCS7SignerInfo>& signers = p7->SignerInfo();
				if (checkCount)
					{
					if (signers.Count() != expectedCount)
						{
						SetTestStepResult(EFail);
						INFO_PRINTF3(_L("Expected %d signers, got %d"), expectedCount, signers.Count());
						}
					}
				CleanupStack::PopAndDestroy (p7);
				}

			}
		}

	
	if (!checkCount && (TestStepResult()==EPass))
		{
		SetTestStepResult(EInconclusive);
		INFO_PRINTF1(_L("No test performed in this step"));
		}

	return TestStepResult();
	}

	
	
//Encrypted Data
TVerdict CTPKCS7EncryptedDataTest::doTestStepL()
	{

	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
		
	TInt reason;
	// Read from INI file.
	GetIntFromConfig(ConfigSection(),_L("Reason"),reason);

	//Is Valid	
  	TBool expectedValid;
  	TBool checkValid;
      
  	checkValid = GetBoolFromConfig(ConfigSection(),_L("IsValid"), expectedValid);
 	
	 __UHEAP_MARK;
    CPKCS7ContentInfo *contentInfo = NULL;
	TRAPD (err, contentInfo = CPKCS7ContentInfo::NewL(iRawData->Des()));
	CleanupStack::PushL(contentInfo);
    if(err == KErrNone)
		{
	     
		CPKCS7EncryptedDataObject* p7 = NULL;
		TInt type = contentInfo->ContentType();
		if( contentInfo->ContentType() == CPKCS7ContentInfo::EContentTypeEncryptedData)
			{
			TRAPD (err, p7 = CPKCS7EncryptedDataObject::NewL(*contentInfo));
		    CleanupStack::PushL(p7);
		    __UHEAP_MARK;

			if (checkValid)
  				{
  				if(!expectedValid && err == reason)
  		 			{ 
					CleanupStack::PopAndDestroy(2,contentInfo);//p7, contentInfo
  		 			return TestStepResult();
  		 			}
  				if(!expectedValid && contentInfo->ContentType() == 1)	
           			{
           			CleanupStack::PopAndDestroy(2,contentInfo);//p7, contentInfo
           		 	return TestStepResult();
           			}
  				}

			if (err != KErrNone)
				{
				SetTestStepResult(EFail);
				ERR_PRINTF2(_L("Got %d building PKCS7 object"), err);
				}
    	
			//CONTENT TYPE
			CPKCS7EncryptedDataObject::TContentType contentType = p7->ContentType();
			if(contentType != CPKCS7EncryptedDataObject::EPkcs7Data)
    			{
     			SetTestStepResult(EFail);
     			ERR_PRINTF1(_L("ContentType does not match"));
     			CleanupStack::PopAndDestroy(3,contentInfo);//expectedContentType, p7, contentInfo
     			return TestStepResult();	
     			}
			 else
     			{
     			INFO_PRINTF1(_L("ContentType matches"));	
     			}
	  
			//ENCRYPTEDCONTENT DATA
			const TDesC8& contentData = p7->EncryptedContentInfoData();
			if(contentData.Compare(*iContentData) != 0)
				{
	   			SetTestStepResult(EFail);
	   			ERR_PRINTF1(_L("EncryptedContentData does not match"));
	   			CleanupStack::PopAndDestroy(2,contentInfo);	//p7, contentInfo
	   			return TestStepResult();
	  			}
			else
	  			{
	  			INFO_PRINTF1(_L("EncryptedContentData matches")); 	
	  			}
		    
	  
			//SALT
			const CPBEncryptParms& encryptParams = p7->EncryptParams();
			if((encryptParams.Salt()).Compare(*iSaltValue) != 0)
				{
	   			SetTestStepResult(EFail);
	   			ERR_PRINTF1(_L("Salt Value does not match"));	
	   			CleanupStack::PopAndDestroy(2, contentInfo);//p7,contentInfo
	   			return TestStepResult();
	  			}
			else
				{
				INFO_PRINTF1(_L("Salt Value matches"));	
				}  	
	        
	
			//ITERATION COUNT
			TBool checkIteration;
			TInt expectedIterationCount;
	
			checkIteration = GetIntFromConfig(ConfigSection(),_L("Iteration"), expectedIterationCount);
			if(checkIteration)
				{
	   			if(encryptParams.Iterations() != expectedIterationCount)
	   				{
	   				SetTestStepResult(EFail);
	   				ERR_PRINTF1(_L("Iterations does not match"));
	   				CleanupStack::PopAndDestroy(2, contentInfo);//p7, contentInfo
	   				return TestStepResult();	
	   				}
	   			else
	   				{
	   				INFO_PRINTF1(_L("Iteration matches")); 
	   				}
	  			}		
	
			//CIPHER
			TBool checkCipher;
			TPtrC expectedCipher;
			TPBECipher cipherValue = (TPBECipher) -1;//assign to impossible value
	
			checkCipher = GetStringFromConfig(ConfigSection(),_L("Algorithm"),  expectedCipher);
		
			if(checkCipher)
				{
				if(expectedCipher.Compare(_L("pbeWithSHA1And40BitRC2_CBC")) == 0)
	   	 			{
	   	  			cipherValue = ECipherRC2_CBC_40_5;
	     			}
				else if(expectedCipher.Compare(_L("pbeWithSHA1And128BitRC4")) == 0)
	   	 			{
	   	  			cipherValue = ECipherARC4_128;
	     			}
	            else if(expectedCipher.Compare(_L("pbeWithSHA1AndDES-CBC")) == 0)
	   	 			{
	   	  			cipherValue = ECipherDES_CBC;
	     			}
	     		else if(expectedCipher.Compare(_L("pbeWithSHAAnd3_KeyTripleDES_CBC")) == 0)
	   	 			{
	   	  			cipherValue = ECipher3DES_CBC;
	     			}
	     		else if(expectedCipher.Compare(_L("pbeWithSHAAnd2-KeyTripleDES-CBC"))==0)
	     			{
	     			cipherValue = ECipher2Key3DES_CBC;	
	     			}
	     		else if(expectedCipher.Compare(_L("pbeWithSHA1And40BitRC4")) == 0)
	   	 			{
	   	  			cipherValue = ECipherARC4_40;
	     			}
	     		else if(expectedCipher.Compare(_L("pbewithSHAAnd128BitRC2CBC")) == 0)
	   	 			{
	   	  			cipherValue = ECipherRC2_CBC_128_16;
	     			}
	     		else if(expectedCipher.Compare(_L("pbeWithSHA1AndRC2_CBC")) == 0)
	     			{
	     			cipherValue = ECipherRC2_CBC_40;
	     			}
	     					
	   			if(encryptParams.Cipher() != cipherValue)
	     			{
	     			SetTestStepResult(EFail);
	     			ERR_PRINTF1(_L("Cipher does not match"));
	     			CleanupStack::PopAndDestroy(3, contentInfo);//expectedCipher,p7,contentInfo
	     			return TestStepResult();	
	     			} 
				else 
	    			{
	   				INFO_PRINTF1(_L("Cipher matches")); 
	    			}
				}
					
			if(contentData.Length() != 0)
				{
				//Decryption
				TBool checkPassword;
				TPtrC password;
				
				checkPassword = GetStringFromConfig(ConfigSection(),_L("Password"), password);
					
				if(checkPassword)
					{
			    	HBufC8* plainText = p7->DecryptDataL(password);
			    	delete plainText;
			    	SetTestStepResult(EPass);
	  				} 
				}
	  		CleanupStack::PopAndDestroy(p7);
            }
        //If ContentType is not Encrypted Data
        else
        	{
        		
        	if(checkValid)
          		{
          		if(!expectedValid && err == reason)
 		 			{
  		 			CleanupStack::PopAndDestroy(contentInfo);
  		 			return TestStepResult();
  		 			}
          		else
          			{
          			SetTestStepResult(EFail);	
          			}
          		}

       		}
		}
	else if(err != reason)
		{
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(contentInfo);
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}
	
	
//Digest Info
TVerdict CTPKCS7DigestInfoTest::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
	__UHEAP_MARK;
	
	TInt reason;
	// Read from INI file.
	GetIntFromConfig(ConfigSection(),_L("Reason"),reason);
			
	CPKCS7DigestInfo* p7 = NULL;
	TRAPD (err, p7 = CPKCS7DigestInfo::NewL(iRawData->Des()));
	CleanupStack::PushL(p7);
	
	if (err != KErrNone)
		{
		if(err == reason)
			{
			CleanupStack::PopAndDestroy(p7);
			SetTestStepResult(EPass);
			return TestStepResult();
			}
		else
			{
			SetTestStepResult(EFail);
			ERR_PRINTF2(_L("Got %d building PKCS7 object"), err);
			}
		}
	
	//DIGEST	
    const TDesC8& digest = p7->Digest();
    if(digest.Compare(*iDigest) != 0)		
    	{
       	SetTestStepResult(EFail);
	   	ERR_PRINTF1(_L("Digest does not match"));
      	}
    else
		{
		INFO_PRINTF1(_L("Digest matches"));		
		}
     
    //ALGORITHM ID
    TBool checkAlgorithm;
    TPtrC expectedAlgorithm;
    TAlgorithmId algorithmId = (TAlgorithmId) -1;
   	
	checkAlgorithm = GetStringFromConfig(ConfigSection(),_L("AlgorithmId"), expectedAlgorithm);

	if(checkAlgorithm)
		{
	   	if(expectedAlgorithm.Compare(_L("SHA1")) == 0)
	   	 	{
	   	  	algorithmId = ESHA1;
	     	}
      	else if(expectedAlgorithm.Compare(_L("MD5")) == 0)
	   	 	{
	   	  	algorithmId = EMD5;
	     	}
      
    	if(algorithmId != p7->Algorithm())  
        	{
       		SetTestStepResult(EFail);
	   		ERR_PRINTF1(_L("Algorithm Id does not match"));	
      		}
      	else
      		{
      		INFO_PRINTF1(_L("Algorithm Id matches"));	
      		}	
    	}
    
    //ENCODED PARAMS
    const TDesC8& encodedParams= p7->EncodedParams();
    
    if(encodedParams.Length() != 0)
    	{
    	if(encodedParams.Compare(*iEncodedParams))  
    		{
       		SetTestStepResult(EFail);
	   		ERR_PRINTF1(_L("EncodedParams does not match"));		
      		}
    	else
    		{
    		INFO_PRINTF1(_L("EncodedParams match"));	
    		}  	
    	}
    else if(!iEncodedParams)
        {
        INFO_PRINTF1(_L("EncodedParams not present"));	
        }
    
    
    CleanupStack::PopAndDestroy(p7);
    __UHEAP_MARKEND;	
	return TestStepResult();
	}
	
	
//EncryptedDataCorrupt
//Step8
TVerdict CTPKCS7EncryptedDataCorruptTest::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
    __UHEAP_MARK;	
	TInt reason;
	GetIntFromConfig(ConfigSection(),_L("Reason"),reason);
		
	CPKCS7ContentInfo *contentInfo = NULL;
	TRAPD (err, contentInfo = CPKCS7ContentInfo::NewL(iRawData->Des()));
	CleanupStack::PushL(contentInfo);
	
	if(err != KErrNone)
		{
		if (err != reason)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF3(_L("Expected result %d, got %d"), reason, err);
			}
		}
	
		
	if (err == reason)
		{
		CleanupStack::PopAndDestroy(contentInfo);
		return TestStepResult();
		}

    CPKCS7EncryptedDataObject* p7 = NULL;
	TRAPD (err1, p7 = CPKCS7EncryptedDataObject::NewL(*contentInfo));
	CleanupStack::PushL(p7);
	
	if (err1 != reason)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("Expected result %d, got %d"), reason, err);
		}
	CleanupStack::PopAndDestroy(2,contentInfo);//p7, contentInfo
	__UHEAP_MARKEND;
	return TestStepResult();
	}

//DigestInfoCorrupt
//Step 9
TVerdict CTPKCS7DigestInfoCorruptTest::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
    __UHEAP_MARK;
	TInt reason;
	GetIntFromConfig(ConfigSection(),_L("Reason"),reason);
	
	CPKCS7DigestInfo* p7 = NULL;
	TRAPD (err, p7 = CPKCS7DigestInfo::NewL(iRawData->Des()));
	delete p7;
	
	if (err != reason)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF3(_L("Expected result %d, got %d"), reason, err);
		}
	__UHEAP_MARKEND;
	return TestStepResult();
	}

