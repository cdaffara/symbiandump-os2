/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file IPSuiteStepBase.cpp
*/

#include "ipsuitestepbase.h"
#include "ipsuitedefs.h"

// Device driver constants
#if (defined __WINS__)
_LIT(KPdd,"ECDRV");
#else
_LIT(KPdd,"EUART1");
#endif
_LIT(KLdd,"ECOMM");

TVerdict CIPSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * Do all initialisation common to derived classes in here.
 * Just leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	// For the emulator and 2 box solutions we need to load the
	// physical device drivers
	// Leave if there are any errors
	TInt ret = KErrNone;
	ret = User::LoadPhysicalDevice(KPdd);
	if(ret != KErrNone && ret != KErrAlreadyExists)
		User::Leave(ret);
	ret = User::LoadLogicalDevice(KLdd);
	if(ret != KErrNone && ret != KErrAlreadyExists)
		User::Leave(ret);

	// Read the ini file to get the destination IP address and the data
	TPtrC IPAddr;
	TPtrC configWriteData;
	TInt port = 7;
	if(!GetStringFromConfig(ConfigSection(),KDemoSuiteIPAddress, IPAddr) ||
	   !GetStringFromConfig(ConfigSection(),KDemoSuiteTestData,configWriteData) ||
	   !GetIntFromConfig(ConfigSection(),KDemoSuiteIPPort,port)
		)
		// Leave if there's any error.
		User::Leave(KErrNotFound);
	// Connect to the socket server
	User::LeaveIfError(iServer.Connect());

	// Set the ip address class
	// Could read the port from the ini file
	iDestAddr.Input(IPAddr);
	iDestAddr.SetPort(port);

	// Set up the read and write buffers
	iReadData = HBufC8::NewMaxL(configWriteData.Length());
	iWriteData = HBufC8::NewMaxL(configWriteData.Length());
	iWriteData->Des().Copy(configWriteData);	

	SetTestStepResult(EPass);
	return TestStepResult();
	}

CIPSuiteStepBase::~CIPSuiteStepBase()
	{
	iServer.Close();
	delete iReadData;
	delete iWriteData;
	}

CIPSuiteStepBase::CIPSuiteStepBase()
	{
	}
