/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SER-COMMS\USB\TESTSRC\SHOWCAPS.CPP
*
*/

#include <c32comm.h>
#include <e32test.h>

#ifndef __WINS__
_LIT(KCommDriverName, "EUSBC");
_LIT(KCommDeviceName, "USBC");
_LIT(KCommModuleCsyName, "ECACM");
_LIT(KCommModulePortNameAndNumber, "ACM::0");
_LIT(KCommModulePortName, "ACM");
#else
_LIT(KCommPhysDriverName, "ECDRV");
_LIT(KCommPhysDeviceName, "Comm.Wins");
_LIT(KCommDriverName, "ECOMM");
_LIT(KCommDeviceName, "Comm");
_LIT(KCommModuleCsyName, "ECUART");
_LIT(KCommModulePortNameAndNumber, "COMM::0");
_LIT(KCommModulePortName, "COMM");
#endif

LOCAL_D RCommServ CommServ;
LOCAL_D RComm CommPort;
LOCAL_D RTest test(_L("SHOWCAPS"));

TInt E32Main()
/**
 * Loads the USB serial driver and prints its capabilities.
 */
	{
	TInt r;

	test.Title();
	test.Start(_L("Starting E32Main"));

 	__UHEAP_MARK;

	test.Next(_L("loading LDD"));
	r = User::LoadLogicalDevice(KCommDriverName);
	test(r == KErrNone || r == KErrAlreadyExists);

#ifdef __WINS__
	test.Next(_L("loading PDD"));
	r = User::LoadPhysicalDevice(KCommPhysDriverName);
	test(r == KErrNone || r == KErrAlreadyExists);
#endif

	test.Next(_L("starting C32"));
	r = StartC32();
	test(r == KErrNone || r == KErrAlreadyExists);

	test.Next(_L("connecting to comms server"));
	test(CommServ.Connect() == KErrNone);

	test.Next(_L("loading CommPort module"));
	r = CommServ.LoadCommModule(KCommModuleCsyName);
	test(r == KErrNone || r == KErrAlreadyExists);

	test.Next(_L("opening CommPort port"));
	test(CommPort.Open(CommServ, KCommModulePortNameAndNumber, ECommExclusive) == KErrNone);

	test.Next(_L("getting caps"));
	TCommCaps2 cc2;
	CommPort.Caps(cc2);
	test.Printf(_L("TCommCapsV01"));
	test.Printf(_L("iRate\t0x%x\n"), cc2().iRate);
	test.Printf(_L("iDataBits\t0x%x\n"), cc2().iDataBits);
	test.Printf(_L("iStopBits\t0x%x\n"), cc2().iStopBits);
	test.Printf(_L("iParity\t0x%x\n"), cc2().iParity);
	test.Printf(_L("iHandshake\t0x%x\n"), cc2().iHandshake);
	test.Printf(_L("iSignals\t0x%x\n"), cc2().iSignals);
	test.Printf(_L("iFifo\t%d\n"), cc2().iFifo);
	test.Printf(_L("iSIR\t%d\n"), cc2().iSIR);
	test.Printf(_L("TCommCapsV02"));
	test.Printf(_L("iNotificationCaps\t0x%x\n"), cc2().iNotificationCaps);
	test.Printf(_L("iRoleCaps\t0x%x\n"), cc2().iRoleCaps);
	test.Printf(_L("iFlowControlCaps\t0x%x\n"), cc2().iFlowControlCaps);

	CommPort.Close();
	test(CommServ.UnloadCommModule(KCommModulePortName) == KErrNone);
	CommServ.Close();
#ifdef __WINS__
	test(User::FreePhysicalDevice(KCommPhysDeviceName) == KErrNone);
#endif
	test(User::FreeLogicalDevice(KCommDeviceName) == KErrNone);

	__UHEAP_MARKEND;

	test.End();
	test.Close();


	return KErrNone;
	}
