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
* Declares test step classes for positive cases of PKCS#10 tests.
*
*/


/**
 @file
 @internalTechnology
*/

#include <f32file.h> 
#include <x500dn.h>
#include <x509keys.h>
#include <pkcs10.h>  
#include <pkcs10attr.h> 
#include <pkcs9attr.h>
#include <unifiedkeystore.h>
#include <asn1dec.h>
#include <asn1enc.h>
#include <hash.h>
#include <e32const.h>

#if (!defined __CERTIFICATEREQUEST_STEP_H__)
#define __CERTIFICATEREQUEST_STEP_H__
#include <test/testexecutestepbase.h>
#include "tpkcs10stepbasev2.h"

_LIT(KCertificateRequestStep,"PKCS10CertRequest");

// forward declarations:
class CCertificateRequestStep;

class CPKCS10TesterActive : public CActive
	{
public:
	CPKCS10TesterActive( CTestExecuteLogger& aLogger );
	~CPKCS10TesterActive();

	TVerdict doActiveCertRequestL(CCertificateRequestStep* aStep);
	
	//Active
	void DoCancel()	{return; };
	void RunL();
	virtual TInt RunError(TInt aError);
	// Generic
	
	void 					AddGenericAttributesL();
	void 					AddChallengePasswordL();
	void 					AddV3ExtensionsL();
	void					DeleteAllKeysL();
	CX500DistinguishedName* MakeDistinguishedNameL();
	TVerdict 				verifyCertReqEncodingL();
	TBool 					CompareRequestToOPENSSLReqL();
    public:

	CTestExecuteLogger& Logger(){return iLogger;}
	//Log buffer
	TBuf<150> iLogInfo;
    CTestExecuteLogger& iLogger;
    CCertificateRequestStep* 	iStepPointer;	
    CUnifiedKeyStore*			iKeyStore;
 	HBufC8*						iKeyData;
 	CCTKeyInfo*					iKeyInfo;
 	CPKCS10Request*        		iCertRequest;
 	CX500DistinguishedName*		iDN;
 	CPKCS10Attributes*     		iAttrCollection;
    CPKCS9ExtensionRequestAttr* iV3ExtensionsCollection;
	TInt   						iError;	
	HBufC8*				        iOutputASN1Encoding;
	HBufC8*				        iSecondOutputASN1Encoding;
	TVerdict                    iTestSuccess;
	RMPointerArray<CCTKeyInfo>	iKeys;
   	TCTKeyAttributeFilter		iKeyFilter;
	TInt						iRunError;
   
  
     RFs iFs;
            
	     enum EState
		{
			EIdle,
			// Delete all keys
			EDeleteAllInit,
			EDeleteAllDelete,

			// Main test
			EInitKeyStore,
			EKeyPolicy,
			EImportKey,
			EGenerateCertRequest,
			EGenerateSecondCertRequest,	
			EDeleteKey
		};  
     
     EState					iState;
    
	} ;
	
class CCertificateRequestStep : public CTPKCS10StepBase
	{                                  
public:
	CCertificateRequestStep();
	~CCertificateRequestStep();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL(); 
 	virtual TVerdict doOOMTestL();
	virtual void doTestL();
	
	TAlgorithmId ConvertNameToDigestId(const TDesC& aHashName);
	TInt ConvertNameToEKeyAlgorithm(const TDesC& aName);
	void OutputEncodingToFileL(const TDesC8& aEncoding);
 	TBool IsMatchingEncodingL(CASN1EncBase* aASN1Enc1, CASN1EncBase* aASN1Enc2);

public:
    RFs	  iFs;
    TBool iOOMCondition;
	TInt  iExpectedError;
	TPtrC iDN_country;
	TPtrC iDN_state;
	TPtrC iDN_locality; 
	TPtrC iDN_organization;
	TPtrC iDN_unit;
	TPtrC iDN_common;
	TPtrC iDN_email;
	TPtrC iPrivateKey;
	TPtrC iOPENSSLCertReq;
	TPtrC iKeyAlg;
	TPtrC iChallengePassword;
	TPtrC iDigestAlg;
	TInt  iElemCertReqCount;
	TInt  iElemCertInfoCount;
	TInt  iCertReqVer;
	TInt  iElemSubPubKeytInfoCount;
	TInt  iElemKeyAlgIdenCount;
    TInt  iElemSigAlgIdenCount;
    TInt  iAttribute_count;
    CPKCS10TesterActive* 		iActiveObjTest;
    CActiveScheduler* 			iSched;
	RArray<TPtrC> 				iArrayGenAttrOID;
	RArray<TPtrC> 				iArrayGenAttrValue;	
	RArray<TPtrC> 				iArrayV3AttrOID;
	RArray<TBool> 				iArrayV3AttrCritical;
	RArray<TPtrC> 				iArrayV3AttrValue;

	TBool 						iGenerateSecondRequest;
	TBool 						iRepopulateDataRequest;
	TInt   						iError;	
	
	};


#endif
