/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>
#include <c32comm.h>
#include "CdcControlInterface.h"
#include "CdcControlInterfaceReader.h"
#include "ClassDescriptor.h"
#include "CdcAcmClass.h"
#include "AcmPanic.h"
#include "AcmUtils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CdcControlInterfaceTraces.h"
#endif


CCdcControlInterface::CCdcControlInterface(const TUint8 aProtocolNum, const TDesC16& aIfcName)
/**
 * Constructor using interface name.
 * @param aProtocolNum Contains the Table 17 protocol number.
 * @param aIfcName contains the interface name
 */
 :	CCdcInterfaceBase(aIfcName), 
	iSerialState(0xFFFF),
	iProtocolNum(aProtocolNum)
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACE_CCDCCONTROLINTERFACE_CONS_ENTRY );
	OstTraceFunctionExit0( CCDCCONTROLINTERFACE_CCDCCONTROLINTERFACE_CONS_EXIT );
	}

CCdcControlInterface* CCdcControlInterface::NewL(CCdcAcmClass& aParent, const TUint8 aProtocolNum, const TDesC16& aIfcName)
/**
 * Create a new CCDCCommClass object and construct it using interface name
 * This call will return an object with a valid USB configuration
 *
 * @param aParent Pointer to the Port using this object
 * @param aProtocolNum contains the Table 17 protocol number.
 * @param aIfcName Contains the interface name
 * @return A pointer to the new object
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACE_NEWL_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCCONTROLINTERFACE_NEWL, "CCdcControlInterface::NewL;\tControl Ifc Name = %S", aIfcName );

	CCdcControlInterface* self = new(ELeave) CCdcControlInterface(aProtocolNum, aIfcName);
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	CLEANUPSTACK_POP(self);
	OstTraceFunctionExit0( CCDCCONTROLINTERFACE_NEWL_EXIT );
	return self;
	}

void CCdcControlInterface::ConstructL(CCdcAcmClass& aParent)
/**
 * 2nd-phase construction.
 * This call registers the object with the USB device driver
 *
 * @param aParent The ACM class.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACE_CONSTRUCTL_ENTRY );
	
	BaseConstructL();

	iReader  = CCdcControlInterfaceReader::NewL(aParent, iLdd);
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_CONSTRUCTL, "CCdcControlInterface::ConstructL;iProtocolNum=%d", (TInt)iProtocolNum );
	OstTraceFunctionExit0( CCDCCONTROLINTERFACE_CONSTRUCTL_EXIT );
	}

TInt CCdcControlInterface::SetUpInterface()
/**
 * Set up the interface for use. This involves finding a "Interrupt IN" 
 * endpoint and, if found, configuring the interface.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACE_SETUPINTERFACE_ENTRY );

	TUsbDeviceCaps dCaps;
	TInt ret = iLdd.DeviceCaps(dCaps);
	OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE, "CCdcControlInterface::SetUpInterface;\tchecking result of DeviceCaps" );
	if ( ret )
		{
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP1, "CCdcControlInterface::SetUpInterface;ret=%d", ret );
		OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SETUPINTERFACE_EXIT );
		return ret;
		}

	const TUint KRequiredNumberOfEndpoints = 1; // in addition to endpoint 0.

	const TUint totalEndpoints = static_cast<TUint>(dCaps().iTotalEndpoints);
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP2, 
			"CCdcControlInterface::SetUpInterface;totalEndpoints=%d", (TInt)totalEndpoints );

	if ( totalEndpoints < KRequiredNumberOfEndpoints )
		{
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP3, 
				"CCdcControlInterface::SetUpInterface;ret=%d", KErrGeneral );
		OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SETUPINTERFACE_EXIT_DUP1 );
		return KErrGeneral;
		}
	
	// Endpoints
	TUsbcEndpointData data[KUsbcMaxEndpoints];
	TPtr8 dataptr(reinterpret_cast<TUint8*>(data), sizeof(data), sizeof(data));
	ret = iLdd.EndpointCaps(dataptr);
	OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP4, 
			"CCdcControlInterface::SetUpInterface;\tchecking result of EndpointCaps" );

	if ( ret )
		{
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP5, "CCdcControlInterface::SetUpInterface;ret=%d", ret );
		OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SETUPINTERFACE_EXIT_DUP2 );
		return ret;
		}

	// Set the active interface
	TUsbcInterfaceInfoBuf ifc;
	TBool epFound = EFalse;
	for ( TUint i = 0 ; i < totalEndpoints ; i++ )
		{
		const TUsbcEndpointCaps* caps = &data[i].iCaps;
		if (!caps)
			{
			OstTraceExt1( TRACE_FATAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP6, "CCdcControlInterface::SetUpInterface;caps=%p", caps );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}

		if (data[i].iInUse)
			{
			continue;
			}

		if ((caps->iTypesAndDir & (KUsbEpTypeInterrupt | KUsbEpDirIn)) == 
			(KUsbEpTypeInterrupt | KUsbEpDirIn))
			{
			// EEndpoint1 is interrupt endpoint
			ifc().iEndpointData[0].iType  = KUsbEpTypeInterrupt;
			ifc().iEndpointData[0].iDir   = KUsbEpDirIn; 

			//get the max packet size it can potentially support
			//it's possible that it can support Isoch (1023) which is greater
			//than max for Int at 64
			TInt maxSize = Min(caps->MaxPacketSize(), KMaxPacketTypeInterrupt);
			
			ifc().iEndpointData[0].iSize  = maxSize;

			ifc().iEndpointData[0].iInterval = KPollInterval; 
			epFound = ETrue;
			break;
			}
		}
	
	
	OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP8, "CCdcControlInterface::SetUpInterface;\tchecking epFound" );
	if ( !epFound )
		{
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP9, "CCdcControlInterface::SetUpInterface;ret=%d", KErrGeneral );
		OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SETUPINTERFACE_EXIT_DUP3 );
		return KErrGeneral;
		}

	ifc().iString = &iIfcName;
	ifc().iTotalEndpointsUsed = KRequiredNumberOfEndpoints;
	// Codes taken from USBCDC 1.1.
	ifc().iClass.iClassNum	  = 0x02; // Table 15- Communication Interface Class
	ifc().iClass.iSubClassNum = 0x02; // Table 16- Abstract Control Model
	ifc().iClass.iProtocolNum = iProtocolNum; // Table 17

	OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP10, 
			"CCdcControlInterface::SetUpInterface;\tabout to call SetInterface" );
	// Zero effectively indicates that alternate interfaces are not used.
	ret = iLdd.SetInterface(0, ifc);
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPINTERFACE_DUP11, "CCdcControlInterface::SetUpInterface;ret=%d", ret );
	OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SETUPINTERFACE_EXIT_DUP4 );
	return ret;
	}

TInt CCdcControlInterface::SetupClassSpecificDescriptor(
							TUint8 aDataInterfaceNumber)
/**
 * Setup the Class Descriptors
 *
 * @param aDataInterfaceNumber The interface number of the data class
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACE_SETUPCLASSSPECIFICDESCRIPTOR_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPCLASSSPECIFICDESCRIPTOR, 
			"CCdcControlInterface::SetupClassSpecificDescriptor;aDataInterfaceNumber=%d", (TInt)aDataInterfaceNumber );

	TInt res;

	TUsbCsClassDescriptor descriptor;

	// Header Functional Descriptor- table 26
	descriptor.iHdrSize 		 = 0x05; // bFunctionLength
	descriptor.iHdrType 		 = 0x24; // bDescriptorType- CS_INTERFACE
	descriptor.iHdrSubType		 = 0x00; // Table 25- Header FD
	descriptor.iHdrBcdCDC		 = 0x0110; // release number

	// Abstract Control Management Functional Descriptor- table 28
	descriptor.iAcmSize 		 = 0x04; // bFunctionLength
	descriptor.iAcmType 		 = 0x24; // bDescriptorType- CS_INTERFACE
	descriptor.iAcmSubType		 = 0x02; // Table 25- ACM FD
	descriptor.iAcmCapabilities  = 0x0f; // capabilities- all

	// Union functional descriptor- table 33
	descriptor.iUnSize			 = 0x05; // bFunctionLength
	descriptor.iUnType			 = 0x24; // bDescriptorType- CS_INTERFACE
	descriptor.iUnSubType		 = 0x06; // Table 25- Union FD
	// Set the control interface as the master...
	res = GetInterfaceNumber(descriptor.iUnMasterInterface);
	// ... and the data interface as the slave.
	descriptor.iUnSlaveInterface = aDataInterfaceNumber;

#if defined(DISABLE_ACM_CF_COUNTRY_SETTING)

	// no functional descriptor needed

#elif defined(ENABLE_ACM_CF_COUNTRY_SETTING)

	// CDC Country Selection Functional Descriptor
	descriptor.iCsSize			 = 0x04 + (0x02*KUsbCommNumCountries); // bFunctionLength
	descriptor.iCsType			 = 0x24; // bDescriptorType- CS_INTERFACE
	descriptor.iCsSubType		 = 0x07; // Table 25- Country Selection FD
	descriptor.iCsRelDate		 = 0x07; // Release date of ISO3166 country codes.
	descriptor.iCsCountryCode[0] = KUsbCommCountryCode0; // Country cide

#endif

	if ( res )
		{
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPCLASSSPECIFICDESCRIPTOR_DUP1, 
				"CCdcControlInterface::SetupClassSpecificDescriptor;\t***GetInterfaceNumber=%d", res );
		OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SETUPCLASSSPECIFICDESCRIPTOR_EXIT );
		return res;
		}

	OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPCLASSSPECIFICDESCRIPTOR_DUP2, 
			"CCdcControlInterface::SetupClassSpecificDescriptor;\tabout to call SetCSInterfaceDescriptorBlock" );
	
	res = iLdd.SetCSInterfaceDescriptorBlock(0, descriptor.Des());
	if ( res )
		{
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SETUPCLASSSPECIFICDESCRIPTOR_DUP3, 
				"CCdcControlInterface::SetupClassSpecificDescriptor;\t***SetCSInterfaceDescriptorBlock=%d", res );
		
		OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SETUPCLASSSPECIFICDESCRIPTOR_EXIT_DUP1 );
		return res;
		}

	OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SETUPCLASSSPECIFICDESCRIPTOR_EXIT_DUP2 );
	return KErrNone;
	}

CCdcControlInterface::~CCdcControlInterface()
/**
 * Destructor
 */
	{
	OstTraceFunctionEntry0( DUP1_CCDCCONTROLINTERFACE_CCDCCONTROLINTERFACE_ENTRY );
	delete iReader;
	OstTraceFunctionExit0( DUP1_CCDCCONTROLINTERFACE_CCDCCONTROLINTERFACE_EXIT );
	}

/**
 * Special data object used to send a NETWORK_CONNECTION notification
 * up to the (USB) Host, using whatever size is available on the
 * control endpoint (derived in the driver below)
 */
const TUint KUSBNotificationNetworkConnectionSize = 8;

NONSHARABLE_CLASS(TUSBNotificationNetworkConnection)
	{
public:
	TUint8	bmRequestType;	///< Request type
	TUint8	bNotification;	///< Notification number
	TUint16 wValue; 		///< Notification value
	TUint16 wIndex; 		///< Notification index
	TUint16 wLength;		///< Notification length
public:
	TDes8&	PackBuffer();

private:
	TBuf8<KUSBNotificationNetworkConnectionSize> iBuffer;
	};

TDes8& TUSBNotificationNetworkConnection::PackBuffer()
/**
 * This function packs the TUSBNotificationSerialState class into a 
 * byte buffer with the correct byte alignment for transmission on 
 * the little-endian USB bus.
 */
	{
	OstTraceFunctionEntry0( TUSBNOTIFICATIONNETWORKCONNECTION_PACKBUFFER_ENTRY );
	
	iBuffer.SetLength(KUSBNotificationNetworkConnectionSize);

	iBuffer[0] = bmRequestType;
	iBuffer[1] = bNotification;
	iBuffer[2] = static_cast<TUint8>( wValue	& 0x00ff);
	iBuffer[3] = static_cast<TUint8>((wValue	& 0xff00) >> 8);
	iBuffer[4] = static_cast<TUint8>( wIndex	& 0x00ff);
	iBuffer[5] = static_cast<TUint8>((wIndex	& 0xff00) >> 8);
	iBuffer[6] = static_cast<TUint8>( wLength & 0x00ff);
	iBuffer[7] = static_cast<TUint8>((wLength & 0xff00) >> 8);

	OstTraceFunctionExit0( TUSBNOTIFICATIONNETWORKCONNECTION_PACKBUFFER_EXIT );
	return iBuffer;
	}

TInt CCdcControlInterface::SendNetworkConnection(TBool aValue)
/**
 * Sends a Network Connection message to the host.
 * Note that this function has not been tested. It is included for 
 * completeness as it may need to be used in modem (DCE) mode. However, it is 
 * unclear how a C32 client would indicate to the CSY that a network 
 * connection had been established.
 *
 * @param aValue	ETrue if Network Connected
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACE_SENDNETWORKCONNECTION_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SENDNETWORKCONNECTION, 
			"CCdcControlInterface::SendNetworkConnection;aValue=%d", (TInt)aValue );

	// form the message and prime it down to the interrupt handler
	// (that is 'interrupt' in the USB sense)

	// Note that this does not need to be aware of endian-ness, this
	// is taken care of in the PackBuffer() function.
	TUSBNotificationNetworkConnection notification;

	notification.bmRequestType = 0xA1;
	notification.bNotification = 0x00; // NETWORK_CONNECTION
	notification.wValue 	   = static_cast<TUint16>((aValue)?0x0001:0x0000); //1 - connected, 0 - disconnected
	notification.wIndex 	   = static_cast<TUint16>((aValue)?0x0001:0x0000);
	notification.wLength       = 0x00;

	TInt ret = WriteData(EEndpoint1, 
		notification.PackBuffer(), 
		notification.PackBuffer().Length());

	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SENDNETWORKCONNECTION_DUP1, 
			"CCdcControlInterface::SendNetworkConnection;ret=%d", ret );
	OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SENDNETWORKCONNECTION_EXIT );
	return ret;
	}

/**
 * Special data object used to send a SERIAL_STATE notification
 * up to the (USB) Host, using whatever size is available on the
 * control endpoint (derived in the driver below)
 */
const TUint KUSBNotificationSerialStateSize = 10;

NONSHARABLE_CLASS(TUSBNotificationSerialState)
	{
public:
	TUint8	bmRequestType;	///< Request type
	TUint8	bNotification;	///< Notification number
	TUint16 wValue; 		///< Notification value
	TUint16 wIndex; 		///< Notification index
	TUint16 wLength;		///< Notification length
	TUint16 wData;			///< 2-byte data payload
public:
	TDes8&	PackBuffer();

private:
	TBuf8<KUSBNotificationSerialStateSize> iBuffer;
	};

TDes8& TUSBNotificationSerialState::PackBuffer()
/**
 * This function packs the TUSBNotificationSerialState class into a 
 * byte buffer with the correct byte alignment for transmission on 
 * the little-endian USB bus.
 */
	{
	OstTraceFunctionEntry0( TUSBNOTIFICATIONSERIALSTATE_PACKBUFFER_ENTRY );
	
	iBuffer.SetLength(KUSBNotificationSerialStateSize);

	iBuffer[0] = bmRequestType;
	iBuffer[1] = bNotification;
	iBuffer[2] = static_cast<TUint8>( wValue	& 0x00ff);
	iBuffer[3] = static_cast<TUint8>((wValue	& 0xff00) >> 8);
	iBuffer[4] = static_cast<TUint8>( wIndex	& 0x00ff);
	iBuffer[5] = static_cast<TUint8>((wIndex	& 0xff00) >> 8);
	iBuffer[6] = static_cast<TUint8>( wLength & 0x00ff);
	iBuffer[7] = static_cast<TUint8>((wLength & 0xff00) >> 8);
	iBuffer[8] = static_cast<TUint8>( wData & 0x00ff);
	iBuffer[9] = static_cast<TUint8>((wData & 0xff00) >> 8);

	OstTraceFunctionExit0( TUSBNOTIFICATIONSERIALSTATE_PACKBUFFER_EXIT );
	return iBuffer;
	}

TInt CCdcControlInterface::SendSerialState(TBool aOverRun, 
									 TBool aParity, 
									 TBool aFraming, 
									 TBool aRing,
									 TBool aBreak, 
									 TBool aTxCarrier, 
									 TBool aRxCarrier)
/**
 * Sends a Serial State message to the host
 * 
 * @param aOverRun True if data discarded due to overrun
 * @param aParity True if a parity error has occured
 * @param aFraming True if a framing error has occured
 * @param aRing True if the device is Ringing
 * @param aBreak True if the device is detecting a break condition
 * @param aTxCarrier True if the transmit carrier is present
 * @param aRxCarrier True if the receive carrier is present
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACE_SENDSERIALSTATE_ENTRY );
	
	OstTrace1( TRACE_DUMP, CCDCCONTROLINTERFACE_SENDSERIALSTATE, 
			"CCdcControlInterface::SendSerialState;aOverRun=%d", (TInt)aOverRun );
	OstTrace1( TRACE_DUMP, CCDCCONTROLINTERFACE_SENDSERIALSTATE_DUP1, 
			"CCdcControlInterface::SendSerialState;aParity=%d", (TInt)aParity );
	OstTrace1( TRACE_DUMP, CCDCCONTROLINTERFACE_SENDSERIALSTATE_DUP2, 
			"CCdcControlInterface::SendSerialState;aFraming=%d", (TInt)aFraming );
	OstTrace1( TRACE_DUMP, CCDCCONTROLINTERFACE_SENDSERIALSTATE_DUP3, 
			"CCdcControlInterface::SendSerialState;aRing=%d", (TInt)aRing );
	OstTrace1( TRACE_DUMP, CCDCCONTROLINTERFACE_SENDSERIALSTATE_DUP4, 
			"CCdcControlInterface::SendSerialState;aBreak=%d", (TInt)aBreak );
	OstTrace1( TRACE_DUMP, CCDCCONTROLINTERFACE_SENDSERIALSTATE_DUP5, 
			"CCdcControlInterface::SendSerialState;aTxCarrier=%d", (TInt)aTxCarrier );
	OstTrace1( TRACE_DUMP, CCDCCONTROLINTERFACE_SENDSERIALSTATE_DUP6, 
			"CCdcControlInterface::SendSerialState;aRxCarrier=%d", (TInt)aRxCarrier );

	// First work out what might need to be sent by assembling the bits into 
	// the correct places. See CDC spec table 69 (UART state bitmap values).
	TUint16 data = static_cast<TUint16>
					(
						(aRxCarrier 	) |
						(aTxCarrier << 1) |
						(aBreak 	<< 2) |
						(aRing		<< 3) |
						(aFraming	<< 4) |
						(aParity	<< 5) |
						(aOverRun	<< 6)
					);

	// now check to see if this has created a different state than
	// last time it was sent, if it is the same, don't bother to
	// send it off.
	if ( data == iSerialState )
		{
		OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_SENDSERIALSTATE_DUP7, 
				"CCdcControlInterface::SendSerialState;\tdata == iSerialState" );
		OstTraceFunctionExit0( CCDCCONTROLINTERFACE_SENDSERIALSTATE_EXIT );
		return KErrNone;
		}

	// state is different, store local to the class object ready for
	// testing next time through
	iSerialState = data;

	// now form the message and prime it down to the interrupt handler
	// (that is 'interrupt' in the USB sense)

	// Note that this does not need to be aware of endian-ness, this
	// is taken care of in the PackBuffer() function.
	TUSBNotificationSerialState notification;

	notification.bmRequestType = 0xA1;
	notification.bNotification = 0x20; // SERIAL_STATE
	notification.wValue 	   = 0x0000;
	notification.wIndex 	   = 0x0000;
	notification.wLength	   = 0x0002;
	notification.wData		   = data;

	TInt ret = WriteData(	EEndpoint1, 
							notification.PackBuffer(), 
							notification.PackBuffer().Length());
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_SENDSERIALSTATE_DUP8, "CCdcControlInterface::SendSerialState;ret=%d", ret );
	OstTraceFunctionExit0( DUP1_CCDCCONTROLINTERFACE_SENDSERIALSTATE_EXIT );
	return ret;
	}

TInt CCdcControlInterface::WriteData(TEndpointNumber aEndPoint, 
							   TDes8& aDes, 
							   TInt aLength)
/**
 *
 *
 * @param aEndPoint
 * @param aDes
 * @param aLength
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACE_WRITEDATA_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_WRITEDATA, "CCdcControlInterface::WriteData;aEndPoint=%d", (TInt)aEndPoint );

	TInt ret;
	RTimer timer;
	ret = timer.CreateLocal();
	if ( ret )
		{
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_WRITEDATA_DUP1, 
				"CCdcControlInterface::WriteData;\ttimer.CreateLocal = %d- returning", ret );
		
		OstTraceFunctionExit0( CCDCCONTROLINTERFACE_WRITEDATA_EXIT );
		return ret;
		}
	TRequestStatus status;
	TRequestStatus timerStatus;
	OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_WRITEDATA_DUP2, 
			"CCdcControlInterface::WriteData;\tAttempting to write data to control interface" );

	iLdd.Write(status, aEndPoint, aDes, aLength);
	timer.After(timerStatus, KWriteDataTimeout);
	User::WaitForRequest(status, timerStatus);
	if ( timerStatus != KRequestPending )
		{
		// Timeout occurred, silently ignore error condition.
		// Assuming that the line has been disconnected
		OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_WRITEDATA_DUP3, 
				"CCdcControlInterface::WriteData;CCdcControlInterface::WriteData() - Timeout occurred" );
		
		iLdd.WriteCancel(aEndPoint);
		User::WaitForRequest(status);
		ret = timerStatus.Int();
		}
	else
		{
		OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACE_WRITEDATA_DUP4, 
				"CCdcControlInterface::WriteData;CCdcControlInterface::WriteData() - Write completed" );
		
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		ret = status.Int();
		}

	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACE_WRITEDATA_DUP5, "CCdcControlInterface::WriteData;ret=%d", ret );
	OstTraceFunctionExit0( CCDCCONTROLINTERFACE_WRITEDATA_EXIT_DUP1 );
	return ret;
	}

//
// End of file
