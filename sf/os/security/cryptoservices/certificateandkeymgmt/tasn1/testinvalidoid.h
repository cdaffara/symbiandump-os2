/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Test object for ASN1 OID objects
*
*/


#ifndef __TESTINVALIDOID_H__
#define __TESTINVALIDOID_H__

#include "testbase.h"

class CTestInvalidOID : public CTestBase
	{
	public:
		static CTestInvalidOID* NewL(CASN1NormalTest &aASN1Action);

		// Overridden from CTestBase
		void GetName(TDes& aBuf);
		TBool PerformTestsL(CConsoleBase& aConsole);

	private:
		CTestInvalidOID(CASN1NormalTest &aASN1Action);
		void FillParameterArray(void);
	};

#endif

