/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*   This file contains global definitions to errors
*/

#ifndef __ERROR__
#define __ERROR__

// GLOBAL DEFINES
#define ERR_BASE			0x1000
#define ERR_BASE_UTIL		(ERR_BASE + 0x0100)
#define ERR_BASE_PHOENIX	(ERR_BASE + 0x0200)
#define ERR_BASE_BLUETOOTH	(ERR_BASE + 0x0300)
#define ERR_BASE_MTC		(ERR_BASE + 0x0400)
#define ERR_BASE_DG	        (ERR_BASE + 0x0500)

#define NO_ERRORS						0
#define ERR_NO_PARAMS					(ERR_BASE + 1)
#define ERR_UNKNOWN_COMMAND				(ERR_BASE + 2)

#define ERR_UTIL_NO_PARAM_VALUE			(ERR_BASE_UTIL + 2)
#define ERR_UTIL_UNKNOWN_PARAM			(ERR_BASE_UTIL + 3)
#define ERR_UTIL_PROPERTIES_NOT_FOUND	(ERR_BASE_UTIL + 4)
#define ERR_UTIL_NO_PROPERTY_VALUE		(ERR_BASE_UTIL + 5)

#define ERR_BLUETOOTH_CONNECTION_FAILED			(ERR_BASE_BLUETOOTH + 1)
#define ERR_BLUETOOTH_DISCONNECTION_FAILED		(ERR_BASE_BLUETOOTH + 2)
#define ERR_BLUETOOTH_LOCAL_SERVICE_MISSING		(ERR_BASE_BLUETOOTH + 3)
#define ERR_BLUETOOTH_REMOTE_SERVICE_MISSING	(ERR_BASE_BLUETOOTH + 4)
#define ERR_BLUETOOTH_DEVICE_ADDRESS_MISSING	(ERR_BASE_BLUETOOTH + 5)

#define ERR_MTC_INVALID_MODE	(ERR_BASE_MTC + 1)

#define ERR_DG_SOCKET					 (ERR_BASE_DG + 1)
#define ERR_DG_COMMCHANNEL				 (ERR_BASE_DG + 2)
#define ERR_DG_UNINITIALISED_COMMCHANNEL (ERR_BASE_DG + 3)
#define ERR_DG_COMMCHANNEL_INIT          (ERR_BASE_DG + 4)
#define ERR_DG_UNKNOWN_COMMCHANNEL		 (ERR_BASE_DG + 5)
#define ERR_DG_COMMCHANNEL_OPEN			 (ERR_BASE_DG + 6)
#define ERR_DG_CONSOLEHANDLER			 (ERR_BASE_DG + 7)
#define ERR_DG_CONNECTION_OPEN					 (ERR_BASE_DG + 8)
#define ERR_DG_CONNECTION_INIT					 (ERR_BASE_DG + 9)
#define ERR_DG_COMM_OPEN		         (ERR_BASE_DG + 10)
#define ERR_DG_COMM_OPEN_TIMEOUT		 (ERR_BASE_DG + 11)
#define ERR_DG_COMM_OPEN_QUERY			 (ERR_BASE_DG + 12)
#define ERR_DG_COMM_DATA_RECV			 (ERR_BASE_DG + 13)
#define ERR_DG_COMM_DATA_RECV_TIMEOUT   (ERR_BASE_DG + 14)
#define ERR_DG_COMM_DATA_SEND			 (ERR_BASE_DG + 15)

#define ERR_DG_COM_INIT					 (ERR_BASE_DG + 16)
#define ERR_DG_COM_IF_QUERY				 (ERR_BASE_DG + 17)

typedef struct
{
	DWORD err_code;
	char *err_msg;
} ErrorLookupEntry;

static const ErrorLookupEntry ErrorLookupTable[] =
	{
		{ERR_DG_SOCKET,						"ERR_DG_SOCKET"},
		{ERR_DG_COMMCHANNEL,				"ERR_DG_COMMCHANNEL"},
		{ERR_DG_UNINITIALISED_COMMCHANNEL,	"ERR_DG_UNINITIALISED_COMMCHANNEL"},
		{ERR_DG_COMMCHANNEL_INIT,			"ERR_DG_COMMCHANNEL_INIT"},
		{ERR_DG_UNKNOWN_COMMCHANNEL,		"ERR_DG_UNKNOWN_COMMCHANNEL"},
		{ERR_DG_COMMCHANNEL_OPEN,			"ERR_DG_COMMCHANNEL_OPEN"},
		{ERR_DG_CONSOLEHANDLER,				"ERR_DG_CONSOLEHANDLER"},
		{ERR_DG_CONNECTION_OPEN,		    "ERR_DG_CONNECTION_OPEN"},
		{ERR_DG_COMM_OPEN_TIMEOUT,			"ERR_DG_COMM_OPEN_TIMEOUT"},
		{ERR_DG_COMM_OPEN_QUERY,			"ERR_DG_COMM_OPEN_QUERY"},
		{ERR_DG_COMM_DATA_RECV,				"ERR_DG_COMM_DATA_RECV"},
		{ERR_DG_COMM_DATA_RECV_TIMEOUT,		"ERR_DG_COMM_DATA_RECV_TIMEOUT"},
		{ERR_DG_COMM_DATA_SEND,				"ERR_DG_COMM_DATA_SEND"},


		{ERR_DG_COM_INIT,					"ERR_DG_COM_INIT"},
		{ERR_DG_COM_IF_QUERY,				"ERR_DG_COM_IF_QUERY"}
	};

#define ERROR_LOOKUP(x) \
	for (int i = 0; i < sizeof(ErrorLookupTable); i++)										  \
	{																					      \
		if (ErrorLookupTable[i].err_code == x)												  \
		{																			          \
			char tmp[256];																	  \
			sprintf(tmp, "[DataGateway] Error: %s (0x%04X)\n", ErrorLookupTable[i].err_msg,   \
                                                               ErrorLookupTable[i].err_code); \
			Util::Error(tmp);																  \
		}																				      \
	}

#endif

// End of file