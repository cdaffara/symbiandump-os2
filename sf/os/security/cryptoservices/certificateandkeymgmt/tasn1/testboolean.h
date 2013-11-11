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
* Test object for ASN1 boolean objects
*
*/


#ifndef __TESTBOOLEAN_H__
#define __TESTBOOLEAN_H__

#include "testbase.h"

class CTestBoolean : public CTestBase
	{
	public:
		static CTestBoolean* NewL(CASN1NormalTest &aASN1Action);

		// Overridden from CTestBase
		void GetName(TDes& aBuf);
		TBool PerformTestsL(CConsoleBase& aConsole);
	private:
		CTestBoolean(CASN1NormalTest &aASN1Action);
		TBool PerformTest(CConsoleBase& aConsole, const TBool& aTest);
		void FillParameterArray(void);
	};

#endif

