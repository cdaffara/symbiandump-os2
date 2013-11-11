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
#if (!defined __TE_REG_CHECK_TEST_RESULT_STEP_H__)
#define __TE_REG_CHECK_TEST_RESULT_STEP_H__
#include <test/testexecutestepbase.h>

class CTe_RegCheckTestResultStep : public CTestStep
	{
public:
	CTe_RegCheckTestResultStep();
	~CTe_RegCheckTestResultStep();
	//virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	//virtual TVerdict doTestStepPostambleL();
	};

_LIT(KTe_RegCheckTestResultStep,"Te_RegCheckTestResultStep");

#endif
