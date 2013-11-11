/*
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



#include "tifioctls.h"
#include <unistd.h> 
#include <errno.h>
#include <stdio.h>
#include <e32std.h>
#include <stdlib.h>
#include <string.h>

CTestIfioctls::~CTestIfioctls() 
	{ 
	}  

CTestIfioctls::CTestIfioctls(const TDesC& aStepName)
	{
	// MANDATORY Call to base class method to set up the human readable name for logging.
	SetTestStepName(aStepName);		
	}

TVerdict CTestIfioctls::doTestStepPreambleL()
	{
	__UHEAP_MARK;	
	SetTestStepResult(EPass);
	return TestStepResult();
	}



TVerdict CTestIfioctls::doTestStepPostambleL()
	{
	__UHEAP_MARKEND;	
	return TestStepResult();
	}


TVerdict CTestIfioctls::doTestStepL()
	{
		int err;

   		if(TestStepName() == KExampleL)
   			{
   	   		INFO_PRINTF1(_L("ExampleL():"));
   	   		err = ExampleL();
   	   		SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   		}
   	   	else if(TestStepName() == KListInterfaces)
   				{
   	   			INFO_PRINTF1(_L("ListInterfaces():"));
   	   			err = ListInterfaces();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KCreateManyActiveInterfaces)
   				{
   	   			INFO_PRINTF1(_L("CreateManyActiveInterfaces():"));
   	   			err = CreateManyActiveInterfaces();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KChooseInterface)
   				{
   	   			INFO_PRINTF1(_L("ChooseInterface():"));
   	   			err = ChooseInterface();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KChooseActiveInterface)
   				{
   	   			INFO_PRINTF1(_L("ChooseActiveInterface():"));
   	   			err = ChooseActiveInterface();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestIfNameIndex)
   				{
   	   			INFO_PRINTF1(_L("TestIfNameIndex():"));
   	   			err = TestIfNameIndex();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestIfIndexToName)
   				{
   	   			INFO_PRINTF1(_L("TestIfIndexToName():"));
   	   			err = TestIfIndexToName();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestIfNameToIndex)
   				{
   	   			INFO_PRINTF1(_L("TestIfNameToIndex():"));
   	   			err = TestIfNameToIndex();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestSiocGIfIndex)
   				{
   	   			INFO_PRINTF1(_L("TestSiocGIfIndex():"));
   	   			err = TestSiocGIfIndex();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KConnectToIpUsingConnection)
   				{
   	   			INFO_PRINTF1(_L("ConnectToIpUsingConnection():"));
   	   			err = ConnectToIpUsingConnection();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else  if(TestStepName() == KSendtoIpUsingConnection )
   				{
   	   			INFO_PRINTF1(_L("SendtoIpUsingConnection():"));
   	   			err = SendtoIpUsingConnection();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KConnectToUrlUsingConnection )
   				{
   	   			INFO_PRINTF1(_L("ConnectToUrlUsingConnection():"));
   	   			err = ConnectToUrlUsingConnection();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == Kioctltest)
   				{
   	   			INFO_PRINTF1(_L("ioctltest():"));
   	   			err = ioctltest();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == Kreadtest)
   				{
   	   			INFO_PRINTF1(_L("readtest():"));
   	   			err = readtest();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestAddDelRoute)
   				{
   	   			INFO_PRINTF1(_L("TestAddDelRoute():"));
   	   			err = TestAddDelRoute();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestAddDelRouteNegative1)
   				{
   	   			INFO_PRINTF1(_L("TestAddDelRouteNegative1():"));
   	   			err = TestAddDelRouteNegative1();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestAddDelRouteNegative2)
   				{
   	   			INFO_PRINTF1(_L("TestAddDelRouteNegative2():"));
   	   			err = TestAddDelRouteNegative2();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestMacAddress)
   				{
   	   			INFO_PRINTF1(_L("TestMacAddress():"));
   	   			err = TestMacAddress();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}
   	   	else if(TestStepName() == KTestMacAddressNegative1)
   				{
   	   			INFO_PRINTF1(_L("TestMacAddressNegative1():"));
   	   			err = TestMacAddressNegative1();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   				}
   		else if(TestStepName() == KTestioctlfile )
   				{
   	   			INFO_PRINTF1(_L("Testioctlfile():"));
   	   			err = Testioctlfile();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}  
   	   	else if(TestStepName() == KTestioctl1)
   				{
   	   			INFO_PRINTF1(_L("Testioctl1():"));
   	   			err = Testioctl1();
   	   			SetTestStepResult(err ? static_cast<TVerdict>(err) : EPass);
   	   			}	
   	   	return TestStepResult(); 

	}
 

