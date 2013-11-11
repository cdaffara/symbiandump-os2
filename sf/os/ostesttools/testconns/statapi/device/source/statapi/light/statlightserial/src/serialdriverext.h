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


/**
@file
@internalComponent

The interface of the serial driver stuff for users of the serial
driver library - not users of the class itself.

*/

#ifndef __SERIAL_DRIVER_EXT_H
#define __SERIAL_DRIVER_EXT_H

#include <d32comm.h>   // gives us 'TCommConfig'

typedef TAny * SerialServer;
typedef TAny * SerialPort;

// The external interface to users of the library,
			  		  
IMPORT_C SerialServer OpenSerialServerL(void);

IMPORT_C void CloseSerialServer(SerialServer aServer);

IMPORT_C SerialPort OpenSerialPortL(SerialServer aServer, TUint aPort);

IMPORT_C TBool CloseSerialPort(SerialPort aPort);

IMPORT_C TBool Read(SerialPort aPort,
			  		TRequestStatus & aStatus, 
			  		TTimeIntervalMicroSeconds32 aTimeout,
			  		TDes8 &aDes,
			  		TInt aLength);

IMPORT_C TBool Write(SerialPort aPort,
					 TRequestStatus & aStatus,
				     TTimeIntervalMicroSeconds32 aTimeout,
				     const TDesC8 &aDes,
				     TInt aLength);
				     
IMPORT_C TInt SetConfig(SerialPort aPort, const TCommConfig & aRequiredConfig);

IMPORT_C TBool GetPortConfig (SerialPort aPort, TCommConfig & config);

IMPORT_C TBool ReadCancel(SerialPort aPort);

IMPORT_C TBool WriteCancel(SerialPort aPort);

IMPORT_C TBool SetReceiveBufferLength(SerialPort aPort, TInt aSize);

IMPORT_C TBool ReceiveBufferLength(SerialPort aPort, TInt & aSize);

// typedef equivalents


typedef SerialServer (*TLibOpenSerialServerL)(void);
typedef void (*TLibCloseSerialServer)(SerialServer aServer);
typedef SerialPort (*TLibOpenSerialPortL)(SerialServer aServer, TUint aPort);
typedef TBool (*TLibCloseSerialPort)(SerialPort aPort);
typedef TBool (*TLibRead)(SerialPort aPort,
			  		  TRequestStatus & aStatus, 
			  		  TTimeIntervalMicroSeconds32 aTimeout,
			  		  TDes8 &aDes,
			  		  TInt aLength);
typedef TBool (*TLibWrite)(SerialPort aPort,
					   TRequestStatus & aStatus,
				       TTimeIntervalMicroSeconds32 aTimeout,
				       const TDesC8 &aDes,
				       TInt aLength);
typedef TInt (*TLibSetConfig)(SerialPort aPort, const TCommConfig & aRequiredConfig );

typedef TBool (*TLibReadCancel)(SerialPort aPort);

typedef TBool (*TLibWriteCancel)(SerialPort aPort);

typedef TBool (*TLibSetReceiveBufferLength)(SerialPort aPort, TInt aSize);

typedef TBool (*TLibReceiveBufferLength)(SerialPort aPort, TInt & aSize);


#endif
