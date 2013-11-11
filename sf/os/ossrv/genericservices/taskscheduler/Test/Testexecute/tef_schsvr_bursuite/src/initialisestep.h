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
#if (!defined __INITIALISE_STEP_H__)
#define __INITIALISE_STEP_H__

#include <test/testexecutestepbase.h>
#include "tef_schsvr_bursuitestepbase.h"

class CInitialiseStep : public CTefSchsvrBURSuiteStepBase
	{
public:
	CInitialiseStep();
	~CInitialiseStep();

	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL(); 	

private:
	};

_LIT(KInitialiseStep,"InitialiseStep");

#endif
