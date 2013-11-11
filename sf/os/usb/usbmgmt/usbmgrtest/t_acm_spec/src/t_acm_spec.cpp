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
* t_acm.cpp
* This program executes all the tests in the ACM CSY Test Specification v 0.1
* and is to be used in conjunction with the host-side application.
*
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32math.h>
#include <badesca.h>
#include <c32comm.h>
#include <usbman.h>

LOCAL_D CConsoleBase* console;

RCommServ TheCommServ;
RUsb TheUsb;

TCommConfig TheConfigBuf;
TCommConfigV01& TheConfig = TheConfigBuf();

const TInt KReceiveBufferLength = 8192;

_LIT(KUsbPortName, "ACM::0");
_LIT(KUsbLddName, "EUSBC");

#define _printf console->Printf
#define _getch console->Getch
#define LEAVE(_x) VerboseLeaveL(_x, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))
#define LEAVEIFERROR(_x) VerboseLeaveIfErrorL(_x, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))
#define CHECK(_x) if (! (_x)) VerboseLeaveL(KErrGeneral, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))

// A define for stack-based buffers
#define MAX_BUFFER_SIZE 1024

// A bigger buffer on the heap
#define MAX_HEAP_BUFFER_SIZE	1024*8
TBuf8<MAX_HEAP_BUFFER_SIZE> readBigBuf;

// A timer for use by several of the tests
RTimer timer;

//The random seed
TInt64 seed = 100;

void VerboseLeaveL(TInt aError, TInt aLineNum, const TDesC8& aFileName, const TDesC8& aCode)
/**
 * For bombing out usefully.
 */
	{
	TInt filenameOffset = aFileName.LocateReverse('\\') + 1;
	if (filenameOffset < 0) filenameOffset = 1;
	TPtrC8 shortFileName = aFileName.Mid(filenameOffset);
	TBuf<64> fName, code;
	fName.Copy(shortFileName.Left(64));
	code.Copy(aCode.Left(64));
	_printf(_L("\nERROR (%d) on line %d of file %S\n"), aError, aLineNum, &fName);
	_printf(_L("Code: %S\n\n"), &code);
	_printf(_L("[ press any key ]"));
	_getch();
	User::Leave(aError);
	}

void VerboseLeaveIfErrorL(TInt aError, TInt aLineNum, const TDesC8& aFileName, const TDesC8& aCode)
/**
 * For bombing out usefully if there's an error.
 */
	{
	if (aError)
		VerboseLeaveL(aError, aLineNum, aFileName, aCode);
	}

void ReadString(TDes& aDes)
/**
 * Reads user input into the start of the descriptor aDes.
 */
	{
	TChar inputKey;
	TInt count = 0;

	aDes.Zero();
	for (;;)
		{
		inputKey = (TInt) _getch();

		if ((TInt)inputKey == EKeyEnter)
			break;

		if(inputKey == EKeyBackspace)
			{
			if (count > 0)
				{
				_printf(_L("%C"), (TUint) inputKey);
				aDes.Delete(--count,1);
				}
			}
		else if(inputKey.IsPrint())
			{
			_printf(_L("%C"), (TUint) inputKey);
			aDes.Append(inputKey);
			count++;
			}
		}
	}

void FillBuffer(TDes8 &aDes)
/**
 * Fills a given buffer with incrementing numbers.
 */
{
	 aDes.Zero();
	 for (TInt i=0; i<aDes.MaxSize(); i++)
	 {
		aDes.Append(i);
	 }
}

bool CheckBuffer(TDes8 &aDes, TInt size)
/**
 * Checks that a given buffer contains incrementing numbers.
 */
{
	 for (TInt i=0; i<size; i++)
	 {
		  if (aDes[i] != i)
			   return FALSE;
	 }

	 return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

void DataStress_SizeVary_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status;
	RComm port;
	TInt i;

	_printf(_L("\n----------------------------------\n"));
	_printf(_L("This test performs varying size read\n"));
	_printf(_L("and writes to and from the host.\n"));
	_printf(_L("------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	TBuf8<MAX_BUFFER_SIZE> readBuf;

	_printf(_L("Reading data.\nLengths:"));
	for (i = 1; i<MAX_BUFFER_SIZE; i*=2)
		{
		_printf(_L(" %d"), i);

		port.Read(status, readBuf, i);
		User::WaitForRequest(status);

		if (status != KErrNone)
			{
			LEAVE(status.Int());
			}

		CheckBuffer(readBuf, i);
		}

	_printf(_L(" done.\nReadOneOrMore()-ing data.\n"));
	for (i = 1; i<MAX_BUFFER_SIZE; i*=2)
		{
		int transferSize = 0;
		while (transferSize != i)
			{
			port.ReadOneOrMore(status, readBuf);
			User::WaitForRequest(status);

			if (status != KErrNone)
				{
					_printf(_L("Read error"));
					LEAVE(status.Int());
				}
			else
				{
					transferSize += readBuf.Length();
				}

			}
			_printf(_L(" %d "), transferSize);
		}

	_printf(_L(" done.\nWriting data "));

	FillBuffer(readBuf);

	for (i = 1; i<MAX_BUFFER_SIZE; i*=2)
		{

		port.Write(status, readBuf, i);

		User::WaitForRequest(status);

		if (status != KErrNone)
			{
			_printf(_L("write error\n"));
			LEAVE(status.Int());
			}

		_printf(_L(" %d"), i);
		}

	_printf(_L(" done.\nTest complete\n"));
	 CleanupStack::PopAndDestroy(); //port
}

void DataStress_RateVary_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status;
	RComm port;
	TInt i;

	_printf(_L("\n----------------------------------\n"));
	_printf(_L("This test performs varying speed read\n"));
	_printf(_L("and writes to and from the host.\n"));
	_printf(_L("------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	TBuf8<MAX_BUFFER_SIZE> readBuf;

	_printf(_L("Reading data.\nLengths:"));
	for (i = 1; i<100; i++)
		{
		_printf(_L(" %d"), i);
		port.Read(status, readBuf);
		User::WaitForRequest(status);

		if (status != KErrNone)
			{
			_printf(_L("Error reading"));
			LEAVE(status.Int());
			}

		CheckBuffer(readBuf, readBuf.MaxSize());
		}


	_printf(_L("done.\nReadOneOrMore()-ing data.\n Lengths: "));
	for (i = 1; i< 20; i++)
		{
		int transferSize = 0;
		while (transferSize != MAX_BUFFER_SIZE) {
			port.ReadOneOrMore(status, readBuf);
			User::WaitForRequest(status);

			if (status != KErrNone)
				{
				LEAVE(status.Int());
				}
			else
				{
				transferSize += readBuf.Length();
				}
		}
		_printf(_L(" %d "), i);
	}

	_printf(_L(" done.\nWriting data\nLengths:"));
	FillBuffer(readBuf);
	for (i = 1; i< 100; i++)
		{
		_printf(_L(" %d "), i);
		port.Write(status, readBuf, 1024);
		User::WaitForRequest(status);

		if (status != KErrNone)
			{
			_printf(_L("Write failed"));
			LEAVE(status.Int());
			}

		// Wait for random time
		timer.After(status, (Math::Rand(seed) % 1001));//1001 to get a value within [0-1000]
		User::WaitForRequest(status);
		}

	_printf(_L(" done.\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void TimeOut_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status;
	RComm port;
	TInt timeout;

	TBuf8<MAX_BUFFER_SIZE> readBuf;

	_printf(_L("\n---------------------------\n"));
	_printf(_L("This test exercises the read\n"));
	_printf(_L("and write timeouts.\n"));
	_printf(_L("-----------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	_printf(_L("Reading data\nTimeouts (ms):"));
	for (timeout = 10; timeout<100; timeout+=10)
		{
		 // Clean buffers
		 port.ResetBuffers();

		_printf(_L(" %d"), timeout);
		port.Read(status, timeout, readBuf, MAX_BUFFER_SIZE);
		User::WaitForRequest(status);

		if (status != KErrTimedOut)
		    {
				LEAVE(status.Int());
			}
		}

	_printf(_L(" done.\nWriting data\nTimeouts (ms):"));
	for (timeout = 10; timeout<100; timeout+=10)
		{
		 // Clean buffers
		 port.ResetBuffers();

		_printf(_L(" %d"), timeout);
		port.Write(status, timeout, readBuf, MAX_BUFFER_SIZE);
		User::WaitForRequest(status);

		if (status != KErrTimedOut)
		    {
				LEAVE(status.Int());
			}
		}

	_printf(_L(" done.\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void CancelTx_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status;
	RComm port;
	TInt timeout;

	TBuf8<MAX_BUFFER_SIZE> readBuf;

	_printf(_L("\n----------------------------------------\n"));
	_printf(_L("This tests the read/write cancel feature\n"));
	_printf(_L("------------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	_printf(_L("Reading data\n"));
	for (timeout = 10; timeout<100; timeout++)
		{
		port.Read(status, readBuf);

		// Wait before cancelling
		timer.After(status, timeout);
		User::WaitForRequest(status);

		port.ReadCancel();
		User::WaitForRequest(status);

		if ( (status != KErrNone) && (status !=KErrCancel) )
			{
			LEAVE(status.Int());
			}
		}

	_printf(_L("Writing data\n"));
	for (timeout = 10; timeout<100; timeout++)
		{
		port.Write(status, readBuf);

		// Wait before cancelling
		timer.After(status, timeout);
		User::WaitForRequest(status);

		port.WriteCancel();
		User::WaitForRequest(status);

		if ( (status != KErrNone) && (status !=KErrCancel) )
			{
			LEAVE(status.Int());
			}
		}

	_printf(_L("\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void InterruptTx_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status;
	RComm port;

	TBuf8<MAX_BUFFER_SIZE> readBuf;

	_printf(_L("\n--------------------------------\n"));
	_printf(_L("This tests the read/write cancel\n"));
	_printf(_L("when the USB cable is pulled\n"));
	_printf(_L("----------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	_printf(_L("Reading data\n"));
	port.Read(status, readBuf);

	_printf(_L("Pull cable now\n"));
	User::WaitForRequest(status);
	_printf(_L("Received error code: %d\n"),status.Int());
	_printf(_L("Expect -29(KErrCommsLineFail) or range -6702 to -6712 (USB driver errors)\n\n"));

	_printf(_L("Plug in cable and press any key\n"));
	_getch();

	_printf(_L("Writing data\n"));
	_printf(_L("Pull cable now\n"));

	do
	{
		 port.Write(status, readBuf);
		 User::WaitForRequest(status);

	} while (status == KErrNone);

	_printf(_L("Received error code: %d\n"),status.Int());
	_printf(_L("Expect -29(KErrCommsLineFail) or range -6702 to -6712 (USB driver errors)\n\n"));

	_printf(_L("\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void Shutdown_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status;
	RComm port;

	TBuf8<MAX_BUFFER_SIZE> readBuf;

	_printf(_L("\n-----------------------------------\n"));
	_printf(_L("This tests the USB Manager shutdown\n"));
	_printf(_L("during reads and writes.\n"));
	_printf(_L("-------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	_printf(_L("Reading data\n"));
	port.Read(status, readBuf);

	_printf(_L("Shutting down USB Manager.\n"));
	TheUsb.Stop();

	_printf(_L("Shutdown complete.\nWaiting for read to terminate."));
	User::WaitForRequest(status);

	if (status != KErrNone)	// Error code ? TODO
		{
		_printf(_L("Read failed\n"));
		}

	_printf(_L("Read complete.\nRestarting USB Manager\n"));
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	_printf(_L("Writing data\n"));

	port.Write(status, readBuf);

	_printf(_L("Shutting down USB Manager\n"));
	TheUsb.Stop();

	_printf(_L("shutdown complete.\nWaiting for write to terminate."));
	User::WaitForRequest(status);

	if (status != KErrNone)	// Error code ? TODO
	    {
		LEAVE(status.Int());
		}

	_printf(_L("Write complete.\nRestarting USB Manager\n"));
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	_printf(_L("\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void BufferOverrun_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status;
	RComm port;
	TInt i;

	_printf(_L("\n--------------------------------\n"));
	_printf(_L("This test read/writes which are\n"));
	_printf(_L("bigger than the buffer length.\n"));
	_printf(_L("----------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);

	_printf(_L("Reading data.\nBuffer lengths:"));
	for (i=128; i<=8*1024; i+=128)
	{
		port.SetReceiveBufferLength(i);

		_printf(_L(" %d"), i);
		port.Read(status, readBigBuf);

		User::WaitForRequest(status);

		if (status != KErrNone)
		{
			LEAVE(status.Int());
		}
	}

	_printf(_L("done.\nWriting data\nBuffer lengths:"));
	for (i=128; i<=8*1024; i+=128)
	{
		port.SetReceiveBufferLength(i);

		FillBuffer(readBigBuf);
		_printf(_L(" %d"), i);
		port.Write(status, readBigBuf);
		User::WaitForRequest(status);

		 if (status != KErrNone)
		 {
			  LEAVE(status.Int());
		 }
	}

	_printf(_L(" done.\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void Break_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status;
	RComm port;

	_printf(_L("\n--------------------------------\n"));
	_printf(_L("This tests break and break cancel.\n"));
	_printf(_L("----------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	_printf(_L("Breaking\n"));
	port.Break(status, 10);

	User::WaitForRequest(status);

	if (status != KErrNone)
	{
		LEAVE(status.Int());
	}

	_printf(_L("Press and key to cancel the break.\n"));
	_getch();

	_printf(_L("Cancelling the break.\n"));
	port.BreakCancel();

	_printf(_L("\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void SignalChange_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status, statusTimer;
	RComm port;

	_printf(_L("\n---------------------------------------\n"));
	_printf(_L("This tests signal change notifications.\n"));
	_printf(_L("---------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	TUint signals = 0;
	signals |= KSignalDTR;

	port.NotifySignalChange(status, signals);
	User::WaitForRequest(status);

	if (status != KErrNone)
		{
		LEAVE(status.Int());
		}

	_printf(_L("Notified\n"));

	port.NotifySignalChange(status, signals);

	// Wait
	timer.After(statusTimer, 10);
	User::WaitForRequest(statusTimer);

	_printf(_L("Cancelling request for signal notifications\n"));

	port.NotifySignalChangeCancel();
	User::WaitForRequest(status);

	if (status != KErrCancel)
		{
		LEAVE(status.Int());
		}

	_printf(_L("\nRequest cancelled.\n Test complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void FlowControl_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status, statusTimer;
	RComm port;

	_printf(_L("\n-------------------------------------------\n"));
	_printf(_L("This tests flow control change notifications.\n"));
	_printf(_L("---------------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	TFlowControl signals;
	port.NotifyFlowControlChange(status, signals);
	User::WaitForRequest(status);

	if (status != KErrNotSupported)
		{
		LEAVE(status.Int());
		}

	port.NotifyFlowControlChange(status, signals);

	// Wait
	timer.After(statusTimer, 10);
	User::WaitForRequest(statusTimer);

	port.NotifyFlowControlChangeCancel();
	User::WaitForRequest(status);
	if (status != KErrNotSupported)
		{
		LEAVE(status.Int());
		}

	_printf(_L("\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void ConfigChange_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	TRequestStatus status, statusTimer;
	RComm port;

	_printf(_L("\n-------------------------------------\n"));
	_printf(_L("This tests config change notifications.\n"));
	_printf(_L("---------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	TBuf8<64> signals;
	port.NotifyConfigChange(status, signals);
	User::WaitForRequest(status);

	if (status != KErrNone)
		{
		LEAVE(status.Int());
		}

	_printf(_L("Notified"));

	port.NotifyConfigChange(status, signals);

	// Wait
	timer.After(statusTimer, 10);
	User::WaitForRequest(statusTimer);

	_printf(_L("Cancelling request for config change notifications"));

	port.NotifyConfigChangeCancel();
	User::WaitForRequest(status);

	if (status != KErrCancel)
		{
		LEAVE(status.Int());
		}

	_printf(_L("\nRequest cancelled.\n Test complete\n"));
	CleanupStack::PopAndDestroy(); // port
}

void SecondClient_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
{
	RComm port1, port2;
	TRequestStatus status1, status2, consoleStatus;

	TBuf8<MAX_BUFFER_SIZE> readBuf1;
	TBuf8<MAX_BUFFER_SIZE> readBuf2;

	_printf(_L("\n---------------------------------------\n"));
	_printf(_L("This tests that we can have a second\n"));
	_printf(_L("client with non-exclusive access.\n"));
	_printf(_L("---------------------------------------\n\n"));

	LEAVEIFERROR(TheCommServ.Connect());
	_printf(_L("Connected to C32\n"));

	LEAVEIFERROR(port1.Open(TheCommServ, KUsbPortName, ECommShared, ECommRoleDCE));
	CleanupClosePushL(port1);
	LEAVEIFERROR(port2.Open(TheCommServ, KUsbPortName, ECommShared, ECommRoleDCE));
	CleanupClosePushL(port2);

	port1.SetConfig(TheConfigBuf);
	port1.SetReceiveBufferLength(256);
	port2.SetConfig(TheConfigBuf);
	port2.SetReceiveBufferLength(256);

	port1.SetReceiveBufferLength(256);
	port2.SetReceiveBufferLength(256);

	console->Read(consoleStatus);

	_printf(_L("Read()-ing data\n"));
	port1.Read(status1, readBuf1);
	port2.Read(status2, readBuf2);

	do
	{
		 User::WaitForAnyRequest();
		 if ( (status1 != KRequestPending) && (status1 != KErrNone) )
		 {
			  LEAVE(status1.Int());
		 }
		 if ( (status2 != KRequestPending) && (status2 != KErrNone) )
		 {
			  LEAVE(status2.Int());
		 }
	}
	while ( (status1 == KRequestPending) || (status2 == KRequestPending) );

	TBuf8<256> readBuf;

	_printf(_L("Write()-ing data\n"));
	FillBuffer(readBuf);

	port2.Write(status2, readBuf, 256);
	User::WaitForRequest(status2);

	if (status2 == KErrNone)
		{
		LEAVE(status2.Int());
		}

	_printf(_L("\nTest complete\n"));
	CleanupStack::PopAndDestroy(); // port
}


// ******************************************************************
// The following are placholders for the ACMRq tests.
// These tests need no support on the platform end, as yet.
// The functions are left here incase someday support is needed.
// ******************************************************************

void ACMRq_EncapCommand_TestL()
{
	 // Not needed to support this here
}

void ACMRq_Break_TestL()
{
	 // TODO: can we check for break status here ?
}

void ACMRq_SetFeature_TestL()
{
	 // TODO: Test doesn't make sense.
}

void ACMRq_ClearFeature_TestL()
{
	 // TODO: Test doesn't make sense.
}

void ACMRq_SetCoding_TestL()
{
	 // TODO: Can we check the line codeing here ?
}

void ACMRq_CtrlState_TestL()
{
	 // TODO: Test doesn't make sense.
}

void ACMRq_EncapResp_TestL()
{
	 // Not needed to support this here.
}

void ACMRq_CommsFeature_TestL()
{
	 // Not needed to support this here.
}

void ACMRq_GetCoding_TestL()
{
	 // Not needed to support this here.
}


void ACMNtf_SendState_TestL()
{
	 // TODO: SendSerialState() ???
}

void ACMNtf_Status_TestL()
{
	 // TODO: SendNetworkConnection() ???
}

void ACMNtf_RespAvail_TestL()
{
	 // Test not supported.
}

void Loopback_TestL()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
	{
	TRequestStatus consoleStatus;
	TRequestStatus status;
	RComm port;

	_printf(_L("\n----------------------------------------\n"));
	_printf(_L("This test listens for data on the ACM\n"));
	_printf(_L("port and echoes anything it receives\n"));
	_printf(_L("back to the PC. Press any key to quit.\n"));
	_printf(_L("----------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	_printf(_L("Echoing received data\n"));

	TBuf8<1024> readBuf;

	console->Read(consoleStatus);

	while (1)
		{
		port.ReadOneOrMore(status, readBuf);
		User::WaitForRequest(status, consoleStatus);

		if (consoleStatus == KRequestPending)
			{
			if (status != KErrNone)
				{
				// need to cancel the read to avoid a CBase-77 panic!
				console->ReadCancel();
				LEAVE(status.Int());
				}
			}
		else
			{
			port.ReadCancel();
			break;
			}


		port.Write(status, readBuf);
		User::WaitForRequest(status, consoleStatus);

		if (consoleStatus == KRequestPending)
			{
			if (status != KErrNone)
				{
				// need to cancel the read to avoid a CBase-77 panic!
				console->ReadCancel();
				LEAVE(status.Int());
				}
			}
		else
			{
			port.WriteCancel();
			break;
			}

		_printf(_L("."));
		}

	_printf(_L("\nTest complete\n"));

	CleanupStack::PopAndDestroy(); // port
	}

void SetHandshakingL()
/**
 * This function allows the user to select a new handshaking mode.
 */
	{
	RComm port;

	TCommCaps capsBuf;
	TCommCapsV01& caps = capsBuf();

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.Caps(capsBuf);
	_printf(_L("\nPort handshaking capabilities: 0x%X\n"), caps.iHandshake);
	_printf(_L("Current handshaking options: 0x%X\n"), TheConfig.iHandshake);

	_printf(_L("\nHandshaking options:\n"));
	_printf(_L("1. No handshaking\n"));
	_printf(_L("2. Toggle Xon/Xoff\n"));
	_printf(_L("3. Toggle obey CTS\n"));
	_printf(_L("4. Toggle obey DSR / free RTS\n"));
	_printf(_L("5. Toggle write buffered complete\n"));

	TInt key = (TInt) _getch();

	switch (key)
		{
	case '1':
		TheConfig.iHandshake = 0;
		break;
	case '2':
		TheConfig.iHandshake ^= KConfigObeyXoff;
		TheConfig.iHandshake ^= KConfigSendXoff;
		break;
	case '3':
		TheConfig.iHandshake ^= KConfigObeyCTS;
		break;
	case '4':
		TheConfig.iHandshake ^= KConfigObeyDSR;
		TheConfig.iHandshake ^= KConfigFreeRTS;
		break;
	case '5':
		TheConfig.iHandshake ^= KConfigWriteBufferedComplete;
		break;
	default:
		break;
		}

	LEAVEIFERROR(port.SetConfig(TheConfigBuf));

	_printf(_L("Handshaking options now: 0x%X\n"), TheConfig.iHandshake);

	CleanupStack::PopAndDestroy();
	}

void RestartUsbL()
/**
 * This function stops and restarts usb manager.
 */
	{
	TheUsb.Stop();

	TRequestStatus status;
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	_printf(_L("Restarted USB.\n"));
	}

void mainL()
/**
 * This function controls test execution as directed by the user.
 */
	{
		 char ch;
		 TInt menu = 1;

	TInt ret = User::LoadLogicalDevice(KUsbLddName);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		LEAVE(ret);

	_printf(_L("Loaded USB LDD\n"));

	ret = StartC32();
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		LEAVE(ret);

	_printf(_L("Started C32\n"));

	LEAVEIFERROR(TheUsb.Connect());

	_printf(_L("Connected to USB Manager\n"));

	TRequestStatus status;
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	_printf(_L("Started USB\n"));

	LEAVEIFERROR(TheCommServ.Connect());

	_printf(_L("Connected to C32\n"));

	RComm port;

	// The port's configuration seems to be junk at the beginning, so we set it to known values.

	TheConfig.iRate = EBps115200;
	TheConfig.iDataBits = EData8;
	TheConfig.iStopBits = EStop1;
	TheConfig.iParity = EParityNone;
	TheConfig.iHandshake = 0;
	TheConfig.iTerminatorCount = 0;

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.SetConfig(TheConfigBuf);

	CleanupStack::PopAndDestroy(); // port

	TBool noExit = ETrue;
	while (noExit)
		{
			 switch(menu)
			 {
			 case 1:
				  _printf(_L("\nAvailable tests:\n\n"));
				  _printf(_L("1. Loopback test\n"));
				  _printf(_L("2. Set handshaking\n"));
				  _printf(_L("3. Restart USB\n"));
				  _printf(_L("4. Test specification menu\n"));
				  _printf(_L("\nSelection (x to exit): "));

				  ch = (char) _getch();
				  _printf(_L("\n"));
				  switch (ch)
				  {
				  case '1': Loopback_TestL(); break;
				  case '2': SetHandshakingL(); break;
				  case '3': RestartUsbL(); break;
				  case '4': menu = 2; break;
				  case 'x':
				  case 'X': noExit = EFalse; break;
				  default:  _printf(_L("\nInvalid key\n")); break;
				  }
				  break;
			 case 2:
				  _printf(_L("\nAvailable tests:\n\n"));
				  _printf(_L("1. Data stress, size varies (test 2.1.1)\n"));
				  _printf(_L("2. Data stress, rate varies (test 2.1.2)\n"));
				  _printf(_L("3. Timeout (test 2.2)\n"));
				  _printf(_L("4. Cancel Transfer (test 2.3)\n"));
				  _printf(_L("5. Interrupt Transfer (test 2.4)\n"));
				  _printf(_L("6. Shutdown (test 2.5)\n"));
				  _printf(_L("7. Buffer overrun (test 2.6)\n"));
				  _printf(_L("8. Break (test 2.7)\n"));
				  _printf(_L("9. Event notification, signal change (test 2.8.1)\n"));
				  _printf(_L("a. Event notification, flow control (test 2.8.2)\n"));
				  _printf(_L("b. Event notification, config change (test 2.8.3)\n"));
				  _printf(_L("c. Second client (test 2.9)\n"));
				  _printf(_L("p. Loopback test (test 2.12)\n"));
				  _printf(_L("q. Main menu\n"));

				  ch = (char) _getch();
				  _printf(_L("\n"));
				  switch (ch)
				  {
				  case '1': DataStress_SizeVary_TestL();	break;
				  case '2': DataStress_RateVary_TestL();	break;
				  case '3': TimeOut_TestL();	break;
				  case '4': CancelTx_TestL();	break;
				  case '5': InterruptTx_TestL();	break;
				  case '6': Shutdown_TestL();	break;
				  case '7': BufferOverrun_TestL();	break;
				  case '8': Break_TestL();	break;
				  case '9': SignalChange_TestL();	break;
				  case 'A':
				  case 'a': FlowControl_TestL();	break;
				  case 'B':
				  case 'b': ConfigChange_TestL();	break;
				  case 'C':
				  case 'c': SecondClient_TestL();	break;
				  case 'D':
				  case 'd': ACMRq_EncapCommand_TestL();	break;
				  case 'E':
				  case 'e': ACMRq_Break_TestL();	break;
				  case 'F':
				  case 'f': ACMRq_SetFeature_TestL();	break;
				  case 'G':
				  case 'g': ACMRq_ClearFeature_TestL();	break;
				  case 'H':
				  case 'h': ACMRq_SetCoding_TestL();	break;
				  case 'I':
				  case 'i': ACMRq_CtrlState_TestL();	break;
				  case 'J':
				  case 'j': ACMRq_EncapResp_TestL();	break;
				  case 'K':
				  case 'k': ACMRq_CommsFeature_TestL();	break;
				  case 'L':
				  case 'l': ACMRq_GetCoding_TestL();	break;
				  case 'M':
				  case 'm': ACMNtf_SendState_TestL();	break;
				  case 'N':
				  case 'n': ACMNtf_Status_TestL();	break;
				  case 'O':
				  case 'o': ACMNtf_RespAvail_TestL();	break;
				  case 'P':
				  case 'p': Loopback_TestL(); break;
				  case 'q':
				  case 'Q': menu = 1; break;
				  default:
					   _printf(_L("\nInvalid key\n"));
					   break;
				  }
				  break;
			 default:
				  LEAVE(-1);
			 }
		}
	TheCommServ.Close();
	TheUsb.Close();
	}

void consoleMainL()
/**
 * Create a console and run mainL().
 */
	{
	console=Console::NewL(_L("T_ACM"),TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	mainL();
	_printf(_L("[ press any key ]"));
	_getch();
	CleanupStack::PopAndDestroy();
	}

GLDEF_C TInt E32Main()
/**
 * Runs the test as specified in the ACM unit test specification.
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();

	// create the timer for use during some of the tests
	timer.CreateLocal();

	TRAP_IGNORE(consoleMainL());
	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}
