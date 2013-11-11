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
* transient_hometime_float.h
* 
*
*/



#if (!defined __transient_hometime_float_STEP_H__)
#define __transient_hometime_float_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_floating_scheduleSuiteStepBase.h"

class Ctransient_hometime_floatStep : public CTe_floating_scheduleSuiteStepBase
	{
public:
	Ctransient_hometime_floatStep();
	~Ctransient_hometime_floatStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(Ktransient_hometime_floatStep,"transient_hometime_floatStep");

#endif
