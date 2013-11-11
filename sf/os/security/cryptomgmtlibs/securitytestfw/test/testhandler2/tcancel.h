/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
*/

#ifndef __TCANCEL_H__
#define __TCANCEL_H__

#include "t_testaction.h"

class CCancelStart : public CTestAction
	{
public:
	static CCancelStart* NewL(CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	static CCancelStart* NewLC(CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CCancelStart();
	virtual void PerformAction(TRequestStatus& aStatus);

private:
	CCancelStart(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	};

class CCancelStop : public CTestAction
	{
public:
	static CCancelStop* NewL(CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	static CCancelStop* NewLC(CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpecl);
	~CCancelStop();
	virtual void PerformAction(TRequestStatus& aStatus);

private:
	CCancelStop(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	};

#endif

