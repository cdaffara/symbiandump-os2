/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_FILETOKENS_H__
#define __T_FILETOKENS_H__

#include "t_testaction.h"

_LIT8(KServerOOMTestStart, "startserveroom");
_LIT8(KServerOOMTestStop, "stopserveroom");
_LIT8(KCheckServerHeapError, "checkserverheaperror");

/**
 * A test action that turns on filetokens server-side OOM testing.
 */
NONSHARABLE_CLASS(CServerOOMTestStart) : public CTestAction
	{
public:
	IMPORT_C static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
									  const TTestActionSpec& aTestActionSpec);
	virtual ~CServerOOMTestStart();
	
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CServerOOMTestStart(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	};

/**
 * A test action that turns off filetokens server-side OOM testing.
 */
NONSHARABLE_CLASS(CServerOOMTestStop) : public CTestAction
	{
public:
	IMPORT_C static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
									  const TTestActionSpec& aTestActionSpec);
	virtual ~CServerOOMTestStop();
	
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CServerOOMTestStop(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	};

/**
 * A test actions to check whether the server has leaked memory.
 *
 * This works by checking for the presence of a file that the server writes when
 * shutting down, if it detects it has leaked memory.  This action must
 * therefore be called after the filetokens server has shut down - ie, there
 * must be an action to delete the keystore and an action to wait 3 seconds for
 * the server to shutdown, before this one.
 */
NONSHARABLE_CLASS(CCheckServerHeapError) : public CTestAction
	{
public:
	IMPORT_C static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CCheckServerHeapError();
	
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();
	
private:
	CCheckServerHeapError(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	RFs& iFs;
	};

#endif
