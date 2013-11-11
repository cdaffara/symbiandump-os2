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



 /**************************************************************************************
 *
 * Packetisation transport for STAT -- for packet based network layers
 *
 *************************************************************************************/

/**************************************************************************************
 *
 * Local Includes
 *
 *************************************************************************************/
#include "assert.h"
#include "stat.h"
#include "stat_serial.h"
#include "msgwin.h"
#include "../../../../common/inc/serialpacketsize.h"

/**************************************************************************************
 *
 * Definitions
 *
 *************************************************************************************/
#ifndef LIGHT_MODE
_LIT(LDD_NAME,"ECOMM");
#ifdef __WINS__
_LIT(PDD_NAME,"ECDRV");
#else
_LIT(PDD_NAME,"EUART1");
#endif
#endif // ifndef LIGHT_MODE

#define KMaxTimeoutRetries	(0x0FFFFFFF)
#define KWriteTimeout		50000000

// Moved this data definitions to the source file where they are
// less public.
#define KReadTimeout								30000000

/********************************************************************************
 *
 * Macro functions
 *
 ********************************************************************************/

/**************************************************************************************
 *
 * CStatApiSerial - Construction
 *
 *************************************************************************************/
CStatApiSerial* CStatApiSerial::NewL( TPtrC16 aModule )
{
    CStatApiSerial *self = new (ELeave) CStatApiSerial();
    CleanupStack::PushL(self);
	self->ConstructL( aModule );
	CleanupStack::Pop();
    return self;
}

CStatApiSerial::CStatApiSerial() : CActive(EPriorityStandard)
{
}

CStatApiSerial::~CStatApiSerial()
{
	// remove from active scheduler
	Deque();

	// clean up params
	if( iRecvBuffer != NULL )
		delete iRecvBuffer;
	if( iRecvBufferPtr != NULL )
		delete iRecvBufferPtr;

#ifndef LIGHT_MODE
	iCommPort.Close();
	iCommServer.Close();
#else // ifndef LIGHT_MODE
	if( NULL != iCommPort )
		{
		CloseSerialPort( iCommPort );
		iCommPort =	NULL;
		}
	if( NULL != iCommServer )
		{
		CloseSerialServer( iCommServer );
		iCommServer =	NULL;
		}
#endif // ifndef LIGHT_MODE
}

void CStatApiSerial::ConstructL( TPtrC16 aModule )
{
	// add to active scheduler
	CActiveScheduler::Add(this);

	// set parameters
	asserte( aModule.Length() <= KModuleSize );
	iModule.Copy( aModule );
	iRecvBuffer = NULL;
	iRecvBufferPtr = NULL;
	iRecvLength = 0;
	iRWStatus = ENoRW;
	iSerialStatus = EIdle;
	iTransport = NULL;
	iRetries = 0;
	iCommOldSettingsValid = 0;

#ifdef LIGHT_MODE
	iCommServer =	NULL;
	iCommPort =	NULL;
#endif // ifdef LIGHT_MODE

	// Initialise the sub-type.  We have the iModule text string
	// and we use it below determine the subtype.
	iSubType = EInvalid;
	iMaxPacketSize = KMaxPacketSize;

	// Check the iModule text string to determine our serial
	// transport sub-type.
	DetermineSubTypeL();
	asserte( EInvalid != iSubType );
}

/**************************************************************************************
 *
 * CStatApiSerial - MStatNetwork - Initialise and Release
 *
 *************************************************************************************/
TInt CStatApiSerial::InitialiseL( MNotifyStatTransport *aTransport )
{
	TInt r = KErrNone;

	// save the transport interface
	iTransport = aTransport;

	// set the state
	SetStatus( EInitialising );

#ifndef LIGHT_MODE
	// Under WINS we must force a link to the file server so that we're sure we'll be 
	// able to load the device drivers. On a MARM implementation, this code would not
	// be required because higher level components (EIKON) will automatically have started 
	// the services. NOTE: this is now no longer required since we are an app and so even
	// on WINS everything else should have started up by now.

	// Load up the physical and the logical device drivers. If they are already loaded 
	// then it won't make any difference.
	r = User::LoadPhysicalDevice( PDD_NAME );
	if( (r != KErrNone) && (r != KErrAlreadyExists) ) {
		User::Leave( r );
	}
	r = User::LoadLogicalDevice( LDD_NAME );	
	if( (r != KErrNone) && (r != KErrAlreadyExists) ) {
		User::Leave( r );
	}
	
	// Both WINS and EIKON will have started the comms server process.
	// (this is only really needed for ARM hardware development racks)
#ifndef __WINS__
	r = StartC32();
	if( (r != KErrNone) && (r != KErrAlreadyExists) )
		User::Leave( r );
#endif
#else // ifndef LIGHT_MODE

	// Do a specific load of the library at this point.	
	r = serialDriverLib.Load(_L("SerialDriver.dll"));
	if (r != KErrNone)
		User::Leave( r );

	iCommServer =	OpenSerialServerL( );
	
	if (iCommServer == NULL)
		User::Leave( KErrNotFound );
		
#endif // ifndef LIGHT_MODE

	r = OnInitialiseL(r);

	return (r);
}

/**************************************************************************************
 *
 * CStatApiSerial - OnInitialiseL
 * 
 * Method added to allow for the seperation of InitialiseL into two parts.
 * The first part is still InitialiseL.  The second part is here and
 * is either called directly or from the RunL callback.
 *************************************************************************************/
TInt CStatApiSerial::OnInitialiseL(TInt resultCode)
{
	TInt r = resultCode;

	if (r >= 0 )
	{
#ifndef LIGHT_MODE
		// Now connect to the comm server
		
		User::LeaveIfError( iCommServer.Connect() );

		// Load the CSY module
		r = iCommServer.LoadCommModule( iModule );
		User::LeaveIfError( r );

		// check we loaded correctly
		TInt numPorts;
		r = iCommServer.NumPorts( numPorts );
		User::LeaveIfError( r );
#endif // ifndef LIGHT_MODE

		// set the state
		SetStatus( EInitialised );
	}

	return r;
}

TInt CStatApiSerial::Release(void)
{
	// make sure the status is as expected
	asserte( (iSerialStatus == EInitialising) || (iSerialStatus == EInitialised) || (iSerialStatus == EDisconnected) );
	SetStatus( EReleasing );

	// disconnect from the comms server
#ifndef LIGHT_MODE
	iCommServer.Close();
#else // ifndef LIGHT_MODE
	if( NULL != iCommServer )
		{
		CloseSerialServer( iCommServer );
		iCommServer =	NULL;

		// Here is an appropriate spot for releasing the library we need for stat-light
		serialDriverLib.Close();

		}
#endif // ifndef LIGHT_MODE

	// update state and finish
	SetStatus( EIdle );
	return KSTErrSuccess;
}

/**************************************************************************************
 *
 * CStatApiSerial - MStatNetwork - Connect and Close
 *
 *************************************************************************************/
static int atoi( const short *str )
{
	int ret = 0;
	for( int i = 0; str[i] != NULL; i++ ) {
		ret *= 10;
		ret += str[i] - '0';
	}
	return ret;
}

void CStatApiSerial::ExtractOptions( TDesC *aRemoteHost, TDes& aPortNumber, TInt& aBaudCap, TBps& aBaudRate )
{
	TInt baud;
	short *delim;

	// get the character array for the remote host (note that this is in unicode)
	short *opt = (short*)aRemoteHost->Ptr();

	int loop = 0;
	while(opt[loop] != OPT_DELIMITER)
	{
		aPortNumber.Append(opt[loop]);
		loop++;
	}
	
	
	// now search for the next delimiter and NULL it
	for( delim = &opt[loop+1]; (*delim != NULL) && (*delim != OPT_DELIMITER); delim++ )
		;

	// if this is a delim then NULL it
	if( *delim == OPT_DELIMITER ) {
		*delim = NULL;
	}

	// extract the baud -- and set the correct constants for the given baud
	baud = atoi( &(opt[loop+1]) ); 
	switch( baud ) {
	case 115200:
		aBaudRate = EBps115200;
		aBaudCap = KCapsBps115200;
		break;
	case 38400:
		aBaudRate = EBps38400;
		aBaudCap = KCapsBps38400;
		break;
	case 19200:
		aBaudRate = EBps19200;
		aBaudCap = KCapsBps19200;
		break;
	case 9600:
		aBaudRate = EBps9600;
		aBaudCap = KCapsBps9600;
		break;
	default:
		;
		break;
	}
}

TInt CStatApiSerial::ConnectL( TDesC *aRemoteHost )
{
	TBuf<100> portNumber;
	TInt baudCap;
	TBps baudRate;
	
#ifdef LIGHT_MODE
	static const TInt	KMaxPortName =	8;
	TBool	result =	EFalse;
#endif // ifndef LIGHT_MODE

	// verify state 
	asserte( iSerialStatus == EInitialised );
	SetStatus( EConnecting );

	// I used to verify the address was valid here -- but this has now been moved
	// to the UI. If it is nonsense then it will just throw an error so it's not
	// dangerous. Error reporting is now good enough that I can remove this.
	
	// extract the options from the string
	ExtractOptions( aRemoteHost, portNumber, baudCap, baudRate ); 

	// construct the address -- aRemoteHost should provide the COM port number
	
	TBuf16<KMaxPortName + 4> portName;
	
	//TInt err = portName.Num( portNumber );
	
	TLex lNum = TLex(portNumber);
	TInt lInt; 
	
	TInt err = lNum.Val(lInt);
	
	if(err == KErrNone)
		{
		
		portName.Insert( 0, portNumber);		
		portName.Insert( 0, _L("::") );

		asserte( EInvalid != iSubType );
		if( SubType() == ESerialCable )
			{
			portName.Insert( 0, _L("COMM") );
			}
		else if( SubType() == EInfraRed )
			{
			portName.Insert( 0, _L("IrCOMM") );
			}
		}
	else
		{
		//the port already contains the Type	
		portName.Insert( 0, portNumber);
		
		}
		
	
	// open the serial port
#ifndef LIGHT_MODE
	
	TInt r = iCommPort.Open( iCommServer, portName, ECommExclusive );
	User::LeaveIfError( r );
	
#else // ifndef LIGHT_MODE
	iCommPort =	OpenSerialPortL( iCommServer, portNumber[0]-'0' );
	
	if( NULL == iCommPort )
		{
		User::Leave( KErrCouldNotConnect );
		}
#endif // ifndef LIGHT_MODE

#ifndef LIGHT_MODE
	// save port settings for restoring later
	iCommPort.Config( iOldPortSettings );
	iCommOldSettingsValid = 1;

	// check our configuration is supported
	TCommCaps ourCapabilities;
	iCommPort.Caps( ourCapabilities );
	if (((ourCapabilities ().iRate & baudCap) == 0) ||
		 ((ourCapabilities ().iDataBits & KCapsData8) == 0) ||
		 ((ourCapabilities ().iStopBits & KCapsStop1) == 0) ||
		 ((ourCapabilities ().iParity & KCapsParityNone) == 0)) 
	{
			User::Leave( KErrNotSupported );
	}
#endif // ifndef LIGHT_MODE

	// set new port settings
#ifndef LIGHT_MODE
	iCommPort.Config( iPortSettings );
#else
	GetPortConfig (iCommPort, iPortSettings);
#endif // ifndef LIGHT_MODE

	iPortSettings().iRate = baudRate;
	iPortSettings().iParity = EParityNone;
	iPortSettings().iDataBits = EData8;
	iPortSettings().iStopBits = EStop1;
	iPortSettings().iFifo = EFifoEnable;
//	iPortSettings().iHandshake = KConfigFreeRTS | KConfigFreeDTR;
	iPortSettings().iHandshake = KConfigObeyCTS;
	iPortSettings().iTerminatorCount = 0;

	// cancel any pending reads / writes to be safe and set the config
#ifndef LIGHT_MODE
	iCommPort.Cancel();
#else // ifndef LIGHT_MODE
	ReadCancel( iCommPort );
	WriteCancel( iCommPort );
	
#endif // ifndef LIGHT_MODE

#ifndef LIGHT_MODE
	r = iCommPort.SetConfig( iPortSettings );	
#else // ifndef LIGHT_MODE
	TInt r = SetConfig( iCommPort, iPortSettings );	
#endif // ifndef LIGHT_MODE

	if( r != KErrNone )
		{
		User::Leave(r);
		}

	// now turn on DTR and RTS, and set our buffer size
//		commPort.SetSignals (KSignalDTR, 0);
//		commPort.SetSignals (KSignalRTS, 0);

	// set the receive buffer length then check it did it ok
#ifndef LIGHT_MODE
	iCommPort.SetReceiveBufferLength( 2*iMaxPacketSize );
#else // ifndef LIGHT_MODE
	result =	SetReceiveBufferLength( iCommPort, 2*iMaxPacketSize );
	if( ! result ) {
		User::Leave(KErrUnknown);
	}
#endif // ifndef LIGHT_MODE

	// check buffer size (use a relative check rather than
	// an absolute check - if we get a bigger buffer than
	// we asked for then that is no problem)
	TInt	size =	0;

#ifndef LIGHT_MODE
	size =	iCommPort.ReceiveBufferLength();
#else // ifndef LIGHT_MODE
	result =	ReceiveBufferLength( iCommPort, size );
	if( ! result ) {
		User::Leave(KErrUnknown);
	}
#endif // ifndef LIGHT_MODE

	if( size < 2*iMaxPacketSize )
	{
		User::Leave( KErrTooBig );
	}

	// allocate enough memory to hold a data read
	iRecvBuffer = HBufC8::New(iMaxPacketSize);
	if( !iRecvBuffer ) {
		User::Leave(KErrNoMemory);
	}
	iRecvBufferPtr = new TPtr8( iRecvBuffer->Des() );

	// power up the serial port by doing a null read on the port
	iRWStatus = EReadPending;
#ifndef LIGHT_MODE
	iCommPort.Read( iStatus, iDummyBuffer, 0);
#else // ifndef LIGHT_MODE
	Read( iCommPort, iStatus, KReadTimeout, iDummyBuffer, 0 );
#endif // ifndef LIGHT_MODE
	SetActive();

	// return asynchronous 
	return KSTErrAsynchronous;
}

TInt CStatApiSerial::Disconnect(void)
{
	// verify the status
	asserte( (iSerialStatus == EConnected) || (iSerialStatus == EConnecting) );
	SetStatus( EDisconnecting );

	// clean up the port
#ifndef LIGHT_MODE
	TInt sessionHandle = iCommPort.SubSessionHandle();
	if( sessionHandle )
	{
		iCommPort.Cancel();
	}
	if( iCommOldSettingsValid )
		iCommPort.SetConfig( iOldPortSettings );
	if( sessionHandle )
	{
		iCommPort.Close();
	}
	sessionHandle = iCommPort.SubSessionHandle();
	asserte(0 == sessionHandle);
	iCommOldSettingsValid = 0;
#else // ifndef LIGHT_MODE
	if( NULL != iCommPort )
		{
		CloseSerialPort( iCommPort );
		iCommPort =	NULL;
		}
#endif // ifndef LIGHT_MODE

	// release the data buffer
	delete iRecvBufferPtr;
	iRecvBufferPtr = NULL;
	delete iRecvBuffer;
	iRecvBuffer = NULL;

	// done
	SetStatus( EDisconnected );
	
	return KSTErrSuccess;
}

/**************************************************************************************
 *
 * CStatApiSerial - MStatNetwork - Receive and Send. The ID / Length / Data nonsense 
 * is handled by the packetisation layer. All serial has to do here is send data -- 
 * serial is also assuming that the upper layer will keep the data until the response
 *
 *************************************************************************************/
TInt CStatApiSerial::RequestSend( TDesC8 *aData, const TUint aDataLength )
{
	// make sure we are in the appropriate state 
	asserte( iSerialStatus == EConnected );
	asserte( iRWStatus == ENoRW );
	iRWStatus = EWritePending;

	// do the send
	asserte( !IsActive() );
	asserte( (unsigned)aData->Length() == aDataLength );
#ifndef LIGHT_MODE
	iCommPort.Write( iStatus, KWriteTimeout, (*aData), aDataLength );
#else // ifndef LIGHT_MODE
	Write( iCommPort, iStatus, KWriteTimeout, (*aData), aDataLength );
#endif // ifndef LIGHT_MODE
	SetActive();
	return KSTErrAsynchronous;
}

TInt CStatApiSerial::RequestReceive( TUint aByteCount )
{
	// make sure we are in the appropriate state
	asserte( iSerialStatus == EConnected );
	asserte( iRWStatus == ENoRW );
	iRWStatus = EReadPending;

	// allocate a buffer for the read
	asserte( aByteCount <= static_cast<TUint>(iMaxPacketSize) );
	asserte( !IsActive() );
	iRecvLength = aByteCount;
	iRecvBufferPtr->SetLength( 0 );
#ifndef LIGHT_MODE
	iCommPort.ReadCancel();
	iCommPort.Read( iStatus, KReadTimeout, *iRecvBufferPtr, aByteCount );
#else // ifndef LIGHT_MODE
	ReadCancel( iCommPort );
	Read( iCommPort, iStatus, KReadTimeout, *iRecvBufferPtr, aByteCount );
#endif // ifndef LIGHT_MODE
	SetActive();
	return KSTErrAsynchronous;
}

TInt CStatApiSerial::GetPacketSize()
{
	// The packet size is configured when we initialise the port.
	return iMaxPacketSize;
}

TText8 *CStatApiSerial::Error( void )
{
	return NULL;
}

/**************************************************************************************
 *
 * CStatApiSerial - Active Object
 *
 *************************************************************************************/
void CStatApiSerial::RunL( void )
{
	// cancels don't require any handling
	if( iStatus == KErrCancel )
		return;

	// if timed out then reissue the read
	if( (iStatus == KErrTimedOut) && (iRWStatus == EReadPending) ) {
		asserte( iSerialStatus == EConnected );
		asserte( iRWStatus == EReadPending );
		iRetries++;
#ifndef LIGHT_MODE
		iCommPort.Read( iStatus, KReadTimeout, *iRecvBufferPtr, iRecvLength );
#else // ifndef LIGHT_MODE
		Read( iCommPort, iStatus, KReadTimeout, *iRecvBufferPtr, iRecvLength );
#endif // ifndef LIGHT_MODE
		SetActive();
		return;
	}
	iRetries = 0;

	// throw an error
	if( iStatus != KErrNone ) {
		iRWStatus = ENoRW;
		iTransport->HandleError( iStatus.Int(), NULL );
	}

	// if we are in connecting state and have just done the null read then
	// we are done and have connected
	if( (iSerialStatus == EConnecting) && (iRWStatus == EReadPending) ) {
		iRWStatus = ENoRW;
		SetStatus( EConnected );
		iTransport->HandleConnect( KErrNone );
		return;
	}


	// if we are writing then notify of the write
	if( iRWStatus == EWritePending ) {
		iRWStatus = ENoRW;
		iTransport->HandleSend( KErrNone );
		return;
	}

	// if we are reading then notify of the read
	if( iRWStatus == EReadPending ) {
		TInt length = iRecvLength;
		iRWStatus = ENoRW;
		iRecvLength = 0;
		iTransport->HandleReceive( KErrNone, iRecvBufferPtr, length );
		return;
	}
}

void CStatApiSerial::DoCancel( void )
{
	if( (iSerialStatus == EConnected) || (iSerialStatus == EConnecting) )
		{
#ifndef LIGHT_MODE
		iCommPort.Cancel();
#else // ifndef LIGHT_MODE
		ReadCancel( iCommPort );
		WriteCancel( iCommPort );
#endif // ifndef LIGHT_MODE
		}
}

/**************************************************************************************
 *
 * CStatApiSerial - Private Functions
 *
 *************************************************************************************/
void CStatApiSerial::SetStatus( TCommStatus aNewStatus )
{
	iSerialStatus = aNewStatus;
}

/**************************************************************************************
 *
 * CStatApiSerial - DetermineSubTypeL
 *
 *************************************************************************************/
void CStatApiSerial::DetermineSubTypeL()
{
	
	
	const TPtrC subTypes[ENumberOfSubTypes] = { 
				_L(""),			// Invalid
				_L("ECUART"), 	// Serial cable
				_L("IrCOMM")	// Infra-red
				};

	const TInt packetSize[ENumberOfSubTypes] = { 
				0,
				KMaxPacketSize,
				KMaxPacketSize
				};

	TInt count;
	for( count = 0; ( EInvalid == iSubType ) && ( count < ENumberOfSubTypes ); count++ )
	{
		if( 0 == ( iModule.Compare( subTypes[count] ) ) )
		{
			iSubType = static_cast<TSerialSubtype>(count);
			iMaxPacketSize = packetSize[count];
		}
	}

	if( EInvalid == iSubType )
	{
		User::Leave( KErrNotSupported );
	}
	
}
