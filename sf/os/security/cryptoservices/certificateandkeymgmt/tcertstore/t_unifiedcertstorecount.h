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
* CUnifiedCertStoreCount class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_UNIFIEDCERTSTORECOUNT_H__
#define __T_UNIFIEDCERTSTORECOUNT_H__

#include "t_certstoreactions.h"

class CUnifiedCertStoreCount : public CCertStoreTestAction
	{
public:
	virtual ~CUnifiedCertStoreCount();
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void PerformCancel();
	virtual void Reset();

protected:
	CUnifiedCertStoreCount(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	void DoReportAction();
	void DoCheckResult(TInt aError);
	virtual void DoPerformAction() = 0;
	virtual void DoWriteResult() = 0;

protected:
	TInt iCount;
	TKeyIdentifier iExpectCount;
	TKeyIdentifier iStoreIndex;
	};

#endif

