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
* testinteger.h
* Test class for big integer (TInteger) encoding/decoding
*
*/


#ifndef __TESTBIGINT_H__
#define __TESTBIGINT_H__


#include <bigint.h>

#include "testbase.h"


class CTestBigInt : public CTestBase
	{
	public:
		static CTestBigInt* NewL(CASN1NormalTest &aASN1Action);

		// Overridden from CTestBase
		void GetName(TDes& aBuf);
		TBool PerformTestsL(CConsoleBase& aConsole);

	private:
		CTestBigInt(CASN1NormalTest &aASN1Action);
		void OutputIntegerL(CConsoleBase& aConsole, RInteger& aInt);
		TBool PerformTest(CConsoleBase& aConsole, const RInteger &aTest, const TInt &aTestNumber, const TInt &aTotalTests);
		void FillParameterArray(void);
	};


#endif

