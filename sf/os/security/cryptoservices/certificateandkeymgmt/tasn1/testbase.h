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
* Defines interface that all test objects comply with
*
*/


#ifndef __TESTBASE_H__
#define __TESTBASE_H__

#include <e32base.h>
#include "t_testactionspec.h"
#include "testparameter.h"
#include "t_testaction.h"


class CConsoleBase;
class CASN1NormalTest;

#define KErrASN1EncodingError 100

class CTestBase : public CBase
	{
	public:
		// What's the name of this test?
		virtual void GetName(TDes& aBuf) = 0;
		// What's it parameter format in script file

		// Run the test, Leave if things go wrong (ie alloc failure).
		// Leaving with KErrASN1EncodingError means a problem with the
		// encoder or the decoder functionality.
		virtual TBool PerformTestsL(CConsoleBase& aConsole) = 0;
		CTestAction::TScriptError ConstructL(const TTestActionSpec& aTestActionSpec);
		virtual ~CTestBase();

	protected:
		CTestBase(CASN1NormalTest &aASN1Action);
		void OutputEncodingL(CConsoleBase& aConsole, TDesC8& aData);
		CTestAction::TScriptError CheckValueParametersL(const TDesC& aParameters);
		CTestAction::TScriptError CheckRandomParametersL(const TDesC& aParameters);
		TBool CountTests(TInt &aTotalTests);
		CASN1NormalTest &iASN1Action;

		// virtual functions
	protected:
		virtual void FillParameterArray(void) = 0;
		RArray<CTestParameter::TType> *iParameters;
		RPointerArray<CTestParameter> *iValues;
		
	public:
		TBuf<KMaxErrorSize> iSyntaxErrorDescription;
	};

#endif

