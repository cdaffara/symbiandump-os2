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




#ifndef STATSERIALBLUETOOTH_H
#define STATSERIALBLUETOOTH_H

#include "statcommon.h"
#include "stattransport.h"

// COM port settings
#define STATBT_WRITETOTALTIMEOUTMULTIPLIER		10
#define STATBT_WRITETOTALTIMEOUTCONSTANT		10
#define STATBT_READINTERVALTIMEOUT				100
#define STATBT_READTOTALTIMEOUTMULTIPLIER		100
#define STATBT_READTOTALTIMEOUTCONSTANT			120000

#define STATBT_BAUDRATE						CBR_115200
#define STATBT_BYTESIZE						8
#define STATBT_PARITY						NOPARITY
#define STATBT_STOPBITS						ONESTOPBIT


class CSTATSerialBluetooth : public CSTATTransport
{
public:
	CSTATSerialBluetooth();
	~CSTATSerialBluetooth();
	int Initialise(void);
	int Connect(const char *pAddress = NULL);
	int Send(const char cIdentifier, const char *pData = NULL, const unsigned long ulLength = 0);
	int Receive(char *cIdentifier, char **ppData = NULL, unsigned long *pLength = NULL);
	int Disconnect(void);
	int Release(void);

private:
	bool OpenComPort(const char *pAddress);
	int ReceiveBytes( char *buff, unsigned long size );
	
	HANDLE hComPort;				// com port to work with
	char *pTemp;					// holds received data
};

#endif
