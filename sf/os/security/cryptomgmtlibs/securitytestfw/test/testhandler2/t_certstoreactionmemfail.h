/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_CERTSTOREACTIONMEMFAIL_H__
#define __T_CERTSTOREACTIONMEMFAIL_H__

#include "t_testaction.h"

class CMemFailStart : public CTestAction
	{
public:
	static CMemFailStart* NewL(CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	static CMemFailStart* NewLC(CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	~CMemFailStart();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus) {	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);};
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus) {	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);};
	virtual void PerformCancel();
	virtual void Reset();

private:
	CMemFailStart(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	};


class CMemFailStop : public CTestAction
	{
public:
	static CMemFailStop* NewL(CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	static CMemFailStop* NewLC(CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	~CMemFailStop();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus) {	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);};
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus) {	
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);};
	virtual void PerformCancel();
	virtual void Reset();

private:
	CMemFailStop(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	};

#endif

