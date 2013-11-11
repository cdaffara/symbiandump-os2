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

#ifndef __TMESSAGE_H__
#define __TMESSAGE_H__

#include "t_testaction.h"

class TTestActionSpec;

class CTestActionMessage : public CTestAction
	{
public:
	static CTestActionMessage* NewL(CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	static CTestActionMessage* NewLC(CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CTestActionMessage();
	virtual void PerformCancel() {};
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformMessage();
	virtual void Reset();

private:
	CTestActionMessage(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	HBufC8* iMessage;
	};

#endif

