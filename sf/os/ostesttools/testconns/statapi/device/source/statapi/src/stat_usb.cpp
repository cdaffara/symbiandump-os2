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
 * USB Packetisation transport for STAT -- for packet based network layers
 *
 *************************************************************************************/

/**************************************************************************************
 *
 * Local Includes
 *
 *************************************************************************************/
#include "assert.h"
#include "stat_usb.h"

/**************************************************************************************
 *
 * Definitions
 *
 *************************************************************************************/

#define KReEnumerationDelay		500000 //0.5 seconds

/********************************************************************************
 *
 * Macro functions
 *
 ********************************************************************************/
  
 _LIT(KLddName, "eusbc");
 
/**************************************************************************************
 *
 * CStatApiUsb - Construction
 *
 *************************************************************************************/
CStatApiUsb* CStatApiUsb::NewL()
{
    CStatApiUsb *self = new (ELeave) CStatApiUsb();
    CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
    return self;
}

CStatApiUsb::CStatApiUsb() : CActive(EPriorityStandard)
{
}

CStatApiUsb::~CStatApiUsb()
{
	
	// clean up params
	if( iBuffer != NULL )
		delete iBuffer;
}

void CStatApiUsb::ConstructL( void )
{
	
	iTimer.CreateLocal();
	
	// add to active scheduler
	CActiveScheduler::Add(this);
		
	// set parameters
	iBuffer = NULL;
	iLength = 0;
	iRWStatus = ENoRW;
	iUsbStatus = EIdle;
	iTransport = NULL;
	
	iConnectingState = EEnum;
	
	iMaxPacketSize = KMaxUSBPacketSize;
	
	iBuffer = HBufC8::New(iMaxPacketSize);
	
	if( !iBuffer ) {
		User::Leave(KErrNoMemory);
	}
	
}

/**************************************************************************************
 *
 * CStatApiUsb - MStatNetwork - Initialise and Release
 *
 *************************************************************************************/
TInt CStatApiUsb::InitialiseL( MNotifyStatTransport *aTransport )
{

	TInt r = KErrNone;

	// save the transport interface
	iTransport = aTransport;

	// set the state
	SetStatus( EInitialising );

	r = User::LoadLogicalDevice(KLddName);

	if( (r != KErrNone) && (r != KErrAlreadyExists) ) {
		User::Leave( r );
	}
	
	if(r == KErrAlreadyExists)
		r = KErrNone;
	
	SetStatus( EInitialised );
		
	return (r);
}




TInt CStatApiUsb::Release(void)
{

	// remove from active scheduler
	Deque();
	
	// close timer
	iTimer.Close();
	
	//close port
	iPort.Close();	
	
	// update state and finish
	SetStatus( EIdle );
	
	return KSTErrSuccess;
}



TInt CStatApiUsb::ConnectL(TDesC */*aRemoteHost*/)
{
	
	SetStatus( EConnecting );

	User::LeaveIfError( iPort.Open(0) );
	
	// Set the active interface

	TUsbDeviceCaps d_caps;
	User::LeaveIfError( iPort.DeviceCaps(d_caps) );
	
	TInt n = d_caps().iTotalEndpoints;
	
	TUsbcEndpointData data[KUsbcMaxEndpoints];
	TPtr8 dataptr(reinterpret_cast<TUint8*>(data), sizeof(data), sizeof(data));
	
	User::LeaveIfError( iPort.EndpointCaps(dataptr) );

	TUsbcInterfaceInfoBuf ifc;
	TInt ep_found = 0;
	TBool foundBulkIN = EFalse;
	TBool foundBulkOUT = EFalse;

	for (TInt i = 0; i < n; i++)
		{
		const TUsbcEndpointCaps* caps = &data[i].iCaps;
		const TInt mps = caps->MaxPacketSize();
		if (!foundBulkIN &&
			(caps->iTypesAndDir & (KUsbEpTypeBulk | KUsbEpDirIn)) == (KUsbEpTypeBulk | KUsbEpDirIn))
			{
			// EEndpoint1 is going to be our TX (IN, write) endpoint
			ifc().iEndpointData[0].iType = KUsbEpTypeBulk;
			ifc().iEndpointData[0].iDir	 = KUsbEpDirIn;
			ifc().iEndpointData[0].iSize =  mps;
			
			foundBulkIN = ETrue;
			if (++ep_found == 2)
				break;
			continue;
			}
		if (!foundBulkOUT &&
			(caps->iTypesAndDir & (KUsbEpTypeBulk | KUsbEpDirOut)) == (KUsbEpTypeBulk | KUsbEpDirOut))
			{
			// EEndpoint2 is going to be our RX (OUT, read) endpoint
			ifc().iEndpointData[1].iType = KUsbEpTypeBulk;
			ifc().iEndpointData[1].iDir	 = KUsbEpDirOut;
			ifc().iEndpointData[1].iSize = mps;
			
			foundBulkOUT = ETrue;
			if (++ep_found == 2)
				break;
			continue;
			}
		}
	
	//check enough IO endpoints	
	asserte( ep_found == 2 );
			
	_LIT16(ifcname, "STAT USB Test Interface");
	HBufC16* string = ifcname().AllocL();
    CleanupStack::PushL(string);

	ifc().iString = string;
	ifc().iTotalEndpointsUsed = 2;
	ifc().iClass.iClassNum	  = 0xff;
	ifc().iClass.iSubClassNum = 0xff;
	ifc().iClass.iProtocolNum = 0xff;

	User::LeaveIfError( iPort.SetInterface(0, ifc, EUsbcBandwidthINMaximum | EUsbcBandwidthOUTMaximum) );

    CleanupStack::PopAndDestroy();

	//force a call to RunL
	SetActive();
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,KErrNone);
	
	return KSTErrAsynchronous;

}


TInt CStatApiUsb::Disconnect(void)
{
	return KSTErrSuccess;
}

/**************************************************************************************
 *
 * CStatApiUsb - MStatNetwork - Receive and Send. The ID / Length / Data nonsense 
 * is handled by the packetisation layer. All Usb has to do here is send data -- 
 * Usb is also assuming that the upper layer will keep the data until the response
 *
 *************************************************************************************/
TInt CStatApiUsb::RequestSend( TDesC8 *aData, const TUint aDataLength )
{
	// make sure we are in the appropriate state 
	asserte( iUsbStatus == EConnected );
	asserte( iRWStatus == ENoRW );
	iRWStatus = EWritePending;

	// do the send
	asserte( !IsActive() );
	asserte( (unsigned)aData->Length() == aDataLength );
	
	iPort.Write(iStatus, EEndpoint1, *aData, aDataLength);
	SetActive();
	
	return KSTErrAsynchronous;
}

TInt CStatApiUsb::RequestReceive( TUint aByteCount )
{
	
	// make sure we are in the appropriate state
	asserte( iUsbStatus == EConnected );
	asserte( iRWStatus == ENoRW );
	asserte( !IsActive() );
	iRWStatus = EReadPending;

	// allocate a buffer for the read
	asserte( aByteCount <= static_cast<TUint>(iMaxPacketSize) );
	iLength = aByteCount;
	
	TPtr8 lPtr = iBuffer->Des();
	
	iPort.Read(iStatus, EEndpoint2 , lPtr , aByteCount );	
	SetActive();	

	return KSTErrAsynchronous;
}

TInt CStatApiUsb::GetPacketSize()
{
	// The packet size is configured when we initialise the port.
	return iMaxPacketSize;
}

TText8 *CStatApiUsb::Error( void )
{
	return NULL;
}

/**************************************************************************************
 *
 * CStatApiUsb - Active Object
 *
 *************************************************************************************/

 void CStatApiUsb::RunL( void )
{
	asserte( !IsActive() );
			
	if( iStatus == KErrCancel )
		return;
	
	switch( iStatus.Int() )
	{
		
		case KErrCancel:
		return;
		
		case KErrNone:
		break;
		
		// the USB device has been released, move to the EConnecting state
		
		// USB error
		 case KErrUsbInterfaceNotReady:
 		// port is released by the PC
 		 case KErrUsbDeviceNotConfigured:
		 iRWStatus = ENoRW;
		 SetStatus(EConnecting);
		 break;
		
		// in other cases, we have an error to report to the upper layers
		default: 
		iRWStatus = ENoRW;
		iTransport->HandleError( iStatus.Int(), NULL );
	}
	
	
	// Status change
	if(iUsbStatus == EConnecting)
		{
		
		iRWStatus = ENoRW;
				
 		switch(iConnectingState)
	 		{
		 	//ReEnumerate
		 	case EEnum:
			 	iPort.ReEnumerate(iStatus);
				SetActive();
			 	iConnectingState = EWait;
			 	break;
		 	
		 	//Wait enough to complete the reenumeration
		 	case EWait:
		 	 	iTimer.After(iStatus,KReEnumerationDelay);
			 	SetActive();
			 	iConnectingState = EConnect;
				break;
		 	
		 	case EConnect:
		 		iConnectingState = EEnum;
		 		
		 		//Set DMA
			 	iPort.AllocateEndpointResource(EEndpoint1, EUsbcEndpointResourceDMA);
				iPort.AllocateEndpointResource(EEndpoint2, EUsbcEndpointResourceDMA);
			 	iPort.QueryEndpointResourceUse(EEndpoint1, EUsbcEndpointResourceDMA);
			 	iPort.QueryEndpointResourceUse(EEndpoint2, EUsbcEndpointResourceDMA);
			
				//Set double buffering
				iPort.AllocateEndpointResource(EEndpoint1, EUsbcEndpointResourceDoubleBuffering);
			 	iPort.AllocateEndpointResource(EEndpoint2, EUsbcEndpointResourceDoubleBuffering);
				iPort.QueryEndpointResourceUse(EEndpoint1, EUsbcEndpointResourceDoubleBuffering);
				iPort.QueryEndpointResourceUse(EEndpoint2, EUsbcEndpointResourceDoubleBuffering);
		 		
				//change status to connected	 		
			 	SetStatus( EConnected );
				iTransport->HandleConnect( KErrNone );
			 	break;
		 	}
 		
		return;
		}
	
		
	// if we are writing then notify of the write
	if( iRWStatus == EWritePending ) 
		{
		iRWStatus = ENoRW;
		iTransport->HandleSend( KErrNone );
		return;
		}

	// if we are reading then notify of the read
	if( iRWStatus == EReadPending )
		{
		TInt length = iLength;
		iRWStatus = ENoRW;
		iLength = 0;
		
		TPtr8 lPtr = iBuffer->Des();
		
		iTransport->HandleReceive( KErrNone, &lPtr, length );
		return;
		}
	
}

void CStatApiUsb::DoCancel( void )
{
	
	iPort.ReEnumerateCancel();
	iPort.ReadCancel(EEndpoint2);
	iPort.WriteCancel(EEndpoint1);
	
}

/**************************************************************************************
 *
 * CStatApiUsb - Private Functions
 *
 *************************************************************************************/
void CStatApiUsb::SetStatus( TCommStatus aNewStatus )
{
	iUsbStatus = aNewStatus;
}


