// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the ECom Performance Test for the SSA discovery.
// 
//

/**
 @file
 @internalComponent
*/

 
#ifndef	TE_ECOMPERFTESTSTEP_H
#define	TE_ECOMPERFTESTSTEP_H

#include <test/testexecutestepbase.h>
#include "EComSessionAux.h"

class CEComPerfTestBase : public CTestStep
	{
public:
	CEComPerfTestBase(const TDesC& aStepName);
	virtual ~CEComPerfTestBase();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
protected:
	void CheckPerformance(TReal aActualTime, TReal aMaxTime, TPtrC aTimingMessage);
#ifdef __ECOM_SERVER_PERFORMANCE__
	TPtrC ClientRequestTypeName(TEComClientRequestType aClientRequestType);
	TPtrC StartupStateName(TStartupStateIdentifier aStartupStateIdentifier);
#endif // #ifdef __ECOM_SERVER_PERFORMANCE__

#ifndef __ECOM_SERVER_PERFORMANCE__
	void MacroNotDefinedError();
#endif // #ifndef __ECOM_SERVER_PERFORMANCE__
	};
	
#endif // #ifndef TE_ECOMPERFTESTSTEP_H
