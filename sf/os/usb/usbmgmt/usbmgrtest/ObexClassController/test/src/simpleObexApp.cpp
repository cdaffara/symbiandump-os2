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


#include <e32base.h>
#include <c32comm.h>

#include <usbman.h>

#include "simpleObexApp.h"
#include "simpleObexClient.h"
#include "simpleObexServer.h"


// Define Physical Device Driver and Logical Device Driver
// for device.



/**
 * Constructor.
 */
CActiveConsole::CActiveConsole(CConsoleBase* aConsole) 
	: CActive(CActive::EPriorityStandard),
	  iConsole(aConsole),
	iMode(E_Inactive)

	{
	}

/**
 * NewLC function, calls 2nd phase constructor.
 */
CActiveConsole* CActiveConsole::NewLC(CConsoleBase* aConsole)
    {
    CActiveConsole* self = new (ELeave) CActiveConsole(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
    return self;
    }


/**
 * 2nd phase construction.
 */
void CActiveConsole::ConstructL ()
    { 
      
    CActiveScheduler::Add(this);// Add to active scheduler

    }



/**
 * Destructor.
 */
CActiveConsole::~CActiveConsole()
	{
	// Make sure we're cancelled
	Cancel();

	// safe to delete NULL
	
    if(iObexClientHandler)
		delete iObexClientHandler;
    if(iObexServerHandler)
		delete iObexServerHandler;
 
	}

/**
 * RequestCharacter function, this is responsible for diplasying a menu to the user.
 */
void CActiveConsole::RequestCharacter()
    {
    
		if (iMode == E_Inactive) 
		{	
			// Initial menu
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|~~~~~~~~~~~~~~~~~OBEX~~~~~~~~~~~~~~~~~|\n"));
			iConsole->Printf(_L("---------------------------------------\n"));
			iConsole->Printf(_L("|-----------Bluetooth options----------|\n"));
			iConsole->Printf(_L("---------------------------------------\n"));
			iConsole->Printf(_L("| 1 - Start Obex Server over Bluetooth |\n"));
			iConsole->Printf(_L("| 2 - Start Obex Client over Bluetooth |\n"));
			iConsole->Printf(_L("---------------------------------------\n"));
			iConsole->Printf(_L("|-------------IrDA options-------------|\n"));
			iConsole->Printf(_L("---------------------------------------\n"));
			iConsole->Printf(_L("|   3 - Start Obex Server over IrDA    |\n"));
			iConsole->Printf(_L("|   4 - Start Obex Client over IrDA    |\n"));
			iConsole->Printf(_L("---------------------------------------\n"));
			iConsole->Printf(_L("|--------------USB options-------------| \n"));
			iConsole->Printf(_L("---------------------------------------\n"));
			iConsole->Printf(_L("|    5 - Start Obex Server over USB    |\n"));
			iConsole->Printf(_L("|    6 - Start Obex Client over USB    |\n"));
			iConsole->Printf(_L("---------------------------------------\n"));
		
		}
		
		if (iMode == E_Server)
			{
			// Menu displayed when Start server is called
			iConsole->Printf(_L("\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|~~~~~~~~~OBEX_SERVER_OPTIONS~~~~~~~~~~|\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|   S - Stop and kill the Obex Server  |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|      1 - Enable authentication       |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|      2 - Disable authentication      |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			}
		
		
		
		if (iMode == E_Client)
			{
			// Menu displayed when start client is started
			iConsole->Printf(_L("\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|~~~~~~~~~OBEX_CONNECT_OPTIONS~~~~~~~~~|\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|        1  -  OBEX connect menu       |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|         2  -  Disconnect Obex        |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|    3  -  Obex Put obj 1 : %S |\n"), &(iObexClientHandler->iFilename1));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|    4  -  Obex Put obj 2 : %S |\n"), &(iObexClientHandler->iFilename2));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|    5  -  Obex Put obj 3 : %S |\n"), &(iObexClientHandler->iFilename3));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|       6  -  Obex Get by name         |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			}
		
		
		
		if (iMode == E_Client_Connect_Menu )
			{
			// Obex connect menu
			iConsole->Printf(_L("\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|~~~~~~~~~~~OBEX_CONNECT_MENU~~~~~~~~~~|\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|       0  -  IrObex Disconnect        |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|         1  -  IrObex Connect         |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|   2  -  Connect with Authentication  |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			iConsole->Printf(_L("|     s  -  Back to Client Menu        |\n"));
			iConsole->Printf(_L("----------------------------------------\n"));
			}
		
		
		iConsole->Printf(_L("press Escape to quit\r\n\r\n"));
    	iConsole->Read(iStatus); 
    	SetActive();
    	
    }


/**
 * The active object DoCancel function
 */
void  CActiveConsole::DoCancel()
    {
    iConsole->ReadCancel();
    }
    
 

/**
 * The active object RunL function. It is responsible for routing the users
 * selection by calling the ProcessKeyPressL function.
 */
void  CActiveConsole::RunL()
    {
    TUint oldMode = iMode;
	TRAPD(err,ProcessKeyPressL(TChar(iConsole->KeyCode())));
	if(err != KErrNone)
		{
		iConsole->Printf(_L("Failed. Error: %d\r\n"),err);
		// Put the test mode back again
		iMode = oldMode;
		RequestCharacter();
		}	
    }


/**
 * This function is called after the user has made a selection.
 */
void CActiveConsole::ProcessKeyPressL(TChar aChar)
    {
    
    if (aChar == EKeyEscape)
		{
		CActiveScheduler::Stop();
		return;
		}
		
	else if (iMode == E_Inactive)
		{
		switch(aChar)
			{
			case '1':
				//Obex Server over Bluetooth
				iMode = E_Server;
				iTransport = EBluetooth;
				iObexServerHandler = CObexServerHandler::NewL(this, EBluetooth);
				break;
			
			case '2':
				//Obex Client over Bluetooth
				iMode = E_SdpQuery;
				iTransport = EBluetooth;
				iObexClientHandler = CObexClientHandler::NewL(this, EBluetooth);
				break;
			
			case '3':
			
				//Obex server over IrDA
				iMode = E_Server;
				iTransport = EIrda;
				iObexServerHandler = CObexServerHandler::NewL(this, EIrda);
				break;
			
			case '4':
				//Obex client over IrDA
				iMode = E_Client;
				iTransport = EIrda;
				iObexClientHandler = CObexClientHandler::NewL(this, EIrda);
				break;
			
			case '5':
				{
				// Obex server over USB.
				iMode = E_Server;
				
				#ifdef __WINS__
					// If starting usb on PC emulator side
					iTransport = EWin32Usb;
					iObexServerHandler = CObexServerHandler::NewL(this, EWin32Usb);
				#else
					// This is where the Class controller code provided is implemented	
					RUsb usb;
					TRequestStatus status;
					TInt obexPersonality = 1;
					User::LeaveIfError(usb.Connect());
					// Start the OBEX personality that will
					// start up an OBEX server.
					usb.TryStop(status);
					User::WaitForRequest(status);
					usb.TryStart(obexPersonality, status);
					User::WaitForRequest(status);
	
				#endif
				}
				break;
			case '6':
				{
				iMode = E_Client;
				
				#ifdef __WINS__
					// If starting client on PC emulator
					iTransport = EWin32Usb;
					iObexClientHandler = CObexClientHandler::NewL(this, EWin32Usb);
				#else
					iTransport = EUsb;
					DoUsbInitialisationL();
					iObexClientHandler = CObexClientHandler::NewL(this, EUsb);
					User::LeaveIfError(iUsbDriver.DeviceConnectToHost());// Plugin driver
					iUsbDriver.Close();
				#endif
				}
			default:
				break; 
			
			}
		
		}
	
	else if (iMode == E_Client_Connect_Menu )
		{
		switch (aChar)
			{
			case '0':
				// Disconnect Obex connection
				iObexClientHandler->Disconnect();
				break;
		
			case '1':
				// Start Obex connection
				iObexClientHandler->Connect();
				break;
		
			case '2':
			// Start Obex connection with authentication
			iObexClientHandler->ConnectWithAuthenticationL();		
			
			default:
				iMode = E_Client;
				break;
			}
		}
		
	else if (iMode == E_Server)
		{
		switch(aChar)
			{
			case 's':
				// Stop the Obex server
				iObexServerHandler->Stop();
				break;
			
			case '1':
				// Force authenication
				iObexServerHandler->EnableAuthenticationL();
				break;
			
			case '2':
				// Stop the need to authenicate
				iObexServerHandler->DisableAuthentication();
			
			default:
				break;
		
			}
		
		
		}
		
	else if (iMode == E_Client)
		{
		switch(aChar)
			{
		
			case '1':
				iMode = E_Client_Connect_Menu;				
				break;
			
			case '2':
				// Disconnect client and server link
				iObexClientHandler->Disconnect();
				break;
			
			case '3':
				// Put iFilename1 object
				iObexClientHandler->Put(iObexClientHandler->iFilename1);
				break;
			
			case '4':
				// Put iFilename2 object
				iObexClientHandler->Put(iObexClientHandler->iFilename2);
				break;
			
			case '5':
				// Put iFilename3 object
				iObexClientHandler->Put(iObexClientHandler->iFilename3);
				break;
			
			case '6':
				// Call get by name on active object
				iObexClientHandler->GetByNameL();
			default:
				break;
			
			}
		}
	
	else 
		{
		CActiveScheduler::Stop();
		return;
		}
	// Dislpay next menu
	RequestCharacter ();
	return;
    
    }


/**
 * Function used in creating a new active console.
 */
CConsoleBase* CActiveConsole::Console()
	{
	return iConsole;
	}


void CActiveConsole::DoUsbInitialisationL()
	{
	// Before creating an Obex client or server some initialisation
	// is needed. 
		
	// Load the USB device driver
	User::LeaveIfError(User::LoadLogicalDevice(KEusbc));
		
	// Now connect to USBmanager		
	RUsb usb;
	User::LeaveIfError(usb.Connect());
	iConsole->Printf(_L("Connected to USB\n"));
	// Before starting the WHCM USB personality foe OBEX
	// we need to stop the current personality.
	TRequestStatus status;
	usb.TryStop(status);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	// Now we want to start the WHCM personality
	const TUint KWhcmPersonality=2;
	usb.TryStart(KWhcmPersonality, status);
	User::WaitForRequest(status);
			
	iConsole->Printf(_L("Starting USB returned %d\n"), status.Int());
	User::LeaveIfError(status.Int());
	// We are now finished with Usb manager so close. 
	usb.Close();
		
	// We now need to open a channel to a client driver
	User::LeaveIfError(iUsbDriver.Open(0));
	// Unplug driver to allow for a new CObexClient to be created.
	User::LeaveIfError(iUsbDriver.DeviceDisconnectFromHost());

	}

/**
 * This is the function called by the entry point code.
 * It exists to perform initialisation
 *
 */
void RunAppL(void)
	{

	CActiveScheduler *myScheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(myScheduler);
	CActiveScheduler::Install(myScheduler); 

	CConsoleBase* console = 	
	Console::NewL(_L("Obex Program"),TSize(KConsFullScreen, KConsFullScreen));
	
	// Load Device drivers
	
	TInt err;
	// Start C32 now
	console->Printf(_L("Loading C32...\n"));
	err=StartC32();
	if (err!=KErrNone && err!=KErrAlreadyExists)
		console->Printf(_L("	Failed %d\n"), err);
	else
		console->Printf(_L("	Sucess\n"));
	// If running on PC emulator
	
	// Load drivers for using Serial communication
	#ifdef __WINS__
		TInt load =	User::LoadLogicalDevice(KWinsLddName);
		console->Printf(_L("Load LDD : %d\n"), load);
		load =	User::LoadPhysicalDevice(KWinsPddName);
		console->Printf(_L("Load PDD : %d\n"), load);
	#endif //__WINS__



	CleanupStack::PushL(console);
	CActiveConsole* my_console = CActiveConsole::NewLC(console);// New active console
	my_console->RequestCharacter();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(3); 
	
	}




/**
 * The E32main function is the main entry point for the
 * code.
 */

TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,RunAppL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(_L("Simple OBEX Application"),error));
	delete cleanup; // destroy clean-up stack
	__UHEAP_MARKEND;

	return 0; // and return
	}
