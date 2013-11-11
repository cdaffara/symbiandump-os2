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
 @file UloggerServerTest0Step.h
 @internalTechnology
*/
#if (!defined __ULOGGERSERVERTEST0_STEP_H__)
#define __ULOGGERSERVERTEST0_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerservertestsuitestepbase.h"
#include "uloggerwatcher.h"

using namespace Ulogger;

class CUloggerServerTest0Step : public CTe_UloggerServerTestSuiteStepBase, public MDataWatcherObserver
	{
public:
	CUloggerServerTest0Step();
	~CUloggerServerTest0Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void DataNotification(){}; //required by test
	
	//test methods
	TInt Test1L(); //CULoggerWatcher::NewL
	TInt Test2L();  //CULoggerWatcher::NewLC
	TInt Test3L();  //CULoggerWatcher::StartWatching
	TInt Test4L(); //CULoggerWatcher::StopWatching
	TInt Test5L();//CULoggerWatcher::RunL
	
	
// Please add/modify your class members here:
private:
	CActiveScheduler *iScheduler;
	TInt iErrors;
	};

_LIT(KUloggerServerTest0Step,"UloggerServerTest0Step");

#endif
