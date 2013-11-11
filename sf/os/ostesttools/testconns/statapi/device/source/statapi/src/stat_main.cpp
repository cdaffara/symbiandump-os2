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



/********************************************************************************
 *
 * System Includes
 *
 ********************************************************************************/
#include <e32base.h>
#include <e32cons.h>
#include <hal.h>
#include <iniparser.h>


/********************************************************************************
 *
 * Local Includes
 *
 ********************************************************************************/
#include "stat.h"
#include "stat_controller.h"
#include "stat_console.h"
#include "assert.h"
#include "activeconsole.h"
#include "filedump.h"

#ifndef LIGHT_MODE
#include "activeconnection.h"
#endif // ifndef LIGHT_MODE

/********************************************************************************
 *
 * Macro functions
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Definitions
 *
 ********************************************************************************/
_LIT( KTxtEPOC32EX, "EPOC32EX" );
_LIT( KFormatFailed, "failed: leave code=%d" );

_LIT( KSerialBaudRate, "115200" );

_LIT( KIniFile, "stat.ini" );
_LIT( KOff, "off" );
_LIT( KOn, "on" );

// Labels used in the ini file.
_LIT( KIniLogging, "logging" );
_LIT( KIniTransport, "transport" );
_LIT( KIniComPort, "comport" );
_LIT( KIniDebugPort, "debugport" );
#ifndef LIGHT_MODE
_LIT( KIniTransIAP,  "iap" );
_LIT( KIniTransSnap, "snap" );
#endif // ifndef LIGHT_MODE

// Default parameters we use if there is no ini file.
static const TInt defaultLogging = 0;
static const TStatConnectType defaultTransport = ESerial;
#ifdef __WINS__
	_LIT(KDefaultPort,"1");
#else // def __WINS__
	_LIT(KDefaultPort,"2");
#endif // def __WINS__

// General value for unset or incorrect data.
static const TInt invalidValue = -1;

// Test string descriptions of the transport types.
// The choice specified in the ini file should match one
// of these.
static const TPtrC transportText[ENumConnectTypes] = { 
			_L("tcpip"),
			_L("serial"),
			_L("ir"),
			_L("bt"),
			_L("usb")
			};

LOCAL_D MNotifyLogMessage *iMsg = NULL;
LOCAL_D	RFs iFsSession;

/********************************************************************************
 *
 * Prototypes
 *
 ********************************************************************************/
LOCAL_D CConsoleBase* console;
void callExampleL();
void statmainL( void );

/********************************************************************************
 *
 * Implementation
 *
 ********************************************************************************/
GLDEF_C TInt E32Main()
{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New(); 
	TRAPD(error,callExampleL()); 
	__ASSERT_ALWAYS(!error,User::Panic(KTxtEPOC32EX,error));
	delete cleanup; 
	__UHEAP_MARKEND;
	return 0; 
}


void callExampleL()
{
	console=Console::NewL( KTxtExampleCode, TSize(KConsFullScreen,KConsFullScreen) );
	CleanupStack::PushL(console);
	TRAPD(error,statmainL());
	if (error)
		console->Printf( KFormatFailed, error);
	CleanupStack::PopAndDestroy();
}

TInt SetDebugPort(TInt aPort)
{
	TInt lError=KErrNone;

#ifndef SYMBIAN_DIST_SERIES60
	lError=HAL::Set(HALData::EDebugPort, aPort);
#endif

	return lError;
}


TInt getLogging(CIniData* aIniFile,TInt *aLogging)
{
	TPtrC result;
	TBool ret=TRUE;

	if(ret==TRUE)
	{
		//read logging
		ret=aIniFile->FindVar(KIniLogging,result);
	}

	if(ret==TRUE)
	{
		TLex lLex(result);
		TInt lLogging;
		lLex.Val(lLogging);
	
		*aLogging = (lLogging)?1:0;
	}

	return (ret==TRUE)?0:1;
}

TInt getTransport(CIniData* aIniFile,TInt *aTransport)
{
	TPtrC result;
	TBool ret=TRUE;

	if(ret==TRUE)
	{
		//read transport that will be used
		ret=aIniFile->FindVar(KIniTransport,result);
	}

	if(ret==TRUE)
	{
		*aTransport = invalidValue;
		TInt count;
		for( count = 0; ( *aTransport == invalidValue ) && ( count < ENumConnectTypes ); count++ )
		{
			if( 0 == ( result.Compare( transportText[count] ) ) )
			{
				*aTransport = count;
			}
		}

		ret = (*aTransport != invalidValue);
	}

	return (ret==TRUE)?0:1;
}

#ifndef LIGHT_MODE

TInt getIAP(CIniData* aIniFile, TDes *aIAP, CActiveConnection::TActiveConnectionMode *mode, TInt *index)
{
	TPtrC result;
	TBool ret=TRUE;
	
	//read IAP
	ret=aIniFile->FindVar(KIniTransIAP,result);
	if(ret==FALSE)
		return KErrNotFound;
	
	TLex lNum = TLex(result);
	TInt lInt; 
	TInt err = lNum.Val(lInt);
	if (err != KErrNone)
		return err;
	
	*mode = CActiveConnection::EModeIAP;
	*index = lInt;
	
	aIAP->Copy(KIniTransIAP);
	aIAP->Append(_L("|"));
	aIAP->Append(result);
	return KErrNone;
}

TInt getSnap(CIniData* aIniFile, TDes *aSnap, CActiveConnection::TActiveConnectionMode *mode, TInt *index)
{
	TPtrC result;
	TBool ret=TRUE;
	
	//read snap
	ret=aIniFile->FindVar(KIniTransSnap,result);
	if(ret==FALSE)
		return KErrNotFound;
	
	TLex lNum = TLex(result);
	TInt lInt; 
	TInt err = lNum.Val(lInt);
	if (err != KErrNone)
		return err;
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	*mode = CActiveConnection::EModeSnap;
	*index = lInt;
	
	aSnap->Copy(KIniTransSnap);
	aSnap->Append(_L("|"));
	aSnap->Append(result);
	return KErrNone;
#else
	_LIT(KError,"INFO: Access Point selection is not supported..\n");
	console->Printf(KError);

	// use default IAP connection
	aSnap->Zero();
	*mode = CActiveConnection::EModeIAP;
	*index = 0;

	return KErrNotSupported;
#endif // SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
}

#endif // ifndef LIGHT_MODE

TInt getPort(CIniData* aIniFile,TDes *AtbAddress)
{
	TPtrC result;
	TBool ret=TRUE;
	TInt lError=KErrNone;
	
	if(ret==TRUE)
	{
		//read comport setting
		ret=aIniFile->FindVar(KIniComPort,result);
	}

	if(ret==TRUE)
	{
		
		if(lError==KErrNone)
		{
			AtbAddress->Append( result );
		}
		else
		{
			ret = FALSE;
		}
	}

	return (ret==TRUE)?0:1;
}

TInt getDebugPort(CIniData* aIniFile,TInt *aDebugPort)
{
	TPtrC result;
	TBool ret=TRUE;
	TInt lError=KErrNone;
	
	if(ret==TRUE)
	{
		//read debug port setting

		// If we have found an entry for the debug
		// port then extract it.
		// If there is no debug port specified then there
		// is no error (and no 'else').
		if(aIniFile->FindVar(KIniDebugPort,result))
		{
			TLex lLexPort(result); //retrive integer value for comport 
			TInt lPort;
			lError=lLexPort.Val(lPort);

			if(lError==KErrNone)
			{
				*aDebugPort = lPort;
			}
			else
			{
				ret = FALSE;
			}
		}
	}
	
	return (ret==TRUE)?0:1;
}

#ifdef LIGHT_MODE
TInt getparams( TInt *logging, TInt *transport, TDes *tbAddress )
#else
TInt getparams( TInt *logging, TInt *transport, TDes *tbAddress, CActiveConnection::TActiveConnectionMode *mode, TInt *index )
#endif // ifndef LIGHT_MODE
{
	// validate params 
	asserte( logging != NULL );
	asserte( transport != NULL );
	asserte( tbAddress != NULL );
#ifndef LIGHT_MODE
	asserte( mode != NULL );
	asserte( index != NULL );
#endif // ifndef LIGHT_MODE
	TInt lError=KErrNone;
	TInt debugPort = invalidValue;

	// open cinidata object
	CIniData* lIniFile=NULL;

	TRAPD(r,lIniFile=CIniData::NewL(KIniFile));
	if(r!=KErrNone)	//ini file not found, use defaults
	{
		_LIT(KMsg,"stat.ini file not found, using defaults...\n");
		console->Printf(KMsg);
		
		*logging = defaultLogging;
		*transport = defaultTransport;
		(*tbAddress) = KDefaultPort;
	}
	else
	{
		if(lError==KErrNone)
		{
			lError=getLogging(lIniFile,logging);

			if(lError!=KErrNone)
			{
				_LIT(KError,"ERROR: Failed to read logging setting in stat.ini file.\n");
				console->Printf(KError);
			}
		}

		if(lError==KErrNone)
		{
			lError=getTransport(lIniFile,transport);

			if(lError!=KErrNone)
			{
				_LIT(KError,"ERROR: Failed to read transport setting in stat.ini file.\n");
				console->Printf(KError);
			}
		}

		if(lError==KErrNone)
		{
			lError=getPort(lIniFile,tbAddress);

			if(lError!=KErrNone)
			{
				_LIT(KError,"ERROR: Failed to read COM port address setting in stat.ini file.\n");
				console->Printf(KError);
			}
		}

		if(lError==KErrNone)
		{
			lError=getDebugPort(lIniFile,&debugPort);

			if(lError!=KErrNone)
			{
				_LIT(KError,"ERROR: Failed to read debug port address setting in stat.ini file.\n");
				console->Printf(KError);
			}
		}
	}

	// Handle any special cases with data.
	if(lError==KErrNone)
	{
		if((*transport==ESerial)||(*transport==EInfrared))
		{
			// Append the baud rate to the COM port text string.
			tbAddress->Append( _L("|") );
			tbAddress->Append( KSerialBaudRate );
		}
#ifndef LIGHT_MODE
		else if (*transport == ETCPIP)
		{
			// Load TCPIP specific parameters
			lError = getIAP(lIniFile, tbAddress, mode, index);
			if (lError!=KErrNone)
				getSnap(lIniFile, tbAddress, mode, index);
			
			// reset error
			lError = KErrNone;
		}
#endif // ifndef LIGHT_MODE
	}

	delete lIniFile;
	lIniFile = NULL;

	// Set the debug port to the value from the ini file
	// of the default value.
	if(lError==KErrNone)
	{
		// Only set the debug port if the user
		// specified a value.
		if(debugPort!=invalidValue)
		{
			lError = SetDebugPort(debugPort);

			if(lError!=KErrNone)
			{
				_LIT(KError,"ERROR: Failed to set the debug port.\n");
				console->Printf(KError);
			}
		}
	}

	// Display data to the user.
	if(lError==KErrNone)
	{
		// Logging.
		console->Printf(_L("logging is %S.\n\r"),
					(0==(*logging))?&KOff:&KOn);

		// Transport.
		console->Printf(_L("transport = %S.\n\r"), &(transportText[*transport]));

		// Port or address.
		console->Printf(_L("port = %S.\n\r"), tbAddress);

		// Debug port.
		if(debugPort!=invalidValue)
		{
			console->Printf(_L("debugport = %d.\n\r"), debugPort);
		}
		else
		{
			console->Printf(_L("debugport is unchanged.\n\r"));
		}
	}

	return lError;
}

void NotifyStarted()
{
	RNotifier notifier;
	if(notifier.Connect() != KErrNone) {
		return;
	}
    notifier.InfoPrint(_L("Starting STAT..."));
    notifier.Close();
}




void statmainL( void )
{
	CActiveScheduler *iActiveScheduler = NULL;
	CStatController *iController = NULL;
	TInt sessionid = 0;
	TInt err = 0;
	TInt logging = 0;
	TInt transport = 0;
	CStatConsole *iConsole = NULL;

#ifndef LIGHT_MODE
	CActiveConnection::TActiveConnectionMode mode = CActiveConnection::EModeIAP;
	TInt index = 0;
#endif // ifndef LIGHT_MODE

	TBuf<KAddressTextLimit> tbAddress;

	User::LeaveIfError( iFsSession.Connect() );
	iMsg = new FileDump();
	
	NotifyStarted(); //print a notification to the screen
	
#ifdef LIGHT_MODE	//lite version can't be hide to background
	console->Printf( _L("** STAT Lite Version %u.%u.%u **\n\n"),KVersionMajor,KVersionMinor,KVersionPatch);	
	console->Printf( _L("Press ESC to exit\n") );
#else
	console->Printf( _L("** STAT Version %u.%u.%u **\n\n"),KVersionMajor,KVersionMinor,KVersionPatch);
	console->Printf( _L("Press ESC to exit, or 'b' to switch to the background\n") );
#endif

	// get the params for STAT
#ifndef LIGHT_MODE
	err = getparams( &logging, &transport, &tbAddress, &mode, &index );
#else
	err = getparams( &logging, &transport, &tbAddress );
#endif // ifndef LIGHT_MODE
	if( err != KErrNone ) {
		console->Printf( _L("ERROR: Couldn't retrieve parameters.\n") );
		console->Printf( _L("Please check stat.ini file.\n") );
		console->Printf( _L("Press any key to exit.\n\r") );
		console->Getch();
		return;
	}
	
#ifdef LIGHT_MODE	//lite version can't be hide to background
	if (transport != ESerial){
		console->Printf( _L("\nERROR: STAT lite only support serial connection.\n") );
		console->Printf( _L("Press any key to exit.\n\r") );
		console->Getch();
		return;
	}
#endif
	TBuf<KMaxFileName> statLogFile;
	
	TDriveNumber defaultSysDrive(EDriveC);
	RLibrary pluginLibrary;
	TInt pluginErr = pluginLibrary.Load(KFileSrvDll);
	
	if (pluginErr == KErrNone)
		{
		typedef TDriveNumber(*fun1)();
		fun1 sysdrive;
	
	#ifdef __EABI__
		sysdrive = (fun1)pluginLibrary.Lookup(336);
	#else
		sysdrive = (fun1)pluginLibrary.Lookup(304);
	#endif
		
		if(sysdrive!=NULL)
			{
			defaultSysDrive = sysdrive();			
			}
		}
	pluginLibrary.Close();
	
	statLogFile.Append(TDriveUnit(defaultSysDrive).Name());
	statLogFile.Append(KFileSeparator);
	statLogFile.Append(KStatLogFile);

	if( logging )
		{
		iMsg->Init( iFsSession, statLogFile, console );
		}

	// create an active scheduler
	iActiveScheduler= new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iActiveScheduler );

#ifndef LIGHT_MODE
	// active object used to start ppp connection, only started if using tcpip 
	CActiveConnection* lAOConnection=CActiveConnection::NewL(_L("81.89.143.203"),80);
	// if we are using tcpip
	if(transport == ETCPIP)
	{
		lAOConnection->Start(mode, index); 
	}
#else
	if( transport != ESerial )
	{
		console->Printf( _L("ERROR: statlite supports only serial connection.\n") );
		console->Printf( _L("Please check stat.ini file.\n") );
		console->Printf( _L("Press any key to exit.\n\r") );
		console->Getch();
		return;
	}
#endif // ifndef LIGHT_MODE

	// create the console object to receive notifications from the controller
	iConsole = CStatConsole::NewL(console, iMsg);
	
	// create and run the controller
	iController = CStatController::NewL(); 
	sessionid = iController->StartSession( (enum TStatConnectType)transport, &tbAddress, iConsole, &iFsSession, iMsg );

	if(-1 == sessionid)
		{
		console->Printf( _L("Initialisation failed.  Press ESC to exit.\n") );
		}

	// create active object to monitor console for escape(exit) and b(push to background) 
	CActiveConsole* lAOConsole=CActiveConsole::NewL(console,iController,iConsole,sessionid,iMsg);
	lAOConsole->Start(); 

	// now wait for events -- there MUST be a read outstanding here
	// Issue the first request
	iActiveScheduler->Start();

	delete lAOConsole;

#ifndef LIGHT_MODE
	//cancel and destroy activeconsole
	if(transport == ETCPIP)
	{
		lAOConnection->CloseSocket();
	}
	delete lAOConnection;
#endif // ifndef LIGHT_MODE

	// cleanup the controller
	delete iController;

	// cleanup the console thing	
	delete iConsole;

	// cleanup the active scheduler
	delete iActiveScheduler;

	if( iMsg->IsInitialised() )
		{
		iMsg->CloseFile();
		}

	iFsSession.Close();

	delete iMsg;
	iMsg =	NULL;
}

/********************************************************************************
 *
 * Notification object
 *
 ********************************************************************************/


