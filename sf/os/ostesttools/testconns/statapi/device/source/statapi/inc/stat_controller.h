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



/*************************************************************************
 *
 * Switches
 *
 ************************************************************************/
#ifndef __STATCONTROLLER_H__
#define __STATCONTROLLER_H__

/*************************************************************************
 *
 * System Includes
 *
 ************************************************************************/
#include <e32std.h>
#include <e32base.h>

/*************************************************************************
 *
 * Local Includes
 *
 ************************************************************************/
#include "stat_interfaces.h"
#include "stat_engine.h"
#include "stat_packetisation.h"
#include "stat_serial.h"

#ifndef LIGHT_MODE
#include "stat_tcpip.h"
#include "stat_bt.h"
#include "stat_usb.h"
#endif // ifndef LIGHT_MODE



/*************************************************************************
 *
 * Definitions
 *
 ************************************************************************/
#define KAddressTextLimit 16

/*************************************************************************
 *
 * CStatController
 *
 ************************************************************************/
class CStatController : public CActive, public MStatController, public MNotifyStatController
{
public:
	~CStatController();
	static CStatController *NewL();

	// from MStatController
	TInt StartSession( TStatConnectType aConnectType, TDesC *aConnectParams, MNotifyUI *aUI, RFs *const aSession, MNotifyLogMessage *const aMsg);
	TInt StopSession( TInt aSessionId );
	TInt SessionStatus( TInt aSessionId );

	// from MNotifyStatController
	void HandleStatusChange( TCommStatus aNewStatus );
	void HandleError( TInt aError, void *aErrorData );
	void HandleInfo( const TDesC *aInfo );

	// from CActive
	void RunL();
	void DoCancel();

private:
	CStatController();
	void ConstructL( void );
	void SetStatus( TCommStatus aNewStatus );
	void Notify( TInt aErrorStatus );
	MStatApiTransport *StartTransportL( TStatConnectType aConnectType );
	void KillTransport();

	TCommStatus iSessionStatus;
	MStatApiTransport *iTransport;
	CStatEngine *iEngine;
	MNotifyUI *iUI;

	TStatConnectType iConnectType;
	CStatTransportPacketisation *iPacketisationTransport;
	CStatApiSerial *iSerialNetwork;
#ifndef LIGHT_MODE
	CStatTransportTCPIP *iTcpipTransport;

	CStatApiUsb *iUsbNetwork;
	CStatTransportBT *iBluetoothTransport;
#endif

	TBool iConnectedSuccessfully, iUserInitiatedExitRequest;
	TBuf<KAddressTextLimit> iConnectParams;

	RFs *iFs;
	MNotifyLogMessage *iMsg;
};


#endif //__STATCONTROLLER_H__
