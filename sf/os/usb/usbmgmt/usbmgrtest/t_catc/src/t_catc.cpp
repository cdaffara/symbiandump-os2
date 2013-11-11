/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TODO: This is used for non-loopback (i.e. bulk IN and OUT) performance 
* testing. It has been modified (see TODO: speed enhancement! marks) from the 
* vanilla version in the team directory (Camfiler01\softeng\Departmental\Pan
* \Team\USB\Useful Stuff\t_catc\t_catc.cpp) to make it faster
*
*/

#include <e32base.h>
#include <e32cons.h>
#include <e32std.h>
#include <badesca.h>
#include <c32comm.h>
#include <usbman.h>

////////////////////////////////////////////////////////////////////////////////

LOCAL_D CConsoleBase* console;

RCommServ TheCommServ;
RUsb TheUsb;

TCommConfig TheConfigBuf;
TCommConfigV01& TheConfig = TheConfigBuf();

const TInt KReceiveBufferLength = 65536/*16384*/; // TODO: speed enhancement!
const TUint KChunkSize = 65536;

////////////////////////////////////////////////////////////////////////////////

_LIT(KUsbPortName, "ACM::0");
_LIT(KUsbLddName, "EUSBC");

////////////////////////////////////////////////////////////////////////////////

#define _printf console->Printf
#define _getch  console->Getch

////////////////////////////////////////////////////////////////////////////////

TInt bReadCall = ETrue ;
TInt bUseZLP   = EFalse ;
TInt bUseTerm  = EFalse ;

////////////////////////////////////////////////////////////////////////////////

#define PARM(_c,_x)			_c, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x)

#define LEAVE(_x)			VerboseLeaveL(PARM(_x,_x))
#define LEAVEIFERROR(_x)	VerboseLeaveIfErrorL(PARM(_x,_x))

#define CHECK(_x)			if(! (_x) )										\
								{											\
								VerboseLeaveL(PARM(KErrGeneral,_x));		\
								}

void VerboseLeaveL(TInt aError, TInt aLineNum, const TDesC8& aFileName, const TDesC8& aCode)
	{
	TInt filenameOffset = aFileName.LocateReverse('\\') + 1;
	if (filenameOffset < 0) filenameOffset = 1;
	TPtrC8 shortFileName = aFileName.Mid(filenameOffset);
	TBuf<64> fName, code;
	fName.Copy(shortFileName.Left(64));
	code.Copy(aCode.Left(64));
	_printf(_L("ERROR (%d) on line %d of file %S\n"), aError, aLineNum, &fName);
	_printf(_L("Code: %S\n\n"), &code);
	_printf(_L("[ press any key ]"));
	_getch();
	User::Leave(aError);
	}

void VerboseLeaveIfErrorL(TInt aError, TInt aLineNum, const TDesC8& aFileName, const TDesC8& aCode)
	{
	if (aError)
		{
		VerboseLeaveL(aError, aLineNum, aFileName, aCode);
		}
	}

////////////////////////////////////////////////////////////////////////////////

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
			{
			break;
			}

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

////////////////////////////////////////////////////////////////////////////////

void Bulk_OUT_TestL()
	{
	TRequestStatus consoleStatus;
	TRequestStatus status;
	RComm port;

	static TBuf8<KChunkSize> OUT_Buf;

	_printf(_L("\n"));
	_printf(_L("+-----------------------------------------+\n"));
	_printf(_L("|+---------------------------------------+|\n"));
	_printf(_L("|| This test listens for data on the ACM ||\n"));
	_printf(_L("|| (Bulk OUT) data port   .              ||\n"));
	_printf(_L("||                                       ||\n"));
	_printf(_L("|| Once running, Press any key to quit.  ||\n"));
	_printf(_L("|+---------------------------------------+|\n"));
	_printf(_L("+------------------------------------[rjf]+\n"));
	_printf(_L("\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	// prime the console for the abandon-ship

	console->Read(consoleStatus);
	
	TInt uReadCount = 0 ;

	_printf(_L("\tWatch :  "));

	TInt uAvgCount = 0 ;
	TInt uNumReads = 0 ;
	TInt uNumRooms = 0 ;

	while (1)
		{
		// setup read to collect a single block by whatever method
		// has been selected.

		if( bReadCall )
			{
			port.Read(status, OUT_Buf, KChunkSize); // TODO: speed enhancement!

			uNumReads++ ;
			}
		else
			{
			port.ReadOneOrMore(status, OUT_Buf);

			uNumRooms++ ;
			}

		// either the read is complete, or user request to stop the test

		User::WaitForRequest(status,consoleStatus);

		if (consoleStatus == KRequestPending)
			{
			// still waiting for keypress, so it must have been the read
			// that completed, check if it worked OK

			if (status != KErrNone)
				{
				console->ReadCancel();

				LEAVE(status.Int());
				}

			// reassure watcher that there is something happening...

			if( uAvgCount == 0 )
				{
				uAvgCount = OUT_Buf.Length() ;
				}
			else
				{
				uAvgCount = ( ( 9 * uAvgCount ) + OUT_Buf.Length() ) / 10 ;
				}

			uReadCount += OUT_Buf.Length() ;
			}
		else
			{
			// user must have wanted to stop, shut down the reader

			_printf(_L("\n"));
			_printf(_L("\n"));
			_printf(_L("\nReads=%d, Rooms=%d, Average Count=%d"),uNumReads,uNumRooms,uAvgCount);
			_printf(_L("\n"));

			_printf(_L("\n\n\tCancelling Read()"));

			port.ReadCancel();

			_printf(_L("\n\n\tRead() Cancelled"));

			break;
			}
		}

	_printf(_L("\n"));
	_printf(_L("\tBulk OUT says bye..."));
	_printf(_L("\n"));

	CleanupStack::PopAndDestroy(); // port	
	}

/////////////////////////////////////////////////////////////////////////////////

void Bulk_IN_TestL()
	{
	TRequestStatus consoleStatus;
	TRequestStatus status;
	RComm port;

#define uInBufSize 65536

	static TBuf8<uInBufSize> IN_Buf;
	static TBuf8<uInBufSize> ZLP_Buf;

	_printf(_L("\n"));
	_printf(_L("+-------------------------------------+\n"));
	_printf(_L("|+-----------------------------------+|\n"));
	_printf(_L("|| This test writes data on the ACM  ||\n"));
	_printf(_L("|| (Bulk IN) data port               ||\n"));
	_printf(_L("||                                   ||\n"));
	_printf(_L("|| On running, Press any key to quit ||\n"));
	_printf(_L("|+-----------------------------------+|\n"));
	_printf(_L("+--------------------------------[rjf]+\n"));
	_printf(_L("\n"));

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.SetConfig(TheConfigBuf);
	port.SetReceiveBufferLength(KReceiveBufferLength);

	// set up the ZLP and set a default pattern into the buffer

	ZLP_Buf.SetLength(0) ;

	IN_Buf.SetLength( uInBufSize ) ;

	int j = 0 ;

	for( int i = 0 ; i < uInBufSize ; i++ )
		{
		if( ( i % 64 ) == 0 )
			{
			j++;
			}

		switch( i % 64 )
			{
		case 0 :
			IN_Buf[i] = 0xFF ;
			break ;
		case 1 :
			IN_Buf[i] = j / 256 ;
			break ;
		case 2 :
			IN_Buf[i] = j % 256 ;
			break ;
		case 3 :
			IN_Buf[i] = 0xFF ;
			break ;
		default:
			IN_Buf[i] = i ;
			break ;
			}
		}

	// prime the console for the abandon-ship

	console->Read(consoleStatus);

	TInt bDoneBlock = EFalse ;

	TInt bNeedZLP = bUseZLP && ((IN_Buf.Length()%64)==0);

	TInt uWriteCount = 0 ;

	_printf(_L("\tWatch :  "));

	while (1)
		{
		// setup write to send a single block, this is either the
		// block in the buffer, or else may be a ZLP which is sent
		// on every other pass, provided the size of the block is
		// exactly N*64

		if( bDoneBlock && bNeedZLP )
		{
			port.Write(status, ZLP_Buf);

			bDoneBlock = EFalse;
		}
		else
		{
			port.Write(status, IN_Buf);

			uWriteCount += IN_Buf.Length() ;

			bDoneBlock = ETrue;
		}

		// either the write is complete, or user request to stop the test

		User::WaitForRequest(status,consoleStatus);

		if (consoleStatus == KRequestPending)
			{
			if (status != KErrNone)
				{
				console->ReadCancel();

				LEAVE(status.Int());
				}

			// reassure watcher that there is something happening...

			}
		else
			{
			_printf(_L("\n\n\tCancelling Write()"));

			port.WriteCancel();

			_printf(_L("\n\n\tWrite() Cancelled"));

			break;
			}
		}

	_printf(_L("\n"));
	_printf(_L("\tBulk IN says bye..."));
	_printf(_L("\n"));

	CleanupStack::PopAndDestroy(); // port	
	}

////////////////////////////////////////////////////////////////////////////////

void SetHandshakingL()
	{
	RComm port;

	TCommCaps capsBuf;
	TCommCapsV01& caps = capsBuf();

	LEAVEIFERROR(port.Open(TheCommServ, KUsbPortName, ECommExclusive, ECommRoleDCE));
	CleanupClosePushL(port);

	port.Caps(capsBuf);

	_printf(_L("\nPort handshaking capabilities: 0x%08X\n"), caps.iHandshake);
	_printf(_L("\nCurrent handshaking options:   0x%08X\n"), TheConfig.iHandshake);

	_printf(_L("\n"));

#define HS(b) _printf(_L("\tTheConfig.%s = %s\n"), #b, ((TheConfig.iHandshake & b) == b) ? "Set" : "Clear" ) ;

	HS(KConfigObeyXoff);
	HS(KConfigSendXoff);
	HS(KConfigObeyCTS);
	HS(KConfigObeyDSR);
	HS(KConfigFreeRTS);
	HS(KConfigWriteBufferedComplete);

	_printf(_L("\n\n"));
	_printf(_L("Handshaking options:\n\n"));
	_printf(_L("1. No handshaking\n"));
	_printf(_L("2. Toggle Xon/Xoff\n"));
	_printf(_L("3. Toggle obey CTS\n"));
	_printf(_L("4. Toggle obey DSR / free RTS\n"));
	_printf(_L("5. Toggle write buffered complete\n"));

	TKeyCode key = console->Getch();

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

////////////////////////////////////////////////////////////////////////////////

void RestartUsbL()
	{
	TheUsb.Stop();

	TRequestStatus status;
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	_printf(_L("Restarted USB.\n"));
	}



void ToggleTermL()
	{
	bUseTerm = ( bUseTerm == EFalse ) ;

	if( bUseTerm )
		{
		TheConfig.iTerminatorCount = 1 ;

		TheConfig.iTerminator[0] = 0x7E ;
		}
	else
		{
		TheConfig.iTerminatorCount = 0 ;
		}
	}

////////////////////////////////////////////////////////////////////////////////

void mainL()
	{
	_printf(_L("\n"));
	_printf(_L("+--------------------------------+\n"));
	_printf(_L("|+------------------------------+|\n"));
	_printf(_L("|| T_CATC : V1.00 : 25-Feb-2003 ||\n"));
	_printf(_L("|+------------------------------+|\n"));
	_printf(_L("+---------------------------[rjf]+\n"));
	_printf(_L("\n"));

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

	_printf(_L("\n"));
	_printf(_L("+-------------------------------------+\n"));
	_printf(_L("|+-----------------------------------+|\n"));
	_printf(_L("|| Please connect the USB cable now! ||\n"));
	_printf(_L("|+-----------------------------------+|\n"));
	_printf(_L("+--------------------------------[rjf]+\n"));
	_printf(_L("\n"));

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

	TBool exit = EFalse;

#define SHOW(h,l,t,f)						\
	{										\
		_printf(_L("\n  "));				\
		_printf(_L(h));						\
		_printf(_L("\t"));					\
											\
		if( l )								\
		{									\
			_printf(_L(t));					\
		}									\
		else								\
		{									\
			_printf(_L(f));					\
		}									\
											\
		_printf(_L("\n"));					\
	}

	while (!exit)
		{
		_printf(_L("\n"));
		_printf(_L("Current Settings:\n"));
		_printf(_L("\n"));
		SHOW("OUT Method", bReadCall,"Read()","ReadOneOrMore()");
		SHOW("ZLP Method", bUseZLP,  "Active","Suppressed"     );
		SHOW("Term Method",bUseTerm, "Active","Suppressed"     );
		_printf(_L("\n"));
		_printf(_L("Available tests:    \n"));
		_printf(_L("\n"));
		_printf(_L("1. Bulk OUT test    \n"));
		_printf(_L("2. Bulk IN  test    \n"));
		_printf(_L("3. Set handshaking  \n"));
		_printf(_L("4. Restart USB      \n"));
		_printf(_L("6. Swap Read Method \n"));
		_printf(_L("7. Swap ZLP Method  \n"));
		_printf(_L("8. Swap Term Method \n"));
		_printf(_L("\n"));
		_printf(_L("Select (q to quit): "));

		char ch = (char) _getch();
		_printf(_L("\n"));

		switch (ch)
			{
		case '1': Bulk_OUT_TestL();							break;
		case '2': Bulk_IN_TestL();							break;
		case '3': SetHandshakingL();						break;
		case '4': RestartUsbL();							break;
		case '6': bReadCall = ( bReadCall == EFalse ) ;		break;
		case '7': bUseZLP   = ( bUseZLP == EFalse ) ;		break;
		case '8': ToggleTermL() ;							break;

		case 'q':
		case 'Q':
			exit = ETrue;
			break;

		default:
			_printf(_L("\nInvalid key\n"));
			break;
			}
		}

	TheCommServ.Close();
	TheUsb.Close();
	}

////////////////////////////////////////////////////////////////////////////////

void consoleMainL()
	{
	console=Console::NewL(_L("T_ACM"),TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	mainL();
	_printf(_L("[ press any key ]"));
	_getch();
	CleanupStack::PopAndDestroy();
	}

////////////////////////////////////////////////////////////////////////////////

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	TRAP_IGNORE(consoleMainL());
	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}

////////////////////////////////////////////////////////////////////////////////

