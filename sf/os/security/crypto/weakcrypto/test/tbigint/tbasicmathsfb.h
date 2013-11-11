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


#ifndef __TBASICMATHSFB_H__
#define __TBASICMATHSFB_H__

#include "t_testaction.h"
#include <e32std.h>

class TInteger;

class CBasicMathsFB : public CTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CBasicMathsFB();
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoReportAction(void);
	virtual void DoCheckResult(TInt);

private:
	CBasicMathsFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void AddSub(const TInteger& aInitial, TUint aRange);
	void MulDiv(const TInteger& aInitial, TUint aRange);

	RFs& iFs;
	TUint iIterations;
	HBufC8* iBody;
};

#endif
