/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This contains the Teststep class which is used for verification of the pkcs12 file
* and log the results
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __TPKCS12LIBTESTSTEP01_H__
#define __TPKCS12LIBTESTSTEP01_H__

// System Include
#include <test/testexecutestepbase.h>

//User Include
#include "tpkcs12common.h"
#include <pkcs12.h>

const TInt KPositiveTest=1;
const TInt KNegativeTest=0;
const TInt KBagDataSecBufLen=10;
const TInt KPrivacyPwdStrLen=17;
const TInt KContentInfoBufLen=12;
const TInt KPasswordKeyStrLen=15;
const TInt KBagDataSectionStrLen=7;
const TInt KContentInfoKeyStrLen=11;
const TInt KAttributeBufLen=6;
const TInt KAttributeKeyStrLen=4;
const TInt KBagValBufLen=8;
const TInt KBagValKeyStrLen=5;

_LIT(KFriendlyNameOid,"1.2.840.113549.1.9.20");
_LIT(KLocalKeyOid,"1.2.840.113549.1.9.21");
_LIT(KMainSec,"main");
_LIT(KAuthSafeSec,"authsafe");
_LIT(KFilePath,"file");
_LIT(KIntegrityMode,"integritymode");
_LIT(KUnitTest,"unittest");
_LIT(KUnitTestValue,"FALSE");
_LIT(KPassword,"password");
_LIT(KIntegrityPassword,"integritypassword");
_LIT(KIntegrityValid,"integrity_valid");
_LIT(KPublicIntegrity,"public");
_LIT(KPrivacyPassword,"PrivacyPassword");
_LIT(KExpectedResult,"ExpectedResult");
_LIT(KVerifyIntgPass,"verifyintegritypassword");
_LIT(KMacPresent,"MacPresent");
_LIT(KMacValueDefault,"FALSE");
_LIT(KMacTestValue,"True");
_LIT(KContentInfoKey,"contentinfo");
_LIT(KContentInfoNum,"numcontentinfos");
_LIT(KBagDataSec,"BagData");
_LIT(KBagType,"BagType");
_LIT(KSafeBagsCount,"SafeBagsCount");
_LIT(KContentInfoType,"ContentInfoType");
_LIT(KBagAttributesKey,"attr");
_LIT(KBagAttributesNum,"numattributes");
_LIT(KBagAttributesID,"id");
_LIT(KBagAttributeValuesNum,"numvalues");
_LIT(KBagAttributeValue,"value");
_LIT(KAlgorithm,"Algorithm");
_LIT(KCertType,"CertType");
_LIT(KContentInfoPwd,"ContentInfoPrivacyPassword");
_LIT(KSafeBagPrivacyPwd,"safeBagprivacypassword");
_LIT(KBagValue,"BagValue");
_LIT(KEncBagValue,"EncodedBagValue");
_LIT(KMac,"Mac");
_LIT(KMacSalt,"MacSalt");
_LIT(KIterationCount,"IterationCount");
_LIT(KIntegrityPwd,"integritypassword");
_LIT(KExpectedError,"ExpectedError");

_LIT(KFriendlyName,"1.2.840.113549.1.9.20");
_LIT(KLocalKey,"1.2.840.113549.1.9.21");


/**
This is a teststep class wherin all the verification for the pkcs12 file
is done.
*/
class CPKCS12LibTestStep : public CTestStep
	{
public:
	CPKCS12LibTestStep ();
	~CPKCS12LibTestStep();
	//From CTestStep class
	TVerdict doTestStepL();
	
private:
	TBool ChkCIType(TDesC &aSecName,CSafeBagData &aBagData);
	TBool ChkBagType(TDesC &aSecName,CSafeBagData &aBagData,TInt aBagId);
	TBool ChkBagAttbutesL(TDesC &aSecName,CSafeBagData &aBagData);
	TBool ChkAtrSecL(TDesC &aSec,CSafeBagData &aBagData,TInt aIndx);
	TBool ChkAtrValL(TDesC &aSecName,CSafeBagData &aBagData,TInt aNumVal);
	TBool ChkAlgId(TDesC &aSecName,CSafeBagData &aBagData);
	TBool ChkCertId(TDesC &aSecName,CSafeBagData &aBagData);
	TBool VerifyMainSecL();
	TBool VerifyAuthSafe();
	TBool ChkBagDataL(TInt aTotalsafebags,TInt aTotalbags);
	TBool ChkSafeContsBagData(TDesC& aBagSec,CSafeContentBag &aSafeContntBagData);
	TBool ChkBagValL(TDesC &aSecName,CSafeBagData &aBagData);
	TBool ChkMacValueL();
	TBool CompareFileDataL(TDesC &aPValue,TDesC8 &aBufferVal);
	void ParsePrivacyPwdL();
	TBool VerifyExpectedError(const TDesC &aSecName);
	TBool ChkEncodedBagValL(TDesC &aSecName,CSafeBagData &aBagData);

private:
	/** privacy password of all the contentinfos in sequence if encrypted */
	RPointerArray<TDesC>iPrivacyPassword ;
	/** SafeBagData pointer */
	CSafeBagData *iBagData;
	/** positive or negative test */
	TBool iExpectedResult;
	/** flag for checking whether the verification function returns true or false */
	TBool  iActualResult;
	/** Unit test value */
	TBool iUnitTest;
	/** BagAttribute Value */
	TInt iBagAttrNumVal;
	/** Argument error. */
	TInt iExpectedError;
	/** ContentData is Set 0, if the contentData is absent. */
	TInt iContentDataPresent;
	/** PKCS12Parser object */
	CPkcs12Parser *iParser;
	/** File server object */
	RFs iFileSession;
	/** Set to True/False depending on the verification of the error. */ 
	TBool iUnitTestContinueFlag;
	
	};

/**
This is a teststep class wherin all the OOM Test for the pkcs12 library
is done.
*/
class CTPKCS12OOMStep : public CTestStep
	{
public:
	CTPKCS12OOMStep ();
	~CTPKCS12OOMStep();
	// From CTestStep class
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
private:
	HBufC8* ReadFileDataL(TPtrC tag);
protected:
	/** contains the data */
	HBufC8*	iRawData;
	/** contains the authsafe data */
	HBufC8* iAuthSafeData;
	/** File server object */
	RFs		iFs;
	};

_LIT(KStep, "PKCS12LibTestStep");
_LIT(KTPKCS12OOMStep,"PKCS12OOMTestStep");

#endif
