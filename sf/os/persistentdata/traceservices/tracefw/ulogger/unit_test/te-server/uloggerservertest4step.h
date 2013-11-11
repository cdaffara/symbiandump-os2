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
 @file UloggerServerTest4Step.h
 @internalTechnology
*/
#if (!defined __ULOGGERSERVERTEST4_STEP_H__)
#define __ULOGGERSERVERTEST4_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggerservertestsuitestepbase.h"
#include "pluginallocator.h"
#include "inputframework.h"

_LIT8(KUsbTestPluginTest, "uloggerusbplugin");
_LIT8(KFileTestPluginTest, "uloggerfileplugin");

namespace Ulogger
{

class CUloggerServerTest4Step : public CTe_UloggerServerTestSuiteStepBase, public MInputFrameworkObserver
	{
	friend class CInputFramework;
public:
	CUloggerServerTest4Step();
	~CUloggerServerTest4Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	//from MControlFrameworkObserver
	ControlData* ProcessCommandL(TCommand aOpCode, RArray<TPtrC8> &aArguments); 
	void DoPostProcessing(TCommand aCmd);
	
	//test methods
	TInt Test1L(); //CControlFramework::StartReading + StopReading
	TInt Test2L(); //CControlFramework:: testing functionality of passing and translating commands
	
	void TestFunctionalityL(const TDesC8& aCommand);
	
// Please add/modify your class members here:
private:
	CActiveScheduler *iScheduler;
	TInt iErrors;
	CInputFramework *iCF;
	CPluginAllocator *iPluginAllocator;
	CInputData* iMan;
	};
}

_LIT(KUloggerServerTest4Step,"UloggerServerTest4Step");

#endif
