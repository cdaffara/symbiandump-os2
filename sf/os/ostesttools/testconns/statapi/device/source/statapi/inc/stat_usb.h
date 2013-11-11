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
 * Stat USB Transport
 *
 *******************************************************************************/
#ifndef __STATUSB_H__
#define __STATUSB_H__

/********************************************************************************
 *
 * System Includes
 *
 *******************************************************************************/
#include <d32usbc.h>

/********************************************************************************
 *
 * Local Includes
 *
 *******************************************************************************/
#include "filedump.h"
#include "../../../../common/inc/serialpacketsize.h"

/********************************************************************************
 *
 * Class
 *
 *******************************************************************************/
class CStatApiUsb : public CActive, public MStatNetwork
{
public:
	// construction
	static CStatApiUsb* NewL( void );
	CStatApiUsb();
	~CStatApiUsb();

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
	void Reset( void );
	
	// from CActive
	void RunL( void );
	void DoCancel( void );

private:
	void ConstructL(void);

	void SetStatus( TCommStatus aNewStatus );

	// Finalise the initialisation started by a call to InitialiseL.
	// Called directly by InitialiseL in a synchronous
	// initialisation; called from RunL in an asynchronous
	// initialisation (such as USB).
	TInt OnInitialiseL(TInt resultCode);

private:
	TBool iReEnum;
	TCommStatus iUsbStatus;
	TReadWriteStatus iRWStatus;
	MNotifyStatTransport *iTransport;
	
	// Store the packetsize.
	TInt iMaxPacketSize;
	
	//USB interface
	TUsbcInterfaceInfoBuf ifc;
	
	//USB channel
	RDevUsbcClient iPort;

	//Timer to wait for device reenumeration
	RTimer iTimer;
	
	typedef enum { EEnum, EWait, EConnect } TConnectingState;

	TConnectingState iConnectingState;
	
	HBufC8 *iBuffer;
	TInt iLength;

};


inline TUint CStatApiUsb::NtoHl( TUint aValue ) const
{
	return ( aValue );
}

inline TUint CStatApiUsb::HtoNl( TUint aValue ) const
{
	return ( aValue );
}
#endif
