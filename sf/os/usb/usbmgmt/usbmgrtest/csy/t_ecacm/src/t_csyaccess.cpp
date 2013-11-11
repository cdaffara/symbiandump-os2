/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains sanity tests for various things.
*
*/

#include <e32base.h>
#include <c32comm.h>
#include <usbman.h>
#include <acminterface.h>
#include <e32test.h>

#ifndef __WINS__
_LIT(KCommDriverName, "EUSBC");
#else
_LIT(KCommPhysDriverName, "ECDRV");
_LIT(KCommPhysDeviceName, "Comm.Wins");
_LIT(KCommDriverName, "ECOMM");
#endif

LOCAL_D RTest gTest(_L("T_CSYACCESS"));
  
enum TInitReturnValue 
	{
	ENotFinished,
	EFinished
	};

TInitReturnValue C32_FailAcmOpen(TUint aCount);
TInitReturnValue C32_SucceedAcmOpen(TUint aCount);
TInitReturnValue USBMAN_SucceedAcmOpen(TUint aCount);

TUint gInitSafetyTimeout = 100000;
TUint gBetweenTestDelay = 1000000;

TInt _1();
TInt _2();
TInt _3();
TInt _4();
TInt _5();
TInt Oom(TInitReturnValue fn(TUint n));
TInt Regression_DEF23333();
TInt RunOneTest(TInt aKey);
void RunAll();

#define TEST(AAA)	{ if ( !(AAA) ) { return __LINE__; } }

TInt _1()
/**
 * Checks: 
 *	1/ can't open ACM port when USBMAN isn't started.
 *	2/ can open ACM port when USBMAN is started.
 *	3/ then stop USBMAN and check Read and Write complete with error.
 *	4/ then close the port and check that opening ACM port returns error.
 *	5/ then restarts USBMAN and checks the ACM port can be opened then shuts 
 *		it all down.
 *	6/ then start USBMAN, open ACM port, issue a Read request, close USBMAN, 
 *		and check the Read is completed with an error.
 */
	{
	gTest.Printf(_L("\nRunning test 1"));

	CActiveScheduler* sch = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sch);

	TName portName(KAcmSerialName);
	portName.AppendFormat(_L("::%d"), KAcmLowUnit);

	// Try to access serial/USB when the registration hasn't been done
	RCommServ sess;
	TEST(sess.Connect() == KErrNone);
	RComm port;
	TEST(sess.LoadCommModule(KAcmCsyName) == KErrNone);
	TInt err = port.Open(sess, portName, ECommExclusive); // should fail
	TEST(err == KErrAccessDenied);

	// Start USB services
	RUsb usb;
	TEST(usb.Connect() == KErrNone);
	TRequestStatus stat;
	usb.Start(stat);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrNone);

	// Try to access serial/USB when the registration HAS been done
	TEST(port.Open(sess, portName, ECommExclusive) == KErrNone); // should work

	// Deregister and check further RComm calls
	usb.Stop();
	usb.Close();
	TBuf8<10> buf;
	port.Read(stat, buf);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrAccessDenied);
	port.Write(stat, _L8("stuff"));
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrAccessDenied);
	port.Close();
	err = port.Open(sess, portName, ECommExclusive); // should fail
	TEST(err == KErrAccessDenied);

	// Check can open again after restarting service
	TEST(usb.Connect() == KErrNone);
	usb.Start(stat);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrNone);
	err = port.Open(sess, portName, ECommExclusive); // should work
	TEST(err == KErrNone);
	port.Close();
	usb.Stop();
	usb.Close();
  
	// Check completion of an outstanding RComm request when usbman pulls the 
	// rug out.
	TEST(usb.Connect() == KErrNone);
	usb.Start(stat);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrNone);
	err = port.Open(sess, portName, ECommExclusive); // should work
	TEST(err == KErrNone);

	// Hacky pause to wait for device to enumerate.
	User::After(1000000);

	port.Read(stat, buf);
	usb.Stop();
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrAccessDenied);
  
	// Clean up
	port.Close(); 
	TEST(sess.UnloadCommModule(KAcmSerialName) == KErrNone);
	sess.Close();

	usb.Close();

	delete sch;

	return KErrNone;
	}

TInt _2()
/**
 * Checks: 
 *	simple open/close of registration port
 */
	{
	gTest.Printf(_L("\nRunning test 2"));

	CActiveScheduler* sch = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sch);

	TName portName(KAcmSerialName);
	portName.AppendFormat(_L("::%d"), 666);

	RCommServ sess;
	TEST(sess.Connect() == KErrNone);
	RComm port;
	TEST(sess.LoadCommModule(KAcmCsyName) == KErrNone);
	TEST(port.Open(sess, portName, ECommExclusive) == KErrNone); 
	port.Close();
	TEST(sess.UnloadCommModule(KAcmSerialName) == KErrNone);
	sess.Close();

	delete sch;

	return KErrNone;
	}

TInt _3()
/**
 * Checks: 
 *	simple open/close of registration port with open/close of ACM port inside 
 *	that.
 */
	{
	gTest.Printf(_L("\nRunning test 3"));

	CActiveScheduler* sch = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sch);

	TName portName(KAcmSerialName);
	portName.AppendFormat(_L("::%d"), 666);

	RCommServ sess;
	TEST(sess.Connect() == KErrNone);
	RComm port;
	TEST(sess.LoadCommModule(KAcmCsyName) == KErrNone);
	TEST(port.Open(sess, portName, ECommExclusive) == KErrNone); 

	TName acmPortName(KAcmSerialName);
	acmPortName.AppendFormat(_L("::%d"), KAcmLowUnit);
	RComm acmPort;
	TEST(acmPort.Open(sess, acmPortName, ECommExclusive) == KErrNone); 
	acmPort.Close();

	port.Close();
	TEST(sess.UnloadCommModule(KAcmSerialName) == KErrNone);
	sess.Close();

	delete sch;

	return KErrNone;
	}

TInt _4()
/**
 * Checks: 
 *	opens registration port (NB actually starts USB- just opening reg port 
 *		doesn't instantiate the LDD, which we need to get the Read test to 
 *		work. Otherwise the read comes back immediately with KErrGeneral.)
 *	opens ACM port
 *	check ACM APIs active
 *	closes registration port (NB stop & close USB)
 *	checks ACM APIs inactive
 *	closes ACM port.
 */
	{
	gTest.Printf(_L("\nRunning test 4"));

	CActiveScheduler* sch = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sch);

	// Start USB services
	RUsb usb;
	TEST(usb.Connect() == KErrNone);
	TRequestStatus stat;
	usb.Start(stat);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrNone);	  

	RCommServ sess;
	TEST(sess.Connect() == KErrNone);

	TName acmPortName(KAcmSerialName);
	acmPortName.AppendFormat(_L("::%d"), KAcmLowUnit); 
	RComm acmPort;
	TEST(acmPort.Open(sess, acmPortName, ECommExclusive) == KErrNone); 

	// Hacky pause to wait for device to enumerate.
	User::After(1000000);

	TRequestStatus readStat;
	TRequestStatus timerStat;
	TBuf8<10> buf;
	acmPort.Read(readStat, buf);
	RTimer timer;
	TEST(timer.CreateLocal() == KErrNone);
	timer.After(timerStat, 1000000); 
	User::WaitForRequest(readStat, timerStat);
	TEST(readStat == KRequestPending);
	TEST(timerStat == KErrNone);
	acmPort.ReadCancel();
	User::WaitForRequest(readStat);
	TEST(readStat == KErrCancel);
	timer.Close();

	usb.Stop();
	usb.Close();

	acmPort.Read(readStat, buf);
	User::WaitForRequest(readStat);
	TEST(readStat == KErrAccessDenied);

	acmPort.Close();

	sess.Close();

	delete sch;

	return KErrNone;
	}

TInt _5()
/**
 * Checks: 
 *	opens registration port
 *	opens ACM port
 *	check ACM APIs active
 *	closes registration port
 *	checks ACM APIs inactive
 *	opens registration port
 *	checks ACM APIs active
 *	closes ACM port
 *	closes registration port
 */
	{
	gTest.Printf(_L("\nRunning test 5"));

	CActiveScheduler* sch = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sch);

	// Start USB services
	RUsb usb;
	TEST(usb.Connect() == KErrNone);
	TRequestStatus stat;
	usb.Start(stat);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrNone);	  

	RCommServ sess;
	TEST(sess.Connect() == KErrNone);

	TName acmPortName(KAcmSerialName);
	acmPortName.AppendFormat(_L("::%d"), KAcmLowUnit);
	RComm acmPort;
	TEST(acmPort.Open(sess, acmPortName, ECommExclusive) == KErrNone); 

	// Hacky pause to wait for device to enumerate.
	User::After(1000000);  

	TRequestStatus readStat;
	TRequestStatus timerStat;
	TBuf8<10> buf;
	acmPort.Read(readStat, buf);
	RTimer timer;
	TEST(timer.CreateLocal() == KErrNone);
	timer.After(timerStat, 1000000); 
	User::WaitForRequest(readStat, timerStat);
	TEST(readStat == KRequestPending);
	TEST(timerStat == KErrNone);
	acmPort.ReadCancel();
	User::WaitForRequest(readStat);
	TEST(readStat == KErrCancel);
	timer.Close();

	usb.Stop();

	acmPort.Read(readStat, buf);
	User::WaitForRequest(readStat);
	TEST(readStat == KErrAccessDenied);

	usb.Start(stat);
	User::WaitForRequest(stat);
	TEST(stat.Int() == KErrNone);	  

	// Hacky pause to wait for device to enumerate.
	User::After(1000000);

	acmPort.Read(readStat, buf);
	TEST(timer.CreateLocal() == KErrNone);
	timer.After(timerStat, 1000000);
	User::WaitForRequest(readStat, timerStat);
	TEST(readStat == KRequestPending);
	TEST(timerStat == KErrNone);
	acmPort.ReadCancel();
	User::WaitForRequest(readStat);
	TEST(readStat == KErrCancel);
	timer.Close();
		
	acmPort.Close();

	usb.Stop();
	usb.Close();
	sess.Close();

	delete sch;

	return KErrNone;
	}

TInitReturnValue C32_FailAcmOpen(TUint aCount)
/**
 * One iteration of loading ECACM.CSY and trying to open the ACM port. Will 
 * fail due to the access control (USB isn't started). Heap failures in C32's 
 * heap.
 */
	{
	RCommServ serv;
	TInt err = serv.Connect();
	if ( !err )
		{
		gTest.Printf(_L("\tconnected to server\n"));
		serv.__DbgMarkHeap();
		serv.__DbgFailNext(aCount);

		err = serv.LoadCommModule(KAcmCsyName);
		if ( !err )
			{
			gTest.Printf(_L("\tloaded comm module\n"));
			RComm comm;
			TName name(KAcmSerialName);
			name.AppendFormat(_L("::%d"), KAcmLowUnit); 
			err = comm.Open(serv, name, ECommExclusive);
			if ( err == KErrAccessDenied ) 
				{
				gTest.Printf(_L("\tsuccessful result from RComm::Open\n"));
				err = KErrNone;
				}
			}
		serv.UnloadCommModule(KAcmSerialName);
		User::After(gInitSafetyTimeout);
		serv.__DbgMarkEnd(0);
		serv.Close();
		User::After(gInitSafetyTimeout);

		if ( !err )
			{
			// Test for if the test has finished. If aCount has become so big 
			// as to skip over all the allocs in the above use case, the next 
			// API call which causes alloc will fail with KErrNoMemory. 
			if ( serv.Connect() == KErrNoMemory )
				{
				serv.Close();
				User::After(gInitSafetyTimeout);
				return EFinished;
				}
			}
		}

	serv.Close();
	User::After(gInitSafetyTimeout);

	return ENotFinished;
	}

TInitReturnValue C32_SucceedAcmOpen(TUint aCount)
/**
 * One iteration of starting USB, loading ECACM.CSY, trying to open the ACM 
 * port and changing the ACM port's buffer sizes. Heap failures in C32's heap.
 */
	{
	RUsb usb;
	TInt err = usb.Connect();
	gTest.Printf(_L("\tRUsb::Connect = %d\n"), err);
	if ( !err )
		{
		RCommServ serv;
		err = serv.Connect();
		gTest.Printf(_L("\tRCommServ::Connect = %d\n"), err);

		if ( !err )
			{
			serv.__DbgMarkHeap();
			serv.__DbgFailNext(aCount);

			TRequestStatus stat;
			usb.Start(stat);
			User::WaitForRequest(stat);
			err = stat.Int();
			gTest.Printf(_L("\tRUsb::Start = %d\n"), err);
			if ( !err )
				{
				err = serv.LoadCommModule(KAcmCsyName);
				gTest.Printf(_L("\tRCommServ::LoadCommModule = %d\n"), err);
				if ( !err )
					{
					RComm comm;
					TName name(KAcmSerialName);
					name.AppendFormat(_L("::%d"), KAcmLowUnit);
					err = comm.Open(serv, name, ECommExclusive);
					gTest.Printf(_L("\tRComm::Open = %d\n"), err);
					if ( !err ) 
						{
						gTest.Printf(_L("\tsuccessful result from RComm::Open\n"));
						comm.Close();			
						}
					}
				serv.UnloadCommModule(KAcmSerialName);
				
				usb.Stop();
				User::After(gInitSafetyTimeout);

				serv.__DbgMarkEnd(0);
				}
			serv.Close();
			User::After(gInitSafetyTimeout);
			}
		
		usb.Close();
		User::After(gInitSafetyTimeout);

		if ( !err )
			{
			// Test for if the test has finished. If aCount has become so big 
			// as to skip over all the allocs in the above use case, the next 
			// API call which causes alloc will fail with KErrNoMemory. 
			if ( serv.Connect() == KErrNoMemory )
				{
				serv.Close();
				User::After(gInitSafetyTimeout);
				return EFinished;
				}
			}
		}

	return ENotFinished;
	}

TInitReturnValue USBMAN_SucceedAcmOpen(TUint aCount)
/**
 * One iteration of doing RUsb::Start/Stop. Heap failures in USBMAN's heap.
 */
	{
	RUsb usb;
	TInt err = usb.Connect();
	if ( !err )
		{
		gTest.Printf(_L("\tconnected to USBMAN\n"));

		err = usb.__DbgMarkHeap();
		if ( !err )
			{
			err = usb.__DbgFailNext(aCount);
			if ( !err )
				{
				TRequestStatus stat;
				usb.Start(stat);
				User::WaitForRequest(stat);
				err = stat.Int();
				gTest.Printf(_L("\tRUsb::Start completed with %d\n"), err);
				if ( !err )
					{
					usb.Stop(stat);
					User::WaitForRequest(stat);
					err = stat.Int();
					gTest.Printf(_L("\tRUsb::Stop completed with %d\n"), err);
					}
				}
			else
				{
				gTest.Printf(_L("\tRUsb::__DbgFailNext completed with %d\n"), err);
				User::Panic(_L("t_csyaccess"), 1);
				}
			}
		else
			{
			gTest.Printf(_L("\tRUsb::__DbgMarkHeap completed with %d\n"), err);
			User::Panic(_L("t_csyaccess"), 1);
			}
		}
	else
		{
		gTest.Printf(_L("\tRUsb::Connect completed with %d\n"), err);
		User::Panic(_L("t_csyaccess"), 1);
		}

	usb.__DbgMarkEnd(0);
	usb.Close();
	
	if ( !err )
		{
		// Test for if the test has finished. If aCount has become so big 
		// as to skip over all the allocs in the above use case, the next 
		// API call which causes alloc will fail with KErrNoMemory. 
		if ( usb.Connect() == KErrNoMemory )
			{
			usb.Close();
			User::After(gInitSafetyTimeout);
			return EFinished;
			}
		}
  
	User::After(gInitSafetyTimeout);

	return ENotFinished;
	}

TInt Oom(TInitReturnValue fn(TUint n))
/**
 * Wrapper for OOM iterations.
 *
 * @param fn Function pointer taking a TUint (the iteration) and returning a 
 * TInitReturnValue indicating whether the test has completed or not.
 * @return KErrNone.
 */
	{
	gTest.Printf(_L("\n"));

	for ( TInt n = 1 ; ; n++ )
		{
		gTest.Printf(_L("Failing alloc %d\n"),n);
	
		if ( fn(n) == EFinished )
			return KErrNone;
		}
	}

TInt Regression_DEF23333()
/**
 * Regression test for defect DEF23333.
 * Buffer size member of Tx class (CAcmWriter) isn't changed when SetBufSize 
 * is used.
 */
	{
	gTest.Printf(_L("\nRunning test Regression_DEF23333"));

	CActiveScheduler* sch = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sch);

	RUsb usb;
	TEST(usb.Connect() == KErrNone);
	TRequestStatus stat;
	usb.Start(stat);
	User::WaitForRequest(stat);
	TEST(stat == KErrNone);

	TName portName(KAcmSerialName);
	portName.AppendFormat(_L("::%d"), KAcmLowUnit);

	RCommServ sess;
	TEST(sess.Connect() == KErrNone);
	RComm acmPort;
	TEST(sess.LoadCommModule(KAcmCsyName) == KErrNone);
	TEST(acmPort.Open(sess, portName, ECommExclusive) == KErrNone);

	// Hacky pause to wait for device to enumerate.
	User::After(1000000);
	
	// This seems to be needed to keep the LDD happy.
	gTest.Printf(_L("\nAttach a HyperTerminal session (emulated serial over USB) and hit a key"));
	gTest.Getch();
	
	TInt len = acmPort.ReceiveBufferLength();
	TEST(len > 10); 
	// Check we can Write amounts of data less than the default starting 
	// buffer size.
	HBufC8* data = HBufC8::NewL(len + 20);
	TPtr8 ptr = data->Des();
	ptr.SetLength(len - 1);
	acmPort.Write(stat, *data);
	User::WaitForRequest(stat);
	TEST(stat == KErrNone);
	// Check writing an amount bigger than this fails.
	ptr.SetLength(len + 1);
	acmPort.Write(stat, *data);
	User::WaitForRequest(stat);
	TEST(stat == KErrNoMemory);	
	// Resize the buffer.
	const TInt newLength = 10;
	acmPort.SetReceiveBufferLength(newLength);
	TEST(acmPort.ReceiveBufferLength() == newLength);
	// Check writing an amount smaller than the new buffer.
	ptr.SetLength(newLength - 1);
	acmPort.Write(stat, *data);
	User::WaitForRequest(stat);
	TEST(stat == KErrNone);
	// Check writing an amount larger than the new size fails.
	ptr.SetLength(newLength + 1);
	acmPort.Write(stat, *data);
	User::WaitForRequest(stat);
	TEST(stat == KErrNoMemory);	
	
	delete data;
	acmPort.Close();
	TEST(sess.UnloadCommModule(KAcmSerialName) == KErrNone);
	sess.Close();
	usb.Stop(stat);
	User::WaitForRequest(stat);
	TEST(stat == KErrNone);
	usb.Close();
	
	delete sch;

	// This seems to be needed to keep the LDD happy.
	gTest.Printf(_L("\nDisconnect the HyperTerminal session and hit a key"));
	gTest.Getch();

	return KErrNone;
	}

void PrintTestInstructions()
	{
	gTest.Printf(_L("\nThese tests should be run with USB OFF,"));
	gTest.Printf(_L("\ni.e. RUsb has not been Started, or, if"));
	gTest.Printf(_L("\nit has, it should be Stopped again."));
	gTest.Printf(_L("\nAlso, there should be no open subsessions"));
	gTest.Printf(_L("\non the ECACM.CSY."));
	gTest.Printf(_L("\nIn a Techview ROM, it will be necessary"));
	gTest.Printf(_L("\nto remove the watchers so that nothing"));
	gTest.Printf(_L("\ninterferes with C32's heap (for the OOM"));
	gTest.Printf(_L("\ntests in C32's heap)."));
	gTest.Printf(_L("\nNothing else should be using USBMAN or"));
	gTest.Printf(_L("\nECACM during the test."));
	}

void PrintTestIds()
	{
	gTest.Printf(_L("\n1/"));
	gTest.Printf(_L("\n2/"));
	gTest.Printf(_L("\n3/"));
	gTest.Printf(_L("\n4/"));
	gTest.Printf(_L("\n5/"));
	gTest.Printf(_L("\n6/ OOM1 (in C32)- failure opening ACM port"));
	gTest.Printf(_L("\n7/ OOM2 (in C32)- success opening ACM port"));
	gTest.Printf(_L("\n8/ OOM3 (in USBMAN)- RUsb::Start"));
	gTest.Printf(_L("\n9/ Regression_DEF23333"));
	gTest.Printf(_L("\n! to run all"));
	gTest.Printf(_L("\nEsc to exit"));
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
												  	
	gTest.Title();
	gTest.Start(_L("Starting E32Main"));

	gTest.Next(_L("loading LDD"));
	TInt r = User::LoadLogicalDevice(KCommDriverName);
	gTest(r == KErrNone || r == KErrAlreadyExists);

#ifdef __WINS__
	gTest.Next(_L("loading PDD"));
	r = User::LoadPhysicalDevice(KCommPhysDriverName);
	gTest(r == KErrNone || r == KErrAlreadyExists);
#endif

	gTest.Next(_L("starting C32"));
	r = StartC32();
	gTest(r == KErrNone || r == KErrAlreadyExists);

	PrintTestInstructions();

	PrintTestIds();

	TInt key = gTest.Getch();

	while ( key != EKeyEscape )
		{
		TInt error = KErrNone;

		RCommServ sess;
		gTest.Next(_L("Connecting to Comm Server"));
		r = sess.Connect();
		gTest(r == KErrNone);
		
		sess.__DbgMarkHeap();

		switch (key )
			{
		case '!':
			RunAll();
			break;
		default:
			error = RunOneTest(key);
			break;
			}

		User::After(gInitSafetyTimeout);

		sess.__DbgCheckHeap(0);
		sess.Close();

		User::After(gBetweenTestDelay);

		gTest.Printf(_L("\nTest completed with error %d"), error);

		PrintTestIds();

		key = gTest.Getch();
		}

	gTest.End();
	gTest.Close();

	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;
	return KErrNone;
	}

TInt RunOneTest(TInt aKey)
	{
	TInt error = KErrNone;

	switch ( aKey )
		{
	case '1':
		error = _1();
		break;
	case '2':
		error = _2();
		break;
	case '3':
		error = _3();
		break;
	case '4':
		error = _4();
		break;
	case '5':
		error = _5();
		break;
	case '6':
		error = Oom(C32_FailAcmOpen);
		break;
	case '7':
		error = Oom(C32_SucceedAcmOpen);
		break;
	case '8':
		error = Oom(USBMAN_SucceedAcmOpen);
		break;
	case '9':
		error = Regression_DEF23333();
		break;
	default:
		gTest.Printf(_L("\nKey not recognised"));
		break;
		}

	return error;
	}
		
void RunAll()
	{
	RCommServ sess;
    gTest.Next(_L("Connecting to Comm Server"));
    TInt r = sess.Connect();
    gTest(r == KErrNone);

	for ( TInt ii = 0 ; ii < 9 ; ii++ ) // TODO: keep 9 up-to-date with number of tests.
		{
		sess.__DbgMarkHeap();
		gTest.Printf(_L("\nTest completed with error %d"), RunOneTest('1'+ii));
		sess.__DbgCheckHeap(0);
		User::After(gBetweenTestDelay);
		}

	sess.Close();

	User::After(gBetweenTestDelay);
	}

//
// End of file
