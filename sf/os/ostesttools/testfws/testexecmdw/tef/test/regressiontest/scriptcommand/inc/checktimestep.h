/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file checkresultstep.h
*/
#if (!defined __CHECKTIME_STEP_H__)
#define __CHECKTIME_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_regscriptcommandserver.h"

// __EDIT_ME__ - Create your own test step definitions
class CCheckTimeStep : public CTestStep
	{
public:
	CCheckTimeStep(CTe_RegScriptCommandServer* aServer);
	~CCheckTimeStep();
	virtual TVerdict doTestStepL();
	CTe_RegScriptCommandServer* Server() { return iServer; }
	
private:
	CTe_RegScriptCommandServer* iServer;
	};

/*@{*/
// Literals
_LIT(KCheckTimeStep,"CheckTime");
/*@{*/

#endif
