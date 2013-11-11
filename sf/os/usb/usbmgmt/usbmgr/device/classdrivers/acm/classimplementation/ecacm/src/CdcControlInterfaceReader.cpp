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
#ifndef __OVER_DUMMYUSBLDD__
#include <d32usbc.h>
#else
#include <dummyusblddapi.h>
#endif
#include "CdcControlInterfaceReader.h"
#include "AcmPanic.h"
#include "AcmUtils.h"
#include "CdcControlInterfaceRequestHandler.h"
#include "AcmConstants.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CdcControlInterfaceReaderTraces.h"
#endif


CCdcControlInterfaceReader::CCdcControlInterfaceReader(
				MCdcCommsClassRequestHandler& aParent, 
				RDevUsbcClient& aLdd)
:	CActive(KEcacmAOPriority),
	iParent(aParent),
	iLdd(aLdd)
/**
 * Constructor.
 *
 * @param aParent	Observer (ACM port)
 * @param aLdd		The USB LDD handle to be used.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_CCDCCONTROLINTERFACEREADER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	ReadMessageHeader();
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_CCDCCONTROLINTERFACEREADER_CONS_EXIT );
	}

CCdcControlInterfaceReader::~CCdcControlInterfaceReader()
/**
 * Destructor
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_CCDCCONTROLINTERFACEREADER_DES_ENTRY );
	Cancel(); //Call CActive::Cancel()	 
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_CCDCCONTROLINTERFACEREADER_DES_EXIT );
	}

CCdcControlInterfaceReader* CCdcControlInterfaceReader::NewL(
	MCdcCommsClassRequestHandler& aParent, 
	RDevUsbcClient& aLdd)
/**
 * Create a new CCdcControlInterfaceReader object and start reading
 *
 * @param aParent	Observer (ACM port)
 * @param aLdd		The USB LDD handle to be used.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_NEWL_ENTRY );
	CCdcControlInterfaceReader* self = new(ELeave) CCdcControlInterfaceReader(
		aParent, 
		aLdd);
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_NEWL_EXIT );
	return self;
	}

void CCdcControlInterfaceReader::RunL()
/**
 * This function will be called when a read completes.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_RUNL, "CCdcControlInterfaceReader::RunL;iStatus=%d", iStatus.Int() );
	HandleReadCompletion(iStatus.Int());
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_RUNL_EXIT );
	}

void CCdcControlInterfaceReader::DoCancel()
/**
 * Cancel an outstanding read.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_DOCANCEL_ENTRY );
	iLdd.ReadCancel(EEndpoint0);
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_DOCANCEL_EXIT );
	}

void CCdcControlInterfaceReader::HandleReadCompletion(TInt aError)
/**
 * This will be called when a new packet has been received.
 * Since "Header" packets may have an associated "Data" packet, which is sent 
 * immediately following the header, this class implements a state machine. 
 * Therefore, it will wait until both "Header" and, if necessary, "Data" 
 * packets have been received before decoding.
 *
 * @param aError Error
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_HANDLEREADCOMPLETION_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_HANDLEREADCOMPLETION, 
			"CCdcControlInterfaceReader::HandleReadCompletion;aError=%d", aError );

	if ( aError )
		{
		ReadMessageHeader();		
		OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_HANDLEREADCOMPLETION_DUP1, 
				"CCdcControlInterfaceReader::HandleReadCompletion" );
		OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_HANDLEREADCOMPLETION_EXIT );
		return;
		}

	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_HANDLEREADCOMPLETION_DUP2, 
			"CCdcControlInterfaceReader::HandleReadCompletion;iState=%d", (TInt)iState );

	switch ( iState)
		{
	case EWaitingForHeader:
		{
		DecodeMessageHeader();
		}
		break;

	case EWaitingForData:
		{
		DecodeMessageData();
		}
		break;

	default:
		{
		OstTrace1( TRACE_FATAL, CCDCCONTROLINTERFACEREADER_HANDLEREADCOMPLETION_DUP3, 
					"CCdcControlInterfaceReader::HandleReadCompletion;iState=%d", (TInt)iState );
		User::Panic(KAcmPanicCat, EPanicIllegalState);
		}
		break;
		}

	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_HANDLEREADCOMPLETION_EXIT_DUP1 );
	}

void CCdcControlInterfaceReader::DecodeMessageHeader()
/**
 * This function decodes a message header. It determines whether the host 
 * requires some data in response and dispatches the request appropriately.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_DECODEMESSAGEHEADER_ENTRY );
	if ( TUsbRequestHdr::Decode(iMessageHeader, iRequestHeader) != KErrNone )
		{
		OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_DECODEMESSAGEHEADER, 
				"CCdcControlInterfaceReader::DecodeMessageHeader;\t- Unable to decode request header!" );
		
		// Stall bus- unknown message. If this fails, there's nothing we can 
		// do.
		static_cast<void>(iLdd.EndpointZeroRequestError()); 
		ReadMessageHeader();
		OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_DECODEMESSAGEHEADER_EXIT );
		return;
		}

	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_DECODEMESSAGEHEADER_DUP1, 
			"CCdcControlInterfaceReader::DecodeMessageHeader;\t- New read! Request %d", (TInt)iRequestHeader.iRequest );
	
	if ( iRequestHeader.IsDataResponseRequired() )
		{
		DecodeMessageDataWithResponseRequired();
		}
	else if ( iRequestHeader.iLength == 0 )
		{
		iMessageData.SetLength(0);
		DecodeMessageData();
		}
	else
		{
		ReadMessageData(iRequestHeader.iLength);
		}
	OstTraceFunctionExit0( DUP1_CCDCCONTROLINTERFACEREADER_DECODEMESSAGEHEADER_EXIT );
	}

void CCdcControlInterfaceReader::DecodeMessageDataWithResponseRequired()
/**
 * Decode a message which requires data to be sent to the host in response.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATAWITHRESPONSEREQUIRED_ENTRY );
	OstTraceExt1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATAWITHRESPONSEREQUIRED, 
			"CCdcControlInterfaceReader::DecodeMessageDataWithResponseRequired;\t- New read! Request=%hhx", iRequestHeader.iRequest );
	
	TBuf8<KAcmControlReadBufferLength> returnBuffer;
	switch ( iRequestHeader.iRequest )
		{
	case KGetEncapsulated:
		{
		if ( iParent.HandleGetEncapResponse(returnBuffer) == KErrNone )
			{
			// Write Back data here
			// At least ack the packet or host will keep sending. If this 
			// fails, the host will ask again until we do successfully reply.
			static_cast<void>(iLdd.SendEp0StatusPacket()); 
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		}
		break;

	case KGetCommFeature:
		{
		if ( iParent.HandleGetCommFeature(iRequestHeader.iValue, returnBuffer) 
			== KErrNone )
			{
			TRequestStatus status;
			iLdd.Write(status, EEndpoint0,
						returnBuffer, 
						returnBuffer.Length(),
						EFalse);
			User::WaitForRequest(status);
			// If this failed, the host will ask again until we do 
			// successfully reply.
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		}
		break;

	case KGetLineCoding:
		{
		if ( iParent.HandleGetLineCoding(returnBuffer) == KErrNone )
			{
			TRequestStatus status;
			iLdd.Write(status, EEndpoint0,
						returnBuffer,
						7,
						EFalse);
			User::WaitForRequest(status);
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		}
		break;

	default:
		{
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATAWITHRESPONSEREQUIRED_DUP1, 
				"CCdcControlInterfaceReader::DecodeMessageDataWithResponseRequired;\t- request number not recognised (%d)", 
				(TInt)(iRequestHeader.iRequest) );
		
		// Stall bus- unknown message. If this fails, there's nothing we can 
		// do.
		static_cast<void>(iLdd.EndpointZeroRequestError()); 
		}
		break;
		}

	ReadMessageHeader();
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATAWITHRESPONSEREQUIRED_EXIT );
	}

void CCdcControlInterfaceReader::DecodeMessageData()
/**
 * Decode a message that does not require any data to be sent to the host in 
 * response. In all the requests here, the completion of the class-specific 
 * function is ack'ed by sending an endpoint zero status packet. The request 
 * can be nack'ed by signalling an endpoint zero request error.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATA_ENTRY );
	if ( iMessageData.Length() != iRequestHeader.iLength )
		{
		OstTrace0( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATA, 
				"CCdcControlInterfaceReader::DecodeMessageData;\t- Data length is incorrect" );
		
		ReadMessageHeader();
		OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATA_EXIT );
		return;
		}

	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATA_DUP1, 
			"CCdcControlInterfaceReader::DecodeMessageData;\tNew read! Request %d", (TInt)(iRequestHeader.iRequest) );
	
	switch ( iRequestHeader.iRequest )
		{
	case KSendEncapsulated:
		if(iParent.HandleSendEncapCommand(iMessageData) == KErrNone)
			{
			// If this fails, the host will send again.
			static_cast<void>(iLdd.SendEp0StatusPacket());
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		break;
	case KSetCommFeature:
		if(iParent.HandleSetCommFeature(iRequestHeader.iValue,iMessageData) 
			== KErrNone)
			{
			// If this fails, the host will send again.
			static_cast<void>(iLdd.SendEp0StatusPacket());
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		break;
	case KClearCommFeature:
		if(iParent.HandleClearCommFeature(iRequestHeader.iValue) == KErrNone)
			{
			// If this fails, the host will send again.
			static_cast<void>(iLdd.SendEp0StatusPacket());
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		break;
	case KSetLineCoding:
		if(iParent.HandleSetLineCoding(iMessageData) == KErrNone)
			{
			// If this fails, the host will send again.
			static_cast<void>(iLdd.SendEp0StatusPacket());
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		break;
	case KSetControlLineState:
		if(iParent.HandleSetControlLineState(
				// See CDC spec table 69 (UART state bitmap values)...
			   (iRequestHeader.iValue & 0x0002) ? ETrue : EFalse, // bTxCarrier
			   (iRequestHeader.iValue & 0x0001) ? ETrue : EFalse) // bRxCarrier
				== KErrNone)
			{
			// If this fails, the host will send again.
			static_cast<void>(iLdd.SendEp0StatusPacket());
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		break;
	case KSendBreak:
		// The time value sent from the host is in milliseconds.
		if(iParent.HandleSendBreak(iRequestHeader.iValue) == KErrNone)
			{
			// If this fails, the host will send again.
			static_cast<void>(iLdd.SendEp0StatusPacket());
			}
		else
			{
			// Stall bus- unknown message. If this fails, there's nothing we 
			// can do.
			static_cast<void>(iLdd.EndpointZeroRequestError()); 
			}
		break;
	default:
		OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATA_DUP2, 
				"CCdcControlInterfaceReader::DecodeMessageData;\t***request number not recognised (%d)", (TInt)(iRequestHeader.iRequest) );
		
		// Stall bus- unknown message. If this fails, there's nothing we can 
		// do.
		static_cast<void>(iLdd.EndpointZeroRequestError()); 
		break;
		}

	ReadMessageHeader();
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_DECODEMESSAGEDATA_EXIT_DUP1 );
	}

void CCdcControlInterfaceReader::ReadMessageHeader()
/**
 * Post a read request and set the state to indicate that we're waiting for a 
 * message header.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_READMESSAGEHEADER_ENTRY );
	iState = EWaitingForHeader;
	iLdd.ReadPacket(iStatus, EEndpoint0, iMessageHeader, KUsbRequestHdrSize);
	SetActive();
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_READMESSAGEHEADER_EXIT );
	}

void CCdcControlInterfaceReader::ReadMessageData(TUint aLength)
/**
 * Post a read request and set the state to indicate that we're waiting for 
 * some message data.
 *
 * @param aLength Length of data to read.
 */
	{
	OstTraceFunctionEntry0( CCDCCONTROLINTERFACEREADER_READMESSAGEDATA_ENTRY );
	OstTrace1( TRACE_NORMAL, CCDCCONTROLINTERFACEREADER_READMESSAGEDATA, 
			"CCdcControlInterfaceReader::ReadMessageData;\tqueuing read, length = %d", (TInt)aLength );

	iState = EWaitingForData;
	iLdd.Read(iStatus, EEndpoint0, iMessageData, static_cast<TInt>(aLength));
	SetActive();
	OstTraceFunctionExit0( CCDCCONTROLINTERFACEREADER_READMESSAGEDATA_EXIT );
	}

//
// End of file
