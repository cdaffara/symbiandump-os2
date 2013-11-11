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
* Test object outputting to \System\TASN1_test_output on system drive.
* Output has sample encoding of each data type, wrapped up
* in a sequence.  Should be readable with DumpASN1
*
*/


#ifndef __TESTOUTPUT_H__
#define __TESTOUTPUT_H__

#include "testbase.h"

class CASN1EncBase;

class CTestOutput : public CTestBase
	{
	public:
		static CTestOutput* NewL(CASN1NormalTest &aASN1Action);

		// Overridden from CTestBase
		void GetName(TDes& aBuf);

	private:
		CTestOutput(CASN1NormalTest &aASN1Action);
		TBool PerformTestsL(CConsoleBase& aConsole);
		void FillParameterArray(void){}

	private:
		CASN1EncBase* MakeEncoderLC(const TBool aNest = ETrue) const;
		CASN1EncBase* MakeEncObjLC(const TUint aIndex) const;
	};

#endif

