/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
*/
#if (!defined __BUR_STEP_H__)
#define __BUR_STEP_H__

#include <test/testexecutestepbase.h>
#include "tef_schsvr_bursuitesimstepbase.h"
#include <bautils.h>

class CBURStep : public CTefSchsvrBURSuiteSimStepBase
	{
public:
	CBURStep();
	~CBURStep();

	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL(); 	
private:
	TPtrC iData;
	};

_LIT(KBURStep,"BURStep");

#endif
