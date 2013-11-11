/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TCMSSTEP_H
#define TCMSSTEP_H

#include <test/testexecutestepbase.h>
#include "tpkcs7server.h"
#include <x509cert.h>
#include <cmssignerinfo.h>


class CDecPKCS8Data;
class MPKCS8DecodedKeyPairData;
class CRSAPrivateKey;
class CDSAPrivateKey;
class CMessageDigest;
class CASN1EncBase;
class CCmsSignedObject;
class CEncapsulatedContentInfo;
class CCmsSignerInfo;


/**
The base class for testing CMS data types
*/
class CTCmsBaseStep : public CTestStep
	{
public:
	CTCmsBaseStep();
	~CTCmsBaseStep();
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	
protected:
	HBufC8*	readFileL (TPtrC tag);
	HBufC8* CreateDEREncodingLC(const CASN1EncBase& aEncoding);
	void OutputResultToFileL(const TDesC8& aSignature);
	TVerdict doOOMTestL();
	virtual void doTestL() = 0;
	TInt CovertContentDataTypeNameToDataType(const TDesC& aDataTypeName);
	
protected:
	RFs	iFs;
	HBufC8*	iDataContent;
	TInt iExpectedDataType;
	HBufC8* iExpectedEncoding;
	TInt iExpectedResult;
	TBool iIsOOMTest;		
	};

/**
The class for testing CMS Content Info
*/
class CTCmsContentInfoDecodingStep : public CTCmsBaseStep
	{
public:
	CTCmsContentInfoDecodingStep();
	~CTCmsContentInfoDecodingStep();
	void doTestL();		
	};

/**
The class for testing CMS data type
*/
class CTCmsDataStep : public CTCmsBaseStep
	{
public:
	CTCmsDataStep();
	~CTCmsDataStep();
	void doTestL();
protected:
	void DecodingAndCheckL(TDesC8& aRawData);
	};

/**
The class for testing CMS data type Decoding
*/
class CTCmsDataDecodingStep : public CTCmsDataStep
	{
public:
	CTCmsDataDecodingStep();
	~CTCmsDataDecodingStep();
	void doTestL();
	};

/**
The base class for testing CMS signed data and PKCS7 signed data
encoding, decoding and validation. OOM test is configurable in this
test base step
*/
class CTSignedDataBaseStep : public CTCmsBaseStep
	{
public:
	CTSignedDataBaseStep();
	~CTSignedDataBaseStep();
	TVerdict doTestStepPreambleL();
	
protected:
	CMessageDigest* CreateHashLC(TAlgorithmId aAlgorithmId);
	TInt CovertHashNameToAlgorithmId(const TDesC& aHashName);
	TInt CovertCertificateNameToCertificateType(const TDesC& aCertificateName);
	
protected:	
	RPointerArray<CDecPKCS8Data> iDecPKCS8Data;
	RPointerArray<CX509Certificate> iCertificates;
	RArray<TInt> iSignerInfoVersion;
	RArray<TInt> iHashName;
	RArray<TBool> iValidateResults;
	RArray<TBool> iSignedAttributePresent;
	RArray<TBool> iUnSignedAttributePresent;
	TInt iContentType;
	TBool iIsHashAvailable;
	TBool iIsDetached;
	TBool iTwoStepCreation;
	TBool iRsaAlgorithm;
	TBool iCertificateSetPresent;
	TBool iCRLsSetPresent;
	TInt iSignedDataVersion;
	TInt iAlgorithmCount;
	TInt iCertsCount;
	TInt iSignerCount;
	CX509Certificate* iRootCertificate;
	CX509Certificate* iAdditionalCertificate;
	TBool iNoCertSet;
	TBool iValidateUsingUserCerts;
	TBool iNoSigning;
	TInt iAdditionalEncodedCertificateType;
	HBufC8* iAdditionalEncodedCertificate;
	TBool iNoValidationTest;
	TBool iValidationDetachedWithoutInput;
	};

/**
The class test encoding, decoding and validation of the CMS signed data object
*/
class CTCmsSignedDataStep : public CTSignedDataBaseStep
	{
public:
	CTCmsSignedDataStep();
	~CTCmsSignedDataStep();
	void doTestL();
	
protected:
	void CheckAndValidateSignedDataL(TDesC8& aRawData);
	void CheckSignedDataFieldsL(const CCmsSignedObject& aSignedData);
	void CheckEncapsulatedContentFieldsL(const CEncapsulatedContentInfo& encapContentInfo);
	void CheckAlgorithmSetFieldsL(const RPointerArray<CX509AlgorithmIdentifier>& aAlgorithms);
	void CheckCertificateSetFieldsL(const CCmsSignedObject& aSignedData);
	void CheckSignerInfoFieldsL(const RPointerArray<CCmsSignerInfo>& signerInfos);
	};

/**
The class test decoding and validation of the CMS signed data object
*/
class CTCmsSignedDataDecodingStep : public CTCmsSignedDataStep
	{
public:
	CTCmsSignedDataDecodingStep();
	~CTCmsSignedDataDecodingStep();
	void doTestL();
	};

#endif //TCMSSTEP_H
