/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TACTIONVECTOR_H__
#define __TACTIONVECTOR_H__

#include "cryptotestaction.h"
#include <symmetric.h>

class CBufferedEncryptor;	//	Forward declare
class CBufferedDecryptor;	//	Forward declare

class CActionVector : public CCryptoTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CActionVector();
protected:
	CActionVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void RunTest(void);
	void TestPerformance(void);
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformPrerequisiteL();
protected:
	CSymmetricCipher* iEncryptor;
	CSymmetricCipher* iDecryptor;
};

#endif
