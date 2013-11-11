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
* Test object for ASN1 generalized time objects
*
*/


#ifndef __TESTGENERALIZEDTIME_H__
#define __TESTGENERALIZEDTIME_H__

#include "testbase.h"
_LIT(KEncoder, "Encoder");
_LIT(KNoEncoder, "NoEncoder");

class CTestGeneralizedTime : public CTestBase
	{
	public:
		static CTestGeneralizedTime* NewL(CASN1NormalTest &aASN1Action);

		// Overridden from CTestBase
		void GetName(TDes& aBuf);
		TBool PerformTestsL(CConsoleBase& aConsole);

	private:
		CTestGeneralizedTime(CASN1NormalTest &aASN1Action);
		TBool PerformTestWithEncoderL(CConsoleBase& aConsole, const TTime &aTest, const TInt &aTestNumber, 
				const TInt &aTotalTests);
		TBool PerformTestWithoutEncoderL(CConsoleBase& aConsole, const TTime &aTest, const TInt &aTestNumber, 
				const TInt &aTotalTests, const TDesC& fraction = KNullDesC, const TDesC& offset = KNullDesC  );
		void FillParameterArray(void);

		static void OutputTime(CConsoleBase& aConsole, const TTime& aTime);
		
		TBool ValidateOutcome(CConsoleBase& aConsole, const TTime& originalTime, const TTime& decodedTime, const TInt &aTestNumber, 
				const TInt &aTotalTests);
			
	};

#endif

