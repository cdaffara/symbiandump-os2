/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CRYPTOTESTACTION_H__
#define __CRYPTOTESTACTION_H__

#include "t_testaction.h"
#include "t_input.h"

_LIT8(KVectorStart, "<vector>");
_LIT8(KVectorEnd, "</vector>");
_LIT8(KIncrementalStart, "<incremental>");
_LIT8(KIncrementalEnd, "</incremental>");
_LIT8(KKeyStart, "<key>");
_LIT8(KKeyEnd, "</key>");
_LIT8(KInputStart, "<input>");
_LIT8(KInputEnd, "</input>");
_LIT8(KOutputStart, "<output>");
_LIT8(KOutputEnd, "</output>");
_LIT8(KCipherTypeStart, "<ciphertype>");
_LIT8(KCipherTypeEnd, "</ciphertype>");
_LIT8(KIVStart, "<iv>");
_LIT8(KIVEnd, "</iv>");
_LIT8(KMonteCarloStart, "<MonteCarlo>");
_LIT8(KMonteCarloEnd, "</MonteCarlo>");
_LIT8(KEffKeyLenStart, "<effectiveKeyLength>");
_LIT8(KEffKeyLenEnd, "</effectiveKeyLength>");
_LIT8(KIterationsStart, "<iterations>");
_LIT8(KIterationsEnd, "</iterations>");
_LIT8(KRandDataSizeStart, "<randDataSize>"); //Used specifically in performance tests where random data is used rather than specific input and output data pairs
_LIT8(KRandDataSizeEnd, "</randDataSize>");  //Used specifically in performance tests where random data is used rather than specific input and output data pairs


class CCryptoTestAction : public CTestAction
{
public:
	CCryptoTestAction(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	~CCryptoTestAction();
public:
	virtual void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoReportAction(void);
	virtual void DoCheckResult(TInt);
	virtual void Reset(){}
protected:	
	virtual void DoPerformPrerequisiteL() = 0;
	virtual void DoPerformActionL() = 0;
	virtual void DoPerformPostrequisiteL() {}
protected:
	void Hex(HBufC8& string);
	void DoInputParseL(TDesC8& aScriptBuffer);
protected:
	enum TCipherType
	{
		EDESECB,
		EDESCBC,
		E3DESECB,
		E3DESCBC,
		EAESECB,
		EAESCBC,
		EAESMonteCarloEncryptECB,	//	These extra categories have been added
		EAESMonteCarloDecryptECB,	//	because the Monte-Carlo tests have to be
		EAESMonteCarloEncryptCBC,	//	split between encrypt and decrypt scripts 
		EAESMonteCarloDecryptCBC,	//	(too big for test framework in one chunk)
		ERC2ECB,
		ERC2CBC,
		ERC4,
		ECipherNull
	};
protected:
	RFs& iFs;
	HBufC8* iBody;
	HBufC8* iKey;
	HBufC8* iInput;
	HBufC8* iOutput;
	HBufC8* iIV;
	HBufC8* iEResult;
	HBufC8* iDResult;
	TInt iEffectiveKeyLen;
	TInt iIterationTime;
	TInt iEncryptIterations;
	TInt iDecryptIterations;
	TCipherType iCipherType;
	TTimeIntervalMicroSeconds iEncryptorCreateTime;
	TTimeIntervalMicroSeconds iDecryptorCreateTime;
	TInt iExpectedWeakResult;
	TInt iRandDataSize;
};

#endif	//	__CRYPTOTESTACTION_H__
