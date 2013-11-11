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
* tactiondecodepkcs5.h
*
*/


#ifndef __TACTIONTESTPKCS8_H__
#define __TACTIONTESTPKCS8_H__

#include "t_testaction.h"
#include <e32std.h>

class CActionTestPKCS8 : public CTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CActionTestPKCS8();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoReportAction(void);
	virtual void DoCheckResult(TInt);

private:
	CActionTestPKCS8(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void ReadInputFileL(const TDesC8& aFilename);
	void SetExpectedOutcomeL(const TDesC8& aOutcome);

	enum TOutcome
		{
		ENoMatch,
		EMatchesPKCS8,
		EMatchesEncryptedPKCS8
		};

	RFs& iFs;
	HBufC8* iInput;
	TOutcome iExpectedOutcome;
};

#endif
