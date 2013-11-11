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


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_CERTSTOREACTIONSHANDLE_H__
#define __T_CERTSTOREACTIONSHANDLE_H__

#include "t_certstoreactions.h"

class CCertStoreHandle : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	virtual ~CCertStoreHandle();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CCertStoreHandle(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		EList,
		EHandle
		};

private:
	TState iState;
	CCertAttributeFilter* iFilter;
	RMPointerArray<CCTCertInfo> iCertInfos;
	TCertLabel iLabel;
	TCTTokenObjectHandle iHandle;
	};


#endif

