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
* Declares test step classes for negative cases of PKCS#10 tests.
*
*/


/**
 @file
 @internalTechnology
*/

#include <f32file.h> 
#include <x500dn.h>
#include <pkcs10.h>  
#include <pkcs10attr.h>
#include <pkcs9attr.h>
#include <unifiedkeystore.h>

#if (!defined __PKCS10NEGATIVE_STEP_H__)
#define __PKCS10NEGATIVE_STEP_H__
#include <test/testexecutestepbase.h>
#include "tpkcs10stepbasev2.h"
#include "tcertrequeststep.h"

// Test steps
_LIT(KPKCS10NegPKCS10GenericAttr,			"PKCS10NegPKCS10GenericAttr");
_LIT(KPKCS10NegPKCS9ChallengePasswordAttr,	"PKCS10NegPKCS9ChPwdAttr");
_LIT(KPKCS10NegPKCS9ExtensionRequestAttr,	"PKCS10NegPKCS9ExtReqAttr");
_LIT(KPKCS10NegPKCS10Attributes,			"PKCS10NegPKCS10Attributes");
_LIT(KPKCS10NegPKCS10Request,				"PKCS10NegPKCS10Request");

// Forward declarations
class CCertificateRequestStep;
class CPKCS10Request;

// Helper active object to import key
class CPKCS10NegTesterActive : public CActive
	{
public:
	CPKCS10NegTesterActive(CTestExecuteLogger& aLogger);
	~CPKCS10NegTesterActive();

	CCTKeyInfo* doImportKeyL(CCertificateRequestStep *aStep);

	// from CActive
	void DoCancel() { return; };
	void RunL();
	TInt RunError(TInt aError);

private:

	CTestExecuteLogger& Logger(){return iLogger;}

private:
	// Log buffer
	TBuf<150> 					iLogInfo;
	RFs 						iFs;
  	CUnifiedKeyStore*			iKeyStore;
  	CCTKeyInfo*					iKeyInfo;
    CTestExecuteLogger& 		iLogger;
	TBool						iKeyImportStarted;
	TInt						iError;
	HBufC8*						iKeyData;
	CCertificateRequestStep*	iStep;
	};

class CPKCS10NegPKCS10GenericAttr : public CCertificateRequestStep
	{
public:
	TVerdict doTestStepL();
	};

class CPKCS10NegPKCS9ChallengePasswordAttr : public CCertificateRequestStep
	{
public:
	TVerdict doTestStepL();
	};

class CPKCS10NegPKCS9ExtensionRequestAttr : public CCertificateRequestStep
	{
public:
	TVerdict doTestStepL();
	};

class CPKCS10NegPKCS10Attributes : public CCertificateRequestStep
	{
public:
	TVerdict doTestStepL();
	};

class CPKCS10NegPKCS10Request : public CCertificateRequestStep
	{
public:
	TVerdict doTestStepL();
	};

#endif

