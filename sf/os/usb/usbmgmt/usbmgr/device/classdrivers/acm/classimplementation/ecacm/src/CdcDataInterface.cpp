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
#include "CdcDataInterface.h"
#include "ActiveReader.h"
#include "ActiveWriter.h"
#include "AcmPanic.h"
#include "AcmUtils.h"
#include "ActiveReadOneOrMoreReader.h"
#include "ActiveDataAvailableNotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CdcDataInterfaceTraces.h"
#endif


#ifdef __HEADLESS_ACM_TEST_CODE__
#pragma message ("Building headless ACM (performance test code for RDevUsbcClient)")
#endif // __HEADLESS_ACM_TEST_CODE__

CCdcDataInterface::CCdcDataInterface(const TDesC16& aIfcName)
/**
 * Overloaded Constructor using interface name.
 * @param aIfcName contains the interface name
 */
 :	CCdcInterfaceBase(aIfcName),
 	iPacketSize(KDefaultMaxPacketTypeBulk)
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_CCDCDATAINTERFACE_CONS_ENTRY );
	OstTraceFunctionExit0( CCDCDATAINTERFACE_CCDCDATAINTERFACE_CONS_EXIT );
	}

CCdcDataInterface* CCdcDataInterface::NewL(const TDesC16& aIfcName)
/**
 * Create a new CCdcDataInterface object and construct it using interface name
 * This call will return an object with a valid USB configuration
 *
 * @param aParent Observer.
 * @param aIfcName Contains the interface name
 * @return A pointer to the new object
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_NEWL_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCDATAINTERFACE_NEWL, "CCdcDataInterface::NewL;\tData Ifc Name = %S", aIfcName );
	CCdcDataInterface* self = new (ELeave) CCdcDataInterface(aIfcName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP(self);
	OstTraceFunctionExit0( CCDCDATAINTERFACE_NEWL_EXIT );
	return self;
	}

void CCdcDataInterface::ConstructL()
/**
 * Construct the object
 * This call registers the object with the USB device driver
 *
 * @param aParent Observer.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_CONSTRUCTL_ENTRY );
	
	BaseConstructL();
	iReadOneOrMoreReader  = CActiveReadOneOrMoreReader::NewL(*this, iLdd, EEndpoint2);
	iReader = CActiveReader::NewL(*this, iLdd, EEndpoint2);
	iDataAvailableNotifier = CActiveDataAvailableNotifier::NewL(*this, iLdd, EEndpoint2);
	iWriter = CActiveWriter::NewL(*this, iLdd, EEndpoint1);
	iLinkState = CLinkStateNotifier::NewL(*this, iLdd);
	
	iLinkState->Start();

	iHostCanHandleZLPs = (KUsbAcmHostCanHandleZLPs != 0);
	
	OstTraceFunctionExit0( CCDCDATAINTERFACE_CONSTRUCTL_EXIT );
	}

TInt CCdcDataInterface::SetUpInterface()
/**
 * Retrieves the device capabilities and searches for suitable input and 
 * output bulk endpoints. If suitable endpoints are found, an interface 
 * descriptor for the endpoints is registered with the LDD.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_SETUPINTERFACE_ENTRY );

	TUsbDeviceCaps dCaps;
	TInt ret = iLdd.DeviceCaps(dCaps);
	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE, "CCdcDataInterface::SetUpInterface;\tchecking result of DeviceCaps" );
	if ( ret )
		{
		OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP1, "CCdcDataInterface::SetUpInterface;ret=%d", ret );
		OstTraceFunctionExit0( CCDCDATAINTERFACE_SETUPINTERFACE_EXIT );
		return ret;
		}

	TInt maxBulkPacketSize = (dCaps().iHighSpeed) ? KMaxPacketTypeBulkHS : KMaxPacketTypeBulkFS;
	const TUint KRequiredNumberOfEndpoints = 2;

	const TUint totalEndpoints = static_cast<TUint>(dCaps().iTotalEndpoints);
	OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP2, 
			"CCdcDataInterface::SetUpInterface;totalEndpoints=%d", (TInt)totalEndpoints );
	
	if ( totalEndpoints < KRequiredNumberOfEndpoints )
		{
		OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP3, "CCdcDataInterface::SetUpInterface;ret=%d", KErrGeneral );
		OstTraceFunctionExit0( CCDCDATAINTERFACE_SETUPINTERFACE_EXIT_DUP1 );
		return KErrGeneral;
		}
	
	// Endpoints
	TUsbcEndpointData data[KUsbcMaxEndpoints];
	TPtr8 dataptr(reinterpret_cast<TUint8*>(data), sizeof(data), sizeof(data));
	ret = iLdd.EndpointCaps(dataptr);
	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP9, "CCdcDataInterface::SetUpInterface;\tchecking result of EndpointCaps" );
	if ( ret )
		{
		OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP4, "CCdcDataInterface::SetUpInterface;ret=%d", ret );
		OstTraceFunctionExit0( CCDCDATAINTERFACE_SETUPINTERFACE_EXIT_DUP2 );
		return ret;
		}

	// 
	TUsbcInterfaceInfoBuf ifc;
	TBool foundIn = EFalse;
	TBool foundOut = EFalse;
	
	for ( TUint i = 0; !(foundIn && foundOut) && i < totalEndpoints; i++ )
		{
		const TUsbcEndpointCaps* caps = &data[i].iCaps;
		if (!caps)
			{
			OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP5, "CCdcDataInterface::SetUpInterface;caps=%p", caps );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		if (data[i].iInUse)
			continue;

		//get the max packet size it can potentially support
		//it's possible that it can support Isoch (1023) which is greater
		//than max for Bulk at 64
		const TInt maxSize = Min (	maxBulkPacketSize, 
									caps->MaxPacketSize() );

		const TUint KBulkInFlags = KUsbEpTypeBulk | KUsbEpDirIn;
		const TUint KBulkOutFlags = KUsbEpTypeBulk | KUsbEpDirOut;

		// use EEndPoint1 for TX (IN) EEndpoint1
		if (! foundIn && (caps->iTypesAndDir & KBulkInFlags) == KBulkInFlags)
			{

			ifc().iEndpointData[0].iType  = KUsbEpTypeBulk;
			ifc().iEndpointData[0].iDir   = KUsbEpDirIn; 

			//get the max packet size it can potentially support
			//it's possible that it can support Isoch (1023) which is greater
			//than max for Bulk at 64
			ifc().iEndpointData[0].iSize  = maxSize;
			foundIn = ETrue;
			}
		// use EEndPoint2 for RX (OUT) endpoint
		else if (	!foundOut 
			&&		(caps->iTypesAndDir & KBulkOutFlags) == KBulkOutFlags
			)
			{
			// EEndpoint2 is going to be our RX (OUT, read) endpoint
			ifc().iEndpointData[1].iType  = KUsbEpTypeBulk;
			ifc().iEndpointData[1].iDir   = KUsbEpDirOut;

			//get the max packet size it can potentially support
			//it's possible that it can support Isoch (1023) which is greater
			//than max for Bulk at 64
			ifc().iEndpointData[1].iSize  = maxSize;
			foundOut = ETrue;
			}
		}

	if (! (foundIn && foundOut))
		{
		OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP6, "CCdcDataInterface::SetUpInterface;ret=%d", KErrGeneral );
		OstTraceFunctionExit0( CCDCDATAINTERFACE_SETUPINTERFACE_EXIT_DUP3 );
		return KErrGeneral;
		}

		// If the device supports USB High-speed, then we request 64KB buffers
		// (otherwise the default 4KB ones will do).

	TUint bandwidthPriority = (EUsbcBandwidthOUTDefault | EUsbcBandwidthINDefault);
	if (dCaps().iHighSpeed)
		{
		bandwidthPriority = (EUsbcBandwidthOUTPlus2 | EUsbcBandwidthINPlus2);
		}

	ifc().iString = &iIfcName;
	ifc().iTotalEndpointsUsed = KRequiredNumberOfEndpoints;
	ifc().iClass.iClassNum	  = 0x0A; // Table 18- Data Interface Class
	ifc().iClass.iSubClassNum = 0x00; // Section 4.6- unused.
	ifc().iClass.iProtocolNum = 0x00; // Table 19- no class specific protocol required

	// Indicate that this interface does not expect any control transfers 
	// from EP0.
	ifc().iFeatureWord |= KUsbcInterfaceInfo_NoEp0RequestsPlease;

	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP7, "CCdcDataInterface::SetUpInterface;\tcalling SetInterface" );
	// Zero effectively indicates that alternate interfaces are not used.
	ret = iLdd.SetInterface(0, ifc, bandwidthPriority);

	OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_SETUPINTERFACE_DUP8, "CCdcDataInterface::SetUpInterface;ret=%d", ret );
	OstTraceFunctionExit0( CCDCDATAINTERFACE_SETUPINTERFACE_EXIT_DUP4 );
	return ret;
	}


void CCdcDataInterface::MLSOStateChange(TInt aPacketSize)
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_MLSOSTATECHANGE_ENTRY );	
	iPacketSize = aPacketSize;
	OstTraceFunctionExit0( CCDCDATAINTERFACE_MLSOSTATECHANGE_EXIT );
	}


CCdcDataInterface::~CCdcDataInterface()
/**
 * Destructor. Cancel and destroy the child classes.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_CCDCDATAINTERFACE_DES_ENTRY );
	delete iLinkState;
	delete iReadOneOrMoreReader;
	delete iReader;
	delete iWriter;
	delete iDataAvailableNotifier;
	OstTraceFunctionExit0( CCDCDATAINTERFACE_CCDCDATAINTERFACE_DES_EXIT );
	}

void CCdcDataInterface::Write(MWriteObserver& aObserver, 
						  const TDesC8& aDes, 
						  TInt aLen)
/**
 * Write data down the interface
 * 
 * @param aObserver The observer to notify of completion.
 * @param aDes Descriptor containing the data to be sent
 * @param aLen Length of the data to be sent
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_WRITE_ENTRY );
	if (iWriteObserver)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_WRITE, "CCdcDataInterface::Write;iWriteObserver=%p", iWriteObserver );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iWriteObserver = &aObserver;
	if (!iWriter)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_WRITE_DUP1, "CCdcDataInterface::Write;iWriter=%p", iWriter );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	
	if ( iHostCanHandleZLPs )
		{
		
		// Make sure the driver sends a Zero Length Packet if required (i.e. if the 
		// data size is an exact multiple of the endpoint's packet size). 
		TBool requestZlp = ( aLen % iPacketSize ) ? EFalse : ETrue;
		iWriter->Write(aDes, aLen, requestZlp); 
		}
	else
		{
		iWriter->Write(aDes, aLen, EFalse); 
		}
		
	OstTraceFunctionExit0( CCDCDATAINTERFACE_WRITE_EXIT );
	}

void CCdcDataInterface::WriteCompleted(TInt aError)
/**
 * Called when a write request completes.
 *
 * @param aError Error.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_WRITECOMPLETED_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_WRITECOMPLETED, "CCdcDataInterface::WriteCompleted;aError=%d", aError );

#ifdef __HEADLESS_ACM_TEST_CODE__
	// Issue another Read or ReadOneOrMore as appropriate.
	// If the Write completed with an error, we panic, as it's invalidating 
	// the test.
	if (aError != KErrNone)
		{
		OstTrace1( TRACE_FATAL, CCDCDATAINTERFACE_WRITECOMPLETED_DUP1, "CCdcDataInterface::WriteCompleted;aError=%d", aError );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	switch ( iHeadlessReadType )
		{
	case ERead:
		OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_WRITECOMPLETED_DUP2, 
				"CCdcDataInterface::WriteCompleted;__HEADLESS_ACM_TEST_CODE__- issuing Read for %d bytes", iHeadlessReadLength );

		if (!iReader)
			{
			OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_WRITECOMPLETED_DUP3, "CCdcDataInterface::WriteCompleted;iReader=%p", iReader );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		iReader->Read(iHeadlessAcmBuffer, iHeadlessReadLength);
		break;
	case EReadOneOrMore:
		OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_WRITECOMPLETED_DUP4, 
				"CCdcDataInterface::WriteCompleted;__HEADLESS_ACM_TEST_CODE__- issuing ReadOneOrMore for %d bytes", iHeadlessReadLength );
	
		if (!iReadOneOrMoreReader)
			{
			OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_WRITECOMPLETED_DUP5, 
							"CCdcDataInterface::WriteCompleted;iReadOneOrMoreReader=%p", iReadOneOrMoreReader );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		iReadOneOrMoreReader->ReadOneOrMore(iHeadlessAcmBuffer, iHeadlessReadLength);
		break;
	case EUnknown:
	default:
		OstTrace0( TRACE_FATAL, CCDCDATAINTERFACE_WRITECOMPLETED_DUP6, "CCdcDataInterface::WriteCompleted;EPanicInternalError" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		break;
		}
#else
	// In case the write observer wants to post another write synchronously on 
	// being informed that this write has completed, use this little 'temp' 
	// fiddle.
	if (!iWriteObserver)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_WRITECOMPLETED_DUP7, 
						"CCdcDataInterface::WriteCompleted;iWriteObserver=%p", iWriteObserver );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	MWriteObserver* temp = iWriteObserver;
	iWriteObserver = NULL;
	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_WRITECOMPLETED_DUP8, 
				"CCdcDataInterface::WriteCompleted;\tcalling WriteCompleted on observer" );
	temp->WriteCompleted(aError);
#endif // __HEADLESS_ACM_TEST_CODE__

	OstTraceFunctionExit0( CCDCDATAINTERFACE_WRITECOMPLETED_EXIT );
	}

void CCdcDataInterface::CancelWrite()
/**
 * Cancel an outstanding write request
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_CANCELWRITE_ENTRY );
	if (!iWriter)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_CANCELWRITE, "CCdcDataInterface::CancelWrite;iWriter=%p", iWriter );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iWriter->Cancel();
	iWriteObserver = NULL;
	OstTraceFunctionExit0( CCDCDATAINTERFACE_CANCELWRITE_EXIT );
	}

void CCdcDataInterface::Read(MReadObserver& aObserver, 
							 TDes8& aDes, 
							 TInt aMaxLen)
/**
 * Read data from the interface. As the LDD supports an appropriate function, 
 * this request can be passed straight down. 
 *
 * @param aObserver The observer to notify of completion.
 * @param aDes Descriptor to put the read data in
 * @param aMaxLen Number of bytes to read
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_READ_ENTRY );
	
#ifdef __HEADLESS_ACM_TEST_CODE__
	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_READ, "CCdcDataInterface::Read;__HEADLESS_ACM_TEST_CODE__" );
	// Issue a Read using our special internal buffer.
	iHeadlessReadType = ERead;
	iHeadlessReadLength = aMaxLen;
	static_cast<void>(&aObserver);
	static_cast<void>(&aDes);
	if (aMaxLen > iHeadlessAcmBuffer.MaxLength())
		{
		OstTrace1( TRACE_FATAL, CCDCDATAINTERFACE_READ_DUP1, "CCdcDataInterface::Read;aMaxLen=%d", aMaxLen );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READ_DUP2, "CCdcDataInterface::Read;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iReader->Read(iHeadlessAcmBuffer, aMaxLen);
#else
	if (iReadObserver)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READ_DUP3, "CCdcDataInterface::Read;iReadObserver=%p", iReadObserver );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iReadObserver = &aObserver;
	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READ_DUP4, "CCdcDataInterface::Read;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	
	iReader->Read(aDes, aMaxLen);
#endif // __HEADLESS_ACM_TEST_CODE__
	OstTraceFunctionExit0( CCDCDATAINTERFACE_READ_EXIT );
	}

void CCdcDataInterface::ReadOneOrMore(MReadOneOrMoreObserver& aObserver, 
								  TDes8& aDes, 
								  TInt aMaxLen)
/**
 * Read a given amount of data from the interface, but complete if any data 
 * arrives.
 *
 * @param aObserver The observer to notify of completion.
 * @param aDes Descriptor to put the read data in
 * @param aMaxLen Number of bytes to read
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_READONEORMORE_ENTRY );
	

#ifdef __HEADLESS_ACM_TEST_CODE__
	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_READONEORMORE, "CCdcDataInterface::ReadOneOrMore;__HEADLESS_ACM_TEST_CODE__" );
	// Issue a ReadOneOrMore using our special internal buffer.
	iHeadlessReadType = EReadOneOrMore;
	iHeadlessReadLength = aMaxLen;
	static_cast<void>(&aObserver);
	static_cast<void>(&aDes);
	if (aMaxLen > iHeadlessAcmBuffer.MaxLength())
		{
		OstTrace1( TRACE_FATAL, CCDCDATAINTERFACE_READONEORMORE_DUP1, "CCdcDataInterface::ReadOneOrMore;aMaxLen=%d", aMaxLen );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	if (!iReadOneOrMoreReader)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READONEORMORE_DUP2, 
									"CCdcDataInterface::ReadOneOrMore;iReadOneOrMoreReader=%p", iReadOneOrMoreReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iReadOneOrMoreReader->ReadOneOrMore(iHeadlessAcmBuffer, aMaxLen);
#else
	if (iReadOneOrMoreObserver)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READONEORMORE_DUP3, 
						"CCdcDataInterface::ReadOneOrMore;iReadOneOrMoreObserver=%p", iReadOneOrMoreObserver );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iReadOneOrMoreObserver = &aObserver;
	if (!iReadOneOrMoreReader)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READONEORMORE_DUP4, 
						"CCdcDataInterface::ReadOneOrMore;iReadOneOrMoreObserver=%p", iReadOneOrMoreObserver );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iReadOneOrMoreReader->ReadOneOrMore(aDes, aMaxLen);
#endif // __HEADLESS_ACM_TEST_CODE__
	OstTraceFunctionExit0( CCDCDATAINTERFACE_READONEORMORE_EXIT );
	}

void CCdcDataInterface::ReadOneOrMoreCompleted(TInt aError)
/**
 * The completion function, called when a ReadOneOrMore request is completed 
 * by the LDD.
 *
 * @param aError The result of the read request.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_READONEORMORECOMPLETED_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_READONEORMORECOMPLETED, "CCdcDataInterface::ReadOneOrMoreCompleted;aError=%d", aError );

#ifdef __HEADLESS_ACM_TEST_CODE__
	OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_READONEORMORECOMPLETED_DUP1, 
			"CCdcDataInterface::ReadOneOrMoreCompleted;__HEADLESS_ACM_TEST_CODE__- issuing Write for %d bytes", iHeadlessAcmBuffer.Length() );

	// Write back the data just read.
	// If the ReadOneOrMore completed with an error, we panic, as it's 
	// invalidating the test.
	if (aError != KErrNone)
		{
		OstTrace1( TRACE_FATAL, CCDCDATAINTERFACE_READONEORMORECOMPLETED_DUP2, 
								"CCdcDataInterface::ReadOneOrMoreCompleted;aError=%d", aError );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	if (!iWriter)
		{	
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READONEORMORECOMPLETED_DUP3, 
							"CCdcDataInterface::ReadOneOrMoreCompleted;iWriter=%p", iWriter );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iWriter->Write(iHeadlessAcmBuffer, iHeadlessAcmBuffer.Length(), EFalse); 
#else
	if (!iReadOneOrMoreObserver)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READONEORMORECOMPLETED_DUP4, 
							"CCdcDataInterface::ReadOneOrMoreCompleted;iReadOneOrMoreObserver=%p", iReadOneOrMoreObserver );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	// See comment in WriteCompleted.
	MReadOneOrMoreObserver* temp = iReadOneOrMoreObserver;
	iReadOneOrMoreObserver = NULL;
	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_READONEORMORECOMPLETED_DUP5, 
			"CCdcDataInterface::ReadOneOrMoreCompleted;\tcalling ReadOneOrMoreCompleted on observer" );
	temp->ReadOneOrMoreCompleted(aError);
#endif // __HEADLESS_ACM_TEST_CODE__

	OstTraceFunctionExit0( CCDCDATAINTERFACE_READONEORMORECOMPLETED_EXIT );
	}

void CCdcDataInterface::ReadCompleted(TInt aError)
/**
 * Called by the active reader object when it completes.
 *
 * @param aError Error.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_READCOMPLETED_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_READCOMPLETED, "CCdcDataInterface::ReadCompleted;aError=%d", aError );

#ifdef __HEADLESS_ACM_TEST_CODE__
	OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_READCOMPLETED_DUP1, 
			"CCdcDataInterface::ReadCompleted;__HEADLESS_ACM_TEST_CODE__- issuing Write for %d bytes", iHeadlessAcmBuffer.Length() );
	
	// Write back the data just read.
	// If the Read completed with an error, we panic, as it's invalidating the 
	// test.				 
	if (aError != KErrNone)
		{
		OstTrace1( TRACE_FATAL, CCDCDATAINTERFACE_READCOMPLETED_DUP2, "CCdcDataInterface::ReadCompleted;aError=%d", aError );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	if (!iWriter)
		{
		OstTraceExt1( TRACE_NORMAL, CCDCDATAINTERFACE_READCOMPLETED_DUP3, "CCdcDataInterface::ReadCompleted;iWriter=%p", iWriter );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iWriter->Write(iHeadlessAcmBuffer, iHeadlessAcmBuffer.Length(), EFalse); 
#else
	if (!iReadObserver)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_READCOMPLETED_DUP4, 
							"CCdcDataInterface::ReadCompleted;iReadObserver=%p", iReadObserver );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	// See comment in WriteCompleted.
	MReadObserver* temp = iReadObserver;
	iReadObserver = NULL;
	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_READCOMPLETED_DUP5, "CCdcDataInterface::ReadCompleted;\tcalled ReadCompleted on observer" );
	temp->ReadCompleted(aError);
#endif // __HEADLESS_ACM_TEST_CODE__

	OstTraceFunctionExit0( CCDCDATAINTERFACE_READCOMPLETED_EXIT );
	}

void CCdcDataInterface::CancelRead()
/**
 * Cancel an outstanding read request
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_CANCELREAD_ENTRY );
	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_CANCELREAD, "CCdcDataInterface::CancelRead;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	if (!iReadOneOrMoreReader)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_CANCELREAD_DUP1, 
							"CCdcDataInterface::CancelRead;iReadOneOrMoreReader=%p", iReadOneOrMoreReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	iReader->Cancel();
	iReadOneOrMoreReader->Cancel();
	iReadObserver = NULL;
	iReadOneOrMoreObserver = NULL;
	OstTraceFunctionExit0( CCDCDATAINTERFACE_CANCELREAD_EXIT );
	}

	
void CCdcDataInterface::NotifyDataAvailableCompleted(TInt aError)
/**
 * Called by the active data available notifier object when it completes.
 *
 * @param aError Error.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_NOTIFYDATAAVAILABLECOMPLETED_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCDATAINTERFACE_NOTIFYDATAAVAILABLECOMPLETED, 
			"CCdcDataInterface::NotifyDataAvailableCompleted;aError=%d", aError );	
	
	// See comment in WriteCompleted.
	MNotifyDataAvailableObserver* temp = iNotifyDataAvailableObserver;
	iNotifyDataAvailableObserver = NULL;
	OstTrace0( TRACE_NORMAL, CCDCDATAINTERFACE_NOTIFYDATAAVAILABLECOMPLETED_DUP1, 
			"CCdcDataInterface::NotifyDataAvailableCompleted;\tcalled NotifyDataAvailableCompleted on observer" );
	temp->NotifyDataAvailableCompleted(aError);
	OstTraceFunctionExit0( CCDCDATAINTERFACE_NOTIFYDATAAVAILABLECOMPLETED_EXIT );
	}
	
void CCdcDataInterface::NotifyDataAvailable(MNotifyDataAvailableObserver& aObserver)
/**
 * Complete if any data arrives.
 *
 * @param aObserver The observer to notify of completion.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_NOTIFYDATAAVAILABLE_ENTRY );
	
	if (iNotifyDataAvailableObserver)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_NOTIFYDATAAVAILABLE, 
							"CCdcDataInterface::NotifyDataAvailable;iNotifyDataAvailableObserver=%p", iNotifyDataAvailableObserver );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iNotifyDataAvailableObserver = &aObserver;
	if (!iDataAvailableNotifier)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_NOTIFYDATAAVAILABLE_DUP1, 
						"CCdcDataInterface::NotifyDataAvailable;iDataAvailableNotifier=%p", iDataAvailableNotifier );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iDataAvailableNotifier->NotifyDataAvailable();
	OstTraceFunctionExit0( CCDCDATAINTERFACE_NOTIFYDATAAVAILABLE_EXIT );
	}

void CCdcDataInterface::CancelNotifyDataAvailable()
/**
 * Cancel notification of arrival of data.
 */
	{
	OstTraceFunctionEntry0( CCDCDATAINTERFACE_CANCELNOTIFYDATAAVAILABLE_ENTRY );
	if (!iDataAvailableNotifier)
		{
		OstTraceExt1( TRACE_FATAL, CCDCDATAINTERFACE_CANCELNOTIFYDATAAVAILABLE, 
						"CCdcDataInterface::CancelNotifyDataAvailable;iDataAvailableNotifier=%p", iDataAvailableNotifier );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iDataAvailableNotifier->Cancel();
	iNotifyDataAvailableObserver = NULL;	
	OstTraceFunctionExit0( CCDCDATAINTERFACE_CANCELNOTIFYDATAAVAILABLE_EXIT );
	}

//
// End of file
