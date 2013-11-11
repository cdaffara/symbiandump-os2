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
// Defines the ECom Performance Logger for the SSA discovery.
// 
//

/**
 @file
 @internalComponent
*/

 
#ifndef	TE_ECOMPERFLOGGERSTEP_H
#define TE_ECOMPERFLOGGERSTEP_H

#include "Te_EComPerfTestStep.h"

_LIT(KEComPerfLogger, "EComPerfLogger");
        
class CEComPerfLogger : public CEComPerfTestBase
	{
public:
	CEComPerfLogger();
	~CEComPerfLogger();
	virtual TVerdict doTestStepL(void);
	// doTestStepPreamble and doTestStepPostamble are implemented in CEComPerfTestBase, 
	// which performs initialization common to all ECom performance tests	
	};
	
#endif //TE_ECOMPERFLOGGERSTEP_H
