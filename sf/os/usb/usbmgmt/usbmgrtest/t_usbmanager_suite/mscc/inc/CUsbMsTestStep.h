/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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
 
#if (!defined __CUSBMSTESTSTEP_H__)
#define __CUSBMSTESTSTEP_H__

#include <e32std.h>
#include <test/testexecuteserverbase.h>
#include "CUsbMsClassController.h"

_LIT(KConfigItemTest, "CConfigItemTest");
NONSHARABLE_CLASS(CConfigItemTest) : public CTestStep
	{
public:
	inline CConfigItemTest(){SetTestStepName(KConfigItemTest);};
	virtual TVerdict doTestStepL(void);
	};

_LIT(KReadMsConfigTest, "CReadMsConfigTest");
NONSHARABLE_CLASS(CReadMsConfigTest) : public CTestStep
	{
public:
	inline CReadMsConfigTest(){SetTestStepName(KReadMsConfigTest);};
	virtual TVerdict doTestStepL(void);
	};


#endif
