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
 * Stat Serial Transport
 *
 *******************************************************************************/
#ifndef __STATSERIAL_H__
#define __STATSERIAL_H__

/********************************************************************************
 *
 * System Includes
 *
 *******************************************************************************/

#ifndef LIGHT_MODE
#include <c32comm.h>
#else // ifndef LIGHT_MODE
#include <serialdriverext.h>
#endif // ifndef LIGHT_MODE

#include <f32file.h>

/********************************************************************************
 *
 * Local Includes
 *
 *******************************************************************************/
#include "filedump.h"
#include "stat_interfaces.h"

/********************************************************************************
 *
 * Definitions
 *
 *******************************************************************************/
#define KModuleSize									8

/********************************************************************************
 *
 * Class
 *
 *******************************************************************************/
class CStatApiSerial : public CActive, public MStatNetwork
{
public:
	// construction
	static CStatApiSerial* NewL( TPtrC16 aModule );
	CStatApiSerial();
	~CStatApiSerial();

	// from MStatNetwork
	TInt GetPacketSize();
	TInt InitialiseL( MNotifyStatTransport *iTransport );
	TInt ConnectL( TDesC *aRemoteHost );
	TInt RequestSend( TDesC8 *aCommandData, const TUint aDataLength );
	TInt RequestReceive( TUint aByteCount );
	TInt Disconnect( void );
	TInt Release( void );
	TText8 *Error( void );
	TUint NtoHl( TUint aValue ) const;
	TUint HtoNl( TUint aValue ) const;

	// from CActive
	void RunL( void );
	void DoCancel( void );

public:
	// Enumeration of different serial sub-types.
	// There is an option of using serial sub-classes but the number of
	// classes (three plus a base serial) is too great for the 
	// very small number of special case changes we need.
	// Leave the serial implementation as one class and continue 
	// to handle special cases.  This ensure all the code we 
	// need continues to be in one place.
	enum TSerialSubtype {
				EInvalid = 0,
				ESerialCable,
				EInfraRed, 
				ENumberOfSubTypes
				};

private:
	void ConstructL( TPtrC16 aModule );

	void SetStatus( TCommStatus aNewStatus );
	void ExtractOptions( TDesC *aRemoteHost, TDes& aPortNumber, TInt& aBaudCap, TBps& aBaudRate );

	// Finalise the initialisation started by a call to InitialiseL.
	// Called directly by InitialiseL in a synchronous
	// initialisation; called from RunL in an asynchronous
	// initialisation (such as USB).
	TInt OnInitialiseL(TInt resultCode);

	// Uses our iModule transport description text string to
	// determine the sub-type.  This will leave if we
	// are pass an unrecognised string in the constructor.
	// The leave value will be KErrNotSupported.
	void DetermineSubTypeL(void);

	// Returns the sub-type that identifies the nature of
	// our transport special case.
	TSerialSubtype SubType(void) const;

private:
	TCommStatus iSerialStatus;
	TReadWriteStatus iRWStatus;
	TBuf16<KModuleSize> iModule;
	MNotifyStatTransport *iTransport;
	TBuf8<1> iDummyBuffer;
	TInt iRetries;
	// Store the packetsize.
	// This depends upon our specific serial subtype.
	TInt iMaxPacketSize;

#ifndef LIGHT_MODE
	RCommServ iCommServer;
	RComm iCommPort;
#else // ifndef LIGHT_MODE
	SerialServer	iCommServer;
	SerialPort		iCommPort;

	RLibrary serialDriverLib;   // use for loading the serial driver lib

#endif // ifndef LIGHT_MODE

	TCommConfig iPortSettings, iOldPortSettings;

	HBufC8 *iRecvBuffer;
	TPtr8 *iRecvBufferPtr;
	TInt iRecvLength, iCommOldSettingsValid;

	// This is the sub-type that identifies the nature of
	// our transport special case.
	TSerialSubtype iSubType;
};

inline TUint CStatApiSerial::NtoHl( TUint aValue ) const
{
	return ( aValue );
}

inline TUint CStatApiSerial::HtoNl( TUint aValue ) const
{
	return ( aValue );
}

inline CStatApiSerial::TSerialSubtype CStatApiSerial::SubType(void) const
{
	return (iSubType);	
}

#endif
