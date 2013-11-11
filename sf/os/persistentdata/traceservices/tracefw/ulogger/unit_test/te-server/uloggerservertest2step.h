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
 @file UloggerServerTest2Step.h
 @internalTechnology
*/
#if (!defined __ULOGGERSERVERTEST2_STEP_H__)
#define __ULOGGERSERVERTEST2_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerservertestsuitestepbase.h"

class CUloggerServerTest2Step : public CTe_UloggerServerTestSuiteStepBase
	{
public:
	CUloggerServerTest2Step();
	~CUloggerServerTest2Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	//test methods
	TInt Test1L();//CULoggerSession::NewL
	TInt Test2L();//CULoggerSession::NewLC
	
// Please add/modify your class members here:
private:
	CActiveScheduler *iScheduler;
	};

_LIT(KUloggerServerTest2Step,"UloggerServerTest2Step");

#endif
