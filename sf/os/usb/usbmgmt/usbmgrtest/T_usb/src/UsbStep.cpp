/*
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
* This contains CTestStepUsb which is the base class for all the Usb Test Steps
*
*/

// EPOC includes
#include <e32base.h>
#include <c32comm.h>

// Test system includes
#include <networking/log.h>
#include "UsbStep.h"
#include "UsbSuite.h"

_LIT(KUsbLddName, "eusbc");

/**
 * The CTestStepUsb::CTestStepUsb method
 *
 *
 *
 * @internalComponent
 *
 * @return	
 */
CTestStepUsb::CTestStepUsb()
	: iUsbSuite(0)
	{
	}


/**
 * The CTestStepUsb::~CTestStepUsb method
 *
 *
 *
 * @internalComponent
 *
 * @return	
 */
CTestStepUsb::~CTestStepUsb()
	{ 
	}	

void CTestStepUsb::OpenCommServerL()
	{
	if (!iUsbSuite->iCommServerOpen)
		{
		TInt err = iUsbSuite->iCommServer.Connect();
		TESTL(err == KErrNone || err == KErrAlreadyExists);
		iUsbSuite->iCommServerOpen = ETrue;

		Log(_L("CTestStepUsb::doTestStepL() - Connected to C32"));

		err = iUsbSuite->iCommServer.LoadCommModule(CSY_NAME);
		TESTL(err == KErrNone || err == KErrAlreadyExists);

		Log(_L("CTestStepUsb::doTestStepL() = Loaded ACM CSY"));
		}
	else
		{
		TEST(ETrue);
		}
	}

TInt CTestStepUsb::OpenLdd()
	{
	if (iUsbSuite->iLddOpen)
		{
		return KErrNone;
		}

	TInt err = User::LoadLogicalDevice(KUsbLddName);

	if (err != KErrNone && err != KErrAlreadyExists)
		{
		Log(_L("CTestStepUsb::OpenLdd() - Unable to load LDD! Error = %d"), err);
		return err;
		}

	err = iUsbSuite->iLdd.Open(0);
	if (err != KErrNone)
		{
		Log(_L("CTestStepUsb::OpenLdd() - Unable to Open LDD! Error = %d"), err);
		return err;
		}

	iUsbSuite->iLddOpen = ETrue;
	iUsbSuite->iLdd.SetDeviceControl();
	iUsbSuite->iLdd.ReleaseDeviceControl();
	return KErrNone;
	}
