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


/**
 @file 
 @internalTechnology
*/

#if (!defined __TPKCS7_STEP_H__)
#define __TPKCS7_STEP_H__
#include <test/testexecutestepbase.h>
#include "tpkcs7server.h"
#include "tpkcs7stepbase.h"

class CTPKCS7CorruptTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7CorruptTest();
	virtual TVerdict doTestStepL();
	};

class CTPKCS7ValidTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7ValidTest();
	virtual TVerdict doTestStepL();
	};

class CTPKCS7ContentTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7ContentTest();
	virtual TVerdict doTestStepL();
	};

class CTPKCS7CertificateTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7CertificateTest();
	virtual TVerdict doTestStepL();
	};

class CTPKCS7SignerTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7SignerTest();
	virtual TVerdict doTestStepL();
	};

class CTPKCS7EncryptedDataTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7EncryptedDataTest();
	virtual TVerdict doTestStepL();
	};
	
class CTPKCS7DigestInfoTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7DigestInfoTest();
	virtual TVerdict doTestStepL();
	};
	
class CTPKCS7EncryptedDataCorruptTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7EncryptedDataCorruptTest();
	virtual TVerdict doTestStepL();
	};
	
class CTPKCS7DigestInfoCorruptTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7DigestInfoCorruptTest();
	virtual TVerdict doTestStepL();
	};

class CTPKCS7OOMTest : public CTPKCS7StepBase
	{
public:
	CTPKCS7OOMTest();
    TVerdict doTestStepL();
	void doTestOOML();
public:
 	TInt iError;
	};

_LIT(KTPKCS7EncryptedDataTest, "EncryptedData");
_LIT(KTPKCS7DigestInfoTest, "DigestInfo");
_LIT(KTPKCS7EncryptedDataCorruptTest, "EncryptedDataCorrupt");
_LIT(KTPKCS7DigestInfoCorruptTest, "DigestInfoCorrupt");
_LIT(KTPKCS7OOMTest,"PKCS7OOM");
_LIT(KTCmsSignedDataTest,"CMSSignedData");
_LIT(KTCmsSignedDataDecodingTest,"CMSSignedDataDecoding");
_LIT(KTCmsDataTest,"CMSData");
_LIT(KTCmsDataDecodingTest,"CMSDataDecoding");
_LIT(KTCmsContentInfoDecodingTest,"CmsContentInfoDecoding");
_LIT(KTPKCS7SignedDataTest,"PKCS7SignedData");
_LIT(KTPKCS7CorruptTest, "Corrupt");
_LIT(KTPKCS7ValidTest, "Valid");
_LIT(KTPKCS7ContentTest, "Content");
_LIT(KTPKCS7CertificateTest, "Certificate");
_LIT(KTPKCS7SignerTest, "Signer");

#endif
