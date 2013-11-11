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
* This program executes all the tests in the ACM CSY Test Specification v 0.1
* and is to be used in conjunction with the host-side application. (t_acm_wins)
*
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32math.h>
#include <c32comm.h>
#include <usbman.h>
#include <e32debug.h>

LOCAL_D CConsoleBase* console;

RCommServ TheCommServ;
RUsb TheUsb;

TCommConfig TheConfigBuf;
TCommConfigV01& TheConfig = TheConfigBuf();

const TInt KReceiveBufferLength = 4096;
const TInt KMaxAcmPortNameLength = 8;
_LIT(KUsbPortName, "ACM::0");
_LIT(KUsbPortNameAcm1, "ACM::1");

_LIT(KUsbLddName, "EUSBC");

#define _printf console->Printf
#define _getch console->Getch
#define LEAVE(_x) VerboseLeaveL(_x, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))
#define LEAVEIFERROR(_x) VerboseLeaveIfErrorL(_x, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))
#define CHECK(_x) if (! (_x)) VerboseLeaveL(KErrGeneral, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))

// A define for stack-based buffers
#define MAX_BUFFER_SIZE 1024

// A bigger buffer on the heap
#define MAX_HEAP_BUFFER_SIZE	(1024*8)
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

//////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  loopback From a Port input by the user to abother port-
 *  ACM::1 KUsbPortNameAcm1 to ACM::0 KUsbPortName by default
 */

void ReadLoopbackFromPortAToPortBTestL()
	{

	TRequestStatus consoleStatus;
	TRequestStatus status;
	RComm portIN;
	RComm portOut;

	_printf(_L("Enter the packet size:"));
	TBufC<8> buf;
	TPtr ptr (buf.Des());
	ReadString(ptr);

	TLex input(buf);

	TInt pktlen = 0;
	while ((input.Peek()).IsDigit()) {
		pktlen = 10*pktlen + (input.Get()) - '0';
	}

	// Get port names
	_printf(_L("\nEnter acm port name as loopback INPUT: (ex ACM::1):"));
	TBufC<KMaxAcmPortNameLength> portINName;
	TPtr portINNamePtr (portINName.Des());
	ReadString(portINNamePtr);
	if ( portINNamePtr.Length() == 0 )
		{
		portINName= KUsbPortNameAcm1;
		}

	_printf(_L("\nEnter acm port name as loopback OUTPUT: (ex ACM::0):"));
	TBufC<KMaxAcmPortNameLength> portOutName;
	TPtr portOutNamePtr (portOutName.Des());
	ReadString(portOutNamePtr);
	if ( portOutNamePtr.Length() == 0 )
		{
		portOutName = KUsbPortName;
		}


	_printf(_L("\n----------------------------------------\n"));
	_printf(_L("This test listens for data on the ACM port: "));
	_printf(portINName);
	_printf(_L("\nwrites anything it receives on the ACM port: "));
	_printf(portOutName);
	_printf(_L("\nPress any key to quit.\n"));
	_printf(_L("----------------------------------------\n\n"));

	LEAVEIFERROR(portIN.Open(TheCommServ, portINName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(portIN);

	LEAVEIFERROR(portOut.Open(TheCommServ, portOutName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(portOut);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	portIN.SetConfig(TheConfigBuf);
	portIN.SetReceiveBufferLength(KReceiveBufferLength);
	portOut.SetConfig(TheConfigBuf);
	portOut.SetReceiveBufferLength(KReceiveBufferLength);

	_printf(_L("loopback received data\n"));

	console->Read(consoleStatus);

	FOREVER
		{
		portIN.Read(status, readBigBuf, pktlen);

		User::WaitForRequest(status, consoleStatus);

		if (consoleStatus == KRequestPending)
			{
			if (status != KErrNone)
				{

				console->ReadCancel();
				LEAVE(status.Int());
				}
			}
		else
			{
			portIN.ReadCancel();
			break;
			}

		portOut.Write(status, readBigBuf);
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
			portOut.WriteCancel();
			break;
			}
		}

	portOut.WriteCancel();

	_printf(_L("\nTest complete\n"));

	CleanupStack::PopAndDestroy(2); // portA	, portOut

	}

//////////////////////////////////////////////////////////////////////////////////////////////
/**
 *  Original loopback test - uses Read().
 */

void ReadLoopbackTestL()
	{

	TRequestStatus consoleStatus;
	TRequestStatus status;
	RComm port;

	_printf(_L("Enter the packet size:"));
	TBufC<8> buf;
	TPtr ptr (buf.Des());
	ReadString(ptr);

	TLex input(buf);

	TInt pktlen = 0;
	while ((input.Peek()).IsDigit()) {
		pktlen = 10*pktlen + (input.Get()) - '0';
	}

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

	console->Read(consoleStatus);

	FOREVER
		{
		port.Read(status, readBigBuf, pktlen);

		User::WaitForRequest(status, consoleStatus);

		if (consoleStatus == KRequestPending)
			{
			if (status != KErrNone)
				{

				console->ReadCancel();
				LEAVE(status.Int());
				}
			}
		else
			{
			port.ReadCancel();
			break;
			}
		port.Write(status, readBigBuf);
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
		}

	_printf(_L("\nTest complete\n"));

	CleanupStack::PopAndDestroy(); // port

	}

/**
 * Original loopback test - uses ReadOneOrMore().
 */

void LoopbackTestL()
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

	TBuf8<256> readBuf;

	console->Read(consoleStatus);

	FOREVER
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
		}

	_printf(_L("\nTest complete\n"));

	CleanupStack::PopAndDestroy(); // port
	}

/**
 * NotifydataAvailable test.
 */

void NotifyDataAvailableTestL()
	{
	TRequestStatus status;
	TRequestStatus stat2;
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

	TBuf8<256> readBuf;

	_printf(_L("*********************************\n"));
	_printf(_L("test NotifyDataAvailable\n"));
	TRequestStatus stat;
	TRequestStatus status2;

	_printf(_L("test NotifyDataAvailable, nominal test\n"));

	port.NotifyDataAvailable(status);

	User::WaitForRequest(status);	// wait until the data is available

	LEAVEIFERROR(status.Int());

	TBuf8<30> receive;
	port.ReadOneOrMore(status,receive);
	User::WaitForRequest(status);

	LEAVEIFERROR(status.Int());

	_printf(_L("test NotifyDataAvailable, nominal test succeeded\n"));


	//=========== Test the KErrInUse that should be returned when a notification of data available is posted at the same
	// time as a Read - or visa versa  ============== //
	_printf(_L("test NotifyDataAvailable, KErrInUse mode\n"));

	TBuf8<10> buf;
	port.ReadOneOrMore(status,buf);
	port.NotifyDataAvailable(status2);
	User::WaitForRequest(status2);
	if (status2.Int() != KErrInUse)
		{
		LEAVE(status2.Int());
		}
	LEAVEIFERROR(port.ReadCancel());

	User::WaitForRequest(status);
	if (status.Int() != KErrCancel)
		{
		LEAVE(status.Int());
		}
	// 2nd case
	port.NotifyDataAvailable(status);
	port.ReadOneOrMore(status2,buf);
	User::WaitForRequest(status2);
	if (status2.Int() != KErrInUse)
		{
		LEAVE(status2.Int());
		}

	// cancel
	LEAVEIFERROR(port.NotifyDataAvailableCancel());

	User::WaitForRequest(status);

	if (status.Int() != KErrCancel)
		{
		LEAVE(status.Int());
		}
	_printf(_L("test NotifyDataAvailable, KErrInUse test succeeded\n"));


	// ====== test NotifyDataAvailable, testing canceling

	_printf(_L("test NotifyDataAvailable, testing canceling\n"));
	port.NotifyDataAvailable(stat);
	User::WaitForRequest(stat);

	LEAVEIFERROR(stat.Int());

	port.ResetBuffers();
	// testing canceling
	stat2=KErrNone;
	port.NotifyDataAvailable(stat2);
	LEAVEIFERROR(port.NotifyDataAvailableCancel());
	User::WaitForRequest(stat2); //
	if (stat2.Int() != KErrCancel)
		{
		LEAVE(stat2.Int());
		}
	stat2=KErrNone;
	port.NotifyDataAvailable(stat2);
    LEAVEIFERROR(port.NotifyDataAvailableCancel());

	User::WaitForRequest(stat2); //
	if (stat2.Int() != KErrCancel)
		{
		LEAVE(stat2.Int());
		}

	_printf(_L("test NotifyDataAvailable, testing canceling succeeded\n"));

	User::After(20000000);

	_printf(_L("\nTest complete\n"));

	CleanupStack::PopAndDestroy(); // port
	}

void ReadWithTerminatorsLoopbackTestL()
	{
	TRequestStatus consoleStatus;
	TRequestStatus status;
	RComm port;

	// Get terminator characters
	_printf(_L("Enter the terminator characters (up to %d):"), KConfigMaxTerminators);
	TBufC<KConfigMaxTerminators> termbuf;
	TPtr termptr (termbuf.Des());
	ReadString(termptr);
	TText8 terminators[KConfigMaxTerminators];
	if ( termptr.Length() == 0 )
		{
		_printf(_L("\nno terminators given- not running test"));
		return;
		}
	TUint termCount = 0;
	TUint ii;
	for ( ii = 0 ; ii < (TUint)termptr.Length() ; ii++ )
		{
		termCount++;
		terminators[ii] = (TText8)termptr[ii];
		}

	_printf(_L("\nEnter the packet size:"));
	TBufC<8> buf;
	TPtr ptr (buf.Des());
	ReadString(ptr);

	TLex input(buf);

	TInt pktlen = 0;
	while ((input.Peek()).IsDigit()) {
		pktlen = 10*pktlen + (input.Get()) - '0';
	}

	_printf(_L("\n----------------------------------------\n"));
	_printf(_L("This test listens for data on the ACM\n"));
	_printf(_L("port and echoes anything it receives\n"));
	_printf(_L("back to the PC. Press any key to quit.\n"));
	_printf(_L("----------------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	// Set the config, including terminator characters.
	TCommConfig configBuf;
	TCommConfigV01& config = configBuf();
	port.Config(configBuf);
	config.iTerminatorCount = termCount;
	for ( ii = 0 ; ii < termCount ; ii++ )
		{
		config.iTerminator[ii] = terminators[ii];
		}
	LEAVEIFERROR(port.SetConfig(configBuf));

	port.SetReceiveBufferLength(KReceiveBufferLength);

	_printf(_L("Echoing received data\n"));

	console->Read(consoleStatus);

	TBuf8<256> readBuf;

	FOREVER
		{
		port.Read(status, readBuf, pktlen);

		User::WaitForRequest(status, consoleStatus);

		if (consoleStatus == KRequestPending)
			{
			if (status != KErrNone)
				{
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
		}

	_printf(_L("\nTest complete\n"));

	CleanupStack::PopAndDestroy(); // port
	}

/////////////////////////////////////////////////////////////////////////////////

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
	TUint totalExpectedTransfer = 0;
	for (i = 1; i<MAX_BUFFER_SIZE; i*=2)
		{
		totalExpectedTransfer += i;
		}
	TUint totalTransfer = 0;
	while ( totalTransfer < totalExpectedTransfer )
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
				totalTransfer += readBuf.Length();
			}
		}

	_printf(_L("\n Writing\n"));

	for (i = 1; i<MAX_BUFFER_SIZE; i*=2)
		{

		FillBuffer(readBuf);

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


	_printf(_L("done.\nReadOneOrMore()-ing data.\n "));
	int totalTransferSize = 19*MAX_BUFFER_SIZE;
	int totalTransfer = 0;
	while (totalTransfer < totalTransferSize) {
		port.ReadOneOrMore(status, readBuf);
		User::WaitForRequest(status);

		if (status != KErrNone)
			{
			LEAVE(status.Int());
			}
		else
			{
			totalTransfer += readBuf.Length();
			}
	}

	_printf(_L(" Writing data\n"));

	FillBuffer(readBuf);

	for (i = 1; i< 100; i++)
		{
		_printf(_L(" %d "), i);
		port.Write(status, readBuf, MAX_BUFFER_SIZE);
		User::WaitForRequest(status);

		if (status != KErrNone)
			{
			_printf(_L("Write failed\n"));
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

	TBuf8<MAX_BUFFER_SIZE> writeBuf;
	writeBuf = _L8("some data");
	_printf(_L(" done.\nWriting data\nTimeouts (ms):"));
	for (timeout = 10; timeout<100; timeout+=10)
		{
		 // Clean buffers
		 port.ResetBuffers();

		_printf(_L(" %d"), timeout);
		port.Write(status, timeout, writeBuf);
		User::WaitForRequest(status);

		if (status != KErrTimedOut && status != KErrNone ) // NB Writes complete very quickly.
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
	User::After(1000000);

	_printf(_L("Pull cable now\n"));
	_printf(_L("Writing data...\n"));
	do
	{
		 port.Write(status, readBuf);
		 User::WaitForRequest(status);

	} while (status == KErrNone);

	_printf(_L("Received error code: %d\n"),status.Int());
	_printf(_L("Expect -29(KErrCommsLineFail) or range -6702 to -6712 (USB driver errors)\n\n"));

	_printf(_L("Plug in cable and press any key- test is now finished\n"));
	_getch();

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

	_printf(_L("shutdown complete.\nWaiting for read to terminate."));
	User::WaitForRequest(status);

	if (status != KErrAccessDenied)
		{
		LEAVE(status.Int());
		}

	_printf(_L("Read complete.\nRestarting USB Manager\n"));
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	// Hacky pause to wait for device to enumerate.
	User::After(1000000);
	_printf(_L("Writing data\n"));

	port.Write(status, readBuf);

	_printf(_L("Shutting down USB Manager\n"));
	TheUsb.Stop();

	_printf(_L("shutdown complete.\nWaiting for write to terminate."));
	if ( status == KErrNone )
		{
		// The Write already completed. This part of the test didn't work
		// (i.e. it didn't actually test 'shutting down the USB Manager
		// causes Write completion'), but there's not much we can do about it.
		}
	else
		{
		// If the Write is still outstanding at this point, it should
		// eventually complete with access denied.
		User::WaitForRequest(status);
		if (status != KErrAccessDenied)
			{
			LEAVE(status.Int());
			}
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
 * Test updated from that in the ACM unit test specification to
 * read/write messages bigger than the receive and transmit buffers.
 * Changed as previous test was no longer valid.
 */
{
	TRequestStatus status;
	RComm port;

	_printf(_L("\n--------------------------------\n"));
	_printf(_L("This tests read/writes which are\n"));
	_printf(_L("bigger than the buffer length.\n"));
	_printf(_L("----------------------------------\n\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);

	_printf(_L("Reading data.\nBuffer length: %d"), MAX_HEAP_BUFFER_SIZE);

	port.SetReceiveBufferLength(128);

	port.Read(status, readBigBuf);

	User::WaitForRequest(status);

	if (status != KErrNone)
		{
		LEAVE(status.Int());
		}

	_printf(_L("done.\nWriting data\nBuffer length: %d"), MAX_HEAP_BUFFER_SIZE);
	FillBuffer(readBigBuf);

	port.SetReceiveBufferLength(128);

	port.Write(status, readBigBuf);
	User::WaitForRequest(status);

	 if (status != KErrNone)
		 {
		  LEAVE(status.Int());
		 }

	_printf(_L(" done.\nTest complete\n"));
	_printf(_L("[ press any key ]"));
	_getch();

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

	// Check that NotifyBreak is not completed at any point (should only be
	// completed by host-driven breaks).
	TRequestStatus notifyBreakStat;
	port.NotifyBreak(notifyBreakStat);

	_printf(_L("Breaking\n"));
	port.Break(status, 10);

	User::WaitForRequest(status);

	if (status != KErrNone)
	{
		LEAVE(status.Int());
	}

	_printf(_L("Press any key to cancel a break.\n"));
	_getch();

	_printf(_L("Cancelling the break.\n"));
	port.Break(status, 1000000);
	port.BreakCancel();
	User::WaitForRequest(status);
	if (status != KErrCancel)
	{
		LEAVE(status.Int());
	}

	User::After(1000000);
	if ( notifyBreakStat != KRequestPending )
		{
		LEAVE(notifyBreakStat.Int());
		}
	port.NotifyBreakCancel();
	User::WaitForRequest(notifyBreakStat);
	if ( notifyBreakStat != KErrCancel )
		{
		LEAVE(notifyBreakStat.Int());
		}

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

	_printf(_L("Notified, waiting for a while before the next test\n"));

	// Wait
	timer.After(statusTimer, 10);
	User::WaitForRequest(statusTimer);

	_printf(_L("Cancelling request for config change notifications\n"));

	port.NotifyConfigChange(status, signals);
	_printf(_L("Cancelling request for config change notifications\n"));
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
 * Updated to use a second RCommServ and to read/read in series.
 * Tests that two clients can read/write on a single serial comms server.
 * Note that it is invalid to have 2 reads/writes outstanding on a
 * serial comms server. It would return a KErrInUse.
 */
{
	RCommServ TheCommServ2;
	RComm port1, port2;
	TRequestStatus status1, status2;

	TBuf8<MAX_BUFFER_SIZE> readBuf1;
	TBuf8<MAX_BUFFER_SIZE> readBuf2;

	_printf(_L("\n---------------------------------------\n"));
	_printf(_L("This tests that we can have a second\n"));
	_printf(_L("client with non-exclusive access.\n"));
	_printf(_L("---------------------------------------\n\n"));

	// TheCommServ is already connected in mainL
	_printf(_L("Connected to C32\n"));
	LEAVEIFERROR(TheCommServ2.Connect());
	CleanupClosePushL(TheCommServ2);
	_printf(_L("Connected to second C32\n"));

	LEAVEIFERROR(port1.Open(TheCommServ, KUsbPortName, ECommShared, ECommRoleDCE));
	CleanupClosePushL(port1);
	LEAVEIFERROR(port2.Open(TheCommServ2, KUsbPortName, ECommShared, ECommRoleDCE));
	CleanupClosePushL(port2);

	port1.SetConfig(TheConfigBuf);
	port1.SetReceiveBufferLength(256);
	port2.SetConfig(TheConfigBuf);
	port2.SetReceiveBufferLength(256);

	_printf(_L("Read()-ing data\n"));
	port1.Read(status1, readBuf1);
	_printf(_L("First data read\n"));

	User::WaitForAnyRequest();
	if ( status1 != KErrNone )
		{
		LEAVE(status1.Int());
		}
	_printf(_L("status of first read: %d\n"), status1.Int());

	port2.Read(status2, readBuf2);
	_printf(_L("Second data read\n"));

	User::WaitForAnyRequest();
	if ( status2 != KErrNone )
		{
		LEAVE(status2.Int());
		}
	_printf(_L("status of second read: %d\n"), status2.Int());

	_printf(_L("Reading complete\n"), status1.Int());

	TBuf8<256> readBuf;

	_printf(_L("Write()-ing data\n"));
	FillBuffer(readBuf);

	// Test Writes
	port1.Write(status1, readBuf, 256);
	User::WaitForAnyRequest();
	if ( status1 != KErrNone )
		{
		LEAVE(status1.Int());
		}
	_printf(_L("status of first write : %d\n"), status1.Int());

	port2.Write(status2, readBuf, 256);
	User::WaitForAnyRequest();
	if ( status2 != KErrNone )
		{
		LEAVE(status2.Int());
		}
	_printf(_L("status of second write : %d\n"), status1.Int(), status2.Int());

	_printf(_L("\nTest complete\n"));
	_printf(_L("[ press any key ]"));
	_getch();

	CleanupStack::PopAndDestroy(); // port
	CleanupStack::PopAndDestroy(); // the other port
	CleanupStack::PopAndDestroy(); // TheCommServ2 
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

void AcmLoopback_TestL()
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

	FOREVER
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
	_printf(_L("----------------------------------------\n"));
	_printf(_L("Initial port config:\n"));
	_printf(_L("  Rate: %d bps\n"), TheConfig.iRate);
	_printf(_L("  Data bits: %d. Parity type: %d. Stop bits: %d\n"),
		TheConfig.iStopBits, TheConfig.iParity, TheConfig.iStopBits);
	_printf(_L("  Handshaking options: 0x%X\n"), TheConfig.iHandshake);
	_printf(_L("----------------------------------------\n\n"));

	CleanupStack::PopAndDestroy(); // port

	TBool noExit = ETrue;
	while (noExit)
		{
			 switch(menu)
			 {
			 case 1:
				  _printf(_L("\nAvailable tests:\n\n"));
				  _printf(_L("1. ReadOneOrMore loopback test\n"));
				  _printf(_L("2. Read loopback test\n"));
				  _printf(_L("3. Set handshaking\n"));
				  _printf(_L("4. Restart USB\n"));
				  _printf(_L("5. Test specification menu\n"));
				  _printf(_L("6. Read with terminators loopback test\n"));
  				  _printf(_L("7. Read loopback from ACM0->ACM1 test\n"));
  				  _printf(_L("8. NotifyDataAvailable test\n"));
				  _printf(_L("\nSelection (x to exit): "));

				  ch = (char) _getch();
				  _printf(_L("\n"));
				  switch (ch)
				  {
				  case '1': LoopbackTestL(); break;
				  case '2': ReadLoopbackTestL(); break;
				  case '3': SetHandshakingL(); break;
				  case '4': RestartUsbL(); break;
				  case '5': menu = 2; break;
				  case '6': ReadWithTerminatorsLoopbackTestL(); break;
				  case '7': ReadLoopbackFromPortAToPortBTestL(); break;
				  case '8': NotifyDataAvailableTestL(); break;
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
				  case 'p': AcmLoopback_TestL(); break;
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
