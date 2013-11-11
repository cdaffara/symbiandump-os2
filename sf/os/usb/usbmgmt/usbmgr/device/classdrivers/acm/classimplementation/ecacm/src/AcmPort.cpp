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

#include <acminterface.h>
#include "AcmPort.h"
#include "AcmPortFactory.h"
#include "AcmUtils.h"
#include "AcmWriter.h"
#include "AcmReader.h"
#include "AcmPanic.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "AcmPortTraces.h"
#endif

const TUint KCapsRate=(   KCapsBps50
						| KCapsBps75
						| KCapsBps110
						| KCapsBps134
						| KCapsBps150
						| KCapsBps300
						| KCapsBps600
						| KCapsBps1200
						| KCapsBps1800
						| KCapsBps2000
						| KCapsBps2400
						| KCapsBps3600
						| KCapsBps4800
						| KCapsBps7200
						| KCapsBps9600
						| KCapsBps19200
						| KCapsBps38400
						| KCapsBps57600
						| KCapsBps115200
						| KCapsBps230400
						| KCapsBps460800
						| KCapsBps576000
						| KCapsBps115200
						| KCapsBps4000000 );
const TUint KCapsDataBits=KCapsData8;		
const TUint KCapsStopBits=KCapsStop1;		
const TUint KCapsParity=KCapsParityNone;	
const TUint KCapsHandshaking=(
						  KCapsSignalCTSSupported
						| KCapsSignalDSRSupported
						| KCapsSignalRTSSupported
						| KCapsSignalDTRSupported); 
const TUint KCapsSignals=(
							KCapsSignalCTSSupported
						|	KCapsSignalDSRSupported
						|	KCapsSignalRTSSupported
						|	KCapsSignalDTRSupported);
const TUint KCapsFifo = 0;
const TUint KCapsSIR = 0; ///< Serial Infrared capabilities
const TUint KCapsNotification = KNotifySignalsChangeSupported;			
const TUint KCapsRoles = 0; 				
const TUint KCapsFlowControl = 0;							

const TBps		KDefaultDataRate = EBps115200;		
const TDataBits KDefaultDataBits = EData8;			
const TParity	KDefaultParity	 = EParityNone; 	
const TStopBits KDefaultStopBits = EStop1;			
const TUint 	KDefaultHandshake = 0;	
///< Default XON character is <ctrl>Q				
const TText8	KDefaultXon 	 = 17; 
///< Default XOFF character is <ctrl>S
const TText8	KDefaultXoff	 = 19; 

// Starting size of each of the receive and transmit buffers.
const TUint KDefaultBufferSize = 0x1000;

CAcmPort* CAcmPort::NewL(const TUint aUnit, MAcmPortObserver& aFactory)
/**
 * Make a new CPort for the comm server
 *
 * @param aFactory The observer of the port, to be notified when the port 
 * closes.
 * @param aUnit The port number.
 * @return Ownership of a newly created CAcmPort object
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NEWL_ENTRY );
	
	CAcmPort* self = new(ELeave) CAcmPort(aUnit, aFactory);
	CleanupClosePushL(*self);
	self->ConstructL();
	CleanupStack::Pop();
	OstTraceFunctionExit0( CACMPORT_NEWL_EXIT );
	return self;
	}

void CAcmPort::ConstructL()
/**
 * Standard second phase constructor. Create owned classes and initialise the 
 * port.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_CONSTRUCTL_ENTRY );
	
	iReader = CAcmReader::NewL(*this, KDefaultBufferSize);
	iWriter = CAcmWriter::NewL(*this, KDefaultBufferSize);

	TName name;
	TInt	err;
	name.Num(iUnit);
	err = SetName(&name);
	if (err < 0)
		{
		OstTrace1( TRACE_FATAL, CACMPORT_CONSTRUCTL, "CAcmPort::ConstructL;err=%d", err );
		User::Leave(err);
		}

	iCommServerConfig.iBufFlags = 0;
	iCommServerConfig.iBufSize = iReader->BufSize();

	iCommConfig.iRate			= KDefaultDataRate;
	iCommConfig.iDataBits		= KDefaultDataBits;
	iCommConfig.iParity 		= KDefaultParity;
	iCommConfig.iStopBits		= KDefaultStopBits;
	iCommConfig.iHandshake		= KDefaultHandshake;
	iCommConfig.iParityError	= 0;
	iCommConfig.iFifo			= 0;
	iCommConfig.iSpecialRate	= 0;
	iCommConfig.iTerminatorCount= 0;
	iCommConfig.iXonChar		= KDefaultXon;
	iCommConfig.iXoffChar		= KDefaultXoff;
	iCommConfig.iParityErrorChar= 0;
	iCommConfig.iSIREnable		= ESIRDisable;
	iCommConfig.iSIRSettings	= 0;
	OstTraceFunctionExit0( CACMPORT_CONSTRUCTL_EXIT );
	}

CAcmPort::CAcmPort(const TUint aUnit, MAcmPortObserver& aObserver) 
 :	iCommNotification(iCommNotificationDes()),
	iObserver(aObserver),
	iUnit(aUnit)
/**
 * Constructor.
 *
 * @param aObserver The observer of the port, to be notified when the port 
 * closes.
 * @param aUnit The port number.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_CACMPORT_CONS_ENTRY );
	OstTraceFunctionExit0( CACMPORT_CACMPORT_DES_EXIT );
	}

void CAcmPort::StartRead(const TAny* aClientBuffer, TInt aLength)
/**
 * Downcall from C32. Queue a read.
 *
 * @param aClientBuffer pointer to the client's buffer
 * @param aLength number of bytes to read
 */
	{
	OstTraceFunctionEntry0( CACMPORT_STARTREAD_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CACMPORT_STARTREAD, "CAcmPort::StartRead;aClientBuffer=%p;aLength=%d", aClientBuffer, aLength );

	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_STARTREAD_DUP1, "CAcmPort::StartRead;\t***access denied" );
		ReadCompleted(KErrAccessDenied);
		OstTraceFunctionExit0( CACMPORT_STARTREAD_EXIT );
		return;
		}

	// Analyse the request and call the relevant API on the data reader. NB We 
	// do not pass requests for zero bytes to the data reader. They are an 
	// RComm oddity we should handle here.
	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_STARTREAD_DUP3, "CAcmPort::StartRead;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	if(iReader->IsNotifyDataAvailableQueryPending())
		{
		ReadCompleted(KErrInUse);
		OstTraceFunctionExit0( CACMPORT_STARTREAD_EXIT_DUP1 );
		return;
		}

	if ( aLength < 0 )
		{
		iReader->ReadOneOrMore(aClientBuffer, static_cast<TUint>(-aLength));
		}
	else if ( aLength > 0 )
		{
		iReader->Read(aClientBuffer, static_cast<TUint>(aLength));
		}
	else
		{
		// Obscure RComm API feature- complete zero-length Read immediately, 
		// to indicate that the hardware is powered up.
		OstTrace0( TRACE_NORMAL, CACMPORT_STARTREAD_DUP2, "CAcmPort::StartRead;\tcompleting immediately with KErrNone" );
		ReadCompleted(KErrNone);
		}
	OstTraceFunctionExit0( CACMPORT_STARTREAD_EXIT_DUP2 );
	}

void CAcmPort::ReadCancel()
/**
 * Downcall from C32. Cancel a read.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_READCANCEL_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_READCANCEL, "CAcmPort::ReadCancel;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_READCANCEL_EXIT );
		return;
		}

	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_READCANCEL_DUP1, "CAcmPort::ReadCancel;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iReader->ReadCancel();
	OstTraceFunctionExit0( CACMPORT_READCANCEL_EXIT_DUP1 );
	}

TInt CAcmPort::QueryReceiveBuffer(TInt& aLength) const
/**
 * Downcall from C32. Get the amount of data in the receive buffer.
 *
 * @param aLength reference to where the amount will be written to
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_QUERYRECEIVEBUFFER_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_QUERYRECEIVEBUFFER, "CAcmPort::QueryReceiveBuffer;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_QUERYRECEIVEBUFFER_EXIT );
		return KErrAccessDenied;
		}

	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_QUERYRECEIVEBUFFER_DUP2, "CAcmPort::QueryReceiveBuffer;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	aLength = static_cast<TInt>(iReader->BufLen());
	OstTrace1( TRACE_NORMAL, CACMPORT_QUERYRECEIVEBUFFER_DUP1, "CAcmPort::QueryReceiveBuffer;aLength=%d", aLength );
	OstTraceFunctionExit0( CACMPORT_QUERYRECEIVEBUFFER_EXIT_DUP1 );
	return KErrNone;
	}

void CAcmPort::ResetBuffers(TUint aFlags)
/**
 * Downcall from C32. Reset zero or more of the Tx and Rx buffers.
 *
 * @param aFlags Flags indicating which buffer(s) to reset.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_RESETBUFFERS_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORT_RESETBUFFERS, "CAcmPort::ResetBuffers;aFlags=%d", aFlags );

	if ( aFlags & KCommResetRx )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_RESETBUFFERS_DUP1, "CAcmPort::ResetBuffers;\tresetting Rx buffer" );
		if (!iReader)
			{
			OstTraceExt1( TRACE_FATAL, CACMPORT_RESETBUFFERS_DUP3, "CAcmPort::ResetBuffers;iReader=%p", iReader );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		iReader->ResetBuffer();
		}

	if ( aFlags & KCommResetTx )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_RESETBUFFERS_DUP2, "CAcmPort::ResetBuffers;\tresetting Tx buffer" );
		if (!iWriter)
			{
			OstTraceExt1( TRACE_FATAL, CACMPORT_RESETBUFFERS_DUP4, "CAcmPort::ResetBuffers;iWriter=%p", iWriter );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		iWriter->ResetBuffer();
		}

	OstTraceFunctionExit0( CACMPORT_RESETBUFFERS_EXIT );
	}

void CAcmPort::StartWrite(const TAny* aClientBuffer, TInt aLength)
/**
 * Downcall from C32. Queue a write
 *
 * @param aClientBuffer pointer to the Client's buffer
 * @param aLength number of bytes to write
 */
	{
	OstTraceFunctionEntry0( CACMPORT_STARTWRITE_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CACMPORT_STARTWRITE, "CAcmPort::StartWrite;aClientBuffer=%p;aLength=%d", aClientBuffer, aLength );

	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_STARTWRITE_DUP1, "CAcmPort::StartWrite;\t***access denied" );
		WriteCompleted(KErrAccessDenied);
		OstTraceFunctionExit0( CACMPORT_STARTWRITE_EXIT );
		return;
		}

	if ( aLength < 0 )
		{
		// Negative length makes no sense.
		OstTrace0( TRACE_NORMAL, CACMPORT_STARTWRITE_DUP2, "CAcmPort::StartWrite;\taLength is negative-completing immediately with KErrArgument" );
		WriteCompleted(KErrArgument);
		OstTraceFunctionExit0( CACMPORT_STARTWRITE_EXIT_DUP1 );
		return;
		}

	// NB We pass zero-byte writes down to the LDD as normal. This results in 
	// a zero-length packet being sent to the host.
	if (!iWriter)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_STARTWRITE_DUP3, "CAcmPort::StartWrite;iWriter=%p", iWriter );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iWriter->Write(aClientBuffer, static_cast<TUint>(aLength));
	OstTraceFunctionExit0( CACMPORT_STARTWRITE_EXIT_DUP2 );
	}

void CAcmPort::WriteCancel()
/**
 * Downcall from C32. Cancel a pending write
 */
	{
	OstTraceFunctionEntry0( CACMPORT_WRITECANCEL_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_WRITECANCEL, "CAcmPort::WriteCancel;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_WRITECANCEL_EXIT );
		return;
		}

	if (!iWriter)
		{
		OstTraceExt1( TRACE_FATAL,CACMPORT_WRITECANCEL_DUP1, "CAcmPort::WriteCancel;iWriter=%p", iWriter );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iWriter->WriteCancel();
	OstTraceFunctionExit0( CACMPORT_WRITECANCEL_EXIT_DUP1 );
	}

void CAcmPort::Break(TInt aTime)
/**
 * Downcall from C32. Send a break signal to the host.
 * Note that it is assumed that the NotifyBreak() request only applies to 
 * break signals sent from the host.
 *
 * @param aTime Length of break in microseconds
 */
	{
	OstTraceFunctionEntry0( CACMPORT_BREAK_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORT_BREAK, "CAcmPort::Break;\taTime=%d (microseconds)", aTime );

	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_BREAK_DUP1, "CAcmPort::Break;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_BREAK_EXIT );
		return;
		}

	iBreak = ETrue;
	iCancellingBreak = EFalse;

	// Use the time value directly as microseconds are given. We are a USB 
	// 'device' so the RComm client is always on the device side.
	TInt err = iAcm->BreakRequest(CBreakController::EDevice,
		CBreakController::ETiming,
		TTimeIntervalMicroSeconds32(aTime));
	OstTrace1( TRACE_NORMAL, CACMPORT_BREAK_DUP2, "CAcmPort::Break;\tBreakRequest = %d", err );
	// Note that the break controller may refuse our request if a host-driven 
	// break is outstanding.
	if ( err )
		{
		OstTrace1( TRACE_NORMAL, CACMPORT_BREAK_DUP3, "CAcmPort::Break;\tcalling BreakCompleted with %d", err );
		iBreak = EFalse;
		BreakCompleted(err);
		}						
	OstTraceFunctionExit0( CACMPORT_BREAK_EXIT_DUP1 );
	}

void CAcmPort::BreakCancel()
/**
 * Downcall from C32. Cancel a pending break.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_BREAKCANCEL_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_BREAKCANCEL, "CAcmPort::BreakCancel;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_BREAKCANCEL_EXIT );
		return;
		}
	
	iCancellingBreak = ETrue;
	TInt err = iAcm->BreakRequest(CBreakController::EDevice,
		CBreakController::EInactive);
	// Note that the device cannot turn off a break if there's a host-driven 
	// break in progress.
	OstTrace1( TRACE_NORMAL, CACMPORT_BREAKCANCEL_DUP1, "CAcmPort::BreakCancel;\tBreakRequest = %d", err );
	if ( err )
		{
		iCancellingBreak = EFalse;
		}

	// Whether BreakOff worked or not, reset our flag saying we're no longer 
	// interested in any subsequent completion anyway.
	iBreak = EFalse;
	OstTraceFunctionExit0( CACMPORT_BREAKCANCEL_EXIT_DUP1 );
	}

TInt CAcmPort::GetConfig(TDes8& aDes) const
/**
 * Downcall from C32. Pass a config request. Only supports V01.
 *
 * @param aDes config will be written to this descriptor 
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_GETCONFIG_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETCONFIG, "CAcmPort::GetConfig;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_GETCONFIG_EXIT );
		return KErrAccessDenied;
		}

	if ( aDes.Length() < static_cast<TInt>(sizeof(TCommConfigV01)) )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP12, "CAcmPort::GetConfig;\t***not supported" );
		OstTraceFunctionExit0( CACMPORT_GETCONFIG_EXIT_DUP1 );
		return KErrNotSupported;
		}

	TCommConfig commConfigPckg;
	TCommConfigV01& commConfig = commConfigPckg();
	commConfig = iCommConfig;
	aDes.Copy(commConfigPckg);
	
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP1, "CAcmPort::GetConfig;\tiCommConfig.iRate=%d", iCommConfig.iRate );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP2, "CAcmPort::GetConfig;\tiCommConfig.iDataBits = %d", iCommConfig.iDataBits );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP3, "CAcmPort::GetConfig;\tiCommConfig.iStopBits = %d", iCommConfig.iStopBits );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP4, "CAcmPort::GetConfig;\tiCommConfig.iParity = %d", iCommConfig.iParity );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP5, "CAcmPort::GetConfig;\tiCommConfig.iHandshake = %d", iCommConfig.iHandshake );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP6, "CAcmPort::GetConfig;\tiCommConfig.iParityError = %d", iCommConfig.iParityError );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP7, "CAcmPort::GetConfig;\tiCommConfig.iFifo = %d", iCommConfig.iFifo );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP8, "CAcmPort::GetConfig;\tiCommConfig.iSpecialRate = %d", iCommConfig.iSpecialRate );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP9, "CAcmPort::GetConfig;\tiCommConfig.iTerminatorCount = %d", iCommConfig.iTerminatorCount );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP10, "CAcmPort::GetConfig;\tiCommConfig.iSIREnable = %d", iCommConfig.iSIREnable );
	OstTrace1( TRACE_NORMAL, CACMPORT_GETCONFIG_DUP11, "CAcmPort::GetConfig;\tiCommConfig.iSIRSettings = %d", iCommConfig.iSIRSettings );

	OstTraceFunctionExit0( CACMPORT_GETCONFIG_EXIT_DUP2 );
	return KErrNone;
	}

TInt CAcmPort::SetConfig(const TDesC8& aDes)
/**
 * Downcall from C32. Set config on the port using a TCommConfigV01 package.
 *
 * @param aDes descriptor containing the new config
 * @return Error
 */
	{
	OstTraceFunctionEntry0( CACMPORT_SETCONFIG_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETCONFIG, "CAcmPort::SetConfig;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_SETCONFIG_EXIT );
		return KErrAccessDenied;
		}

	OstTraceExt3( TRACE_NORMAL, CACMPORT_SETCONFIG_DUP1, "CAcmPort::SetConfig;\tlength of argument=%d;TCommConfigV01=%d;TCommConfigV02=%d", aDes.Length(), (TInt)sizeof(TCommConfigV01), (TInt)sizeof(TCommConfigV02) );
	
	if ( aDes.Length() < static_cast<TInt>(sizeof(TCommConfigV01)) )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETCONFIG_DUP2, "CAcmPort::SetConfig;\t***not supported" );
		OstTraceFunctionExit0( CACMPORT_SETCONFIG_EXIT_DUP1 );
		return KErrNotSupported;
		}

	TCommConfig configPckg;
	configPckg.Copy(aDes);
	TCommConfigV01& config = configPckg();

	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP3, "CAcmPort::SetConfig;\tconfig.iRate=%d", config.iRate );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP4, "CAcmPort::SetConfig;\tconfig.iDataBits=%d", config.iDataBits );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP5, "CAcmPort::SetConfig;\tconfig.iStopBits=%d", config.iStopBits );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP6, "CAcmPort::SetConfig;\tconfig.iParity=%d", config.iParity );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP7, "CAcmPort::SetConfig;\tconfig.iHandshake=%d", config.iHandshake );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP8, "CAcmPort::SetConfig;\tconfig.iParityError=%d", config.iParityError );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP9, "CAcmPort::SetConfig;\tconfig.iFifo=%d", config.iFifo );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP10, "CAcmPort::SetConfig;\tconfig.iSpecialRate=%d", config.iSpecialRate );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP11, "CAcmPort::SetConfig;\tconfig.iTerminatorCount=%d", config.iTerminatorCount );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP12, "CAcmPort::SetConfig;\tconfig.iSIREnable=%d", config.iSIREnable );
	OstTrace1( TRACE_DUMP, CACMPORT_SETCONFIG_DUP13, "CAcmPort::SetConfig;\tconfig.iSIRSettings=%d", config.iSIRSettings );

	// Tell the reader object about the new terminators. Pass the whole config 
	// struct by reference for ease.
	iReader->SetTerminators(config);

	HandleConfigNotification(config.iRate,
		config.iDataBits,
		config.iParity,
		config.iStopBits,
		config.iHandshake);
	iCommConfig = config;

	OstTraceFunctionExit0( CACMPORT_SETCONFIG_EXIT_DUP2 );
	return KErrNone;
	}

void CAcmPort::HandleConfigNotification(TBps aRate,
										TDataBits aDataBits,
										TParity aParity,
										TStopBits aStopBits,
										TUint aHandshake)
/**
 * Construct and complete the config notification data structure.
 * @param aRate 		New data rate
 * @param aDataBits 	New number of data bits
 * @param aParity		New parity check setting
 * @param aStopBits 	New stop bit setting
 * @param aHandshake	New handshake setting
 */
	{
	OstTraceFunctionEntry0( CACMPORT_HANDLECONFIGNOTIFICATION_ENTRY );
	
	OstTrace0( TRACE_DUMP, CACMPORT_HANDLECONFIGNOTIFICATION, "CAcmPort::HandleConfigNotification;>>CAcmPort::HandleConfigNotification" );
	OstTrace1( TRACE_DUMP, CACMPORT_HANDLECONFIGNOTIFICATION_DUP1, "CAcmPort::HandleConfigNotification;aRate=%d", aRate );
	OstTrace1( TRACE_DUMP, CACMPORT_HANDLECONFIGNOTIFICATION_DUP2, "CAcmPort::HandleConfigNotification;aDataBits=%d", aDataBits );
	OstTrace1( TRACE_DUMP, CACMPORT_HANDLECONFIGNOTIFICATION_DUP3, "CAcmPort::HandleConfigNotification;aStopBits=%d", aStopBits );
	OstTrace1( TRACE_DUMP, CACMPORT_HANDLECONFIGNOTIFICATION_DUP4, "CAcmPort::HandleConfigNotification;aParity=%d", aParity );
	OstTrace1( TRACE_DUMP, CACMPORT_HANDLECONFIGNOTIFICATION_DUP5, "CAcmPort::HandleConfigNotification;aHandshake=%d", aHandshake );
			
	iCommNotification.iChangedMembers = 0;

	iCommNotification.iRate = aRate;
	if ( iCommConfig.iRate != aRate )
		{
		iCommNotification.iChangedMembers |= KRateChanged;
		}

	iCommNotification.iDataBits = aDataBits;
	if ( iCommConfig.iDataBits != aDataBits )
		{
		iCommNotification.iChangedMembers |= KDataFormatChanged;
		}

	iCommNotification.iParity = aParity;
	if ( iCommConfig.iParity != aParity )
		{
		iCommNotification.iChangedMembers |= KDataFormatChanged;
		}

	iCommNotification.iStopBits = aStopBits;
	if ( iCommConfig.iStopBits != aStopBits )
		{
		iCommNotification.iChangedMembers |= KDataFormatChanged;
		}

	iCommNotification.iHandshake = aHandshake;
	if ( iCommConfig.iHandshake != aHandshake )
		{
		iCommNotification.iChangedMembers |= KHandshakeChanged;
		}

	if ( iCommNotification.iChangedMembers )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_HANDLECONFIGNOTIFICATION_DUP6, "CAcmPort::HandleConfigNotification;\tcalling ConfigChangeCompleted with KErrNone" );
		ConfigChangeCompleted(iCommNotificationDes,KErrNone);
		iNotifyConfigChange = EFalse;
		}

	OstTraceFunctionExit0( CACMPORT_HANDLECONFIGNOTIFICATION_EXIT );
	}

TInt CAcmPort::SetServerConfig(const TDesC8& aDes)
/**
 * Downcall from C32. Set server config using a TCommServerConfigV01 package.
 *
 * @param aDes package with new server configurations
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_SETSERVERCONFIG_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETSERVERCONFIG, "CAcmPort::SetServerConfig;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_SETSERVERCONFIG_EXIT );
		return KErrAccessDenied;
		}

	if ( aDes.Length() < static_cast<TInt>(sizeof(TCommServerConfigV01)) )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETSERVERCONFIG_DUP1, "CAcmPort::SetServerConfig;\t***not supported" );
		OstTraceFunctionExit0( CACMPORT_SETSERVERCONFIG_EXIT_DUP1 );
		return KErrNotSupported;
		}

	TCommServerConfig serverConfigPckg;
	serverConfigPckg.Copy(aDes);
	TCommServerConfigV01& serverConfig = serverConfigPckg();

	if ( serverConfig.iBufFlags != KCommBufferFull )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETSERVERCONFIG_DUP2, "CAcmPort::SetServerConfig;\t***not supported" );
		OstTraceFunctionExit0( CACMPORT_SETSERVERCONFIG_EXIT_DUP2 );
		return KErrNotSupported;
		}

	// Apply the buffer size setting to Rx and Tx buffers. 
	TInt err = DoSetBufferLengths(serverConfig.iBufSize);
	if ( err )
		{
		// Failure- the buffer lengths will have been left as they were, so 
		// just return error.
		OstTrace1( TRACE_NORMAL, CACMPORT_SETSERVERCONFIG_DUP3, "CAcmPort::SetServerConfig;\t***DoSetBufferLengths=%d", err );
		OstTraceFunctionExit0( CACMPORT_SETSERVERCONFIG_EXIT_DUP3 );
		return err;
		}

	// Changed buffer sizes OK. Note that new config.
	iCommServerConfig = serverConfig;
	OstTraceFunctionExit0( CACMPORT_SETSERVERCONFIG_EXIT_DUP4 );
	return KErrNone;
	}

TInt CAcmPort::GetServerConfig(TDes8& aDes)
/**
 * Downcall from C32. Get the server configs. Only supports V01.
 *
 * @param aDes server configs will be written to this descriptor
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_GETSERVERCONFIG_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETSERVERCONFIG, "CAcmPort::GetServerConfig;\t***access denied" );	
		OstTraceFunctionExit0( CACMPORT_GETSERVERCONFIG_EXIT );
		return KErrAccessDenied;
		}

	if ( aDes.Length() < static_cast<TInt>(sizeof(TCommServerConfigV01)) )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETSERVERCONFIG_DUP1, "CAcmPort::GetServerConfig;\t***not supported" );
		OstTraceFunctionExit0( CACMPORT_GETSERVERCONFIG_EXIT_DUP1 );
		return KErrNotSupported;
		}

	TCommServerConfig* serverConfigPckg = 
		reinterpret_cast<TCommServerConfig*>(&aDes);
	TCommServerConfigV01& serverConfig = (*serverConfigPckg)();

	serverConfig = iCommServerConfig;
	OstTraceFunctionExit0( CACMPORT_GETSERVERCONFIG_EXIT_DUP2 );
	return KErrNone;
	}

TInt CAcmPort::GetCaps(TDes8& aDes)
/**
 * Downcall from C32. Get the port's capabilities. Supports V01 and V02. 
 *
 * @param aDes caps will be written to this descriptor
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_GETCAPS_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETCAPS, "CAcmPort::GetCaps;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_GETCAPS_EXIT );
		return KErrAccessDenied;
		}

	TCommCaps2 capsPckg;
	TCommCapsV02& caps = capsPckg();

	switch ( aDes.Length() )
		{
	case sizeof(TCommCapsV02):
		{
		caps.iNotificationCaps = (KCapsNotification | KNotifyDataAvailableSupported);
		caps.iRoleCaps = KCapsRoles;
		caps.iFlowControlCaps = KCapsFlowControl;
		}
		// no break is deliberate
	case sizeof(TCommCapsV01):
		{
		caps.iRate = KCapsRate;
		caps.iDataBits = KCapsDataBits;
		caps.iStopBits = KCapsStopBits;
		caps.iParity = KCapsParity;
		caps.iHandshake = KCapsHandshaking;
		caps.iSignals = KCapsSignals;
		caps.iFifo = KCapsFifo;
		caps.iSIR = KCapsSIR;
		}
		break;
	default:
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETCAPS_DUP1, "CAcmPort::GetCaps;\t***bad argument" );
		OstTraceFunctionExit0( CACMPORT_GETCAPS_EXIT_DUP1 );
		return KErrArgument;
		}
		}

	aDes.Copy(capsPckg.Left(aDes.Length()));

	OstTraceFunctionExit0( CACMPORT_GETCAPS_EXIT_DUP2 );
	return KErrNone;
	}

TInt CAcmPort::GetSignals(TUint& aSignals)
/**
 * Downcall from C32. Get the status of the signal pins
 *
 * @param aSignals signals will be written to this descriptor
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_GETSIGNALS_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETSIGNALS, "CAcmPort::GetSignals;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_GETSIGNALS_EXIT );
		return KErrAccessDenied;
		}

	aSignals = ConvertSignals(iSignals);

	OstTraceExt2( TRACE_NORMAL, CACMPORT_GETSIGNALS_DUP1, "CAcmPort::GetSignals;iSignals=%x;aSignals=%x", iSignals, (TUint32)aSignals );
	OstTraceFunctionExit0( CACMPORT_GETSIGNALS_EXIT_DUP1 );
	return KErrNone;
	}

TInt CAcmPort::SetSignalsToMark(TUint aSignals)
/**
 * Downcall from C32. Set selected signals to high (logical 1)
 *
 * @param aSignals bitmask with the signals to be set
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_SETSIGNALSTOMARK_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETSIGNALSTOMARK, "CAcmPort::SetSignalsToMark;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_SETSIGNALSTOMARK_EXIT );
		return KErrAccessDenied;
		}

	TUint32 newSignals = iSignals | ConvertAndFilterSignals(aSignals);
	TInt err = SetSignals(newSignals);
	if ( err )
		{
		OstTrace1( TRACE_NORMAL, CACMPORT_SETSIGNALSTOMARK_DUP1, "CAcmPort::SetSignalsToMark;***SetSignals = %d", err );
		OstTraceFunctionExit0( CACMPORT_SETSIGNALSTOMARK_EXIT_DUP1 );
		return err;
		}

	OstTraceExt2( TRACE_NORMAL, CACMPORT_SETSIGNALSTOMARK_DUP2, "CAcmPort::SetSignalsToMark;iSignals=%x;aSignals=%x", iSignals, (TUint32)aSignals );
	OstTraceFunctionExit0( CACMPORT_SETSIGNALSTOMARK_EXIT_DUP2 );
	return KErrNone;
	}

TInt CAcmPort::SetSignalsToSpace(TUint aSignals)
/**
 * Downcall from C32. Set selected signals to low (logical 0)
 *
 * @param aSignals bitmask with the signals to be cleared
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_SETSIGNALSTOSPACE_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETSIGNALSTOSPACE, "CAcmPort::SetSignalsToSpace;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_SETSIGNALSTOSPACE_EXIT );
		return KErrAccessDenied;
		}

	TUint32 newSignals = iSignals & ~ConvertAndFilterSignals(aSignals);
	TInt err = SetSignals(newSignals);
	if ( err )
		{
		OstTrace1( TRACE_NORMAL, CACMPORT_SETSIGNALSTOSPACE_DUP1, "CAcmPort::SetSignalsToSpace;***SetSignals = %d", err );
		OstTraceFunctionExit0( CACMPORT_SETSIGNALSTOSPACE_EXIT_DUP1 );
		return err;
		}

	OstTraceExt2( TRACE_NORMAL, CACMPORT_SETSIGNALSTOSPACE_DUP2, "CAcmPort::SetSignalsToSpace;iSignals=%x;aSignals=%x", iSignals, (TUint32)aSignals );
	OstTraceFunctionExit0( CACMPORT_SETSIGNALSTOSPACE_EXIT_DUP2 );
	return KErrNone;
	}

TInt CAcmPort::SetSignals(TUint32 aNewSignals)
/**
 * Handle a request by the client to change signal state. This function pushes 
 * the change to the host, completes any outstanding notifications, and stores
 * the new signals in iSignals if there are no errors.
 *
 * @param aSignals bitmask with the signals to be set
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_SETSIGNALS_ENTRY );

	TBool ring =((aNewSignals & KSignalRNG)==KSignalRNG);
	TBool dsr  =((aNewSignals & KSignalDSR)==KSignalDSR);
	TBool dcd  =((aNewSignals & KSignalDCD)==KSignalDCD);
 
 	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETSIGNALS, "CAcmPort::SetSignals;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_SETSIGNALS_EXIT );
		return KErrAccessDenied;
		}

	TInt err = iAcm->SendSerialState(ring,dsr,dcd);
	if ( err )
		{
		OstTrace1( TRACE_NORMAL, CACMPORT_SETSIGNALS_DUP1, "CAcmPort::SetSignals;\t***SendSerialState = %d- returning", err );
		OstTraceFunctionExit0( CACMPORT_SETSIGNALS_EXIT_DUP1 );
		return err;
		}

	// Report the new signals if they have changed and notification mask shows
	// the upper layer wanted to be sensitive to these signals
	if ( iNotifySignalChange )
		{
		TUint32 changedSignals = ( iSignals ^ aNewSignals ) & iNotifySignalMask;
		if ( changedSignals != 0 )
			{
			// Mark which signals have changed
			// Could do something like:
			//     changedSignalsMask = ( changedSignals & (KSignalRNG|KSignalDCD|KSignalDSR) ) * KSignalChanged
			// but this relies on the how the signal constants are formed in d32comm.h
			TUint32 changedSignalsMask = 0;
			if ( ( changedSignals & KSignalRNG ) != 0 )
				{
				changedSignalsMask |= KRNGChanged;
				}
			if ( ( changedSignals & KSignalDCD ) != 0 )
				{
				changedSignalsMask |= KDCDChanged;
				}
			if ( ( changedSignals & KSignalDSR ) != 0 )
				{
				changedSignalsMask |= KDSRChanged;
				}

			// Report correctly mapped signals that client asked for
			OstTrace0( TRACE_NORMAL, CACMPORT_SETSIGNALS_DUP2, "CAcmPort::SetSignals;\tcalling SignalChangeCompleted with KErrNone" );
			TUint32 signals = ConvertSignals ( changedSignalsMask | ( aNewSignals & iNotifySignalMask ) );
			SignalChangeCompleted ( signals, KErrNone );
			iNotifySignalChange = EFalse;
			}
		}

	// Store new signals in iSignals
	iSignals = aNewSignals;
	OstTraceFunctionExit0( CACMPORT_SETSIGNALS_EXIT_DUP2 );
	return KErrNone;
	}

TUint32 CAcmPort::ConvertAndFilterSignals(TUint32 aSignals) const
/**
 * This function maps from signals used for the port's current role
 * to the DCE signals used internally, filtering out signals that are
 * inputs for the current role.
 *
 * This would be used, for example, to parse the signals specified by the 
 * client for a "SetSignals" request before storage in the iSignals member 
 * variable (always DCE).
 *
 * @param aSignals Input signals.
 * @return The signals, with the lines switched round according to the port's 
 * role.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_CONVERTANDFILTERSIGNALS_ENTRY );
	
	TUint32 signals = 0;
	// note that this never allows the client to use this method
	// to diddle the BREAK state

	// handle role-specific mapping
	if ( iRole == ECommRoleDTE )
		{
		// Ignore DSR, DCD, RI and CTS as these are DTE inputs.

		// Map DTR to DSR
		if((aSignals&KSignalDTR)==KSignalDTR)
			{
			signals|=KSignalDSR;
			}

		// Map RTS to CTS
		if((aSignals&KSignalRTS)==KSignalRTS)
			{
			signals|=KSignalCTS;
			}
		}
	else	// iRole==EDce
		{
		// Ignore RTS and DTR as these are DCE inputs and therefore cannot be set.
		// Keep mapping of DSR, CTS, DCD and RI.
		signals |= aSignals & 
			(	KSignalDSR
			|	KSignalCTS
			|	KSignalDCD
			|	KSignalRNG);
		}

	OstTraceFunctionExit0( CACMPORT_CONVERTANDFILTERSIGNALS_EXIT );
	return signals;
	}

TUint32 CAcmPort::ConvertSignals(TUint32 aSignals) const
/**
 * This function converts the given signals between the internal mapping
 * and the client mapping, taking into account the port's currnt role.
 * As the mapping is one to one, the input can be either the client or
 * internal signals.
 *
 * This would be used, for example, to parse the signals being sent to the 
 * client in a "GetSignals" request, or to parse the signals sent from the
 * client when setting up a notification.
 *
 * It should not be used for "SetSignals", as it does not take into account
 * what signals are inputs or outputs for the current role.
 *
 * @param aSignals Input signals (internal or client).
 * @return Output signals.
 * port is.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_CONVERTSIGNALS_ENTRY );
	
	// Swap signals around if the client is expecting DTE signalling
	if ( iRole == ECommRoleDTE )
		{
		TUint32 swappedSignals = 0;

		// Map DSR to DTR
		if ( ( aSignals & KSignalDSR ) == KSignalDSR )
			{
			swappedSignals |= KSignalDTR;
			}
		if ( ( aSignals & KSignalDTR ) == KSignalDTR )
			{
			swappedSignals |= KSignalDSR;
			}
		if ( ( aSignals & KDSRChanged ) != 0 )
			{
			swappedSignals |= KDTRChanged;
			}
		if ( ( aSignals & KDTRChanged ) != 0 )
			{
			swappedSignals |= KDSRChanged;
			}

		// Map CTS to RTS
		if ( ( aSignals&KSignalCTS ) == KSignalCTS )
			{
			swappedSignals|=KSignalRTS;
			}
		if ( ( aSignals&KSignalRTS ) == KSignalRTS )
			{
			swappedSignals|=KSignalCTS;
			}
		if ( ( aSignals & KCTSChanged ) != 0 )
			{
			swappedSignals |= KRTSChanged;
			}
		if ( ( aSignals & KRTSChanged ) != 0 )
			{
			swappedSignals |= KCTSChanged;
			}

		// Put remapped signals back in
		const TUint32 KSwapSignals = KSignalDSR | KSignalDTR | KSignalCTS | KSignalRTS
									| KDSRChanged | KDTRChanged | KCTSChanged | KRTSChanged;
		aSignals = ( aSignals & ~KSwapSignals ) | swappedSignals;
		}

	OstTraceFunctionExit0( CACMPORT_CONVERTSIGNALS_EXIT );
	return aSignals;
	}

TInt CAcmPort::GetReceiveBufferLength(TInt& aLength) const
/**
 * Downcall from C32. Get size of Rx buffer. Note that the Rx and Tx 
 * buffers are assumed by the RComm API to always have the same size. 
 * We don't check here, but the value returned should be the size of the Tx 
 * buffer also.
 *
 * @param aLength reference to where the length will be written to
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_GETRECEIVEBUFFERLENGTH_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETRECEIVEBUFFERLENGTH, "CAcmPort::GetReceiveBufferLength;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_GETRECEIVEBUFFERLENGTH_EXIT );
		return KErrAccessDenied;
		}

	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_GETRECEIVEBUFFERLENGTH_DUP2, "CAcmPort::GetReceiveBufferLength;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	aLength = static_cast<TInt>(iReader->BufSize());
	OstTrace1( TRACE_NORMAL, CACMPORT_GETRECEIVEBUFFERLENGTH_DUP1, "CAcmPort::GetReceiveBufferLength;aLength=%d", aLength );
	OstTraceFunctionExit0( CACMPORT_GETRECEIVEBUFFERLENGTH_EXIT_DUP1 );
	return KErrNone;
	}

TInt CAcmPort::DoSetBufferLengths(TUint aLength)
/**
 * Utility to set the sizes of the Rx and Tx buffers. On failure, the buffers 
 * will be left at the size they were.
 *
 * @param aLength The size required.
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_DOSETBUFFERLENGTHS_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORT_DOSETBUFFERLENGTHS, "CAcmPort::DoSetBufferLengths;aLength=%d", aLength );

	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_DOSETBUFFERLENGTHS_DUP5, "CAcmPort::DoSetBufferLengths;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	// Sart trying to resize buffers. Start with the reader. 
	// Before we start though, set some stuff up we may need later (see 
	// comments below).
	const TUint oldSize = iReader->BufSize();
	HBufC* dummyBuffer = HBufC::New(static_cast<TInt>(oldSize));
	if ( !dummyBuffer )
		{
		// If we can't allocate the dummy buffer, we can't guarantee that we 
		// can roll back this API safely if it fails halfway through. So abort 
		// the entire operation.
		OstTrace0( TRACE_NORMAL, CACMPORT_DOSETBUFFERLENGTHS_DUP1, "CAcmPort::DoSetBufferLengths;\t***failed to allocate dummy buffer- returning KErrNoMemory" );
		OstTraceFunctionExit0( CACMPORT_DOSETBUFFERLENGTHS_EXIT );
		return KErrNoMemory;
		}

	TInt ret = iReader->SetBufSize(aLength);
	if ( ret )
		{
		// A failure in SetBufSize will have maintained the old buffer 
		// (i.e. at its old size). This is a safe failure case- simply 
		// clean up and return the error to the client.
		delete dummyBuffer;
		OstTrace1( TRACE_NORMAL, CACMPORT_DOSETBUFFERLENGTHS_DUP2, "CAcmPort::DoSetBufferLengths;\t***SetBufSize on reader failed with %d", ret );
		OstTraceFunctionExit0( CACMPORT_DOSETBUFFERLENGTHS_EXIT_DUP1 );
		return ret;
		}

	// OK, the Rx buffer has been resized, now for the Tx buffer...
	if (!iWriter)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_DOSETBUFFERLENGTHS_DUP6, "CAcmPort::DoSetBufferLengths;iWriter=%p", iWriter );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	ret = iWriter->SetBufSize(aLength);
	if ( ret )
		{
		// Failure! The Tx buffer is still safe, at its old size. However, 
		// to maintain the integrity of the API we need to reset the Rx 
		// buffer back to the old size. To make sure that this will work, free 
		// the dummy buffer we initially allocated.
		delete dummyBuffer;
		OstTrace1( TRACE_NORMAL, CACMPORT_DOSETBUFFERLENGTHS_DUP3, "CAcmPort::DoSetBufferLengths;\t***SetBufSize on writer failed with %d", ret );
		TInt err = iReader->SetBufSize(oldSize);
		if (err)
			{
			OstTrace1( TRACE_FATAL, CACMPORT_DOSETBUFFERLENGTHS_DUP7, "CAcmPort::DoSetBufferLengths;err=%d", err );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		static_cast<void>(err);
		// Now both buffers are at the size they started at, and the 
		// request failed with error code 'ret'.
		OstTraceFunctionExit0( CACMPORT_DOSETBUFFERLENGTHS_EXIT_DUP2 );
		return ret;
		}

	// We seem to have successfully resized both buffers... clean up and 
	// return.
	delete dummyBuffer;
	OstTrace0( TRACE_NORMAL, CACMPORT_DOSETBUFFERLENGTHS_DUP4, "CAcmPort::DoSetBufferLengths;\treturning KErrNone" );
	OstTraceFunctionExit0( CACMPORT_DOSETBUFFERLENGTHS_EXIT_DUP3 );
	return KErrNone;
	}

TInt CAcmPort::SetReceiveBufferLength(TInt aLength)
/**
 * Downcall from C32. Set the size of Rx buffer. Note that the Rx and Tx 
 * buffers are assumed by the RComm API to always have the same size. 
 * Therefore set the size of the Tx buffer also.
 *
 * @param aLength new length of Tx and Rx buffer
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_SETRECEIVEBUFFERLENGTH_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORT_SETRECEIVEBUFFERLENGTH, "CAcmPort::SetReceiveBufferLength;aLength=%d", aLength );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_SETRECEIVEBUFFERLENGTH_DUP1, "CAcmPort::SetReceiveBufferLength;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_SETRECEIVEBUFFERLENGTH_EXIT );
		return KErrAccessDenied;
		}

	TInt ret = DoSetBufferLengths(static_cast<TUint>(aLength));
	OstTrace1( TRACE_NORMAL, CACMPORT_SETRECEIVEBUFFERLENGTH_DUP2, "CAcmPort::SetReceiveBufferLength;\tDoSetBufferLengths = return value = %d", ret );
	OstTraceFunctionExit0( CACMPORT_SETRECEIVEBUFFERLENGTH_EXIT_DUP1 );
	return ret;
	}

void CAcmPort::Destruct()
/**
 * Downcall from C32. Destruct - we must (eventually) call delete this
 */
	{
	OstTraceFunctionEntry0( CACMPORT_DESTRUCT_ENTRY );
	delete this;
	OstTraceFunctionExit0( CACMPORT_DESTRUCT_EXIT );
	}

void CAcmPort::FreeMemory()
/**
 * Downcall from C32. Attempt to reduce our memory foot print.
 * Implemented to do nothing.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_FREEMEMORY_ENTRY );
	OstTraceFunctionExit0( CACMPORT_FREEMEMORY_EXIT );
	}

void CAcmPort::NotifyDataAvailable()
/**
 * Downcall from C32. Notify client when data is available. 
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYDATAAVAILABLE_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYDATAAVAILABLE, "CAcmPort::NotifyDataAvailable;\t***access denied" );
		NotifyDataAvailableCompleted(KErrAccessDenied);
		OstTraceFunctionExit0( CACMPORT_NOTIFYDATAAVAILABLE_EXIT );
		return;
		}
	
	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_NOTIFYDATAAVAILABLE_DUP1, "CAcmPort::NotifyDataAvailable;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iReader->NotifyDataAvailable();		
	OstTraceFunctionExit0( CACMPORT_NOTIFYDATAAVAILABLE_EXIT_DUP1 );
	}

void CAcmPort::NotifyDataAvailableCancel()
/**
 * Downcall from C32. Cancel an outstanding data available notification. Note 
 * that C32 actually completes the client's request for us.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYDATAAVAILABLECANCEL_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYDATAAVAILABLECANCEL, "CAcmPort::NotifyDataAvailableCancel;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_NOTIFYDATAAVAILABLECANCEL_EXIT );
		return;
		}

	if (!iReader)
		{
		OstTraceExt1( TRACE_FATAL, CACMPORT_NOTIFYDATAAVAILABLECANCEL_DUP1, "CAcmPort::NotifyDataAvailableCancel;iReader=%p", iReader );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iReader->NotifyDataAvailableCancel();
	OstTraceFunctionExit0( CACMPORT_NOTIFYDATAAVAILABLECANCEL_EXIT_DUP1 );
	}

TInt CAcmPort::GetFlowControlStatus(TFlowControl& /*aFlowControl*/)
/**
 * Downcall from C32. Get the flow control status. NB This is not supported.
 *
 * @param aFlowControl flow control status will be written to this descriptor
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_GETFLOWCONTROLSTATUS_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETFLOWCONTROLSTATUS, "CAcmPort::GetFlowControlStatus;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_GETFLOWCONTROLSTATUS_EXIT );
		return KErrAccessDenied;
		}

	OstTrace0( TRACE_NORMAL, CACMPORT_GETFLOWCONTROLSTATUS_DUP1, "CAcmPort::GetFlowControlStatus;\t***not supported" );
	OstTraceFunctionExit0( CACMPORT_GETFLOWCONTROLSTATUS_EXIT_DUP1 );
	return KErrNotSupported;
	}

void CAcmPort::NotifyOutputEmpty()
/**
 * Downcall from C32. Notify the client when the output buffer is empty. Note 
 * that this is not supported. Write requests only complete when the data has 
 * been sent to the LDD (KConfigWriteBufferedComplete is not supported) so 
 * it's not very useful.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYOUTPUTEMPTY_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYOUTPUTEMPTY, "CAcmPort::NotifyOutputEmpty;\t***access denied" );
		NotifyOutputEmptyCompleted(KErrAccessDenied);
		OstTraceFunctionExit0( CACMPORT_NOTIFYOUTPUTEMPTY_EXIT );
		return;
		}

	OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYOUTPUTEMPTY_DUP1, "CAcmPort::NotifyOutputEmpty;\t***not supported" );
	NotifyOutputEmptyCompleted(KErrNotSupported);
	OstTraceFunctionExit0( CACMPORT_NOTIFYOUTPUTEMPTY_EXIT_DUP1 );
	}

void CAcmPort::NotifyOutputEmptyCancel()
/**
 * Downcall from C32. Cancel a pending request to be notified when the output 
 * buffer is empty. Note that C32 actually completes the client's request for 
 * us.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYOUTPUTEMPTYCANCEL_ENTRY );
	OstTraceFunctionExit0( CACMPORT_NOTIFYOUTPUTEMPTYCANCEL_EXIT );
	}

void CAcmPort::NotifyBreak()
/**
 * Downcall from C32. Notify a client of a host-driven break on the serial 
 * line.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYBREAK_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYBREAK, "CAcmPort::NotifyBreak;\t***access denied" );
		BreakNotifyCompleted(KErrAccessDenied);
		OstTraceFunctionExit0( CACMPORT_NOTIFYBREAK_EXIT );
		return;
		}

	// C32 protects us against this.
	if (iNotifyBreak)
		{
		OstTrace1( TRACE_FATAL, CACMPORT_NOTIFYBREAK_DUP1, "CAcmPort::NotifyBreak;iNotifyBreak=%d", (TInt)iNotifyBreak );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	iNotifyBreak = ETrue;
	OstTraceFunctionExit0( CACMPORT_NOTIFYBREAK_EXIT_DUP1 );
	}

void CAcmPort::NotifyBreakCancel()
/**
 * Downcall from C32. Cancel a pending notification of a serial line break. 
 * Note that C32 actually completes the client's request for us.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYBREAKCANCEL_ENTRY );
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYBREAKCANCEL, "CAcmPort::NotifyBreakCancel;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_NOTIFYBREAKCANCEL_EXIT );
		return;
		}

	iNotifyBreak = EFalse;
	OstTraceFunctionExit0( CACMPORT_NOTIFYBREAKCANCEL_EXIT_DUP1 );
	}

void CAcmPort::BreakRequestCompleted()
/**
 * Callback for completion of a break request. 
 * This is called when the break state next goes inactive after a break has 
 * been requested from the ACM port. 
 * This is implemented to complete the RComm client's Break request.
 */
 	{
	OstTraceFunctionEntry0( CACMPORT_BREAKREQUESTCOMPLETED_ENTRY );
	

	if ( !iCancellingBreak )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_BREAKREQUESTCOMPLETED, "CAcmPort::BreakRequestCompleted;\tcalling BreakCompleted with KErrNone" );
		BreakCompleted(KErrNone);
		}
	else
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_BREAKREQUESTCOMPLETED_DUP1, "CAcmPort::BreakRequestCompleted;\tbreak is being cancelled-letting C32 complete the request" );
		}
	// In the event of an RComm::BreakCancel, this function is called 
	// sychronously with CAcmPort::BreakCancel, and C32 will complete the 
	// original RComm::Break request with KErrCancel when that returns. So we 
	// only need to complete it if there isn't a cancel going on.
	iBreak = EFalse;
	iCancellingBreak = EFalse;
	OstTraceFunctionExit0( CACMPORT_BREAKREQUESTCOMPLETED_EXIT );
	}

void CAcmPort::BreakStateChange()
/**
 * Callback for a change in the break state. 
 * This is called both when the break state changes because the RComm client 
 * wanted it to, and when the break state changes because the host asked us 
 * to.
 * This is implemented to update our signal line flags, complete 
 * NotifySignalChange requests, and complete NotifyBreak requests.
 */
 	{
	OstTraceFunctionEntry0( CACMPORT_BREAKSTATECHANGE_ENTRY );
	// TODO: what if no iAcm?

	// Take a copy of the current signal state for comparison later
	TUint32 oldSignals = iSignals;

	// grab the state of the BREAK from the ACM class that manages
	// it and apply the appropriate flag state to the local iSignals
	if ( iAcm && iAcm->BreakActive() )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_BREAKSTATECHANGE, "CAcmPort::BreakStateChange;\tbreak is active" );
		iSignals |= KSignalBreak;
		}
	else
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_BREAKSTATECHANGE_DUP1, "CAcmPort::BreakStateChange;\tbreak is inactive" );
		iSignals &= ~KSignalBreak;
		}

	// first tell the client (if interested, and if it's not a client-driven 
	// break) that there has been a BREAK event of some sort (either into or 
	// out of the break-active condition...
	if ( iNotifyBreak && !iBreak )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_BREAKSTATECHANGE_DUP2, "CAcmPort::BreakStateChange;\tcalling BreakNotifyCompleted with KErrNone" );
		BreakNotifyCompleted(KErrNone);
		iNotifyBreak = EFalse;
		}

	// and now tell the client (again if interested) the new state
	// of the BREAK signal, if it has changed
	if (	iNotifySignalChange
		&&	( ( iNotifySignalMask & ( iSignals ^ oldSignals ) ) != 0 ) )
		{
		// Report correctly mapped signals that client asked for
		OstTrace0( TRACE_NORMAL, CACMPORT_BREAKSTATECHANGE_DUP3, "CAcmPort::BreakStateChange;\tcalling SignalChangeCompleted with KErrNone" );
		TUint32 signals = ConvertSignals ( KBreakChanged | ( iSignals & iNotifySignalMask ) );
		SignalChangeCompleted( signals, KErrNone);
		iNotifySignalChange = EFalse;
		}
	OstTraceFunctionExit0( CACMPORT_BREAKSTATECHANGE_EXIT );
	}

void CAcmPort::NotifyFlowControlChange()
/**
 * Downcall from C32. Notify a client of a change in the flow control state. 
 * Note that this is not supported.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYFLOWCONTROLCHANGE_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYFLOWCONTROLCHANGE, "CAcmPort::NotifyFlowControlChange;\t***access denied" );
		FlowControlChangeCompleted(EFlowControlOn, KErrAccessDenied);
		OstTraceFunctionExit0( CACMPORT_NOTIFYFLOWCONTROLCHANGE_EXIT );
		return;
		}

	OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYFLOWCONTROLCHANGE_DUP1, "CAcmPort::NotifyFlowControlChange;\t***not supported" );
	FlowControlChangeCompleted(EFlowControlOn,KErrNotSupported);
	OstTraceFunctionExit0( CACMPORT_NOTIFYFLOWCONTROLCHANGE_EXIT_DUP1 );
	}

void CAcmPort::NotifyFlowControlChangeCancel()
/**
 * Downcall from C32. Cancel a pending request to be notified when the flow 
 * control state changes. Note that C32 actually completes the client's 
 * request for us.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYFLOWCONTROLCHANGECANCEL_ENTRY );
	OstTraceFunctionExit0( CACMPORT_NOTIFYFLOWCONTROLCHANGECANCEL_EXIT );
	}

void CAcmPort::NotifyConfigChange()
/**
 * Downcall from C32. Notify a client of a change to the serial port 
 * configuration.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYCONFIGCHANGE_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYCONFIGCHANGE, "CAcmPort::NotifyConfigChange;\t***access denied" );
		ConfigChangeCompleted(iCommNotificationDes, KErrAccessDenied);
		OstTraceFunctionExit0( CACMPORT_NOTIFYCONFIGCHANGE_EXIT );
		return;
		}

	if ( iNotifyConfigChange )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYCONFIGCHANGE_DUP1, "CAcmPort::NotifyConfigChange;\t***in use" );
		ConfigChangeCompleted(iCommNotificationDes,KErrInUse);
		}
	else
		{
		iNotifyConfigChange = ETrue;
		}
	OstTraceFunctionExit0( CACMPORT_NOTIFYCONFIGCHANGE_EXIT_DUP1 );
	}

void CAcmPort::NotifyConfigChangeCancel()
/**
 * Downcall from C32. Cancel a pending request to be notified of a change to 
 * the serial port configuration. Note that C32 actually completes the 
 * client's request for us.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYCONFIGCHANGECANCEL_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYCONFIGCHANGECANCEL, "CAcmPort::NotifyConfigChangeCancel;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_NOTIFYCONFIGCHANGECANCEL_EXIT );
		return;
		}

	iNotifyConfigChange = EFalse;
	OstTraceFunctionExit0( CACMPORT_NOTIFYCONFIGCHANGECANCEL_EXIT_DUP1 );
	}

void CAcmPort::HostConfigChange(const TCommConfigV01& aConfig)
/**
 * Callback from a "host-pushed" configuration change packet.
 *
 * @param aConfig	New configuration data.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_HOSTCONFIGCHANGE_ENTRY );
	HandleConfigNotification(aConfig.iRate,
		aConfig.iDataBits,
		aConfig.iParity,
		aConfig.iStopBits,
		iCommConfig.iHandshake);
	iCommConfig.iRate = aConfig.iRate;
	iCommConfig.iDataBits = aConfig.iDataBits;
	iCommConfig.iParity = aConfig.iParity;
	iCommConfig.iStopBits = aConfig.iStopBits;
	OstTraceFunctionExit0( CACMPORT_HOSTCONFIGCHANGE_EXIT );
	}

void CAcmPort::NotifySignalChange(TUint aSignalMask)
/**
 * Downcall from C32. Notify a client of a change to the signal lines.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYSIGNALCHANGE_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORT_NOTIFYSIGNALCHANGE, "CAcmPort::NotifySignalChange;>>CAcmPort::NotifySignalChange aSignalMask=0x%08x", aSignalMask );
	if ( iNotifySignalChange )
		{
		if ( !iAcm )
			{
			OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYSIGNALCHANGE_DUP1, "CAcmPort::NotifySignalChange;\t***access denied" );
			SignalChangeCompleted(0, KErrAccessDenied);
			}
		else
			{
			OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYSIGNALCHANGE_DUP2, "CAcmPort::NotifySignalChange;\t***in use" );
			SignalChangeCompleted(0, KErrInUse);
			}
		}
	else
		{
		iNotifySignalChange = ETrue;
		// convert signals to internal format
		// no need to filter as can notify on inputs or outputs
		iNotifySignalMask = ConvertSignals(aSignalMask);
		}

	OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYSIGNALCHANGE_DUP3, "CAcmPort::NotifySignalChange;<<CAcmPort::NotifySignalChange" );
	OstTraceFunctionExit0( CACMPORT_NOTIFYSIGNALCHANGE_EXIT );
	}

void CAcmPort::NotifySignalChangeCancel()
/**
 * Downcall from C32. Cancel a pending client request to be notified about a 
 * change to the signal lines. Note that C32 actually completes the client's 
 * request for us.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_NOTIFYSIGNALCHANGECANCEL_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_NOTIFYSIGNALCHANGECANCEL, "CAcmPort::NotifySignalChangeCancel;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_NOTIFYSIGNALCHANGECANCEL_EXIT );
		return;
		}

	iNotifySignalChange = EFalse;
	OstTraceFunctionExit0( CACMPORT_NOTIFYSIGNALCHANGECANCEL_EXIT_DUP1 );
	}

void CAcmPort::HostSignalChange(TBool aDtr, TBool aRts)
/**
 * Callback from a "host-pushed" line state change.
 *
 * @param aDtr The state of the DTR signal.
 * @param aRts The state of the RTS signal.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_HOSTSIGNALCHANGE_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CACMPORT_HOSTSIGNALCHANGE, "CAcmPort::HostSignalChange;>>CAcmPort::HostSignalChange aDtr=%d, aRts=%d", aDtr, aRts );
	
	// Take a copy of the current signal state for comparison later
	TUint32 oldSignals = iSignals;

	if ( aDtr )
		{
		iSignals |= KSignalDTR;
		}
	else
		{
		iSignals &= (~KSignalDTR);
		}

	if ( aRts )
		{
		iSignals |= KSignalRTS;
		}
	else
		{
		iSignals &= (~KSignalRTS);
		}

	// Report the new state of the signals if they have changed
	// and the mask shows the upper layer wanted to be sensitive to these signals.
	if ( iNotifySignalChange )
		{
		// Create bitfield of changed signals that client is interested in
		TUint32 changedSignals = ( iSignals ^ oldSignals ) & iNotifySignalMask;
		OstTraceExt4( TRACE_NORMAL, CACMPORT_HOSTSIGNALCHANGE_DUP1, "CAcmPort::HostSignalChange;iSignals=%x;oldSignals=%x;changedSignals=%x;iNotifySignalMask=%x", iSignals, oldSignals, changedSignals, (TUint32)iNotifySignalMask );
		if ( changedSignals != 0 )
			{
			// Mark which signals have changed
			TUint32 changedSignalsMask = 0;
			if ( ( changedSignals & KSignalDTR ) != 0 )
				{
				changedSignalsMask |= KDTRChanged;
				}
			if ( ( changedSignals & KSignalRTS ) != 0 )
				{
				changedSignalsMask |= KRTSChanged;
				}
			
			OstTrace1( TRACE_NORMAL, CACMPORT_HOSTSIGNALCHANGE_DUP2, "CAcmPort::HostSignalChange;changedSignalsMask=%x", changedSignalsMask );
			// Report correctly mapped signals that client asked for
			TUint32 signals = ConvertSignals ( changedSignalsMask | ( iSignals & iNotifySignalMask ) );
			OstTrace1( TRACE_NORMAL, CACMPORT_HOSTSIGNALCHANGE_DUP3, "CAcmPort::HostSignalChange;signals=%x", signals );
			SignalChangeCompleted( signals, KErrNone);
			iNotifySignalChange = EFalse;
			}
		}

	OstTrace0( TRACE_NORMAL, CACMPORT_HOSTSIGNALCHANGE_DUP4, "CAcmPort::HostSignalChange;<<CAcmPort::HostSignalChange" );
	OstTraceFunctionExit0( CACMPORT_HOSTSIGNALCHANGE_EXIT );
	}

TInt CAcmPort::GetRole(TCommRole& aRole)
/**
 * Downcall from C32. Get the role of this port unit
 *
 * @param aRole reference to where the role will be written to
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_GETROLE_ENTRY );
	
	if ( !iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_GETROLE, "CAcmPort::GetRole;\t***access denied" );
		OstTraceFunctionExit0( CACMPORT_GETROLE_EXIT );
		return KErrAccessDenied;
		}

	aRole = iRole;
	OstTrace1( TRACE_NORMAL, CACMPORT_GETROLE_DUP1, "CAcmPort::GetRole;\trole=%d", aRole );
	OstTraceFunctionExit0( DUP1_CACMPORT_GETROLE_EXIT );
	return KErrNone;
	}

TInt CAcmPort::SetRole(TCommRole aRole)
/**
 * Downcall from C32. Set the role of this port unit
 *
 * @param aRole the new role
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_SETROLE_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORT_SETROLE, "CAcmPort::SetRole;>>CAcmPort::SetRole aRole=%d", aRole );
	TInt ret = KErrNone;
	
	if ( !iAcm )
		{
		ret = KErrAccessDenied;
		}
	else
		{
		iRole = aRole;
		}

	OstTrace1( TRACE_NORMAL, CACMPORT_SETROLE_DUP1, "CAcmPort::SetRole;<<CAcmPort::SetRole ret=%d", ret );
	OstTraceFunctionExit0( CACMPORT_SETROLE_EXIT );
	return ret;
	}

void CAcmPort::SetAcm(CCdcAcmClass* aAcm)
/**
 * Called by the factory when the ACM interface for this port is about to be 
 * either created or destroyed. 
 * If it's going to be destroyed, we must cancel any currently outstanding 
 * requests on the reader and writer objects, complete any outstanding 
 * requests with KErrAccessDenied, and refuse any new requests with the same 
 * error until the registration port is back open again. 
 * If it's going to be opened, we simply remember it and use it to pass future 
 * client requests onto the bus.
 *
 * @param aAcm The new ACM (may be NULL).
 */
	{
	OstTraceFunctionEntry0( CACMPORT_SETACM_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORT_SETACM, "CAcmPort::SetAcm;>>CAcmPort::SetAcm aAcm = 0x%08x", aAcm );
	OstTrace1( TRACE_NORMAL, CACMPORT_SETACM_DUP3, "CAcmPort::SetAcm;iAcm=0x%08x", iAcm );
	
	
	if ( !aAcm )
		{
		// Cancel any outstanding operations on the reader and writer.
		if (!iReader)
			{
			OstTraceExt1( TRACE_FATAL, CACMPORT_SETACM_DUP4, "CAcmPort::SetAcm;iReader=%p", iReader );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		iReader->ReadCancel();
		if (!iWriter)
			{
			OstTraceExt1( TRACE_FATAL, CACMPORT_SETACM_DUP5, "CAcmPort::SetAcm;iWriter=%p", iWriter );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		iWriter->WriteCancel();

		// Complete any outstanding requests
		OstTrace0( TRACE_NORMAL, CACMPORT_SETACM_DUP1, "CAcmPort::SetAcm;\tcompleting outstanding requests with KErrAccessDenied" );
		SignalChangeCompleted(iSignals,KErrAccessDenied);
		ReadCompleted(KErrAccessDenied);
		WriteCompleted(KErrAccessDenied);
		ConfigChangeCompleted(iCommNotificationDes,KErrAccessDenied);
		NotifyDataAvailableCompleted(KErrAccessDenied);
		NotifyOutputEmptyCompleted(KErrAccessDenied);
		BreakNotifyCompleted(KErrAccessDenied);
		FlowControlChangeCompleted(EFlowControlOn, KErrAccessDenied);
		}
	else
		{
		if (iAcm)
			{
			OstTraceExt1( TRACE_FATAL, CACMPORT_SETACM_DUP6, "CAcmPort::SetAcm;iAcm=%p", iAcm );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		aAcm->SetCallback(this);
		// Set the port as the observer of break events.
		aAcm->SetBreakCallback(this);
		}

	iAcm = aAcm;
	OstTrace0( TRACE_NORMAL, CACMPORT_SETACM_DUP2, "CAcmPort::SetAcm;<<CAcmPort::SetAcm" );
	OstTraceFunctionExit0( CACMPORT_SETACM_EXIT );
	}

CAcmPort::~CAcmPort()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CACMPORT_CACMPORT_DESTRUCTRURE_ENTRY );
	
	delete iReader;
	delete iWriter;
	
	// Remove this as a sink for the ACM class to call back about host-pushed 
	// changes.
	if ( iAcm )
		{
		OstTrace0( TRACE_NORMAL, CACMPORT_CACMPORT_DESTRUCTRURE, "CAcmPort::~CAcmPort;\tiAcm exists- calling SetCallback(NULL)" );
		iAcm->SetCallback(NULL);
		}

	OstTrace0( TRACE_NORMAL, CACMPORT_CACMPORT_DESTRUCTRURE_DUP1, "CAcmPort::~CAcmPort;\tcalling AcmPortClosed on observer" );
	iObserver.AcmPortClosed(iUnit);
	OstTraceFunctionExit0( CACMPORT_CACMPORT_DESTRUCTRURE_EXIT );
	}

//
// End of file
