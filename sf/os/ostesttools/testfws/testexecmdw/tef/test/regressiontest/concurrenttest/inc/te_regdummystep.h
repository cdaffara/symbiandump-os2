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
 @file te_regconcurrentteststep4.h
 @internalTechnology
*/
#if (!defined __TE_REG_DUMMY_STEP_H__)
#define __TE_REG_DUMMY_STEP_H__
#include "te_regconcurrentteststepbase.h"

class CTe_RegDummyStep : public CTe_RegConcurrentTestStepBase
	{
public:
	CTe_RegDummyStep();
	~CTe_RegDummyStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	};

_LIT(KTe_RegDummyStep,"Te_RegDummyStep");

#endif
