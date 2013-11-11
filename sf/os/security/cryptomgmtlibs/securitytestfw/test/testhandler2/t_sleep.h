/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_SLEEP_H__
#define __T_SLEEP_H__

#include "t_testaction.h"

/**
 * A test action that sleeps for a specified length of time.
 */
class CSleep : public CTestAction
	{
public:
	static CTestAction* NewL(CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CSleep();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
private:
	CSleep(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
private:
	void DoReportAction();
	void DoCheckResult(TInt aError);
private:
	enum TState
		{
		ESleeping, 
		EFinished
		};
private:
	TState iState;
	RTimer iTimer;
	TUint iSeconds;	
	};

#endif
