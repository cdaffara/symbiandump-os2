/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <cusbclasscontrolleriterator.h>
#include "CUsbMsTestStep.h"
#include <musbclasscontrollernotify.h>

class TNotify : public MUsbClassControllerNotify
	{
	CUsbClassControllerIterator* UccnGetClassControllerIteratorL(){return 0;}			
	void UccnError(TInt /*Error*/){}
	} owner;
	
TVerdict CConfigItemTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CConfigItemTest::doTestStepL"));
	SetTestStepResult(EPass);

	TPtrC origString;
	TPtrC expectedString;
	TInt stringLength;
	if(	!GetStringFromConfig(ConfigSection(), _L("origString"), origString) ||
	   	!GetStringFromConfig(ConfigSection(), _L("expectedString"), expectedString) ||
	   	!GetIntFromConfig(ConfigSection(), _L("stringLength"), stringLength))
		{
		INFO_PRINTF1(_L("Failed to get init data"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}

	// Create an active scheduler
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	CUsbMsClassController* controller = CUsbMsClassController::NewL(owner);
	
	CleanupStack::PopAndDestroy(sched);
	
	TBuf<64> copiedString;
	controller->ConfigItem(origString, copiedString, stringLength);
	delete controller;

	TEST(copiedString.Compare(expectedString) == 0);

	return TestStepResult();
	}

TVerdict CReadMsConfigTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CReadMsConfigTest::doTestStepL"));
	SetTestStepResult(EPass);
	
	TPtrC vendorId;
	TPtrC productId;
	TPtrC productRev;
	
	if(	!GetStringFromConfig(ConfigSection(), _L("vendorId"), vendorId) ||
	   	!GetStringFromConfig(ConfigSection(), _L("productId"), productId) ||
	   	!GetStringFromConfig(ConfigSection(), _L("productRev"), productRev))
		{
		INFO_PRINTF1(_L("Failed to get init data"));
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	// Create an active scheduler
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	CUsbMsClassController* controller = CUsbMsClassController::NewL(owner);
	CleanupStack::PushL(controller);
	
	controller->ReadMassStorageConfigL();
	
	// Do comparison
	TEST(controller->iMsConfig.iVendorId.Compare(vendorId) == 0 &&
		controller->iMsConfig.iProductId.Compare(productId) == 0  &&
		controller->iMsConfig.iProductRev.Compare(productRev) == 0);

		
	// controller and sched
	CleanupStack::PopAndDestroy(2);
	
	return TestStepResult();
	}
	
