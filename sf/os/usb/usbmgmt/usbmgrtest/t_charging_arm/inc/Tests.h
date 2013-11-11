/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TESTS_H__
#define __TESTS_H__

// -- Add specific test header files here.
#include "usbchargingarmtest.h"
// -- End

class CTestBase;

struct TTestDefinition
	{
	TTestDefinition(const TDesC8& aDescription, CTestBase* (*aFactoryL)(MTestManager&));

	TBuf8<256> iDescription;
	CTestBase* (*iFactoryL)(MTestManager&);
	};

TTestDefinition::TTestDefinition(const TDesC8& aDescription, CTestBase* (*aFactoryL)(MTestManager&))
	{
	iDescription = aDescription;
	iFactoryL = aFactoryL;
	}

TTestDefinition gTestDefinitions[] = 
	{
// -- Add information about specific tests here
		TTestDefinition(_L8("Test 1 (USBChargingARMTest)"), CUsbChargingArmTest::NewL),
// -- End
	};

#endif // __TESTS_H__
