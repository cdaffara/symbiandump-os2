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
* This test is used to pipe data to and from the USB ACM comm port to
* a real comm port connected to a modem.
*
*/

#include "t_usbmodem.h"
#include <e32debug.h>

//
//--- class CSimplexRead ----------------------------------------------------
//
CSimplexRead* CSimplexRead::NewL(CDuplex& aDuplex)
	{
	CSimplexRead* self = new (ELeave) CSimplexRead(aDuplex);
	self->ConstructL();
	CActiveScheduler::Add(self);
	return (self);	
	}

void CSimplexRead::ConstructL()
	{
	// Not much to do yet
	}

void CSimplexRead::StartL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CSimplexRead::StartL\n"));
#endif
	if(IsActive())
		{
		RDebug::Print(_L(": CSimplexRead::StartL Warning - Already active\n"));
		return;
		}
	iCommPort->ReadOneOrMore(iStatus, iBuffer);
	SetActive();
	}

void CSimplexRead::RunL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CSimplexRead::RunL - iStatus:%d iBuffer.Length:%d TotalAmount Left:%d\n"),iStatus.Int(),iBuffer.Length(),iTotalAmount);
#endif
	iDuplex.NotifyRead(iStatus.Int(),iBuffer.Length(), iBuffer);
	}

void CSimplexRead::DoCancel(void)
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CSimplexRead::DoCancel\n"));
#endif
	if(iCommPort)
		iCommPort->ReadCancel();
#ifdef _DEBUG
	RDebug::Print(_L(": CSimplexRead::DoCancel done\n"));
#endif
	}

//
//--- class CSimplexWrite ----------------------------------------------------
//
CSimplexWrite* CSimplexWrite::NewL(CDuplex& aDuplex)
	{
	CSimplexWrite* self = new (ELeave) CSimplexWrite(aDuplex);
	self->ConstructL();
	CActiveScheduler::Add(self);
	return (self);	
	}

void CSimplexWrite::ConstructL()
	{
	// Not much to do yet
	}

void CSimplexWrite::StartL(TDes8& aBuf)
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CSimplexWrite::StartL - About to write l=%d\n"),aBuf.Length());
#endif
	if(IsActive())
		{
		RDebug::Print(_L(": CSimplexWrite::StartL Warning - Already active\n"));
		return;
		}

	iCommPort->Write(iStatus, aBuf);
	SetActive();
	}

void CSimplexWrite::RunL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CSimplexWrite::RunL - iStatus:%d"),iStatus.Int());
#endif
	//Notify Duplex object
	iDuplex.NotifyWrite(iStatus.Int());
	}

void CSimplexWrite::DoCancel(void)
	{
	if(iCommPort)
		iCommPort->WriteCancel();
#ifdef _DEBUG
	RDebug::Print(_L(": CSimplexWrite:: Comm port write Canceled\n"));
#endif
	}

//
// --- class CDuplex ---------------------------------------------------------
//
CDuplex* CDuplex::NewL()
	{
	CDuplex* self = new (ELeave) CDuplex();
	self->ConstructL();
	return (self);	
	}

void CDuplex::ConstructL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CDuplex::ConstructL\n"));
#endif
	//Create reader and writer
	iSimplexRead = CSimplexRead::NewL(*this);
	iSimplexWrite = CSimplexWrite::NewL(*this);
	}

void CDuplex::StartL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CDuplex::StartL\n"));
#endif
	iSimplexRead->StartL();
	}

void CDuplex::Cancel()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CDuplex::Cancel\n"));
#endif
	if(iSimplexRead)
		iSimplexRead->Cancel(); 
	if(iSimplexWrite)
		iSimplexWrite->Cancel();
	}

#ifdef _DEBUG
	void CDuplex::NotifyRead(TInt aErr,TInt aSize, TDes8& aBuf)
#endif

#ifndef _DEBUG
	void CDuplex::NotifyRead(TInt aErr,TInt /*aSize*/, TDes8& aBuf)
#endif
	{
	TInt ignoreErr;

	// Check if error
	switch(aErr)
		{
		case KErrNone:
			// Start the write
#ifdef _DEBUG
			RDebug::Print(_L(": CDuplex::NotifyRead err:%d recvd:%d\n"), aErr, aSize);
#endif
			TRAP(ignoreErr, iSimplexWrite->StartL(aBuf));
			break;
		default:
#ifdef _DEBUG
			RDebug::Print(_L(": CDuplex::NotifyRead error:%d\n"), aErr);
#endif
			Cancel();
			break;
		}
#ifdef _DEBUG
	RDebug::Print(_L(": CDuplex::NotifyRead Done \n"), aErr, aSize);
#endif
	return;
	}

void CDuplex::NotifyWrite(TInt aErr)
	{
	TInt ignoreErr;

	// Check if error
	switch(aErr)
		{
		case KErrNone:
			//Not much right now, just trigger another read
#ifdef _DEBUG
			RDebug::Print(_L(": CDuplex::NotifyWrite err:%d \n"), aErr);
#endif
			TRAP(ignoreErr, iSimplexRead->StartL());
			break;
		default:
#ifdef _DEBUG
			RDebug::Print(_L(": CDuplex::NotifyWrite error:%d\n"), aErr);
#endif
			Cancel();
			break;
		}
#ifdef _DEBUG
	RDebug::Print(_L(": CDuplex::NotifyWrite Done \n"), aErr);
#endif

	return;
	}

CDuplex::~CDuplex()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": Deleting CDuplex\n"));
#endif
	Cancel();
	delete iSimplexRead;
	delete iSimplexWrite;
	}

//
//----- class CConfigChangeNotifier -----------------------------------------
//
CConfigChangeNotifier::CConfigChangeNotifier()
	:CActive(CActive::EPriorityStandard)
	{
	}

CConfigChangeNotifier* CConfigChangeNotifier::NewL()
	{
	CConfigChangeNotifier* self = new (ELeave) CConfigChangeNotifier();
	CActiveScheduler::Add(self);				// Add to active scheduler
	return self;
	}
	
void CConfigChangeNotifier::DoCancel()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CConfigChangeNotifier::DoCancel"));
#endif
	TheUsbPort.NotifyConfigChangeCancel();
	}

void CConfigChangeNotifier::StartL()
	{
	if (IsActive())
		{
		RDebug::Print(_L(": Config chabge Notifier Already active"));
		return;
		}
	TheUsbPort.NotifyConfigChange(iStatus, TheUsbConfigBuf);
	SetActive();
	}

void CConfigChangeNotifier::RunL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": Config Change Notifier activated"));
#endif
	// Set the serial port config
	TCommConfig aConfig;
	TheUsbPort.Config(aConfig);
	TheSerialPort.SetConfig(aConfig);
	StartL();
	}

//
//----- class CSignalChageNotifier -----------------------------------------
//
CSignalChangeNotifier::CSignalChangeNotifier(TBool aIsUsbPort)
	:CActive(CActive::EPriorityStandard),
	iIsUsbPort(aIsUsbPort)
	{
	}

CSignalChangeNotifier* CSignalChangeNotifier::NewL(TBool aIsUsbPort)
	{
	CSignalChangeNotifier* self = new (ELeave) CSignalChangeNotifier(aIsUsbPort);
	CActiveScheduler::Add(self);				// Add to active scheduler
	return self;
	}
	
void CSignalChangeNotifier::DoCancel()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CSignalChangeNotifier::DoCancel"));
#endif
	if (iIsUsbPort)
		TheUsbPort.NotifySignalChangeCancel();
	else 
		TheSerialPort.NotifySignalChangeCancel();
	}

void CSignalChangeNotifier::StartL()
	{
	if (IsActive())
		{
		RDebug::Print(_L(": Signal Notifier Already active"));
		return;
		}
	else if (iIsUsbPort)
		{
		RDebug::Print(_L(": Starting Signal Notifier for USB"));
		TheUsbPort.NotifySignalChange(iStatus, iInSignals, KSignalDCEInputs | KSignalDCEOutputs | KSignalBreak);
		}
	else 
		{
		RDebug::Print(_L(": Starting Signal Notifier for serial port"));
		TheSerialPort.NotifySignalChange(iStatus, iInSignals, KSignalDTEInputs);
		}

	SetActive();
	}

void CSignalChangeNotifier::RunL()
	{
	// if this object is for USB,
	if (iIsUsbPort)
		{
#ifdef _DEBUG
		RDebug::Print(_L(": Signal Change Notifier activated for USB - %x"), iInSignals);
#endif
		if ( iInSignals & KRTSChanged )
			{
			if ( iInSignals & KSignalRTS )
				{
#ifdef _DEBUG
				RDebug::Print(_L(": Serial RTS on"));
#endif
				TheSerialPort.SetSignals( KSignalRTS, 0 );
				}
			else
				{
#ifdef _DEBUG
				RDebug::Print(_L(": Serial RTS off"));
#endif
				TheSerialPort.SetSignals( 0, KSignalRTS );
				}
			}
	
		if ( iInSignals & KDTRChanged )
			{
			if ( iInSignals & KSignalDTR )
				{
#ifdef _DEBUG
				RDebug::Print(_L(": Serial DTR on"));
#endif
				TheSerialPort.SetSignals( KSignalDTR, 0 );
				}
			else
				{
#ifdef _DEBUG
				RDebug::Print(_L(": Serial DTR off"));
#endif
				TheSerialPort.SetSignals( 0, KSignalDTR );
				}
			}

		}
	else 
		{
#ifdef _DEBUG
		RDebug::Print(_L(": Signal Change Notifier activated for Serial port - %x"), iInSignals);
#endif
		if ( iInSignals & KCTSChanged )
			{
			if ( iInSignals & KSignalCTS )
				{
#ifdef _DEBUG
				RDebug::Print(_L(": USB CTS on"));
#endif
				TheUsbPort.SetSignals( KSignalCTS, 0 );
				}
			else
				{
#ifdef _DEBUG
				RDebug::Print(_L(": USB CTS off"));
#endif
				TheUsbPort.SetSignals( 0, KSignalCTS );
				}
			}
		if ( iInSignals & KDSRChanged )
			{
			if ( iInSignals & KSignalDSR )
				{
#ifdef _DEBUG
				RDebug::Print(_L(": USB DSR on"));
#endif
				TheUsbPort.SetSignals( KSignalDSR, 0 );
				}
			else
				{
#ifdef _DEBUG
				RDebug::Print(_L(": USB DSR off"));
#endif
				TheUsbPort.SetSignals( 0, KSignalDSR );
				}
			}

		if ( iInSignals & KDCDChanged )
			{
				if ( iInSignals & KSignalDCD )
				{
#ifdef _DEBUG
				RDebug::Print(_L(": USB DCD on"));
#endif
				TheUsbPort.SetSignals( KSignalDCD, 0 );
				}
			else
				{
#ifdef _DEBUG
				RDebug::Print(_L(": USB DCD off"));
#endif
				TheUsbPort.SetSignals( 0, KSignalDCD );
				}
			}

		if ( iInSignals & KRNGChanged )
			{
			if ( iInSignals & KSignalRNG )
				{
#ifdef _DEBUG
				RDebug::Print(_L(": USB RNG on"));
#endif
				TheUsbPort.SetSignals( KSignalRNG, 0 );
				}
			else
				{
#ifdef _DEBUG
				RDebug::Print(_L(": USB RNG off"));
#endif
				TheUsbPort.SetSignals( 0, KSignalRNG );
				}
			}
		}
	StartL();
	}

//
//----- class CFControlChangeNotifier -----------------------------------------
//
CFControlChangeNotifier::CFControlChangeNotifier()
	:CActive(CActive::EPriorityStandard)
	{
	}

CFControlChangeNotifier* CFControlChangeNotifier::NewL()
	{
	CFControlChangeNotifier* self = new (ELeave) CFControlChangeNotifier();
	CActiveScheduler::Add(self);				// Add to active scheduler
	return self;
	}
	
void CFControlChangeNotifier::DoCancel()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CFControlChangeNotifier::DoCancel()"));
#endif
	TheUsbPort.NotifyFlowControlChangeCancel();
	}

void CFControlChangeNotifier::StartL()
	{
	if (IsActive())
		{
		RDebug::Print(_L(": FlowControl Notifier Already active"));
		return;
		}
	TheUsbPort.NotifyFlowControlChange(iStatus, iFlowControl);
	SetActive();
	}

void CFControlChangeNotifier::RunL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": FlowControl Notifier activated"));
#endif
	StartL();
	}

//
// --- class CActiveConsole -------------------------------------------------
//

CActiveConsole::CActiveConsole()
	: CActive(CActive::EPriorityStandard),
	iIsRunning(EFalse)
	{}


CActiveConsole* CActiveConsole::NewLC()
	{
	CActiveConsole* self = new (ELeave) CActiveConsole();
	self->ConstructL();
	return self;
	}


CActiveConsole* CActiveConsole::NewL()
	{
	CActiveConsole* self = NewLC();
	return self;
	}

CActiveConsole::~CActiveConsole()
	{
	delete iUsbToSerial;
	delete iSerialToUsb;
	}

void CActiveConsole::DoCancel()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CActiveConsole::DoCancel()"));
#endif
	iUsbConfigChangeNotifier->Cancel();
	iSerialSignalChangeNotifier->Cancel();
	iUsbSignalChangeNotifier->Cancel();
	
	iUsbToSerial->Cancel();
	iSerialToUsb->Cancel();
	
	iIsRunning = EFalse;
	}

void CActiveConsole::RunL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CActiveConsole::RunL()"));
#endif
	ProcessKeyPressL();
	WaitForKeyPress();
	}

void CActiveConsole::ConstructL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CActiveConsole::ConstructL()"));
#endif
	CActiveScheduler::Add(this);				// Add to active scheduler

	// Create 2 reader writer AOs/ threads
	// 1 for reading from USb and writing to serial
	iUsbToSerial = CDuplex::NewL();
	iUsbToSerial->SetRxPort(&TheUsbPort);
	iUsbToSerial->SetTxPort(&TheSerialPort);
	
	// 1 for reading from Serial and writing to Usb
	iSerialToUsb = CDuplex::NewL();
	iSerialToUsb->SetRxPort(&TheSerialPort);
	iSerialToUsb->SetTxPort(&TheUsbPort);

	iSerialSignalChangeNotifier = CSignalChangeNotifier::NewL(EFalse);
	iUsbSignalChangeNotifier = CSignalChangeNotifier::NewL(ETrue);
	iUsbConfigChangeNotifier = CConfigChangeNotifier::NewL();
	}

void CActiveConsole::Start()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CActiveConsole::Start()"));
#endif
	if (iIsRunning)
		{
		TheWindow.Write(_L(": Invalid Key pressed, already running\n"));
		return;
		}
	iIsRunning = ETrue;
	TInt ignoreErr;
	TRAP(ignoreErr, iUsbConfigChangeNotifier->StartL());
	TRAP(ignoreErr, iSerialSignalChangeNotifier->StartL());
	TRAP(ignoreErr, iUsbSignalChangeNotifier->StartL());
	
	TRAP(ignoreErr, iUsbToSerial->StartL());
	TRAP(ignoreErr, iSerialToUsb->StartL());
#ifdef _DEBUG
	RDebug::Print(_L(": CActiveConsole::Start() done"));
#endif
	}

void CActiveConsole::WaitForKeyPress()
	{
	if (IsActive())
		{
		RDebug::Print(_L(": ActiveConsole -- Already Running\n"));
		return;
		}
		
	TheWindow.Write(_L("Press Escape to STOP\n"));
	TheWindow.Write(_L("Press Enter to EXIT\n"));
	if (!iIsRunning)
		{
		TheWindow.Write(_L("Press Spacebar to START test\n"));
		}
	else
		{
		TheWindow.Write(_L("1 DTR on	A DTR off\n"));
		TheWindow.Write(_L("2 RTS on	B RTS off\n"));
		TheWindow.Write(_L("3 DSR on	C DSR off\n"));
		TheWindow.Write(_L("4 CTS on	D CTS off\n"));
		TheWindow.Write(_L("5 DCD on	E DCD off\n"));
		TheWindow.Write(_L("6 RNG on	F RNG off\n"));
		TheWindow.Write(_L("7 BRK on	G BRK off\n"));
		}
	TheWindow.Write(_L("\n"));
	TheWindow.Write(_L("\n"));
	TheWindow.Read(iKeypress, iStatus);
	SetActive();
	}

void CActiveConsole::ProcessKeyPressL()
	{
#ifdef _DEBUG
	RDebug::Print(_L(": CActiveConsole::ProcessKeyPressL()"));
#endif
	switch (iKeypress.Code())
		{
		case EKeyEscape:
			RDebug::Print(_L(": Stopping CactiveConsole"));
			DoCancel();
			break;
		case EKeySpace:
			RDebug::Print(_L(": Starting"));
			Start();
			break;
		case EKeyEnter:
			RDebug::Print(_L(": Exiting"));
			DoCancel();

			RDebug::Print(_L(": Closing ports and servers"));
	
			TheUsbPort.Close();
			TheSerialPort.Close();
			TheUsbServer.Close();
			TheCommServer.Close();

			CActiveScheduler::Stop();
			break;
		case '1':
			TheUsbPort.SetSignals ( KSignalDTR, 0 );
			break;
		case '2':
			TheUsbPort.SetSignals ( KSignalRTS, 0 );
			break;
		case '3':
			TheUsbPort.SetSignals ( KSignalDSR, 0 );
			break;
		case '4':
			TheUsbPort.SetSignals ( KSignalCTS, 0 );
			break;
		case '5':
			TheUsbPort.SetSignals ( KSignalDCD, 0 );
			break;
		case '6':
			TheUsbPort.SetSignals ( KSignalRNG, 0 );
			break;
		case '7':
			TheUsbPort.Break ( iBreakRequest, 1000 );
			break;
		case 'A':
			TheUsbPort.SetSignals ( 0, KSignalDTR );
			break;
		case 'B':
			TheUsbPort.SetSignals ( 0, KSignalRTS );
			break;
		case 'C':
			TheUsbPort.SetSignals ( 0, KSignalDSR );
			break;
		case 'D':
			TheUsbPort.SetSignals ( 0, KSignalCTS );
			break;
		case 'E':
			TheUsbPort.SetSignals ( 0, KSignalDCD );
			break;
		case 'F':
			TheUsbPort.SetSignals ( 0, KSignalRNG );
			break;
		default:
			TheWindow.Write(_L(": Invalid Key pressed\n"));
		}
	}

LOCAL_C void DoInitL()
	{
	// Do the necessary initialisation of the C32, 2 comm ports, USB stuff.
	// Load Device Drivers
	TInt r;

	RDebug::Print(_L("E32Main: Load USB LDD \n"));
	r=User::LoadLogicalDevice(USBLDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		RDebug::Print(_L("E32Main: Failed 0x%X\n\r"),r);
		User::Panic(_L("T_TERM"), ELoadLogicalDeviceErr);
		}

	RDebug::Print(_L("E32Main: Load Serial PDD \n "));
	r=User::LoadPhysicalDevice(SERIALPDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		RDebug::Print(_L("E32Main: Failed 0x%X\n\r"),r);
		User::Panic(_L("T_TERM"), ELoadLogicalDeviceErr);
		}

	RDebug::Print(_L("E32Main: Load SERIAL LDD \n"));
	r=User::LoadLogicalDevice(SERIALLDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		RDebug::Print(_L("E32Main: Failed 0x%X\n\r"),r);
		User::Panic(_L("T_TERM"), ELoadLogicalDeviceErr);
		}
	RDebug::Print(_L("E32Main: Loaded Device Drivers\n"));
	
	r = StartC32();
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		RDebug::Print(_L("E32Main: Failed to start C32. Error = %d"), r);
		User::Panic(_L("T_TERM"), EOpenErr);
		}
	RDebug::Print(_L("E32Main: Started c32"));

	r = TheUsbServer.Connect();
	if (r!=KErrNone)
		{
		RDebug::Print(_L("E32Main: Failed to connect to UsbMan Server. Error = %d"), r);
		User::Panic(_L("T_TERM"), EOpenErr);
		}
	RDebug::Print(_L("E32Main: Connected to UsbMan Server"));

	TRequestStatus startStatus;
	TheUsbServer.Start(startStatus);
	User::WaitForRequest(startStatus);
	if (startStatus != KErrNone && startStatus != KErrAlreadyExists)
		{
		RDebug::Print(_L("E32Main: Failed to Start USB services. Error = %d"), r);
		User::Panic(_L("T_TERM"), EOpenErr);
		}
	RDebug::Print(_L("E32Main: Started USB services"));

	// Comms Config
	r = TheCommServer.Connect();
	if (r!=KErrNone)
		{
		RDebug::Print(_L("E32Main: Failed to Connect to C32. Error = %d"), r);
		User::Panic(_L("T_TERM"), EOpenErr);
		}
	RDebug::Print(_L("E32Main: Connected to C32 Server"));

	r = TheCommServer.LoadCommModule(USBCSY_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		RDebug::Print(_L("E32Main: Failed to load USB CSY. Error = %d"), r);
		User::Panic(_L("T_TERM"), EOpenErr);
		}
	RDebug::Print(_L("E32Main: Loaded USB CSY"));

	r = TheCommServer.LoadCommModule(SERIALCSY_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		RDebug::Print(_L("E32Main: Failed to load serial CSY. Error = %d"), r);
		User::Panic(_L("T_TERM"), EOpenErr);
		}
	RDebug::Print(_L("E32Main: Loaded Serial CSY"));

	r = TheUsbPort.Open(TheCommServer, USBPORT_NAME,ECommExclusive,ECommRoleDCE); // Comm port
	if (r!=KErrNone)
		{
		RDebug::Print(_L("E32Main: Failed to Open USB Comm Port. Error = %d"), r);
		User::Panic(_L("T_TERM"), EOpenErr);
		}
	RDebug::Print(_L("E32Main: Opened USB Comm Port"));


	r = TheSerialPort.Open(TheCommServer, SERIALPORT_NAME,ECommExclusive,ECommRoleDTE); // Comm port
	if (r!=KErrNone)
		{
		RDebug::Print(_L("E32Main: Failed to Open Serial Comm Port. Error = %d"), r);
		User::Panic(_L("T_TERM"), EOpenErr);
		}
	RDebug::Print(_L("E32Main: Opened Serial Comm Port"));


	RDebug::Print(_L("E32Main: Reading Serial Comm Port config"));
	TheSerialPort.Config(TheSerialConfigBuf);	// get config
	RDebug::Print(_L("E32Main: Reading Usb Comm Port config"));
	TheUsbPort.Config(TheUsbConfigBuf);	// get config

	TBuf<80> buf;
	buf.FillZ();
	RDebug::Print(_L("E32Main: Old USB Port Settings"));
	RDebug::Print(buf);
	RDebug::Print(_L(""));
	buf.FillZ();
	RDebug::Print(_L("E32Main: Old Serial Port Settings"));
	RDebug::Print(buf);
	RDebug::Print(_L(""));

	TCommConfig serialConfig;
	TheSerialPort.Config(serialConfig);	// get config
	serialConfig().iHandshake = 0;
	serialConfig().iRate = EBps1152000;
	TheSerialPort.SetConfig(serialConfig);

	RDebug::Print(_L(""));	}

/// Main function
//
GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("E32Main: Starting!"));
	// Construct the active scheduler
	CActiveScheduler* myScheduler = new (ELeave) CActiveScheduler();

	// Install as the active scheduler
	CActiveScheduler::Install(myScheduler);

	// Create objects and console handler
	// Open the window asap
	TheWindow.Init(_L("TERM"),TSize(KConsFullScreen,KConsFullScreen));

	TheWindow.Control(_L("+Maximize +Newline"));
	RDebug::Print(_L("E32Main: Initialised Window!"));

	DoInitL();
	
	// Does the construction, 
	CActiveConsole* myActiveConsole = CActiveConsole::NewL();

    // Call request function to start the test
	myActiveConsole->WaitForKeyPress();	
	// Start active scheduler
	CActiveScheduler::Start();

	// Suspend thread for 2 secs
	User::After(2000000);

	delete myActiveConsole;
	return KErrNone;
	}

