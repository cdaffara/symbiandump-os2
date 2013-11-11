/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __TPERFORMANCETEST_H__
#define __TPERFORMANCETEST_H__

#include "cryptotestaction.h"

class CSymmetricCipher;

class CPerformanceTest : public CCryptoTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CPerformanceTest();
protected:
	CPerformanceTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
protected:
	void DoPerformActionL();
	void DoPerformPrerequisiteL();	
protected:
	CSymmetricCipher* iEncryptor;
	CSymmetricCipher* iDecryptor;
};

#endif
