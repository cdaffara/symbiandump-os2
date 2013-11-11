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
* and is to be used in conjunction with the host-side application.
*
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32math.h>
#include <c32comm.h>
#include <usbman.h>

LOCAL_D CConsoleBase* console;

RCommServ TheCommServ;


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
#define MAX_HEAP_BUFFER_SIZE	1024*8
TBuf8<MAX_HEAP_BUFFER_SIZE> readBigBuf;

// A timer for use by several of the tests
RTimer timer;

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
	RComm port2;

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

	LEAVEIFERROR(port2.Open(TheCommServ, _L("ACM::1"), ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port2);

	_printf(_L("Setting handshaking & receive buffer length\n"));

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);
	port2.SetConfig(TheConfigBuf);
	port2.SetReceiveBufferLength(KReceiveBufferLength);

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


	console->Read(consoleStatus);
	
	FOREVER
		{
		port2.Read(status, readBigBuf, pktlen);

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
			port2.ReadCancel();
			break;
			}

		port2.Write(status, readBigBuf);
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
			port2.WriteCancel();
			break;
			}
		}


	_printf(_L("\nTest complete\n"));

	CleanupStack::PopAndDestroy(2); // port	, port2
	
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


void mainL()
/**
 * This function controls test execution as directed by the user.
 */
	{
		 char ch;

	TInt ret = User::LoadLogicalDevice(KUsbLddName);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		LEAVE(ret);

	_printf(_L("Loaded USB LDD\n"));

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

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortNameAcm1, ECommExclusive, ECommRoleDCE));
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
		_printf(_L("\nAvailable tests:\n\n"));
		  _printf(_L("1. Read loopback from ACM0->ACM1 test\n"));
		_printf(_L("\nSelection (x to exit): "));

		ch = (char) _getch();
		_printf(_L("\n"));
		switch (ch)
		{
		case '1': ReadLoopbackFromPortAToPortBTestL(); break;
		case 'x':
		case 'X': noExit = EFalse; break;
		default:  _printf(_L("\nInvalid key\n")); break;
			 }
		}
	TheCommServ.Close();
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

