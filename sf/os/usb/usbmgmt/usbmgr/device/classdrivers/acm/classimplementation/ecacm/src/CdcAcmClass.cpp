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

#include "CdcAcmClass.h"
#include "CdcControlInterface.h"
#include "CdcDataInterface.h"
#include "AcmPanic.h"
#include "AcmUtils.h"
#include "HostPushedChangeObserver.h"
#include "BreakController.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CdcAcmClassTraces.h"
#endif

extern const TInt32 KUsbAcmHostCanHandleZLPs = 0;

CCdcAcmClass::CCdcAcmClass()
/**
 * Constructor.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_CCDCACMCLASS_CONS_ENTRY );
	SetDefaultAcm();
	OstTraceFunctionExit0( CCDCACMCLASS_CCDCACMCLASS_CONS_EXIT );
	}

CCdcAcmClass* CCdcAcmClass::NewL(const TUint8 aProtocolNum, const TDesC16& aAcmControlIfcName, const TDesC16& aAcmDataIfcName)
/**
 * Create a new CCdcAcmClass object
 *
 * @return Ownership of a new CCdcAcmClass object
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_NEWL_ENTRY );
	CCdcAcmClass* self = new(ELeave) CCdcAcmClass;
	CleanupStack::PushL(self);
	self->ConstructL(aProtocolNum, aAcmControlIfcName, aAcmDataIfcName);
	CLEANUPSTACK_POP(self);
	OstTraceFunctionExit0( CCDCACMCLASS_NEWL_EXIT );
	return self;
	}

void CCdcAcmClass::ConstructL(const TUint8 aProtocolNum, const TDesC16& aControlIfcName, const TDesC16& aDataIfcName)
/**
 * 2nd-phase construction. Creates both the control and data objects.
 * @param aProtocolNum contains the Table 17 protocol number.
 * @param aControlIfcName contains the interface name
 * @param aDataIfcName contains the interface name
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_CONSTRUCTL_ENTRY );
	
	TUint8 interfaceNumber;
	TInt res;

	OstTrace0( TRACE_NORMAL, CCDCACMCLASS_CONSTRUCTL, "CCdcAcmClass::ConstructL;\tabout to create control interface with name" );
	iControl = CCdcControlInterface::NewL(*this, aProtocolNum, aControlIfcName);

	OstTrace0( TRACE_NORMAL, CCDCACMCLASS_CONSTRUCTL_DUP1, "CCdcAcmClass::ConstructL;\tabout to create data interface with name" );
	iData = CCdcDataInterface::NewL(aDataIfcName);

	iBreakController = CBreakController::NewL(*this);

	OstTrace0( TRACE_NORMAL, CCDCACMCLASS_CONSTRUCTL_DUP2, "CCdcAcmClass::ConstructL;\tabout to call GetInterfaceNumber" );
	res = iData->GetInterfaceNumber(interfaceNumber);
	if ( res )
		{
		OstTrace1( TRACE_NORMAL, CCDCACMCLASS_CONSTRUCTL_DUP3, "CCdcAcmClass::ConstructL;\tGetInterfaceNumber=%d", res );
		if (res < 0)
			{
			 User::Leave(res);
			}
		}

	OstTrace0( TRACE_NORMAL, CCDCACMCLASS_CONSTRUCTL_DUP4, "CCdcAcmClass::ConstructL;\tabout to call SetupClassSpecificDescriptor" );
	res = iControl->SetupClassSpecificDescriptor(interfaceNumber);
	if ( res )
		{
		OstTrace1( TRACE_NORMAL, CCDCACMCLASS_CONSTRUCTL_DUP5, "CCdcAcmClass::ConstructL;\tSetupClassSpecificDescriptor=%d", res );
		if (res < 0)
			{
			User::Leave(res);
			}
		}
	OstTraceFunctionExit0( CCDCACMCLASS_CONSTRUCTL_EXIT );
	}

CCdcAcmClass::~CCdcAcmClass()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_CCDCACMCLASS_DES_ENTRY );
	delete iControl;
	delete iData;
	delete iBreakController;
	OstTraceFunctionExit0( CCDCACMCLASS_CCDCACMCLASS_DES_EXIT );
	}

void CCdcAcmClass::SetCallback(MHostPushedChangeObserver* aCallback)
/**
 * Set the ACM class callback object. This cannot be done at construction 
 * because the ACM class and the port have different lifetimes.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_SETCALLBACK_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_SETCALLBACK, "CCdcAcmClass::SetCallback;aCallback=%p", aCallback );
	iCallback = aCallback;

	// remember that this function can also be called to
	// unset the callback when it is given a NULL pointer
	if ( iCallback )
		{
		// send off whatever has been seen from Host
		// requests to change the line coding
		TCommConfigV01 epocConfig;
		ConvertUsbConfigCodingToEpoc(iUsbConfig,epocConfig);
		iCallback->HostConfigChange(epocConfig);

		// send off whatever has been seen from Host
		// requests to set the control line state
		iCallback->HostSignalChange(iDtrState,iRtsState);
		}
	
	OstTraceFunctionExit0( CCDCACMCLASS_SETCALLBACK_EXIT );
	}

void CCdcAcmClass::SetBreakCallback(MBreakObserver* aBreakCallback)
/**
 * Set the observer for break events. This cannot be done at construction 
 * because the ACM class and the port have different lifetimes.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_SETBREAKCALLBACK_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_SETBREAKCALLBACK, "CCdcAcmClass::SetBreakCallback;aBreakCallback=%p", aBreakCallback );
	iBreakCallback = aBreakCallback;
	OstTraceFunctionExit0( CCDCACMCLASS_SETBREAKCALLBACK_EXIT );
	}

void CCdcAcmClass::ReadOneOrMore(MReadOneOrMoreObserver& aObserver, TDes8& aDes)
/**
 * Read from the bus a specified amount but complete if any data arrives.
 *
 * @param aObserver The observer to notify completion to.
 * @param aDes Descriptor to read into
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_READONEORMORE_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_READONEORMORE, "CCdcAcmClass::ReadOneOrMore;aObserver=%p", &aObserver );
	ReadOneOrMore(aObserver, aDes, aDes.Length());
	OstTraceFunctionExit0( CCDCACMCLASS_READONEORMORE_EXIT );
	}

void CCdcAcmClass::ReadOneOrMore(MReadOneOrMoreObserver& aObserver, 
								 TDes8& aDes, 
								 TInt aLength)
/**
 * Read from the bus a specified amount but complete if any data arrives.
 *
 * @param aObserver The observer to notify completion to.
 * @param aDes Descriptor to read into
 * @param aLength Amount of data to read
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_READONEORMORE_MREADONEORMOREOBSERVERREF_TDES8REF_TINT_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CCDCACMCLASS_READONEORMORE_MREADONEORMOREOBSERVERREF_TDES8REF_TINT, 
			"CCdcAcmClass::ReadOneOrMore;aObserver=%p;aLength=%d", &aObserver, aLength );

	if (!iData)
		{
		OstTrace1( TRACE_FATAL, CCDCACMCLASS_READONEORMORE_MREADONEORMOREOBSERVERREF_TDES8REF_TINT_DUP1, "CCdcAcmClass::ReadOneOrMore;iData=%d", (TInt)iData );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iData->ReadOneOrMore(aObserver, aDes, aLength);

	OstTraceFunctionExit0( CCDCACMCLASS_READONEORMORE_MREADONEORMOREOBSERVERREF_TDES8REF_TINT_EXIT );
	}

void CCdcAcmClass::Read(MReadObserver& aObserver, TDes8& aDes)
/**
 * Read from the bus
 *
 * @param aObserver The observer to notify completion to.
 * @param aDes Descriptor to read into.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_READ_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_READ, "CCdcAcmClass::Read;aObserver=%p", &aObserver );
	Read(aObserver, aDes, aDes.Length());
	OstTraceFunctionExit0( CCDCACMCLASS_READ_EXIT );
	}

void CCdcAcmClass::Read(MReadObserver& aObserver, TDes8& aDes, TInt aLength)
/**
 * Read from the bus a specified amount.
 *
 * @param aObserver The observer to notify completion to.
 * @param aDes Descriptor to read into.
 * @param aLength Amount of data to read.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_READ_ENTRY_DUP1 );
	OstTraceExt2( TRACE_NORMAL, CCDCACMCLASS_READ_DUP1, "CCdcAcmClass::Read;aObserver=%p;aLength=%d", &aObserver, aLength );
	if (!iData)
		{		
		OstTraceExt1( TRACE_FATAL, CCDCACMCLASS_READ_DUP2, "CCdcAcmClass::Read;iData=%p", iData );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iData->Read(aObserver, aDes, aLength);
	OstTraceFunctionExit0( CCDCACMCLASS_READ_ENTRY_DUP1_EXIT );
	}

void CCdcAcmClass::ReadCancel()
/**
 * Cancel a read request.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_READCANCEL_ENTRY );
	
	if (!iData)
		{
		OstTraceExt1( TRACE_FATAL, CCDCACMCLASS_READCANCEL, "CCdcAcmClass::ReadCancel;iData=%p", iData );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iData->CancelRead();
	OstTraceFunctionExit0( CCDCACMCLASS_READCANCEL_EXIT );
	}

void CCdcAcmClass::Write(MWriteObserver& aObserver, const TDesC8& aDes)
/**
 * Write to the bus.
 *
 * @param aObserver The observer to notify completion to.
 * @param aDes Descriptor containing the data to be written.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_WRITE_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_WRITE, "CCdcAcmClass::Write;aObserver=%p", &aObserver );
	Write(aObserver, aDes, aDes.Length());
	OstTraceFunctionExit0( CCDCACMCLASS_WRITE_EXIT );
	}

void CCdcAcmClass::Write(MWriteObserver& aObserver, 
						 const TDesC8& aDes, 
						 TInt aLength)
/**
 * Write to the bus the specified ammount.
 *
 * @param aObserver The observer to notify completion to.
 * @param aDes Descriptor containing the data to be written.
 * @param aLength The amount of data to write.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_WRITE_ENTRY_DUP1 );
	OstTraceExt2( TRACE_NORMAL, CCDCACMCLASS_WRITE_DUP1, "CCdcAcmClass::Write;aObserver=%p;aLength=%d", &aObserver, aLength );
	if (!iData)
		{
		OstTraceExt1( TRACE_FATAL, CCDCACMCLASS_WRITE_DUP2, "CCdcAcmClass::Write;iData=%p", iData );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iData->Write(aObserver, aDes, aLength);
	OstTraceFunctionExit0( CCDCACMCLASS_WRITE_ENTRY_DUP1_EXIT );
	}

void CCdcAcmClass::WriteCancel()
/**
 * Cancel the write request.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_WRITECANCEL_ENTRY );
	if (!iData)
		{
		OstTraceExt1( TRACE_FATAL, CCDCACMCLASS_WRITECANCEL, "CCdcAcmClass::WriteCancel;iData=%p", iData );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iData->CancelWrite();
	OstTraceFunctionExit0( CCDCACMCLASS_WRITECANCEL_EXIT );
	}

TInt CCdcAcmClass::HandleGetCommFeature(const TUint16 aSelector, 
										TDes8& aReturnData)
/**
 * Callback for Get Comm Feature requests.
 *
 * @param aSelector Multiplex control for the feature is held in wValue field
 * @param aReturnData Descriptor containing the multiplexed and idle state of 
 *					the ACM device or the country code from ISO 3166.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLEGETCOMMFEATURE_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLEGETCOMMFEATURE, "CCdcAcmClass::HandleGetCommFeature;aSelector=%d", (TInt)aSelector );
	TInt ret = KErrNone;

	// check the feature selector from the header and reject if invalid,
	// otherwise deal with it.
	switch ( aSelector )
		{
	case EUsbCommFeatureSelectAbstractState:
		{
		aReturnData.SetLength(2);

		TUint8* pbuffer;
		TUint8** ppbuffer;
		pbuffer = &aReturnData[0];
		ppbuffer = &pbuffer;
		
		CCdcControlInterface::PutU16(ppbuffer,iAcmState);
		OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_HANDLEGETCOMMFEATURE_DUP1, 
				"CCdcAcmClass::HandleGetCommFeature;\tAbstract State [0x%hx]", iAcmState );
		}
		break;

	case EUsbCommFeatureSelectCountryCode:
		{
#if defined(DISABLE_ACM_CF_COUNTRY_SETTING)

		aReturnData.SetLength(0);
		OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLEGETCOMMFEATURE_DUP2, 
				"CCdcAcmClass::HandleGetCommFeature;\tCountry Code Not Supported" );
		ret = KErrNotSupported;

#elif defined(ENABLE_ACM_CF_COUNTRY_SETTING)

		aReturnData.SetLength(2);

		TUint8* pbuffer;
		TUint8** ppbuffer;
		pbuffer = &aReturnData[0];
		ppbuffer = &pbuffer;
		
		CCdcControlInterface::PutU16(ppbuffer,iCountryCode);
		OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_HANDLEGETCOMMFEATURE_DUP3, 
				"CCdcAcmClass::HandleGetCommFeature;\tCountry Code [0x%hx]", iCountryCode );
#endif
		}
		break;

	default:
		aReturnData.SetLength(0);
		OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLEGETCOMMFEATURE_DUP4, "CCdcAcmClass::HandleGetCommFeature;\tBad Selector" );
		ret = KErrUnknown;
		}

	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLEGETCOMMFEATURE_DUP5, 
			"CCdcAcmClass::HandleGetCommFeature;ret=%d", ret );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLEGETCOMMFEATURE_EXIT );
	return ret;
	}

TInt CCdcAcmClass::HandleClearCommFeature(const TUint16 aSelector)
/**
 * Callback for Clear Comm Feature requests
 * @param aSelector Multiplex control for the feature is held in wValue field
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLECLEARCOMMFEATURE_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLECLEARCOMMFEATURE, "CCdcAcmClass::HandleClearCommFeature;aSelector=%d", (TInt)aSelector );
	TInt ret = KErrNone;

	// check the feature selector from the header and reject if invalid,
	// otherwise deal with it.
	switch ( aSelector )
		{
	case EUsbCommFeatureSelectAbstractState:
		{
		// reset to guaranteed-success default, so ignore return value
		static_cast<void>(HandleNewAbstractState(EUsbAbstractStateDataMultiplex));
		OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_HANDLECLEARCOMMFEATURE_DUP1, 
				"CCdcAcmClass::HandleClearCommFeature;\tAbstract State [0x%hx]", iAcmState );
		}
		break;
	
	case EUsbCommFeatureSelectCountryCode:
		{
#if defined(DISABLE_ACM_CF_COUNTRY_SETTING)
		
		OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLECLEARCOMMFEATURE_DUP2, 
				"CCdcAcmClass::HandleClearCommFeature;\tCountry Code Not Supported" );
		ret = KErrNotSupported;

#elif defined(ENABLE_ACM_CF_COUNTRY_SETTING)

		HandleNewCountryCode(KUsbCommCountryCode0);
		OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_HANDLECLEARCOMMFEATURE_DUP3, 
				"CCdcAcmClass::HandleClearCommFeature;\tCountry Code [0x%hx]", iCountryCode );

#endif
		}
		break;
	
	default:
		{
		OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLECLEARCOMMFEATURE_DUP4, "CCdcAcmClass::HandleClearCommFeature;\tBad Selector" );
		ret = KErrUnknown;
		}
		}

	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLECLEARCOMMFEATURE_DUP5, "CCdcAcmClass::HandleClearCommFeature;ret=%d", ret );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLECLEARCOMMFEATURE_EXIT );
	return ret;
	}

TInt CCdcAcmClass::HandleSetLineCoding(const TDesC8& aData)
/**
 * Callback for Set Line Coding requests.
 * This function extracts the parameters from the host packet, stores them 
 * locally, then converts them to an EPOC format and passes them up to the CSY 
 * class.
 * Note that we are intentionally keeping two copies of the settings in the 
 * CSY: the "host pushed" settings and the "client pushed" settings.  Since 
 * the EPOC machine is a "device" rather than a "host", it can't push 
 * configuration changes to the host. We are therefore propagating host-pushed 
 * configuration changes to the C32 client, but not propagating C32 client 
 * configuration requests back to the host.
 * 
 * @param aData Descriptor containing the new data rate, stop bits, parity and 
 * data bit settings.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLESETLINECODING_ENTRY );
	

	if (aData.Length() != 7) // TODO: magic number?
		{
		OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLESETLINECODING, "CCdcAcmClass::HandleSetLineCoding;\t***buffer too small" );
		OstTraceFunctionExit0( CCDCACMCLASS_HANDLESETLINECODING_EXIT );
		return KErrGeneral;
		}

	TUint8*  pbuffer;
	TUint8** ppbuffer;
	pbuffer = const_cast<TUint8*>(&aData[0]);
	ppbuffer = &pbuffer;

	iUsbConfig.iRate	 =				  CCdcControlInterface::GetU32(ppbuffer);
	iUsbConfig.iStopBits = static_cast<TUsbStopBits>(CCdcControlInterface::GetU08(ppbuffer));
	iUsbConfig.iParity	 = static_cast<TUsbParity>(CCdcControlInterface::GetU08(ppbuffer));
	iUsbConfig.iDataBits = static_cast<TUsbDataBits>(CCdcControlInterface::GetU08(ppbuffer));

	if ( iCallback )
		{
		TCommConfigV01 epocConfig;

		ConvertUsbConfigCodingToEpoc(iUsbConfig, epocConfig);

		iCallback->HostConfigChange(epocConfig);
		}

	OstTraceFunctionExit0( CCDCACMCLASS_HANDLESETLINECODING_EXIT_DUP1 );
	return KErrNone;
	}

void CCdcAcmClass::SetDefaultAcm()
/**
 * Reset the data fields to a set of known default values. Will be used when 
 * we implement restart, which involves a host re-enumeration without 
 * destroying the ACM class, so we need a function to reset the ACM to 
 * defaults.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_SETDEFAULTACM_ENTRY );
	
	iUsbConfig.iRate	 = 115200;
	iUsbConfig.iStopBits = EUsbStopBitOne;
	iUsbConfig.iParity	 = EUsbParityNone;
	iUsbConfig.iDataBits = EUsbDataBitsEight;

	iAcmState	 = EUsbAbstractStateDataMultiplex;
	iCountryCode = 0x2A2A;	// 0x2A2A is "**"- just an invalid code

	iAcmDataMultiplex = (iAcmState & EUsbAbstractStateDataMultiplex) 
		? ETrue : EFalse;
	iAcmIdleSetting   = (iAcmState & EUsbAbstractStateIdleSetting) 
		? ETrue : EFalse;

	iRtsState = EFalse;
	iDtrState = EFalse;

	iRingState = EFalse;
	iDsrState  = EFalse;
	iDcdState  = EFalse;

	iBreakActive = EFalse;
	OstTraceFunctionExit0( CCDCACMCLASS_SETDEFAULTACM_EXIT );
	}

void CCdcAcmClass::ConvertUsbConfigCodingToEpoc(const TUsbConfig& aUsbConfig, 
								  TCommConfigV01& aEpocConfig)
/**
 * Convert configuration information received from a Host in "USB" format to 
 * the native "EPOC" configuration format.
 *
 * @param aUsbConfig	The USB configuration to be converted from.
 * @param aEpocConfig	The EPOC configuration to be converted to.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_CONVERTUSBCONFIGCODINGTOEPOC_ENTRY );
	
	switch ( aUsbConfig.iDataBits )
		{
	case EUsbDataBitsFive:
		aEpocConfig.iDataBits = EData5;
		break;

	case EUsbDataBitsSix:
		aEpocConfig.iDataBits = EData6;
		break;

	case EUsbDataBitsSeven:
		aEpocConfig.iDataBits = EData7;
		break;

	case EUsbDataBitsEight:
		aEpocConfig.iDataBits = EData8;
		break;

	// EPOC doesn't support this, so map it to "8" for the time being...
	case EUsbDataBitsSixteen:		
		aEpocConfig.iDataBits = EData8;
		break;

	// Map any other signals to "8"...
	default:						
		aEpocConfig.iDataBits = EData8;
		break;
		}

	switch ( aUsbConfig.iStopBits )
		{
	case EUsbStopBitOne:
		aEpocConfig.iStopBits = EStop1;
		break;

	// EPOC doesn't have any "1.5" setting, so go for "1".
	case EUsbStopBitOnePtFive:		
		aEpocConfig.iStopBits = EStop1;
		break;

	case EUsbStopBitTwo:
		aEpocConfig.iStopBits = EStop2;
		break;

	// Map any other signals to "1"...
	default:						
		aEpocConfig.iStopBits = EStop1;
		break;
		}

	switch ( aUsbConfig.iParity )
		{
	case EUsbParityNone:
		aEpocConfig.iParity = EParityNone;
		break;

	case EUsbParityOdd:
		aEpocConfig.iParity = EParityOdd;
		break;

	case EUsbParityEven:
		aEpocConfig.iParity = EParityEven;
		break;

	case EUsbParityMark:
		aEpocConfig.iParity = EParityMark;
		break;

	case EUsbParitySpace:
		aEpocConfig.iParity = EParitySpace;
		break;

	// Map any other signals to "No Parity"...
	default:						
		aEpocConfig.iParity = EParityNone;
		break;
		}

	static const TUint32 KUsbDataRate[] = { 50,	75, 	110,	
									 134,	150,	300,		
									 600,	1200,	1800,		
									 2000,	2400,	3600,
									 4800,	7200,	9600,	
									 19200, 38400,	57600,		
									 115200,230400, 460800, 	
									 576000,1152000,4000000};

	static const TBps KEpocDataRate[] = {	EBps50,    EBps75,	   EBps110, 
									EBps134,   EBps150,    EBps300, 
									EBps600,   EBps1200,   EBps1800,	
									EBps2000,  EBps2400,   EBps3600,
									EBps4800,  EBps7200,   EBps9600,	
									EBps19200, EBps38400,  EBps57600,	
									EBps115200,EBps230400, EBps460800,	
									EBps576000,EBps1152000,EBps4000000};

	aEpocConfig.iRate = EBpsSpecial;	// Default to "Special"
	TInt arraySize = sizeof(KUsbDataRate)/sizeof(TUint32);
	for ( TInt i = 0 ; i < arraySize ; i++ )
		{
		if ( aUsbConfig.iRate == KUsbDataRate[i] )
			{
			aEpocConfig.iRate = KEpocDataRate[i];
			break;
			}
		}
	OstTraceFunctionExit0( CCDCACMCLASS_CONVERTUSBCONFIGCODINGTOEPOC_EXIT );
	}

TInt CCdcAcmClass::HandleNewAbstractState(const TUint16 aAbstractState)
/**
 * Handle new Abstract State as received from Host, check values
 * and inform client watcher (if registered)
 * @param aAbstractState	The new Abstract State which contains
 *							significant bits D0 and D1 where:
 *
 *	D0 controls 'idle setting'			 -> iAcmIdleSetting
 *	D1 controls 'data multiplexed state' -> iAcmDataMultiplex
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLENEWABSTRACTSTATE_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLENEWABSTRACTSTATE, "CCdcAcmClass::HandleNewAbstractState;aAbstractState=%d", (TInt)aAbstractState );

	TBool	multiplex;
	TBool	idle;
	TUint16 newstate;
	TInt ret;

	// collect local booleans from incoming combo ready to do local
	// discrepancy check
	multiplex = (aAbstractState & EUsbAbstractStateDataMultiplex) 
		? ETrue : EFalse;
	idle	  = (aAbstractState & EUsbAbstractStateIdleSetting) 
		? ETrue : EFalse;

	// apply any necessary overrides due to incomplete ACM class
	// support (this may change the local booleans and also may be
	// the reason to inform caller of a failure)
	ret = KErrNone;

	if ( multiplex != iAcmDataMultiplex )
		{
#if defined(DISABLE_ACM_CF_DATA_MULTIPLEX)
		// if this selector is disabled then the attempt to change the
		// bit must fail
		ret = KErrNotSupported;

		multiplex = iAcmDataMultiplex;
#endif
		}

	if ( idle != iAcmIdleSetting )
		{
#if defined(DISABLE_ACM_CF_IDLE_SETTING)
		// if this selector is disabled then the attempt to change the
		// bit must fail
		ret = KErrNotSupported;

		idle = iAcmIdleSetting;
#endif
		}

	// save the new booleans into the private store
	iAcmDataMultiplex = multiplex;
	iAcmIdleSetting   = idle;

	// recreate the private combo from these booleans
	newstate = static_cast<TUint16>(	
			( iAcmIdleSetting ? EUsbAbstractStateIdleSetting : EUsbAbstractStateNoBits )
		|	( iAcmDataMultiplex ? EUsbAbstractStateDataMultiplex : EUsbAbstractStateNoBits)
		);

	// discrepancy check to see if the client application needs to be 
	// informed, note that the callback may not have been placed.
	if( iAcmState != newstate )
		{
		OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_HANDLENEWABSTRACTSTATE_DUP1, 
				"CCdcAcmClass::HandleNewAbstractState;\tNew Combo [0x%hx]", newstate );

		if ( iCallback )
			{
			// If there was such a function in class 
			// MHostPushedChangeObserver, notify 
			// via iCallback->HostAbstractStateChange(newstate);
			OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLENEWABSTRACTSTATE_DUP2, 
					"CCdcAcmClass::HandleNewAbstractState;\tHas No Notification Method in class MHostPushedChangeObserver" );
			}
		}

	// and save the new state ready for next check
	iAcmState = newstate;
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLENEWABSTRACTSTATE_DUP3, "CCdcAcmClass::HandleNewAbstractState;ret=%d", ret );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLENEWABSTRACTSTATE_EXIT );
	return ret;
	}

TInt CCdcAcmClass::HandleNewCountryCode(const TUint16 aCountryCode)
/**
 * Handle new Country Setting as received from Host, check values
 * and inform client watcher (if registered)
 *
 * @param aCountryCode	The new Country Code as defined in ISO-3166
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLENEWCOUNTRYCODE_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLENEWCOUNTRYCODE, "CCdcAcmClass::HandleNewCountryCode;aCountryCode=%d", (TInt)aCountryCode );
	TInt ret;

#if defined(DISABLE_ACM_CF_COUNTRY_SETTING)

	// cite the incoming parameter to suppress 'unreferenced formal parameter'
	// warning and then set the return value to show this code does not handle
	// the country code.
	(void)aCountryCode;

	ret = KErrNotSupported;

#else

	// check the received code against the array in the descriptor
	// and confirm it is one of those present, otherwise return
	// 'unknown' : for now, just pretend it always works
	iCountryCode = aCountryCode;

	ret = KErrNone;

#endif

	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLENEWCOUNTRYCODE_DUP1, "CCdcAcmClass::HandleNewCountryCode;ret=%d", ret );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLENEWCOUNTRYCODE_EXIT );
	return ret;
	}

TInt CCdcAcmClass::HandleSendEncapCommand(const TDesC8& /*aData*/)
/**
 * Callback for Send Encapsulated Command requests
 *
 * @param aData Pointer to the Encapsulated message
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLESENDENCAPCOMMAND_ENTRY );	
	OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLESENDENCAPCOMMAND, "CCdcAcmClass::HandleSendEncapCommand;\t***not supported" );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLESENDENCAPCOMMAND_EXIT );
	return KErrNotSupported;
	}

TInt CCdcAcmClass::HandleGetEncapResponse(TDes8& /*aReturnData*/)
/**
 * Callback for Get Encapsulated Response requests
 *
 * @param aReturnData Pointer to the Response field
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLEGETENCAPRESPONSE_ENTRY );
	OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLEGETENCAPRESPONSE, "CCdcAcmClass::HandleGetEncapResponse;\t***not supported" );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLEGETENCAPRESPONSE_EXIT );
	return KErrNotSupported;
	}

TInt CCdcAcmClass::HandleSetCommFeature(const TUint16 aSelector, 
										const TDes8& aData)
/**
 * Callback for Set Comm Feature requests.
 *
 * @param aSelector Multiplex control for the feature is held in wValue field
 * @param aData 	Descriptor containing the multiplexed and idle state of 
 *					the ACM device or the country code from ISO 3166.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLESETCOMMFEATURE_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCOMMFEATURE, "CCdcAcmClass::HandleSetCommFeature;aSelector=%d", (TInt)aSelector );

	// reject any message that has malformed data
	if ( aData.Length() != 2 )
		{
		OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCOMMFEATURE_DUP1, 
				"CCdcAcmClass::HandleSetCommFeature;\t***aData.Length (%d) incorrect", aData.Length() );
		OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCOMMFEATURE_DUP2, 
				"CCdcAcmClass::HandleSetCommFeature;ret=%d", KErrArgument );
		OstTraceFunctionExit0( CCDCACMCLASS_HANDLESETCOMMFEATURE_EXIT );
		return KErrArgument;
		}

	TInt ret = KErrNone;

	// check the feature selector from the header and reject if invalid,
	// otherwise deal with it.
	switch ( aSelector )
		{
	case EUsbCommFeatureSelectAbstractState:
		{
		TUint16 newstate;

		TUint8* pbuffer;
		TUint8** ppbuffer;
		pbuffer = const_cast<TUint8*>(&aData[0]);
		ppbuffer = &pbuffer;
		
		newstate = CCdcControlInterface::GetU16(ppbuffer);

		if ( newstate != iAcmState )
			{
			ret = HandleNewAbstractState(newstate);
			OstTraceExt3( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCOMMFEATURE_DUP3, 
					"CCdcAcmClass::HandleSetCommFeature;\tHandleNewAbstractState=%d [%hu]->[%hu]", 
					ret, iAcmState, newstate );
			}
		}
		break;

	case EUsbCommFeatureSelectCountryCode:
		{
#if defined(DISABLE_ACM_CF_COUNTRY_SETTING)

		OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCOMMFEATURE_DUP4, "CCdcAcmClass::HandleSetCommFeature;Country Code Not Supported" );
		ret = KErrNotSupported;

#elif defined(ENABLE_ACM_CF_COUNTRY_SETTING)

		TUint16 newcountry;

		TUint8* pbuffer;
		TUint8** ppbuffer;
		pbuffer = &aData[0];
		ppbuffer = &pbuffer;

		newcountry = CCdcControlInterface::GetU16(ppbuffer);

		if( newcountry != iCountryCode )
			{
			ret = HandleNewCountryCode(newcountry);
			OstTraceExt3( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCOMMFEATURE_DUP5, 
					"CCdcAcmClass::HandleSetCommFeature;\tHandleNewCountryCode=%d [%u]->[%u]", ret, iCountryCode, newcountry );
			}

#endif
		break;
		}

	default:
		{
		OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCOMMFEATURE_DUP6, "CCdcAcmClass::HandleSetCommFeature;\tBad Selector" );
		ret = KErrUnknown;
		}
		}
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCOMMFEATURE_DUP7, "CCdcAcmClass::HandleSetCommFeature;ret=%d", ret );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLESETCOMMFEATURE_EXIT_DUP1 );
	return ret;
	}

TInt CCdcAcmClass::HandleGetLineCoding(TDes8& aReturnData)
/**
 * Callback for Get Line Coding requests.
 * Note that this function returns the configuration set by the host, rather 
 * than the one configured by the C32 client.
 *
 * @param aReturnData Descriptor containing the data rate, number of stop 
 * bits, parity and data bits.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLEGETLINECODING_ENTRY );
	aReturnData.SetLength(7);

	TUint8* pbuffer;
	TUint8** ppbuffer;
	pbuffer = &aReturnData[0];
	ppbuffer = &pbuffer;

	CCdcControlInterface::PutU32(ppbuffer,		   iUsbConfig.iRate    );
	CCdcControlInterface::PutU08(ppbuffer, static_cast<TUint8>(iUsbConfig.iStopBits));
	CCdcControlInterface::PutU08(ppbuffer, static_cast<TUint8>(iUsbConfig.iParity  ));
	CCdcControlInterface::PutU08(ppbuffer, static_cast<TUint8>(iUsbConfig.iDataBits));

	OstTraceFunctionExit0( CCDCACMCLASS_HANDLEGETLINECODING_EXIT );
	return KErrNone;
	}

TInt CCdcAcmClass::HandleSetControlLineState(TBool aRtsState, TBool aDtrState)
/**
 * Callback for Set Control Line State requests
 *
 * @param aRtsState RTS state.
 * @param aDtrState DTR state.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLESETCONTROLLINESTATE_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCONTROLLINESTATE, 
			"CCdcAcmClass::HandleSetControlLineState;aRtsState=%d;aDtrState=%d", (TInt)aRtsState, (TInt)aDtrState );

	iRtsState	= aRtsState;
	iDtrState	= aDtrState;
	if ( iCallback )
		{
		iCallback->HostSignalChange(iDtrState,iRtsState);
		}

	OstTrace0( TRACE_NORMAL, CCDCACMCLASS_HANDLESETCONTROLLINESTATE_DUP1, "CCdcAcmClass::HandleSetControlLineState;ret=0" );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLESETCONTROLLINESTATE_EXIT );
	return KErrNone;
	}

TInt CCdcAcmClass::HandleSendBreak(TUint16 aDuration)
/**
 * Callback for Send Break requests
 *
 * @param aDuration Duration of the break in milliseconds.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_HANDLESENDBREAK_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLESENDBREAK, "CCdcAcmClass::HandleSendBreak;aDuration=%d", (TInt)aDuration );
	TInt ret = KErrNone;

	// timing value as given is checked for 'special' values
	if ( aDuration == 0 )
		{
		ret = iBreakController->BreakRequest(CBreakController::EHost,
			CBreakController::EInactive);
		}
	else if ( aDuration == 0xFFFF )
		{
		ret = iBreakController->BreakRequest(CBreakController::EHost,
			CBreakController::ELocked); // i.e. locked ON, until further 
		// notice.
		}
	else
		{
		ret = iBreakController->BreakRequest(CBreakController::EHost, 
			CBreakController::ETiming,
			// Convert from host-supplied milliseconds to microseconds.
			aDuration*1000);
		}

	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_HANDLESENDBREAK_DUP1, "CCdcAcmClass::HandleSendBreak;ret=%d", ret );
	OstTraceFunctionExit0( CCDCACMCLASS_HANDLESENDBREAK_EXIT );
	return ret;
	}

TInt CCdcAcmClass::SendSerialState(TBool aRing, TBool aDsr, TBool aDcd)
/**
 * Send a change in serial state to the host.
 *
 * Note that Overrun, Parity and Framing errors are not currently supported, 
 * so are stubbed out with EFalse.
 *
 * Note that the BREAK signal is managed locally in the break controller
 * Active Object.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_SENDSERIALSTATE_ENTRY );
	OstTraceExt3( TRACE_NORMAL, CCDCACMCLASS_SENDSERIALSTATE, 
			"CCdcAcmClass::SendSerialState;aRing=%d;aDsr=%d;aDcd=%d", (TInt)aRing, (TInt)aDsr, (TInt)aDcd );

	// stub non-supported flags
	TBool overrun = EFalse;
	TBool parity  = EFalse;
	TBool framing = EFalse;

	// Save incoming state flags for possible use when notifying break
	// changes.
	iRingState = aRing;
	iDsrState  = aDsr;
	iDcdState  = aDcd;

	// send new state off to the host (this will use an interrupt transfer, 
	// and will handle all discrepancy checking to suppress same-state 
	// notifications)
	TInt ret = iControl->SendSerialState(overrun,
		parity,
		framing,
		aRing,
		BreakActive(),
		aDsr,
		aDcd);

	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_SENDSERIALSTATE_DUP1, "CCdcAcmClass::SendSerialState;ret=%d", ret );
	OstTraceFunctionExit0( CCDCACMCLASS_SENDSERIALSTATE_EXIT );
	return ret;
	}

TBool CCdcAcmClass::BreakActive() const
/**
 * Simply pass the state of the local BREAK flag back to caller
 *
 * @return Break flag.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_BREAKACTIVE_ENTRY );
	OstTraceFunctionExit0( CCDCACMCLASS_BREAKACTIVE_EXIT );
	return iBreakActive;
	}

TBool CCdcAcmClass::RingState() const
/**
 * Accessor for the ring state.
 *
 * @return Whether RNG is set or not.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_RINGSTATE_ENTRY );
	OstTraceFunctionExit0( CCDCACMCLASS_RINGSTATE_EXIT );
	return iRingState;
	}

TBool CCdcAcmClass::DsrState() const
/**
 * Accessor for the DSR state.
 *
 * @return Whether DSR is set or not.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_DSRSTATE_ENTRY );
	OstTraceFunctionExit0( CCDCACMCLASS_DSRSTATE_EXIT );
	return iDsrState;
	}

TBool CCdcAcmClass::DcdState() const
/**
 * Accessor for the DCD state.
 *
 * @return Whether DCD is set or not.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_DCDSTATE_ENTRY );
	OstTraceFunctionExit0( CCDCACMCLASS_DCDSTATE_EXIT );
	return iDcdState;
	}

void CCdcAcmClass::SetBreakActive(TBool aBreakActive)
/**
 * Sets the break flag on or off.
 *
 * @param aBreakActive The break flag is set to this value.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_SETBREAKACTIVE_ENTRY );
	iBreakActive = aBreakActive;
	OstTraceFunctionExit0( CCDCACMCLASS_SETBREAKACTIVE_EXIT );
	}

MHostPushedChangeObserver* CCdcAcmClass::Callback()
/**
 * Accessor for the MHostPushedChangeObserver callback.
 *
 * @return The observer of host-pushed changes.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_CALLBACK_ENTRY );
	OstTraceFunctionExit0( CCDCACMCLASS_CALLBACK_EXIT );
	return iCallback;
	}

MBreakObserver* CCdcAcmClass::BreakCallback()
/**
 * Accessor for the MBreakObserver callback.
 *
 * @return The observer of break changes.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_BREAKCALLBACK_ENTRY );
	OstTraceFunctionExit0( CCDCACMCLASS_BREAKCALLBACK_EXIT );
	return iBreakCallback;
	}

TInt CCdcAcmClass::BreakRequest(CBreakController::TRequester aRequester, 
								CBreakController::TState aState, 
								TTimeIntervalMicroSeconds32 aDelay)
/**
 * Make a break-related request.
 *
 * @param aRequester The entity requesting the break.
 * @param aState The request- either a locked break, a timed break, or to 
 * make the break inactive.
 * @param aDelay The time delay, only used for a timed break.
 * @return Error, for instance if a different entity already owns the break.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_BREAKREQUEST_ENTRY );

	if (!iBreakController)
		{
		OstTraceExt1( TRACE_FATAL, CCDCACMCLASS_BREAKREQUEST, "CCdcAcmClass::BreakRequest;iBreakController=%p", iBreakController );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	TInt err = iBreakController->BreakRequest(aRequester, aState, aDelay);
	OstTrace1( TRACE_NORMAL, CCDCACMCLASS_BREAKREQUEST_DUP1, "CCdcAcmClass::BreakRequest;err=%d", err );
	OstTraceFunctionExit0( CCDCACMCLASS_BREAKREQUEST_EXIT );
	return err;
	}

void CCdcAcmClass::NotifyDataAvailable(MNotifyDataAvailableObserver& aObserver)
/**
 * Notify the caller when data is available to be read from the LDD.
 *
 * @param aObserver The observer to notify completion to.
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_NOTIFYDATAAVAILABLE_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCACMCLASS_NOTIFYDATAAVAILABLE, "CCdcAcmClass::NotifyDataAvailable;aObserver=%p", &aObserver );

	if (!iData)
		{
		OstTraceExt1( TRACE_FATAL, CCDCACMCLASS_NOTIFYDATAAVAILABLE_DUP1, "CCdcAcmClass::NotifyDataAvailable;iData=%p", iData );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iData->NotifyDataAvailable(aObserver);
	OstTraceFunctionExit0( CCDCACMCLASS_NOTIFYDATAAVAILABLE_EXIT );
	}

void CCdcAcmClass::NotifyDataAvailableCancel()
/**
 * Cancel the request to be notified (when data is available to be read from the LDD).
 */
	{
	OstTraceFunctionEntry0( CCDCACMCLASS_NOTIFYDATAAVAILABLECANCEL_ENTRY );
	if (!iData)
		{
		OstTraceExt1( TRACE_FATAL, CCDCACMCLASS_NOTIFYDATAAVAILABLECANCEL, "CCdcAcmClass::NotifyDataAvailableCancel;iData=%p", iData );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iData->CancelNotifyDataAvailable();
	OstTraceFunctionExit0( CCDCACMCLASS_NOTIFYDATAAVAILABLECANCEL_EXIT );
	}

//
// End of file
