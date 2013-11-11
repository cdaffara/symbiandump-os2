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
* This contains CTestSuiteUsb 
* This is the container class for all the Usb test steps
*
*/


#ifndef __USBSUITE_H__
#define __USBSUITE_H__

#include <networking/testsuite.h>
#include <c32comm.h>
#include <d32usbc.h>

#define CSY_NAME _L("ECACM")
#define PORT_NAME _L("ACM::0")

NONSHARABLE_CLASS(CTestSuiteUsb) : public CTestSuite
{
public:
	void InitialiseL( );
	virtual ~CTestSuiteUsb();
	
	void AddTestStepL(CTestStepUsb* aTestStep);

	// Override of test system getversion method  
	TPtrC GetVersion();

	void CloseAll();

public:
	RUsb			iUsb;
	RCommServ		iCommServer;
	RComm			iCommPort;
	RDevUsbcClient  iLdd;

	TBool			iPortOpen;
	TBool			iCommServerOpen;
	TBool			iUsbOpen;
	TBool			iLddOpen;

	TRequestStatus	iStartStatus;
	TRequestStatus	iReadStatus;
	TRequestStatus	iWriteStatus;
	TRequestStatus	iNotifySignalChangeStatus;
	TRequestStatus	iNotifyConfigChangeStatus;
	TRequestStatus  iNotifyBreakStatus;
	TRequestStatus  iNotifyStateStatus;

	TUint				  iSignals;
	TCommNotificationPckg iConfig;
	TUint				  iUsbState;

	TUsbDeviceState		  iUsbManState;

	TBuf8<1024>     iReadBuffer;
	TBuf8<1024>     iWriteBuffer;
};

#endif /* __USBSUITE_H__ */
