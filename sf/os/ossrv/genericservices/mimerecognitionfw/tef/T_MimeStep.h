// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __T_MIME_STEP_H__)
#define __T_MIME_STEP_H__
#include <test/testexecutestepbase.h>
#include "TEmimeTestServer.h"
#include <apmfndr.h>
#include <apmrec.h>
#include <s32file.h>

class CTestDataRecognizerType	: public CApaDataRecognizer
	{
public:
		static inline CTestDataRecognizerType * NewDataRecogTypeL(RFs &aFs);
		CTestDataRecognizerType(RFs &aFs);
		~CTestDataRecognizerType();
		inline void AddDataL(CApaDataRecognizerType* aDataRecognizerType)
		{
			 AddDataRecognizerType(aDataRecognizerType);
		};
		inline TInt RemoveData( CApaDataRecognizerType* aDataRecognizerType)
		{
			return (RemoveDataRecognizerType(aDataRecognizerType));
		};
		
	
		
	};

class CT_MimeStep : public CTestStep
	{
public:
	CT_MimeStep();
	~CT_MimeStep();
//	virtual TVerdict doTestStepPreambleL();
//	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	void testScanningMimeTheRecognizerL();
	void testRecognizersL();
	void testTDataTypeL();
	void DoTest(TInt aReturnValue);
	void testRecognizer(const CApaScanningDataRecognizer::TRecognizer& aRec);
	void ChkConfAndMimeL(RFs &aIfs,const TUid &aUid);
	void ChkLockAndUnlockL(RFs &aIfs);
	
private:
	CApaScanningDataRecognizer* iRecognizer;
	RFs iFs;
	CTrapCleanup* iTrapCleanup;
	CTestDataRecognizerType * iTestDataRecogType;
	CApaDataRecognizerType * iData;
	};

_LIT(KT_MimeStep,"T_Mime");
#endif

