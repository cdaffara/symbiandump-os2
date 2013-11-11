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
// Declaration for class to test the performance of ECom client calls
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef	TE_ECOMCLIENTREQUESTPERFTESTSTEP_H
#define	TE_ECOMCLIENTREQUESTPERFTESTSTEP_H

#include "Te_EComPerfTestStep.h"
#include <ecom/ecom.h>

_LIT(KEComClientRequestsPerfTest, "EComClientRequestsPerfTest");
        
/** 
 Class to test the performance of ECom client calls
 */
class CEComClientRequestsPerfTest : public CEComPerfTestBase
	{
public:
	CEComClientRequestsPerfTest();
	~CEComClientRequestsPerfTest();
	TVerdict doTestStepL();
	// doTestStepPreamble and doTestStepPostamble are implemented in CEComPerfTestBase, 
	// which performs initialization common to all ECom performance tests	
private:
	void WaitForDllUnload();
	};
	
#endif //TE_ECOMCLIENTREQUESTPERFTESTSTEP_H
