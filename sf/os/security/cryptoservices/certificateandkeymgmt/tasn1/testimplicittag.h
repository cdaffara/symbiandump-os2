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
* Test object for implicitly changing tagging on ASN1 DER encoded objects
*
*/


#ifndef __TESTIMPLICITTAG_H__
#define __TESTIMPLICITTAG_H__

#include "testbase.h"
#include <asn1dec.h>

class CTestImplicitTag : public CTestBase
	{
	public:
		static CTestImplicitTag* NewL(CASN1NormalTest &aASN1Action);

		// Overridden from CTestBase
		void GetName(TDes& aBuf);
		TBool PerformTestsL(CConsoleBase& aConsole);
	private:
		CTestImplicitTag(CASN1NormalTest &aASN1Action);
		TBool PerformTest(CConsoleBase& aConsole, const TASN1Class &aClass, const TUint &aTest, const TInt &aTestNumber, const TInt &aTotalTests);
		void FillParameterArray(void);
	};

#endif

