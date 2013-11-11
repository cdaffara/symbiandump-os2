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
* CUnifiedCertStoreWritableCertStoreCount class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_UNIFIEDCERTSTOREWRITABLECERTSTORECOUNT_H__
#define __T_UNIFIEDCERTSTOREWRITABLECERTSTORECOUNT_H__

#include "t_unifiedcertstorecount.h"

class CUnifiedCertStoreWritableCertStoreCount : public CUnifiedCertStoreCount
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
		const TTestActionSpec& aTestActionSpec);
	virtual ~CUnifiedCertStoreWritableCertStoreCount();

protected:
	virtual void DoPerformAction();
	virtual void DoWriteResult();

private:
	CUnifiedCertStoreWritableCertStoreCount(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	};

#endif

