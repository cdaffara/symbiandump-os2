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
 @internalTechnology
*/

#ifndef __T_CERTSTOREACTIONSFILECERTSTORE_H__
#define __T_CERTSTOREACTIONSFILECERTSTORE_H__

#include "t_certstoreactions.h"
#include "t_testactionspec.h"

class CCreateFileCertStore : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs &aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs &aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	virtual ~CCreateFileCertStore();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CCreateFileCertStore(RFs &aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	void HandleEInit();

	void DoReportAction();
	void DoCheckResult(TInt aError);
	void CreateStoreL(const TDesC& aFileName, RFs& aFs);

private:
	enum TState
		{
		EInit,
		EFinished
		};

private:
	TState iState;
	RFs& iFs;

	HBufC* iFileName;
	};

#endif

