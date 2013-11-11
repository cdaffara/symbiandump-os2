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
 @file te_regconcurrentteststep1.h
 @internalTechnology
*/
#if (!defined __TE_REG_WAIT_STEP_H__)
#define __TE_REG_WAIT_STEP_H__
#include "te_regconcurrentteststepbase.h"

class CTe_RegWaitStep : public CTe_RegConcurrentTestStepBase
	{
public:
	CTe_RegWaitStep();
	~CTe_RegWaitStep();
	virtual TVerdict doTestStepL();
	
// Please add/modify your class members here:
private:
	};

_LIT(KTe_RegWaitStep,"Te_RegWaitStep");

#endif
