/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TACTIONINCREMENTALLEGACY_H_
#define TACTIONINCREMENTALLEGACY_H_

#include "cryptotestaction.h"
#include "padding.h"
#include <symmetric.h>

class CActionIncrementalLegacy : public CCryptoTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CActionIncrementalLegacy();	
protected:
	CActionIncrementalLegacy(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
protected:
	virtual void DoPerformActionL();
	virtual void DoDoPerformActionL();
	virtual void DoPerformPrerequisiteL();
	virtual void DoPerformPostrequisiteL();
private:
	CSymmetricCipher* iEncryptor;
	CSymmetricCipher* iDecryptor;
};


#endif /*TACTIONINCREMENTALLEGACY_H_*/
