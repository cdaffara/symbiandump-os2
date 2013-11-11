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




#include "stdafx.h"
#include "STATComms.h"
#include "statserial.h"
#include "statsocket_block.h"

////////////////////////////////////////////////////////////////////////////////////////
// Constructor
STATComms::STATComms()
: pTransport(NULL)
{
}


////////////////////////////////////////////////////////////////////////////////////////
// Destructor
STATComms::~STATComms()
{
	if (pTransport)
	{
		pTransport->Disconnect();
		pTransport->Release();

		delete pTransport; 
		pTransport = NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////////////
// 
int STATComms::SetTransport(const STATCONNECTTYPE eConnect)
{
	// additional transports may be added here as needed...
	switch(eConnect)
	{
		case SymbianSocket:
			pTransport = new CSTATSocket;
			break;
		case SymbianSerial:
			pTransport = new CSTATSerial(eConnect);
			break;
		case SymbianInfrared:
			pTransport = new CSTATSerial(eConnect);
			break;
		case SymbianBluetooth:
			pTransport = new CSTATSerial(eConnect);
			break;
		case SymbianUsb:
			pTransport = NULL;
			break;
	};

	if (!pTransport)
 		return E_OUTOFMEMNOTSUPPORTED;

	return ITS_OK;
}


////////////////////////////////////////////////////////////////////////////////////////
// Initialise
int STATComms::Initialise(void)
{
	if (!pTransport)
 		return E_OUTOFMEMNOTSUPPORTED;

	return pTransport->Initialise();
}


////////////////////////////////////////////////////////////////////////////////////////
// Connect
int STATComms::Connect(const char *pAddress)
{
	if (!pTransport)
 		return E_OUTOFMEMNOTSUPPORTED;

	return pTransport->Connect(pAddress);
}


////////////////////////////////////////////////////////////////////////////////////////
// Send
int STATComms::Send(const char cIdentifier, const char *pData, const unsigned long ulLength)
{
	if (!pTransport)
 		return E_OUTOFMEMNOTSUPPORTED;

	return pTransport->Send(cIdentifier, pData, ulLength);
}


////////////////////////////////////////////////////////////////////////////////////////
// Receive
int STATComms::Receive(char *cIdentifier, char **ppData, unsigned long *pLength)
{
	if (!pTransport)
 		return E_OUTOFMEMNOTSUPPORTED;

	return pTransport->Receive(cIdentifier, ppData, pLength);
}


////////////////////////////////////////////////////////////////////////////////////////
// Disconnect
int STATComms::Disconnect(void)
{
	if (!pTransport)
 		return E_OUTOFMEMNOTSUPPORTED;

	return pTransport->Disconnect();
}


////////////////////////////////////////////////////////////////////////////////////////
// Release
int STATComms::Release(void)
{
	if (!pTransport)
 		return E_OUTOFMEMNOTSUPPORTED;

	return pTransport->Release();
}


////////////////////////////////////////////////////////////////////////////////////////
// Return error text
char * STATComms::Error(void)
{
	if (!pTransport)
 		return NULL;

	return pTransport->Error();
}
