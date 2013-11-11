/**
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
 @file
*/
#if (!defined __INITAILISE_DEF_STEP_H__)
#define __INITAILISE_DEF_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_centrep_BURSuiteStepBase.h"

class CinitialiseDefStep : public CTe_centrep_BURSuiteStepBase
	{
public:
	CinitialiseDefStep();
	~CinitialiseDefStep();
	virtual TVerdict doTestStepL();

private:
	};

_LIT(KinitialiseDefStep,"initialiseDefStep");

#endif
