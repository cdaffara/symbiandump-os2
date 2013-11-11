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
 @file UloggerServerTest6Step.h
 @internalTechnology
*/
#if (!defined __ULOGGERSERVERTEST6_STEP_H__)
#define __ULOGGERSERVERTEST6_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerservertestsuitestepbase.h"
#include "inputdata.h"

namespace Ulogger
{

class CUloggerServerTest6Step : public CTe_UloggerServerTestSuiteStepBase
	{
public:
	CUloggerServerTest6Step();
	~CUloggerServerTest6Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
// Please add/modify your class members here:
private:
	CActiveScheduler *iScheduler;
	TInt iErrors;
	CInputData* iMan;
	};
}

_LIT(KUloggerServerTest6Step,"UloggerServerTest6Step");

#endif
