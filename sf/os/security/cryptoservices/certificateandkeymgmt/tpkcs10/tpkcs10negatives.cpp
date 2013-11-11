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
* Implementation of negative step classes for PKCS#10 tests.
*
*/


#include "tpkcs10negatives.h"
#include <x500dn.h>
#include <unifiedkeystore.h>

// Key label
_LIT(KKeyLabel, "new pkcs10 neg test key"); 

// Panics
_LIT(KPanicNegativeTests, "PKCS10NegativeTests");
TInt EPanicNegInvalidTestData = 1;

// String of 256 characters
_LIT8(KHugeString, "p000000001p000000002p000000003p000000004p000000005p000000006p000000007"
				   "p000000008p000000009p000000010p000000011p000000012p000000013p000000014"
				   "p000000015p000000016p000000017p000000018p000000019p000000020p000000021"
				   "p000000022p000000023p000000024p000000025p23456");


CPKCS10NegTesterActive::CPKCS10NegTesterActive(CTestExecuteLogger& aLogger) : 
	CActive(EPriorityStandard),
	iLogger(aLogger),
	iKeyImportStarted(EFalse)
	{
	CActiveScheduler::Add(this);
	User::LeaveIfError(iFs.Connect());
	}

CPKCS10NegTesterActive::~CPKCS10NegTesterActive()
	{
	Cancel();
	delete iKeyData;
	if (iKeyInfo)
		{
		iKeyInfo->Release();
 		}
 	delete iKeyStore;
	iFs.Close();
	}

CCTKeyInfo* CPKCS10NegTesterActive::doImportKeyL(CCertificateRequestStep *aStep)
{
	INFO_PRINTF1(_L("Active object tester for Key Import"));

	iStep = aStep;
	// Initialise Key store
  	iKeyStore = CUnifiedKeyStore::NewL(iFs);
	iKeyStore->Initialize(iStatus);  

	SetActive();
   	CActiveScheduler::Start();

	User::LeaveIfError(iStatus.Int());

	return iKeyInfo;
}

void CPKCS10NegTesterActive::RunL()
	{
	iError = iStatus.Int();
	if (iError != KErrNone)
		{
		CActiveScheduler::Stop();
		}

	if (iKeyImportStarted == EFalse) 
		{
  		INFO_PRINTF1(_L("Importing keys"));
		TFileName filename;
		filename = iStep->iPrivateKey;
		RFile file;
		User::LeaveIfError(file.Open(iFs, filename, EFileRead));
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
	 	ASSERT(iKeyInfo == NULL);      
		iKeyStore->ImportKey(0, *iKeyData, EPKCS15UsageSign, KKeyLabel,0, start, end, iKeyInfo, iStatus);
  		iKeyImportStarted = ETrue;
  		SetActive();
		}
	else
		{
		// Key import done
		CActiveScheduler::Stop();
		}
	}

TInt CPKCS10NegTesterActive::RunError(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	return KErrNone;
	}

TVerdict CPKCS10NegPKCS10GenericAttr::doTestStepL()
	{
	// Set it to pass initially
	SetTestStepResult(EPass);

	//////////////////////////////////////////////////////////////////////////////
	// Test for invalid attribute for NewL
 	TInt numAttr = iArrayGenAttrOID.Count();
 	__ASSERT_ALWAYS((numAttr > 0), User::Panic(KPanicNegativeTests, EPanicNegInvalidTestData));

	HBufC8 *string = HBufC8::NewMaxLC(iArrayGenAttrValue[0].Length());
	string->Des().Copy(iArrayGenAttrValue[0]);

	CASN1EncOctetString* attrString = CASN1EncOctetString::NewLC(*string);

	CPKCS10Attribute *genericAttr = NULL;
	TRAPD(err, genericAttr = CPKCS10Attribute::NewL(iArrayGenAttrOID[0], attrString));
	CleanupStack::Pop(attrString);
	CleanupStack::PopAndDestroy(string);

	if (err != KErrNone)
		{
		delete attrString;
		}
	else
		{
		delete genericAttr;
		}

	if (err != KErrBadDescriptor)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for NULL value for NewL
 	__ASSERT_ALWAYS((numAttr > 1), User::Panic(KPanicNegativeTests, EPanicNegInvalidTestData));

	string = HBufC8::NewMaxLC(iArrayGenAttrValue[1].Length());
	string->Des().Copy(iArrayGenAttrValue[1]);

	if (*string == KNullDesC8)
		{
		attrString = NULL;
		}
	else
		{
		attrString = CASN1EncOctetString::NewLC(*string);
		}

	TRAP(err, genericAttr = CPKCS10Attribute::NewL(iArrayGenAttrOID[1], attrString));
	if (attrString != NULL)
		{
		CleanupStack::Pop(attrString);
		}
	CleanupStack::PopAndDestroy(string);

	if (err != KErrNone)
		{
		delete attrString;
		}
	else
		{
		delete genericAttr;
		}

	if (err != 	KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for NULL value for AddValueL
 	__ASSERT_ALWAYS((numAttr > 2), User::Panic(KPanicNegativeTests, EPanicNegInvalidTestData));
	string = HBufC8::NewMaxLC(iArrayGenAttrValue[2].Length());
	string->Des().Copy(iArrayGenAttrValue[2]);

	attrString = CASN1EncOctetString::NewLC(*string);

	TRAP(err, genericAttr = CPKCS10Attribute::NewL(iArrayGenAttrOID[2], attrString));
	CleanupStack::Pop(attrString);
	CleanupStack::PopAndDestroy(string);

	// Should not be an error
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		}

	// Try to add a NULL value
	TRAP(err, genericAttr->AddValueL(NULL));
	delete genericAttr;

	// Should return an error
	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for invalid attribute for ResetL
	string = HBufC8::NewMaxLC(iArrayGenAttrValue[2].Length());
	string->Des().Copy(iArrayGenAttrValue[2]);

	attrString = CASN1EncOctetString::NewLC(*string);

	TRAP(err, genericAttr = CPKCS10Attribute::NewL(iArrayGenAttrOID[2], attrString));
	CleanupStack::Pop(attrString);

	// Should not be an error
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		}

	// Try to reset with an invalid attribute OID
	attrString = CASN1EncOctetString::NewLC(*string);
	TRAP(err, genericAttr->ResetL(KNullDesC, attrString));

	if (err != KErrNone)
		{
		delete attrString;
		}

	CleanupStack::Pop(attrString);
	CleanupStack::PopAndDestroy(string);
	delete genericAttr;

	// Should return an error
	if (err != 	KErrBadDescriptor)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for NULL value for ResetL
	string = HBufC8::NewMaxLC(iArrayGenAttrValue[2].Length());
	string->Des().Copy(iArrayGenAttrValue[2]);

	attrString = CASN1EncOctetString::NewLC(*string);

	TRAP(err, genericAttr = CPKCS10Attribute::NewL(iArrayGenAttrOID[2], attrString));
	CleanupStack::Pop(attrString);
	CleanupStack::PopAndDestroy(string);

	// Should not be an error
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		}

	// Try to reset with a NULL value
	TRAP(err, genericAttr->ResetL(iArrayGenAttrOID[2], NULL));
	delete genericAttr;

	// Should return an error
	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

TVerdict CPKCS10NegPKCS9ChallengePasswordAttr::doTestStepL()
	{
	// Set it to pass initially
	SetTestStepResult(EPass);

	//////////////////////////////////////////////////////////////////////////////
	// Test for empty password for NewL
	HBufC8 *passwordString = HBufC8::NewMaxLC(iChallengePassword.Length());
	passwordString->Des().Copy(iChallengePassword);

	CPKCS9ChallengePasswordAttr *chPasswordAttr = NULL;
	TRAPD(err, chPasswordAttr = CPKCS9ChallengePasswordAttr::NewL(*passwordString));
	CleanupStack::PopAndDestroy(passwordString);

	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for big password for ResetL
	TRAP(err, chPasswordAttr = CPKCS9ChallengePasswordAttr::NewL(_L8("password")));

	// Should not fail
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		}

	TRAP(err, chPasswordAttr->ResetL(KHugeString));
	delete chPasswordAttr;

	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

TVerdict CPKCS10NegPKCS9ExtensionRequestAttr::doTestStepL()
	{
	// Set it to pass initially
	SetTestStepResult(EPass);

	//////////////////////////////////////////////////////////////////////////////
	// Test for empty attribute OID for NewL
 	TInt numAttr = iArrayV3AttrOID.Count();
 	__ASSERT_ALWAYS((numAttr > 0), User::Panic(KPanicNegativeTests, EPanicNegInvalidTestData));

	CX509CertExtension* x509CertExtn = CX509CertExtension::NewL(iArrayV3AttrOID[0], iArrayV3AttrCritical[0], _L8("value"));

	CPKCS9ExtensionRequestAttr *chExtReqAttr = NULL;
	TRAPD(err, chExtReqAttr = CPKCS9ExtensionRequestAttr::NewL(*x509CertExtn));

	delete x509CertExtn;

	if (err != KErrBadDescriptor)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for empty attribute OID for AddExtensionL
 	__ASSERT_ALWAYS((numAttr > 1), User::Panic(KPanicNegativeTests, EPanicNegInvalidTestData));

	x509CertExtn = CX509CertExtension::NewLC(iArrayV3AttrOID[1], iArrayV3AttrCritical[1], _L8("value"));
	chExtReqAttr = CPKCS9ExtensionRequestAttr::NewLC(*x509CertExtn);

	CX509CertExtension *x509CertExtnNew = CX509CertExtension::NewLC(iArrayV3AttrOID[0], iArrayV3AttrCritical[0], _L8("value"));
	TRAP(err, chExtReqAttr->AddExtensionL(*x509CertExtnNew));

	CleanupStack::PopAndDestroy(3, x509CertExtn);	// chExtReqAttr, x509CertExtnNew

	if (err != KErrBadDescriptor)
		{
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

TVerdict CPKCS10NegPKCS10Attributes::doTestStepL()
	{
	// Set it to pass initially
	SetTestStepResult(EPass);

	//////////////////////////////////////////////////////////////////////////////
	// Test for NULL attribute for AddPKCSAttributeL
	CPKCS10Attributes *attrList = NULL;
	attrList = CPKCS10Attributes::NewLC();
	TRAPD(err, attrList->AddPKCSAttributeL(NULL));
	CleanupStack::PopAndDestroy(attrList);

	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

TVerdict CPKCS10NegPKCS10Request::doTestStepL()
	{
	// Set it to pass initially
	SetTestStepResult(EPass);

	//////////////////////////////////////////////////////////////////////////////
	// Test for invalid DN for NewL
	CActiveScheduler* sch = new(ELeave) CActiveScheduler;   
	CActiveScheduler::Install(sch);

	// Import the key
	CPKCS10NegTesterActive* activeObj = new (ELeave) CPKCS10NegTesterActive(Logger());
	CCTKeyInfo *keyInfo = activeObj->doImportKeyL(this);

	// Try to generate a cert req
	CX500DistinguishedName* dn = NULL;
	CPKCS10Request* req = NULL;
	TRAPD(err, req = CPKCS10Request::NewL(*dn, *keyInfo));

	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for EDH for SetDigestAlgL
    CArrayPtrFlat<CX520AttributeTypeAndValue>* array = new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue>(7);
	CleanupStack::PushL(array);
	dn = CX500DistinguishedName::NewL(*array);
	CleanupStack::PushL(dn);

	req = CPKCS10Request::NewL(*dn, *keyInfo);

	TRAP(err, req->SetDigestAlgL(EDH));

	delete req;
	array->ResetAndDestroy();
	CleanupStack::PopAndDestroy(2, array); //array, dn

	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for NULL DN for SetDistinguishedNameL
    array = new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue>(7);
	CleanupStack::PushL(array);
	dn = CX500DistinguishedName::NewL(*array);
	CleanupStack::PushL(dn);

	req = CPKCS10Request::NewL(*dn, *keyInfo);

	CX500DistinguishedName* nullDN = NULL;
	TRAP(err, req->SetDistinguishedNameL(*nullDN));

	delete req;
	array->ResetAndDestroy();
	CleanupStack::PopAndDestroy(2, array); //array, dn

	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	//////////////////////////////////////////////////////////////////////////////
	// Test for NULL for SetKeyInfoL
    array = new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue>(7);
	CleanupStack::PushL(array);
	dn = CX500DistinguishedName::NewL(*array);
	CleanupStack::PushL(dn);

	req = CPKCS10Request::NewL(*dn, *keyInfo);

	CCTKeyInfo* nullKeyInfo = NULL;
	TRAP(err, req->SetKeyInfoL(*nullKeyInfo));

	delete req;
	array->ResetAndDestroy();
	CleanupStack::PopAndDestroy(2, array); //array, dn

	if (err != KErrArgument)
		{
		SetTestStepResult(EFail);
		}

	delete activeObj;		// Will release keyInfo
	delete sch;

	return TestStepResult();
	}

