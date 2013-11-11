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
// Defines ECom Test to determine performance of ECom client calls during different stages of start-up.
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef	TE_ECOMACCUMULATEDCLIENTREQUESTPERFTESTSTEP_H
#define	TE_ECOMACCUMULATEDCLIENTREQUESTPERFTESTSTEP_H

#include "Te_EComPerfTestStep.h"

_LIT(KEComAccumulatedClientRequestsPerfTest, "EComAccumulatedClientRequestsPerfTest");
        
/** 
 Test to determine performance of ECom client calls during different stages of start-up.
 */
class CEComAccumulatedClientRequestsPerfTest : public CEComPerfTestBase
	{
public:
	CEComAccumulatedClientRequestsPerfTest();
	~CEComAccumulatedClientRequestsPerfTest();
	virtual TVerdict doTestStepL(void);
	// doTestStepPreamble and doTestStepPostamble are implemented in CEComPerfTestBase, 
	// which performs initialization common to all ECom performance tests	
	
private:
	static const TStartupStateIdentifier iMeasuredStartupStates[];
	static const TInt iNumMeasuredStartupStates;
#ifdef __ECOM_SERVER_PERFORMANCE__
	static const TReal iMaxTimeForRequests[][EEComNumClientRequestTypes];
#endif // #ifdef __ECOM_SERVER_PERFORMANCE__
	};
	
#endif // #define TE_ECOMACCUMULATEDCLIENTREQUESTPERFTESTSTEP_H
