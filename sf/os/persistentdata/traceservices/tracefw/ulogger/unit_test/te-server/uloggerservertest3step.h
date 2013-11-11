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
 @file UloggerServerTest1Step.h
 @internalTechnology
*/
#if (!defined __ULOGGERSERVERTEST3_STEP_H__)
#define __ULOGGERSERVERTEST3_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerservertestsuitestepbase.h"
#include "uloggerwatcher.h"

_LIT8(KUsbTestPlugin, "uloggerusbplugin");
_LIT8(KFileTestPlugin, "uloggerfileplugin");
_LIT8(KNullTestPlugin, "");
_LIT8(KBadNameTestPlugin, "ulopggerbadnameplugin");


using namespace Ulogger;

class CUloggerServerTest3Step : public CTe_UloggerServerTestSuiteStepBase, public MDataWatcherObserver
	{
public:
	CUloggerServerTest3Step();
	~CUloggerServerTest3Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
	void DataNotification(){}; //required by test
	
	//test methods
	
	TInt Test1L(); //CPluginAllocator::NewL 
	TInt Test1_2L(); //CPluginAllocator::NewL - condition 2
	TInt Test1_3L(); //CPluginAllocator::NewL - condition 3
	TInt Test1_4L(); //CPluginAllocator::NewL - condition 4
	TInt Test2L();  //CPluginAllocator::NewLC
	
// Please add/modify your class members here:
private:
	CActiveScheduler *iScheduler;
	TInt iErrors;
	};

_LIT(KUloggerServerTest3Step,"UloggerServerTest3Step");

#endif
