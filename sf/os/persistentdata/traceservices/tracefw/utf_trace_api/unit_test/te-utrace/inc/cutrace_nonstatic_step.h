/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file UTrace0Step.h
 @internalTechnology
*/
#if (!defined __CUTRACE_PRINTF_CHAR_STEP_H__)
#define __CUTRACE_PRINTF_CHAR_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_utracesuitestepbase.h"


class CUTrace_nonStatic_Step : public CTe_UTraceSuiteStepBase
	{
public:
	CUTrace_nonStatic_Step();
	~CUTrace_nonStatic_Step();
//	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
//	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KUTrace_nonStatic_Step,"CUTrace_nonStatic_Step");

#endif
