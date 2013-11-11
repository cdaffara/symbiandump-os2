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


#ifndef __TACTIONMONTECARLO_H__
#define __TACTIONMONTECARLO_H__

#include "cryptotestaction.h"

class CBufferedEncryptor;	//	Forward declare
class CBufferedDecryptor;	//	Forward declare

const TInt KMonteCarloIterations = 10000;

class CActionMonteCarlo : public CCryptoTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CActionMonteCarlo();
protected:
	CActionMonteCarlo(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
protected:
	virtual void DoPerformPrerequisiteL();
	virtual void DoPerformActionL();
protected:
	void DoAESEncryptECB();
	void DoAESDecryptECB();
	void DoAESEncryptCBC();
	void DoAESDecryptCBC();
protected:
	CBufferedEncryptor* iEncrypt;
	CBufferedDecryptor* iDecrypt;
};

#endif	//	__TACTIONMONTECARLO_H__
