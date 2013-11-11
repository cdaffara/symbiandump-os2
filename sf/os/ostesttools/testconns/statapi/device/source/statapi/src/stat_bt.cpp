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
 *******************************************************************************/
#include <in_sock.h>
#include <e32std.h>

/********************************************************************************
 *
 * Local Includes
 *
 *******************************************************************************/
#include "assert.h"
#include "ntoh.h"
#include "stat_bt.h"
#include "../../../../common/inc/serialpacketsize.h"

/********************************************************************************
 *
 * Macro functions
 *
 ********************************************************************************/
#define NOTISDISCONNECTING(s)	(((s) != EDisconnectingData) && ((s) != EDisconnectingListen) && ((s) != EDisconnected) && ((s) != EDisconnectingUnregister))
#define STANDARDBUFVALUE	4

/********************************************************************************
 *
 * CStatTransportBT -- Constructor
 *
 *******************************************************************************/
CStatTransportBT *CStatTransportBT::NewL( void )
{
    CStatTransportBT *self = new (ELeave) CStatTransportBT();
    CleanupStack::PushL(self);
	self->ConstructL( );
	CleanupStack::Pop();
    return self;
}

CStatTransportBT::CStatTransportBT() : CActive(EPriorityStandard)
{
}

void CStatTransportBT::ConstructL( void )
{
	// add this to active scheduler
	CActiveScheduler::Add(this); 

	// initialise all params	

	iWrCommandData = NULL;
	iRecvBuffer = NULL;
	iRecvBufferPtr = NULL;
	iTransport = NULL;
	iBTTransportStatus = EIdle;
	iBTTransportDisconnectStatusBeforeUnregister = EIdle;
	iRWStatus = ENoRW;

	iMaxPacketSize = KMaxBluetoothPacketSize;
}

CStatTransportBT::~CStatTransportBT()
{
	// this will call cancel and remove the active object -- this will call cancel
	Deque(); 

	// cleanup the sockets
	switch( iBTTransportStatus ) {

	case EIdle:
	case EInitialised:
	case EDisconnected:
	case EError:
		break;

	case EConnected:
		iDataSocket.Shutdown( RSocket::EImmediate, iStatus );
		/* fall through */

	case EConnecting:
		iDataSocket.Close();
		iListenSocket.Shutdown( RSocket::EImmediate, iStatus );
		iListenSocket.Close();
		iSocketServ.Close();
		break;

	case EDisconnectingData:
	case EDisconnectingListen:
		;
		break;
	default:
		break;
	}

	if( iWrCommandData )
		{
		delete iWrCommandData;
		iWrCommandData = NULL;
		}

	if( iRecvBuffer )
		{
		delete iRecvBuffer;
		iRecvBuffer = NULL;
		}

	if( iRecvBufferPtr )
		{
		delete iRecvBufferPtr;
		iRecvBufferPtr = NULL;
		}
}

/********************************************************************************
 *
 * CStatTransportBT -- MStatApiTransport
 *
 *******************************************************************************/
TInt CStatTransportBT::InitialiseL( MNotifyStatTransport *aTransport )
{
	// save the transport interface
	iTransport = aTransport;

	// everything here is done in connect
	iBTTransportStatus = EInitialised;
	iRecHandle = NULL;
	return KSTErrSuccess;
}

TInt CStatTransportBT::Release( void )
{
	// release has nothing to do (disconnecting unregister is in here in case the original connection failed)
	asserte( (iBTTransportStatus == EDisconnected) || (iBTTransportStatus == EInitialised) || (iBTTransportStatus == EDisconnectingUnregister) );
	iBTTransportStatus = EIdle;
	return KSTErrSuccess;
}

//Register with the Security Manager then goes on and does other stuff in the RunL
TInt CStatTransportBT::ConnectL( TDesC* /*aRemoteHost*/ )
{
	// make sure we are in the correct state
	asserte( iBTTransportStatus == EInitialised );


	// update the state
	iBTTransportStatus = EConnectingRegisterMgr;

	//	Force a call to RunL instead of waiting for a call back (CAcitve) after registering 
	//	with the security manager as in V1
	SetActive();
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,KErrNone);
	// tell the client to wait for an asynchronous response
	return KSTErrAsynchronous;
}
	
TInt CStatTransportBT::StartSocketL()
{
	TUint error;

	//make sure we are in the correct state
	asserte( iBTTransportStatus == EConnectingRegisterMgr );

	// connect to the socket server (as we are the receiver and not the initiator), create a socket, bind, listen, accept
	User::LeaveIfError( iSocketServ.Connect() );

	//now select the protocol to use (RFCOMM (serial emulation - boo.  Problems with demultiplexing if 1 generic serial port is used for multiple BT connections) or L2CAP)
	TProtocolDesc pInfo;	
	User::LeaveIfError( iSocketServ.FindProtocol(_L("RFCOMM"),pInfo ) ); 

	//open the listener socket
	User::LeaveIfError( iListenSocket.Open(iSocketServ, pInfo.iAddrFamily, pInfo.iSockType, pInfo.iProtocol) );


	//	RFComm Socket
	TRfcommSockAddr addr;
	
	//	Get First available server channel
	addr.SetPort(KRfcommPassiveAutoBind);

	//	Set the service security	
	//Set user defined EPOC TUid to internally represent the service
	iServiceSecurity.SetUid( TUid::Uid( 0x1234 ) );
	
	//Define security requirements
	iServiceSecurity.SetAuthentication( EFalse );
	iServiceSecurity.SetEncryption( EFalse ); 
	iServiceSecurity.SetAuthorisation( EFalse );
	addr.SetSecurity(iServiceSecurity);
	
	//bind
	User::LeaveIfError( iListenSocket.Bind( addr ) );

	//	Get the assigned port
	iPort=iListenSocket.LocalPort();
		
	// register with the SDP database
	error = RegWithSDPDatabaseL();
	
	if( error != KSTErrSuccess ) 
	{
		iTransport->HandleError( error, (void*)iStatus.Int() );
		return KSTErrGeneralFailure;
	}

	//listen
	User::LeaveIfError( iListenSocket.Listen( KLittleStatBTListenQueue ) );

	// create a blank socket which is used as the data socket
	User::LeaveIfError( iDataSocket.Open(iSocketServ) );

	// everything should now be set up, we just wait for a stat connection
	asserte( !IsActive() );
	iListenSocket.Accept( iDataSocket, iStatus );
	SetActive();
	return KSTErrSuccess;
}

//------------------------------------------------------------------------------

TInt CStatTransportBT::Disconnect( void )
{
	// must be connected 
	asserte( (iBTTransportStatus == EInitialised) || 
			(iBTTransportStatus == EConnected)   || 
			(iBTTransportStatus == EConnectingSockets)  ||
			(iBTTransportStatus == EDisconnectingData) ||
			(iBTTransportStatus == EDisconnectingListen) ||
            (iBTTransportStatus == EConnectingRegisterMgr) );

	// cancel any pending ops 
	Cancel();

	iBTTransportDisconnectStatusBeforeUnregister = iBTTransportStatus;
	iBTTransportStatus = EDisconnectingUnregister;
	
	//	Force call to RunL instead of waiting for a return from unregistring security service as in bluetooth V1
	SetActive();
	TRequestStatus* iDisconnectStatus=&iStatus;
	User::RequestComplete(iDisconnectStatus,KErrNone);
	
	return KSTErrAsynchronous;
}

TInt CStatTransportBT::RequestSend( TDesC8 *aData, const TUint aDataLength )
{
	// make sure the state is correct
	asserte( iBTTransportStatus == EConnected );
	asserte( iRWStatus == ENoRW );
	iRWStatus = EWritePending;

	// copy the data to members to local members
	if( aDataLength > 0 )
	{
		if( iWrCommandData && ( aDataLength != static_cast<TUint>(iWrCommandData->Length( )) ) )
		{
			delete iWrCommandData;
			iWrCommandData = NULL;
		}

		if( ! iWrCommandData )
		{
			iWrCommandData = HBufC8::New( aDataLength );

			if( ! iWrCommandData )
			{
				User::Leave(KErrNoMemory);
			}
		}
		
		TPtr8 dataPointer( iWrCommandData->Des() );
		dataPointer.Copy( aData->Ptr(), aDataLength );
	}

	// do the send
	asserte( !IsActive() );
	asserte( (unsigned)aData->Length() == aDataLength );
	iDataSocket.Write( *iWrCommandData, iStatus );
	SetActive();
	
	// tell the caller to wait for an asynchronous response
	return KSTErrAsynchronous;
}

TInt CStatTransportBT::RequestReceive( TUint aByteCount )
{
	// ensure that there are no reads in progress
	asserte( iBTTransportStatus == EConnected );
	asserte( !IsActive() );
	asserte( iRWStatus == ENoRW );
	iRWStatus = EReadPending;

	asserte( aByteCount <= static_cast<TUint>(iMaxPacketSize) );
	asserte( !IsActive() );
	
	if( iRecvBuffer && ( aByteCount != static_cast<TUint>(iRecvBuffer->Length( )) ) )
	{
		delete iRecvBuffer;
		iRecvBuffer = NULL;
		delete iRecvBufferPtr;
		iRecvBufferPtr = NULL;
	}

	if( ! iRecvBuffer )
	{
		iRecvBuffer = HBufC8::New( aByteCount );

		if( ! iRecvBuffer )
		{
			User::Leave(KErrNoMemory);
		}

		iRecvBufferPtr = new TPtr8(
					const_cast<unsigned char*>(iRecvBuffer->Ptr( )),
										aByteCount, aByteCount );

		if( ! iRecvBufferPtr )
		{
			User::Leave(KErrNoMemory);
		}

#ifdef _DEBUG
		// Check the size of the pointer is the same
		// as the size of the buffer.
		TInt length = iRecvBufferPtr->Length( );
		length = 0;	// Extra likne to remove compiler warning.
#endif // def _DEBUG
	}

	iDataSocket.Read( *iRecvBufferPtr, iStatus );
	SetActive();

	// return to the caller
	return KSTErrAsynchronous;
}

TText8 *CStatTransportBT::Error( void )
{
	return NULL;
}

TInt CStatTransportBT::GetPacketSize()
{
	// The packet size is configured when we initialise the port.
	return iMaxPacketSize;
}


//Register it in the SDP database - determine attribs, construct the record and then add attribs to new record
TInt CStatTransportBT::RegWithSDPDatabaseL( void )
{
	asserte( iBTTransportStatus == EConnectingRegisterMgr );

	//Connect and open to the session and the DB
	//User::LeaveIfError( iSdpSession.Connect() );

     TInt ret2;
	 ret2 = iSdpSession.Connect();

     if(ret2!=KErrNone)
     {
         User::Leave(ret2);
     }

	User::LeaveIfError( iSdpDatabaseSession.Open( iSdpSession ) );
	TBuf8<STANDARDBUFVALUE> value1;

	TBuf8<STANDARDBUFVALUE> value2;
	CSdpAttrValue* attrVal = 0;
	CSdpAttrValueDES* attrValDES = 0;

	//initialise
	value1.FillZ(STANDARDBUFVALUE);
	value2.FillZ(STANDARDBUFVALUE);

	// Set Attr 1 (service class list) to list with UUID = 0x1101 (serial port)
	iSdpDatabaseSession.CreateServiceRecordL(TUUID(0x1101), iRecHandle);

	// Set Service name
	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdBasePrimaryLanguage + 
												 KSdpAttrIdOffsetServiceName, 
												 _L("STATAPI") ); 

	// Set Service description
	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdBasePrimaryLanguage + 
												 KSdpAttrIdOffsetServiceDescription, 
												 _L("Symbian Test Automation Tool using Serial BT") );

	attrVal = CSdpAttrValueString::NewStringL( _L8( "Test Solutions Dept Symbian Ltd." ) );
	CleanupStack::PushL(attrVal);
	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdBasePrimaryLanguage + 
													 KSdpAttrIdOffsetProviderName, *attrVal);
	CleanupStack::PopAndDestroy(); //attrVal
	attrVal = 0;

	// Set Attr 2 (service record state) to 0
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdServiceRecordState, *attrVal);
	CleanupStack::PopAndDestroy(); //attrVal
	attrVal = 0;

	// Set attr 4 (protocol list) to RFCOMM
	//initialise
	TBuf8<1> serverChannel;
	serverChannel.FillZ(1);
	serverChannel[0] = (unsigned char)iPort;

	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);
	
	attrValDES->StartListL()
			->BuildDESL()->StartListL()
				->BuildUUIDL( TUUID( TUint16( 0x0003 ) ) ) // RFCOMM
				->BuildUintL( serverChannel )	//Channel ID = 3 (listening port)
			->EndListL()
	->EndListL();

	//update attr 4
	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdProtocolDescriptorList, *attrValDES);
	CleanupStack::PopAndDestroy(); //attrValDES
	attrValDES = 0;

	// Set Attr 5 (browse group list) to list with one UUID
	// 0x1101 (serial port class)
	// this should be updated with other service classes when other services are added.
	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);

	attrValDES->StartListL()
			->BuildUUIDL( TUUID( 0x1002 ) )
	->EndListL();
	
	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdBrowseGroupList, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;

	// Set Attr 0x006 (language base)
	value1.FillZ(4);
	value1[2] = 0x65;
	value1[3] = 0x6e;
	TBuf8<STANDARDBUFVALUE> val2;
	TBuf8<STANDARDBUFVALUE> val3;
	val2.FillZ(STANDARDBUFVALUE);
	val3.FillZ(STANDARDBUFVALUE);
	val2[3] = 0x6a;
	val3[2] = 0x01;

	attrValDES = CSdpAttrValueDES::NewDESL(0);
	CleanupStack::PushL(attrValDES);

	attrValDES->StartListL()
			->BuildUintL( value1 ) // speka de english
			->BuildUintL( val2 )   // UTF-8
			->BuildUintL( val3 )   // language base
	->EndListL();

	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdLanguageBaseAttributeIDList, *attrValDES);
	CleanupStack::PopAndDestroy();
	attrValDES = 0;

	// Set Attr 0x007 (time to live) to 600 (0x258) seconds (10 minutes)
	//initialise buffer
	value1.FillZ(4);
	value1[2]=2;
	value1[3]=0x58;

	attrVal = CSdpAttrValueUint::NewUintL( value1 );
	CleanupStack::PushL( attrVal );
	iSdpDatabaseSession.UpdateAttributeL( iRecHandle, KSdpAttrIdServiceInfoTimeToLive, *attrVal );
	CleanupStack::PopAndDestroy(); //attrVal 
	attrVal = 0;

	//Set Attr 0x08 (availability) to 0xff - fully available - not in use
	//initialise
	TBuf8<1> val4;
	val4.FillZ(1);
	val4[0]=0xff;
	
	attrVal = CSdpAttrValueUint::NewUintL(val4);
	CleanupStack::PushL(attrVal);
	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdServiceAvailability, *attrVal);
	CleanupStack::PopAndDestroy(); //attrVal 
	attrVal = 0;

	//Set Attr 0x201 (service database state) to 0
	//initialise
	value1.FillZ(4);
	
	attrVal = CSdpAttrValueUint::NewUintL(value1);
	CleanupStack::PushL(attrVal);
	iSdpDatabaseSession.UpdateAttributeL(iRecHandle, KSdpAttrIdSdpServerServiceDatabaseState, *attrVal);
	CleanupStack::PopAndDestroy(); //attrVal
	attrVal = 0;

	return KSTErrSuccess;
}

/********************************************************************************
 *
 * CStatTransportBT -- Active Object
 *
 *******************************************************************************/
void CStatTransportBT::RunL( void )
{
	TInt error = KErrNone;

	// if there was an error during connecting then tell the engine this
	if( (iBTTransportStatus == EConnectingSockets) && (iStatus != KErrNone) ) {

		iTransport->HandleError( KSTErrConnectFailure, (void*)iStatus.Int() );
		return;
	}

    // the other end have disconnected. just cleanup the resource by calling Disconnect function.
    if(iStatus == KErrDisconnected && iBTTransportStatus == EConnected) {

        Disconnect();
        return;
    }

	// if there was any other error then also tell the engine about it
	if( (iStatus != KErrNone) && NOTISDISCONNECTING(iBTTransportStatus) ) {

		iTransport->HandleError( KSTErrGeneralFailure, (void*)iStatus.Int() );
		return;
	}

	// Now we are registered with the security manager, reg with the SDP
	if( iBTTransportStatus == EConnectingRegisterMgr ) 
	{
		// start the socket and make it listen (async call)
		error = StartSocketL();
		if( error != KSTErrSuccess ) 
		{
			iTransport->HandleError( error, (void*)iStatus.Int() );
			return;
		}

		iBTTransportStatus = EConnectingSockets;
		return;
	}

	// We are now connected
	if( iBTTransportStatus == EConnectingSockets ) 
	{
		iBTTransportStatus = EConnected;
		iTransport->HandleConnect( KErrNone );
		return;
	}

	// handle unregister service
	if( iBTTransportStatus == EDisconnectingUnregister ) {
		HandleAsyncDisconnect();
		return;
	}
		
	// handle shutdown data socket
	if( iBTTransportStatus == EDisconnectingData ) {
		iDataSocket.Close();
		iListenSocket.Shutdown( RSocket::ENormal, iStatus );
		SetActive();
		iBTTransportStatus = EDisconnectingListen;
		return;
	}

	// handle shutdown listen
	if( iBTTransportStatus == EDisconnectingListen ) {
		iListenSocket.Close();
		iBTTransportStatus = EDisconnected;
		iSocketServ.Close();
		iTransport->HandleDisconnect( KErrNone );
		return;
	}

	// if we are writing then notify of the write
	if( iRWStatus == EWritePending ) {
		iRWStatus = ENoRW;
		asserte( iWrCommandData != NULL );
		iTransport->HandleSend( KErrNone );
		return;
	}

	// if we are reading then notify of the read
	if( iRWStatus == EReadPending ) {
		iRWStatus = ENoRW;
		TInt length = iRecvBufferPtr->Length( );
		iTransport->HandleReceive( KErrNone, iRecvBufferPtr, length );
		return;
	}
}

//------------------------------------------------------------------------------

void CStatTransportBT::DoCancel( void )
{
	// if we are connecting then cancel the connect() call
	if( iBTTransportStatus == EConnectingSockets )
		{
		iListenSocket.CancelAccept();
		}

	if( iRWStatus == EReadPending )
		{
		iDataSocket.CancelRead();
		}

	if( iRWStatus == EWritePending )
		{
		iDataSocket.CancelWrite();
		}

	iRWStatus = ENoRW;
}


//------------------------------------------------------------------------------
//
// Handle async disconnect is called by the RunL when we get a successful response 
// to the unregister service request. We can now shutdown the transport normally,
// but we have to know which state it had reached.
//
//------------------------------------------------------------------------------
void CStatTransportBT::HandleAsyncDisconnect( void )
{
	TInt tv = KErrNone;

	if(iRecHandle != NULL)
	{
		//Close the SDP database objects and delete the record
		TRAP( tv, iSdpDatabaseSession.DeleteRecordL( iRecHandle) );
		if( tv != KErrNone ) 
		{
			_LIT(KErrDelRecord,"An error occured while deleting the SDP record.");

			iTransport->HandleInfo(&KErrDelRecord);
		}
	}
	
	iSdpDatabaseSession.Close();
	iSdpSession.Close();
	iRecHandle = NULL;

	// clean up the sockets depending on the state
	switch( iBTTransportDisconnectStatusBeforeUnregister ) {

	case EConnected:
		iDataSocket.Shutdown( RSocket::ENormal, iStatus );
		SetActive();
		iBTTransportStatus = EDisconnectingData;
		break;

	case EConnecting:
	case EDisconnectingData:
		iDataSocket.Close();
		iListenSocket.Shutdown( RSocket::ENormal, iStatus );
		SetActive();
		iBTTransportStatus = EDisconnectingListen;
		break;

	case EInitialised:
	case EDisconnectingListen:
    case EConnectingRegisterMgr:
		// initialised may mean that ConnectL threw an error -- so close the resources
		iDataSocket.Close();
		iListenSocket.Close();
		iSocketServ.Close();
		iTransport->HandleDisconnect( KErrNone );
		break;

	default:
		;
	}
}

