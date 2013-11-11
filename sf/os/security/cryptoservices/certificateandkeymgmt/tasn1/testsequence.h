/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Test object for ASN1 sequence objects
*
*/


#ifndef __TESTSEQUENCE_H__
#define __TESTSEQUENCE_H__

#include "testbase.h"

class CASN1EncSequence;

class CTestSequence : public CTestBase
	{
	public:
		static CTestSequence* NewL(CASN1NormalTest &aASN1Action);

		// Overridden from CTestBase
		void GetName(TDes& aBuf);
		TBool PerformTestsL(CConsoleBase& aConsole);

	private:
		CTestSequence(CASN1NormalTest &aASN1Action);
		CASN1EncSequence* MakeSequenceEncoderLC(TInt aMaxDepth, TInt aMinSize, TInt aMaxSize);
		TInt ReadAndCountL(const TDesC8& aBuf);
		TBool PerformTest(CConsoleBase& aConsole, const TInt &aTestDepth, const TInt &aTestSize, const TInt &aTestNumber, const TInt &aTotalTests);
		void FillParameterArray(void);

		TInt iObjectsEncoded;
	};

#endif

