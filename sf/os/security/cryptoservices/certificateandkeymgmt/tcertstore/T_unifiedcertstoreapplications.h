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
* CUnifiedCertStoreApplications class implementations
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_UNIFIEDCERTSTOREAPPLICATIONS_H__
#define __T_UNIFIEDCERTSTOREAPPLICATIONS_H__

#include "t_certstoreactions.h"

class CUnifiedCertStoreApplications : public CCertStoreTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	virtual ~CUnifiedCertStoreApplications();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

private:
	CUnifiedCertStoreApplications(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void GetApplications(TRequestStatus& aStatus);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	enum TState
		{
		EGetCert,
		EGettingApplications,
		EFinished
		};

private:
	TState iState;
	TCertLabel iCertificateLabel;
	RMPointerArray<CCTCertInfo> iCertInfos;
	RArray<TUid> iExpectedApplications;
	CCTCertInfo* iCertInfoForApplications;
	RArray<TUid> iApplications;
	CCertAttributeFilter* iFilter;
	};

#endif

