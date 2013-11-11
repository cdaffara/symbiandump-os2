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
 @file uloggerservertest5step.h
 @internalTechnology
*/

#if (!defined __ULOGGERSERVERTEST5_STEP_H__)
#define __ULOGGERSERVERTEST5_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerservertestsuitestepbase.h"


namespace Ulogger
{

class CUloggerServerTest5Step : public CTe_UloggerServerTestSuiteStepBase
	{
	friend class CControlFramework;
public:
	CUloggerServerTest5Step();
	~CUloggerServerTest5Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	
	//test methods
	TInt Test1L(); //CUloggerChannelSettings
	
// Please add/modify your class members here:
private:
	CActiveScheduler *iScheduler;
	TInt iErrors;
	};
}

_LIT(KUloggerServerTest5Step,"UloggerServerTest5Step");

#endif
