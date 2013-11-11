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


#ifndef __TACTIONDERIVEKEY_H__
#define __TACTIONDERIVEKEY_H__

#include "t_testaction.h"
#include <e32std.h>

class CActionDeriveKey : public CTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CActionDeriveKey();
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoReportAction(void);
	virtual void DoCheckResult(TInt);

private:
	CActionDeriveKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void Hex(HBufC8& string);

	RFs& iFs;
	HBufC8* iBody;
	HBufC8* iPasswd;
	HBufC8* iSalt;
	HBufC8* iKey;
	HBufC8* iOutput;
	HBufC8* iIterations;
	HBufC8* iKdf;
	TInt iLeaveInPerformAction;
};

#endif
