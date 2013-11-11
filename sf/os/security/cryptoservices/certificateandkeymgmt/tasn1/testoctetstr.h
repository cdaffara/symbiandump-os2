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
* Test object for octet string types
*
*/


#ifndef __TESTOCTETSTR_H__
#define __TESTOCTETSTR_H__

#include "testbase.h"

class CTestOctetString : public CTestBase
	{
	public:
		static CTestOctetString* NewL(CASN1NormalTest &aASN1Action);
		~CTestOctetString();

		// Overridden from CTestBase
		void GetName(TDes& aBuf);
		TBool PerformTestsL(CConsoleBase& aConsole);
		void ConstructL(void);

	private:
		CTestOctetString(CASN1NormalTest &aASN1Action);
		TBool PerformTest(CConsoleBase& aConsole, const TPtr8 &toEncodePtr, const TInt &aTestNumber, const TInt &aTotalTests);
		void FillParameterArray(void); 

	private:


		HBufC8* toEncodeBuf;
		HBufC8* encodedBuf;
	};

#endif

