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



#ifndef __TESTIFIOCTLS_H__
#define __TESTIFIOCTLS_H__

#include <test/testexecutestepbase.h>

_LIT(KExampleL, "ExampleL");
_LIT(KListInterfaces, "ListInterfaces");
_LIT(KCreateManyActiveInterfaces, "CreateManyActiveInterfaces");
_LIT(KChooseInterface, "ChooseInterface");
_LIT(KChooseActiveInterface, "ChooseActiveInterface");
_LIT(KTestIfNameIndex, "TestIfNameIndex");
_LIT(KTestIfIndexToName, "TestIfIndexToName");
_LIT(KTestIfNameToIndex, "TestIfNameToIndex");
_LIT(KTestSiocGIfIndex, "TestSiocGIfIndex");
_LIT(KConnectToIpUsingConnection, "ConnectToIpUsingConnection");
_LIT(KSendtoIpUsingConnection, "SendtoIpUsingConnection");
_LIT(KConnectToUrlUsingConnection, "ConnectToUrlUsingConnection");
_LIT(Kioctltest, "ioctltest");
_LIT(Kreadtest, "readtest");
_LIT(KTestAddDelRoute, "TestAddDelRoute");
_LIT(KTestAddDelRouteNegative1, "TestAddDelRouteNegative1");
_LIT(KTestAddDelRouteNegative2, "TestAddDelRouteNegative2");
_LIT(KTestMacAddress, "TestMacAddress");
_LIT(KTestMacAddressNegative1, "TestMacAddressNegative1");
_LIT(KTestioctlfile, "Testioctlfile");
_LIT(KTestioctl1, "Testioctl1");


class CTestIfioctls : public CTestStep
	{
public:
	~CTestIfioctls(); 
	CTestIfioctls(const TDesC& aStepName);
	TVerdict doTestStepL();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
private:
	TInt ExampleL();
	TInt ListInterfaces();
	TInt CreateManyActiveInterfaces();
	TInt ChooseInterface();
	TInt ChooseActiveInterface();
	TInt TestIfNameIndex();
	TInt TestIfIndexToName();
	TInt TestIfNameToIndex();
	TInt TestSiocGIfIndex();
	TInt ConnectToIpUsingConnection();
	TInt SendtoIpUsingConnection();
	TInt ConnectToUrlUsingConnection();
	TInt ioctltest();
	TInt readtest();
	TInt TestAddDelRoute();
	TInt TestAddDelRouteNegative1();
	TInt TestAddDelRouteNegative2();
	TInt TestMacAddress();
	TInt TestMacAddressNegative1();
	TInt Testioctlfile();
	TInt Testioctl1();
	
	// util function
	void ReadStringParam(char* aString);
public:
	TInt iParamCount;
	
	};
#endif //
