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

#ifndef SIMPLEOBEXAPP_H
#define SIMPLEOBEXAPP_H


#include <e32base.h>
#include <e32cons.h>
#include <obex.h>
#include <btsdp.h>
#include <d32usbc.h>



#ifdef __WINS__
_LIT(KWinsPddName,"ecdrv");
_LIT(KWinsLddName,"ecomm");
#endif //__WINS__


_LIT(KEusbc,"EUSBC");


enum Mode
	{
	E_Inactive,
	E_Server,
	E_Server_File,
	E_Client,
	E_Client_Connect_Menu,
	E_Client_Setup_Menu,
	E_SdpQuery

	};

enum TTransport
	{
	EBluetooth,
	EIrda,
	EUsb,
	EWin32Usb
	};



class CObexClientHandler;
class CObexServerHandler;




/**
 * CActiveConsole is a CActive derived class, it is used to provide
 * a means for the user to interact with the OBEX application. It provides
 * a test shell menu for the user to select options from, it is also responsible for 
 * initialising user selected processes.
 */

class CActiveConsole : public CActive
	{
	public:
		static CActiveConsole* NewLC(CConsoleBase* aConsole);
		~CActiveConsole();

		void DoCancel();
		void RunL();
		void RequestCharacter();
		void ProcessKeyPressL(TChar aChar);
		void DoUsbInitialisationL();
		
		CConsoleBase* Console();
	private:
		void ConstructL();
		CActiveConsole(CConsoleBase* aConsole);

	public:
		// Data members defined by this class
		CConsoleBase*    iConsole;					// A console for reading from
		CObexClientHandler*  iObexClientHandler;	// Client wrapper for CObexClient
		CObexServerHandler*  iObexServerHandler;	// Client wrapper for CObexServer
		TUint iMode;
		TTransport iTransport;
		RDevUsbcClient iUsbDriver;

	};
	

	

#endif // SIMPLEOBEXAPP_H