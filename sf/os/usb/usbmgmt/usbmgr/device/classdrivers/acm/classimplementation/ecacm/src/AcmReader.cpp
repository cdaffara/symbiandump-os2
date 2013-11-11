/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <cs_port.h>
#include "AcmReader.h"
#include "AcmUtils.h"
#include "ActiveReader.h"
#include "ActiveReadOneOrMoreReader.h"
#include "CdcAcmClass.h"
#include "AcmPanic.h"
#include "AcmPort.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "AcmReaderTraces.h"
#endif


CAcmReader* CAcmReader::NewL(CAcmPort& aPort,
							 TUint aBufSize)
/**
 * Factory function.
 *
 * @param aPort The CAcmPort parent.
 * @param aBufSize The size of the buffer.
 * @return Ownership of a new CAcmReader.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_NEWL_ENTRY );	
	CAcmReader* self = new(ELeave) CAcmReader(aPort, aBufSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP(self);
	OstTraceFunctionExit0( CACMREADER_NEWL_EXIT );
	return self;
	}

CAcmReader::~CAcmReader()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_CACMREADER_DES_ENTRY );
	ReadCancel();
	delete iBuffer;
	OstTraceFunctionExit0( CACMREADER_CACMREADER_DES_EXIT );
	}

CAcmReader::CAcmReader(CAcmPort& aPort,
					   TUint aBufSize)
/**
 * Constructor.
 *
 * @param aPort The CPort parent.
 * @param aBufSize The size of the buffer.
 */
 :	iBufSize(aBufSize),
	iInBuf(NULL,0,0),
	iPort(aPort)
	{
	OstTraceFunctionEntry0( CACMREADER_CACMREADER_CONS_ENTRY );
	OstTraceFunctionExit0( CACMREADER_CACMREADER_CONS_EXIT );
	}

void CAcmReader::ConstructL()
/**
 * 2nd-phase construction.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_CONSTRUCTL_ENTRY );
	// Create iBuffer. 
	OstTrace0( TRACE_NORMAL, CACMREADER_CONSTRUCTL, "CAcmReader::ConstructL;\tabout to create iBuffer" );
	TInt	err = SetBufSize(iBufSize);
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CACMREADER_CONSTRUCTL_DUP1, "CAcmReader::ConstructL;err=%d", err );
		User::Leave(err);
		}
	OstTraceFunctionExit0( CACMREADER_CONSTRUCTL_EXIT );
	}

void CAcmReader::Read(const TAny* aClientBuffer, TUint aMaxLen)
/**
 * Read API.
 *
 * @param aClientBuffer Pointer to the client's memory space.
 * @param aMaxLen Maximum length to read.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_READ_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CACMREADER_READ, "CAcmReader::Read;\taClientBuffer=%p, aMaxLen=%d", aClientBuffer, (TInt)aMaxLen );

	// Check we're open to requests and make a note of interesting data.
	// including iLengthToGo
	CheckNewRequest(aClientBuffer, aMaxLen);
	iCurrentRequest.iRequestType = ERead;

	// A read larger than our internal buffer limit will result in us doing
	// multiple reads to the ports below. 
	// We used to just refuse the request with a KErrNoMemory.
	if ( iTerminatorCount == 0 )
		{
		ReadWithoutTerminators();
		}
	else
		{
		ReadWithTerminators();
		}
	OstTraceFunctionExit0( CACMREADER_READ_EXIT );
	}

void CAcmReader::ReadWithoutTerminators()
/**
 * Process a read request given that no terminator characters are set.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_READWITHOUTTERMINATORS_ENTRY );
	// Can we complete immediately from the buffer?
	const TUint bufLen = BufLen();
	OstTraceExt2( TRACE_NORMAL, CACMREADER_READWITHOUTTERMINATORS, 
			"CAcmReader::ReadWithoutTerminators;\thave %d bytes in buffer;\tLength to go is %d bytes", 
			(TInt)bufLen, (TInt)iLengthToGo );
	if ( iLengthToGo <= bufLen )
		{
		OstTrace1( TRACE_NORMAL, CACMREADER_READWITHOUTTERMINATORS_DUP1, 
				"CAcmReader::ReadWithoutTerminators;\tcompleting  "
				"request immediately from buffer with %d bytes", 
				(TInt)iLengthToGo );
		
		WriteBackData(iLengthToGo);
		CompleteRequest(KErrNone);
		OstTraceFunctionExit0( CACMREADER_READWITHOUTTERMINATORS_EXIT );
		return;
		}

	// There isn't enough in the buffer to complete the request, so we write 
	// back as much as we have, and issue a Read for more.
	if ( bufLen )
		{
		OstTrace1( TRACE_NORMAL, CACMREADER_READWITHOUTTERMINATORS_DUP2, 
				"CAcmReader::ReadWithoutTerminators;\twriting back %d bytes", (TInt)bufLen );
		// Write back as much data as we've got already.
		WriteBackData(bufLen);
		}

	// Issue a read for the data we still need. 
	OstTrace1( TRACE_NORMAL, CACMREADER_READWITHOUTTERMINATORS_DUP3, 
			"CAcmReader::ReadWithoutTerminators;iLengthToGo=%d", (TInt)iLengthToGo );
	IssueRead();
	OstTraceFunctionExit0( CACMREADER_READWITHOUTTERMINATORS_EXIT_DUP1 );
	}

void CAcmReader::ReadWithTerminators()
/**
 * Process a read request given that terminator characters are set.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_READWITHTERMINATORS_ENTRY );
	// Can we complete immediately from the buffer? Search the buffer we have 
	// for any terminators. If found, complete back to the client. If not 
	// found, start issuing ReadOneOrMores until we either find one or run out 
	// of client buffer.

	const TUint bufLen = BufLen();
	OstTrace1( TRACE_NORMAL, CACMREADER_READWITHTERMINATORS, "CAcmReader::ReadWithTerminators;bufLen=%d", (TInt)bufLen );
	if ( bufLen )
		{
		CheckForBufferedTerminatorsAndProceed();
		OstTraceFunctionExit0( CACMREADER_READWITHTERMINATORS_EXIT );
		return;
		}

	// There's no buffered data. Get some.
	IssueReadOneOrMore();
	OstTraceFunctionExit0( CACMREADER_READWITHTERMINATORS_EXIT_DUP1 );
	}

void CAcmReader::ReadOneOrMore(const TAny* aClientBuffer, TUint aMaxLen)
/**
 * ReadOneOrMore API. Note that this is implemented to completely ignore 
 * terminator characters.
 *
 * @param aClientBuffer Pointer to the client's memory space.
 * @param aMaxLen Maximum length to read.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_READONEORMORE_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CACMREADER_READONEORMORE, 
			"CAcmReader::ReadOneOrMore;aClientBuffer=%p;aMaxLen=%d", aClientBuffer, (TInt)aMaxLen );
	// Check we're open to requests and make a note of interesting data.
	CheckNewRequest(aClientBuffer, aMaxLen);
	iCurrentRequest.iRequestType = EReadOneOrMore;	

	// Check to see if there's anything in our buffer- if there is, we can 
	// complete immediately.
	const TUint bufLen = BufLen();
	OstTrace1( TRACE_NORMAL, CACMREADER_READONEORMORE_DUP1, "CAcmReader::ReadOneOrMore;bufLen=%d", (TInt)bufLen );
	if ( bufLen )
		{
		// Complete request with what's in the buffer
		OstTrace1( TRACE_FLOW, CACMREADER_READONEORMORE_DUP2, 
				"CAcmReader::ReadOneOrMore;\tcompleting request immediately from buffer with %d bytes", (TInt)bufLen );
		WriteBackData(bufLen);
		CompleteRequest(KErrNone);
		OstTraceFunctionExit0( CACMREADER_READONEORMORE_EXIT );
		return;
		}

	// Get some more data.
	IssueReadOneOrMore();
	OstTraceFunctionExit0( CACMREADER_READONEORMORE_EXIT_DUP1 );
	}

void CAcmReader::NotifyDataAvailable()
/** 
 * NotifyDataAvailable API. If a request is pending completes the client with KErrInUse.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_NOTIFYDATAAVAILABLE_ENTRY );
	if(iCurrentRequest.iClientPtr) // a request is pending
		{
		iPort.NotifyDataAvailableCompleted(KErrInUse);
		OstTraceFunctionExit0( CACMREADER_NOTIFYDATAAVAILABLE_EXIT );
		return;
		}
	iCurrentRequest.iClientPtr = iBuffer;
	iCurrentRequest.iRequestType = ENotifyDataAvailable;	 	
	iPort.Acm()->NotifyDataAvailable(*this);		
	OstTraceFunctionExit0( CACMREADER_NOTIFYDATAAVAILABLE_EXIT_DUP1 );
	} 
		
void CAcmReader::NotifyDataAvailableCancel()
/**
 * NotifyDataAvailableCancel API. Issues a ReadCancel() to abort pending requests on the LDD  
 *
 */
	{
	OstTraceFunctionEntry0( CACMREADER_NOTIFYDATAAVAILABLECANCEL_ENTRY );	
	if (ENotifyDataAvailable == iCurrentRequest.iRequestType)
		{
		// Cancel any outstanding request on the LDD.		
		if (iPort.Acm())
			{
			OstTrace0( TRACE_NORMAL, CACMREADER_NOTIFYDATAAVAILABLECANCEL, 
					"CAcmReader::NotifyDataAvailableCancel;\tiPort.Acm() exists- calling NotifyDataAvailableCancel() on it" );
			iPort.Acm()->NotifyDataAvailableCancel();
			}
		// Reset our flag to say there's no current outstanding request. What's 
		// already in our buffer can stay there.
		iCurrentRequest.iClientPtr = NULL;
		}	
	OstTraceFunctionExit0( CACMREADER_NOTIFYDATAAVAILABLECANCEL_EXIT );
	}

void CAcmReader::ReadCancel()
/**
 * Cancel API. Cancels any outstanding (Read or ReadOneOrMore) request.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_READCANCEL_ENTRY );
	if (ENotifyDataAvailable != iCurrentRequest.iRequestType)
		{
		// Cancel any outstanding request on the LDD.
		if (iPort.Acm())
			{
			OstTrace0( TRACE_NORMAL, CACMREADER_READCANCEL, 
					"CAcmReader::ReadCancel;\tiPort.Acm() exists- calling ReadCancel on it" );
			iPort.Acm()->ReadCancel();
			}
		
		// Reset our flag to say there's no current outstanding request. What's 
		// already in our buffer can stay there.
		iCurrentRequest.iClientPtr = NULL;
		}
	OstTraceFunctionExit0( CACMREADER_READCANCEL_EXIT );
	}
	
TUint CAcmReader::BufLen() const
/**
 * This function returns the amount of data (in bytes) still remaining in the 
 * circular buffer.
 *
 * @return Length of data in buffer.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_BUFLEN_ENTRY );
	OstTrace0( TRACE_NORMAL, CACMREADER_BUFLEN, "CAcmReader::BufLen;>>CAcmReader::BufLen" );
	TUint len = 0;
	if ( BufWrap() )
		{
		OstTrace0( TRACE_NORMAL, CACMREADER_BUFLEN_DUP1, "CAcmReader::BufLen;\tbuf wrapped" );
		len = iBufSize - ( iOutPtr - iInPtr );
		}
	else
		{
	OstTrace0( TRACE_NORMAL, CACMREADER_BUFLEN_DUP2, "CAcmReader::BufLen;\tbuf not wrapped" );
		len = iInPtr - iOutPtr;
		}

	OstTrace1( TRACE_NORMAL, CACMREADER_BUFLEN_DUP3, "CAcmReader::BufLen;len=%d", (TInt)len );
	OstTraceFunctionExit0( CACMREADER_BUFLEN_EXIT );
	return len;
	}

void CAcmReader::ResetBuffer()
/**
 * Called by the port to clear the buffer.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_RESETBUFFER_ENTRY );
	// A request is outstanding- C32 should protect against this.
	if (iCurrentRequest.iClientPtr)
		{
		OstTraceExt1( TRACE_FATAL, CACMREADER_RESETBUFFER, 
				"CAcmReader::ResetBuffer;iCurrentRequest.iClientPtr=%p", iCurrentRequest.iClientPtr );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	// Reset the pointers. All data is 'lost'.
	iOutPtr = iInPtr = iBufStart;
	OstTraceFunctionExit0( CACMREADER_RESETBUFFER_EXIT );
	}

TInt CAcmReader::SetBufSize(TUint aSize)
/**
 * Called by the port to set the buffer size. Also used as a utility by us to 
 * create the buffer at instantiation. Note that this causes what was in the 
 * buffer at the time to be lost.
 *
 * @param aSize The required size of the buffer.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_SETBUFSIZE_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMREADER_SETBUFSIZE, "CAcmReader::SetBufSize;aSize=%d", (TInt)aSize );
	if ( iCurrentRequest.iClientPtr )
		{
		// A request is outstanding. C32 does not protect us against this.
		OstTrace0( TRACE_NORMAL, CACMREADER_SETBUFSIZE_DUP1, 
				"CAcmReader::SetBufSize;\t***a request is outstanding- returning KErrInUse" );
		OstTraceFunctionExit0( CACMREADER_SETBUFSIZE_EXIT );
		return KErrInUse;
		}

	// Create the new buffer.
	HBufC8* newBuf = HBufC8::New(aSize);
	if ( !newBuf )
		{
		OstTrace0( TRACE_NORMAL, CACMREADER_SETBUFSIZE_DUP2, 
				"CAcmReader::SetBufSize;\tfailed to create new buffer- returning KErrNoMemory" );
		OstTraceFunctionExit0( CACMREADER_SETBUFSIZE_EXIT_DUP1 );
		return KErrNoMemory;
		}
	delete iBuffer;
	iBuffer = newBuf;

	// Update pointers etc.
	TPtr8 ptr = iBuffer->Des();
	iBufStart = const_cast<TUint8*>(reinterpret_cast<const TUint8*>(ptr.Ptr()));
	iBufSize = aSize;
	CheckBufferEmptyAndResetPtrs();

	OstTraceFunctionExit0( CACMREADER_SETBUFSIZE_EXIT_DUP2 );
	return KErrNone;
	}

void CAcmReader::SetTerminators(const TCommConfigV01& aConfig)
/**
 * API to change the terminator characters.
 *
 * @param aConfig The new configuration.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_SETTERMINATORS_ENTRY );
	
	// C32 protects the port against having config set while there's a request 
	// outstanding.
	if (iCurrentRequest.iClientPtr)
		{
		OstTraceExt1( TRACE_FATAL, CACMREADER_SETTERMINATORS_DUP2, 
				"CAcmReader::SetTerminators;iCurrentRequest.iClientPtr=%p", iCurrentRequest.iClientPtr );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	iTerminatorCount = aConfig.iTerminatorCount;
	OstTrace1( TRACE_NORMAL, CACMREADER_SETTERMINATORS, 
			"CAcmReader::SetTerminators;\tnow %d terminators:", (TInt)iTerminatorCount );
	
	for ( TUint ii = 0; ii < static_cast<TUint>(KConfigMaxTerminators) ; ii++ )
		{
		iTerminator[ii] = aConfig.iTerminator[ii];
		OstTrace1( TRACE_NORMAL, CACMREADER_SETTERMINATORS_DUP1, 
				"CAcmReader::SetTerminators;\t\t%d", iTerminator[ii]);
		}
	OstTraceFunctionExit0( CACMREADER_SETTERMINATORS_EXIT );
	}
	
void CAcmReader::ReadCompleted(TInt aError)
/**
 * Called by lower classes when an LDD Read completes. 
 *
 * @param aError Error.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_READCOMPLETED_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMREADER_READCOMPLETED, 
			"CAcmReader::ReadCompleted;>>CAcmReader::ReadCompleted aError=%d", aError );				   	

	const TUint justRead = static_cast<TUint>(iInBuf.Length());
	OstTraceExt2( TRACE_NORMAL, CACMREADER_READCOMPLETED_DUP1, 
			"CAcmReader::ReadCompleted;\tiInBuf length=%d, iLengthToGo=%d", (TInt)justRead, (TInt)iLengthToGo );

	// This protects against a regression in the LDD- read requests shouldn't 
	// ever complete with zero bytes and KErrNone.
	if ( justRead == 0 && aError == KErrNone )
		{
		OstTrace0( TRACE_FATAL, CACMREADER_READCOMPLETED_DUP3, "CAcmReader::ReadCompleted;ustRead == 0 && aError == KErrNone" );	
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	// The new data will have been added to our buffer. Move iInPtr up by the 
	// length just read.
	iInPtr += justRead;

	if ( aError )
		{
		// If the read failed, we complete back to the client and don't do 
		// anything more. (We don't want to get into retry strategies.) In a 
		// multi-stage Read the client will get any data already written back 
		// to them with IPCWrite.
		CompleteRequest(aError);
		OstTraceFunctionExit0( CACMREADER_READCOMPLETED_EXIT );
		return;
		}

	// calling this will write the data to the clients address space
	// it will also complete the request if we've given the client enough data
	// or will reissue another read if not
	ReadWithoutTerminators();
	OstTrace0( TRACE_NORMAL, CACMREADER_READCOMPLETED_DUP2, "CAcmReader::ReadCompleted;<<CAcmReader::ReadCompleted" );
	OstTraceFunctionExit0( CACMREADER_READCOMPLETED_EXIT_DUP1 );
	}

void CAcmReader::ReadOneOrMoreCompleted(TInt aError)
/**
 * Called by lower classes when an LDD ReadOneOrMore completes. 
 *
 * @param aError Error.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_READONEORMORECOMPLETED_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMREADER_READONEORMORECOMPLETED, 
			"CAcmReader::ReadOneOrMoreCompleted;>>CAcmReader::ReadOneOrMoreCompleted aError=%d", aError );						   	

	const TUint justRead = static_cast<TUint>(iInBuf.Length());
	OstTrace1( TRACE_NORMAL, CACMREADER_READONEORMORECOMPLETED_DUP1, 
			"CAcmReader::ReadOneOrMoreCompleted;\tjustRead = %d", (TInt)justRead );

	// The new data will have been added to our buffer. Move iInPtr 
	// up by the length just read.
	iInPtr += justRead;
	if ( aError )
		{
		// If the ReadOneOrMore failed, we complete back to the client and 
		// don't do anything more. The client will get any data already 
		// written back to them with IPCWrite.
		CompleteRequest(aError);
		OstTraceFunctionExit0( CACMREADER_READONEORMORECOMPLETED_EXIT );
		return;
		}

	// TODO: may the LDD complete ROOM with zero bytes, eg if a ZLP comes in?
	// NB The LDD is at liberty to complete a ReadPacket request with zero 
	// bytes and KErrNone, if the given packet was zero-length (a ZLP). In 
	// this case, we have to reissue the packet read until we do get some 
	// data. 
	if ( justRead == 0 )
		{
		OstTrace0( TRACE_NORMAL, CACMREADER_READONEORMORECOMPLETED_DUP2, 
				"CAcmReader::ReadOneOrMoreCompleted;\twe appear to have a ZLP- reissuing ReadOneOrMore" );
		IssueReadOneOrMore();
		OstTraceFunctionExit0( CACMREADER_READONEORMORECOMPLETED_EXIT_DUP1 );
		return;
		}

	if ( EReadOneOrMore == iCurrentRequest.iRequestType )
		{
		// Complete the client's request with as much data as we can. NB 
		// Opinion may be divided over whether to do this, or complete with 
		// just 1 byte. We implement the more generous approach.
		OstTrace1( TRACE_NORMAL, CACMREADER_READONEORMORECOMPLETED_DUP3, 
				"CAcmReader::ReadOneOrMoreCompleted;\tcurrent request is ReadOneOrMore- completing with %d bytes", (TInt)justRead );
		WriteBackData(justRead);
		CompleteRequest(KErrNone);
		}
	else
		{
		// Outstanding request is a Read with terminators. (Except for 
		// ReadOneOrMore, we only request LDD::ReadOneOrMore in this case.)

		// Process the buffer for terminators. 
		OstTrace0( TRACE_NORMAL, CACMREADER_READONEORMORECOMPLETED_DUP4, 
				"CAcmReader::ReadOneOrMoreCompleted;\tcurrent request is Read with terminators" );
		CheckForBufferedTerminatorsAndProceed();
		}

	OstTrace0( TRACE_NORMAL, CACMREADER_READONEORMORECOMPLETED_DUP5, 
			"CAcmReader::ReadOneOrMoreCompleted;<<CAcmReader::ReadOneOrMoreCompleted" );
	OstTraceFunctionExit0( CACMREADER_READONEORMORECOMPLETED_EXIT_DUP2 );
	}

void CAcmReader::NotifyDataAvailableCompleted(TInt aError)
	{
	OstTraceFunctionEntry0( CACMREADER_NOTIFYDATAAVAILABLECOMPLETED_ENTRY );
/**
 * Called by lower classes when data has arrived at the LDD after a 
 * NotifyDataAvailable request has been posted on the port. 
 *
 * @param aError Error.
 */
	OstTrace1( TRACE_NORMAL, CACMREADER_NOTIFYDATAAVAILABLECOMPLETED, 
			"CAcmReader::NotifyDataAvailableCompleted;aError=%d", aError );
	
	// If the NotifyDataAvailable request failed, we complete back 
	// to the client and don't do anything more.
	CompleteRequest(aError);
	OstTrace0( TRACE_NORMAL, CACMREADER_NOTIFYDATAAVAILABLECOMPLETED_DUP2, "<<CAcmReader::NotifyDataAvailableCompleted" );
	OstTraceFunctionExit0( CACMREADER_NOTIFYDATAAVAILABLECOMPLETED_EXIT );
	}

void CAcmReader::CheckBufferEmptyAndResetPtrs()
/**
 * Utility to assert that the buffer is empty and to reset the read and write 
 * pointers to the beginning of the buffer. We reset them there to cut down on 
 * fiddling around wrapping at the end of the buffer.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_CHECKBUFFEREMPTYANDRESETPTRS_ENTRY );
	TInt	blen = BufLen();
	if ( blen != 0 )
		{
		OstTrace1( TRACE_FATAL, CACMREADER_CHECKBUFFEREMPTYANDRESETPTRS, "CAcmReader::CheckBufferEmptyAndResetPtrs;blen=%d", blen );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	iOutPtr = iInPtr = iBufStart;
	OstTraceFunctionExit0( CACMREADER_CHECKBUFFEREMPTYANDRESETPTRS_EXIT );
	}

void CAcmReader::CheckNewRequest(const TAny* aClientBuffer, TUint aMaxLen)
/**
 * Utility function to check a Read or ReadOneOrMore request from the port. 
 * Also checks that there isn't a request already outstanding. Makes a note of 
 * the relevant parameters and sets up internal counters.
 *
 * @param aClientBuffer Pointer to the client's memory space.
 * @param aMaxLen Maximum length to read.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_CHECKNEWREQUEST_ENTRY );
	
	// The port should handle zero-length reads, not us.
	if (aMaxLen <= 0)
		{
		OstTrace0( TRACE_FATAL, CACMREADER_CHECKNEWREQUEST, "CAcmReader::CheckNewRequest;aMaxLen <= 0" );
		
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	if (aMaxLen > static_cast<TUint>(KMaxTInt))
		{
		OstTrace0( TRACE_FATAL, CACMREADER_CHECKNEWREQUEST_DUP1, "CAcmReader::CheckNewRequest;aMaxLen > static_cast<TUint>(KMaxTInt)" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	// Check we have no outstanding request already.
	if ( iCurrentRequest.iClientPtr )// just panic in case of concurrent Read or ReadOneOrMore queries.
		{							 // in case of NotifyDataAvailable queries, we already have completed the client with KErrInUse.
									 // This code is kept for legacy purpose. That justifies the existence of IsNotifyDataAvailableQueryPending
		OstTraceExt1( TRACE_FATAL, CACMREADER_CHECKNEWREQUEST_DUP3, 
				"CAcmReader::CheckNewRequest;iCurrentRequest.iClientPtr=%p", iCurrentRequest.iClientPtr );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	// Sanity check on what C32 gave us.
	if (!aClientBuffer)
		{
		OstTrace0( TRACE_FATAL, CACMREADER_CHECKNEWREQUEST_DUP2, "CAcmReader::CheckNewRequest;aClientBuffer = NULL" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	// Make a note of interesting data.
	iCurrentRequest.iClientPtr = aClientBuffer;
	iLengthToGo = aMaxLen;
	iOffsetIntoClientsMemory = 0;
	OstTraceFunctionExit0( CACMREADER_CHECKNEWREQUEST_EXIT );
	}

void CAcmReader::CheckForBufferedTerminatorsAndProceed()
/**
 * Checks for terminator characters in the buffer. Completes the client's 
 * request if possible, and issues further ReadOneOrMores for the appropriate 
 * amount if not.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED_ENTRY );
	
	TInt ret = FindTerminator();
	OstTrace1( TRACE_NORMAL, CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED_DUP6, 
			"CAcmReader::CheckForBufferedTerminatorsAndProceed;\tFindTerminator = %d", ret );

	if ( ret < KErrNone )
		{
		OstTrace0( TRACE_NORMAL, CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED, 
				"CAcmReader::CheckForBufferedTerminatorsAndProceed;\tno terminator found" );
		
		const TUint bufLen = BufLen();
		OstTrace1( TRACE_NORMAL, CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED_DUP1, 
				"CAcmReader::CheckForBufferedTerminatorsAndProceed;\tbufLen = %d", (TInt)bufLen );
		
		// No terminator was found. Does the buffer already exceed the 
		// client's descriptor?
		if ( bufLen >= iLengthToGo )
			{
			// Yes- complete as much data to the client as their 
			// descriptor can handle.
			OstTrace1( TRACE_NORMAL, CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED_DUP2, 
					"CAcmReader::CheckForBufferedTerminatorsAndProceed;iLengthToGo=%d", (TInt)iLengthToGo );
			WriteBackData(iLengthToGo);
			CompleteRequest(KErrNone);
			}
		else
			{
			// No- write back the data we've got and issue a request to get 
			// some more data. 
			WriteBackData(bufLen);
			IssueReadOneOrMore();
			}
		}
	else
		{
		OstTrace0( TRACE_NORMAL, CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED_DUP3, 
				"CAcmReader::CheckForBufferedTerminatorsAndProceed;\tterminator found!" );
		// Will the terminator position fit within the client's descriptor?
		if ( static_cast<TUint>(ret) <= iLengthToGo )
			{
			// Yes- complete (up to the terminator) back to the client.
			OstTrace1( TRACE_NORMAL, CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED_DUP4, 
					"CAcmReader::CheckForBufferedTerminatorsAndProceed;\tterminator will fit "
					"in client's descriptor- writing back %d bytes ", (TInt)ret );
			WriteBackData(static_cast<TUint>(ret));
			CompleteRequest(KErrNone);
			}
		else
			{
			// No- complete as much data to the client as their descriptor can 
			// handle.
			OstTrace1( TRACE_NORMAL, CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED_DUP5, 
							"CAcmReader::CheckForBufferedTerminatorsAndProceed; "
							"\tterminator won't fit in client's descriptor- writing back %d bytes", (TInt)iLengthToGo );
			WriteBackData(iLengthToGo);
			CompleteRequest(KErrNone);
			}
		}
	OstTraceFunctionExit0( CACMREADER_CHECKFORBUFFEREDTERMINATORSANDPROCEED_EXIT );
	}

void CAcmReader::WriteBackData(TUint aLength)
/**
 * This function writes back data to the client address space. The write will 
 * be performed in one go if the data to be written back is not wrapped across 
 * the end of the circular buffer. If the data is wrapped, the write is done 
 * in two stages. The read pointer is updated to reflect the data consumed, as 
 * is the counter for remaining data required by the client (iLengthToGo), and 
 * the pointer into the client's memory (iOffsetIntoClientsMemory).
 *
 * @param aLength Amount of data to write back.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_WRITEBACKDATA_ENTRY );
	OstTrace1( TRACE_DUMP, CACMREADER_WRITEBACKDATA, "CAcmReader::WriteBackData;aLength=%d", (TInt)aLength );
	OstTrace1( TRACE_DUMP, CACMREADER_WRITEBACKDATA_DUP1, "CAcmReader::WriteBackData;\tBufLen() = %d", (TInt)BufLen() );

	if (aLength > BufLen())
		{
		OstTrace0( TRACE_FATAL, CACMREADER_WRITEBACKDATA_DUP15, "CAcmReader::WriteBackData;aLength > BufLen()" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}

	const TUint lenBeforeWrap = iBufStart + iBufSize - iOutPtr;

	OstTrace1( TRACE_DUMP, CACMREADER_WRITEBACKDATA_DUP2, "CAcmReader::WriteBackData;\tiOutPtr=%d", (TInt)(iOutPtr - iBufStart) );
	OstTrace1( TRACE_DUMP, CACMREADER_WRITEBACKDATA_DUP3, "CAcmReader::WriteBackData;\tiInPtr=%d", (TInt)(iInPtr - iBufStart) );
	OstTrace1( TRACE_DUMP, CACMREADER_WRITEBACKDATA_DUP4, "CAcmReader::WriteBackData;\tiOffsetIntoClientsMemory=%d", (TInt)iOffsetIntoClientsMemory );
	OstTrace1( TRACE_DUMP, CACMREADER_WRITEBACKDATA_DUP5, "CAcmReader::WriteBackData;\tlenBeforeWrap=%d", (TInt)lenBeforeWrap );
	
	if ( aLength > lenBeforeWrap )
		{ 
		// We'll have to do this in two stages...
		OstTrace0( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP6, "CAcmReader::WriteBackData;\twriting back in two stages" );
		if (!BufWrap())
			{
			OstTrace0( TRACE_FATAL, CACMREADER_WRITEBACKDATA_DUP16, "CAcmReader::WriteBackData;BufWrap() is NULL" );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}

		// Stage 1...
		TPtrC8 ptrBeforeWrap(iOutPtr, lenBeforeWrap);
		TInt err = iPort.IPCWrite(iCurrentRequest.iClientPtr,
			ptrBeforeWrap,
			iOffsetIntoClientsMemory);
		OstTrace1( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP7, "CAcmReader::WriteBackData;\tIPCWrite = %d", (TInt)err );
		if (err)
			{
			OstTrace1( TRACE_FATAL, CACMREADER_WRITEBACKDATA_DUP17, "CAcmReader::WriteBackData;err=%d", err );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		static_cast<void>(err);
		iOffsetIntoClientsMemory += lenBeforeWrap;

		// Stage 2...
		TInt seg2Len = aLength - lenBeforeWrap;
		TPtrC8 ptrAfterWrap(iBufStart, seg2Len);
		err = iPort.IPCWrite(iCurrentRequest.iClientPtr,
			ptrAfterWrap,
			iOffsetIntoClientsMemory);
		OstTrace1( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP8, "CAcmReader::WriteBackData;\tIPCWrite = %d", (TInt)err );
		if (err)
			{
			OstTrace1( TRACE_FATAL, CACMREADER_WRITEBACKDATA_DUP18, "CAcmReader::WriteBackData;err=%d", err );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		iOffsetIntoClientsMemory += seg2Len;

		// and set the pointers to show that we've consumed the data...
		iOutPtr = iBufStart + seg2Len;
		OstTrace0( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP9, "CAcmReader::WriteBackData;\twrite in two segments completed" );
		}
	else // We can do it in one go...
		{
		OstTrace0( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP10, "CAcmReader::WriteBackData;\twriting in one segment" );

		TPtrC8 ptr(iOutPtr, aLength);
		TInt err = iPort.IPCWrite(iCurrentRequest.iClientPtr,
			ptr,
			iOffsetIntoClientsMemory);
		OstTrace1( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP11, "CAcmReader::WriteBackData;\tIPCWrite = %d", (TInt)err );
		if (err)
			{
			OstTrace1( TRACE_FATAL, CACMREADER_WRITEBACKDATA_DUP19, "CAcmReader::WriteBackData;err=%d", err );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		static_cast<void>(err);
		iOffsetIntoClientsMemory += aLength;

		// Set the pointers to show that we've consumed the data...
		iOutPtr += aLength;
		OstTrace0( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP12, "CAcmReader::WriteBackData;\twrite in one segments completed" );
		}

	OstTrace1( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP13, 
			"CAcmReader::WriteBackData;\tiOutPtr=%d", (TInt)(iOutPtr - iBufStart) );
	OstTrace1( TRACE_NORMAL, CACMREADER_WRITEBACKDATA_DUP14, 
			"CAcmReader::WriteBackData;\tiOffsetIntoClientsMemory=%d", (TInt)iOffsetIntoClientsMemory );

	// Adjust iLengthToGo
	if (iLengthToGo < aLength)
		{
		OstTraceExt2( TRACE_FATAL, CACMREADER_WRITEBACKDATA_DUP20, 
				"CAcmReader::WriteBackData;iLengthToGo=%d;aLength=%d", (TInt)iLengthToGo, (TInt)aLength );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iLengthToGo -= aLength;
	OstTraceFunctionExit0( CACMREADER_WRITEBACKDATA_EXIT );
	}

void CAcmReader::CompleteRequest(TInt aError)
/**
 * Utility to reset our 'outstanding request' flag and complete the client's 
 * request back to them. Does not actually write back data to the client's 
 * address space.
 * 
 * @param aError The error code to complete with.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_COMPLETEREQUEST_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMREADER_COMPLETEREQUEST, "CAcmReader::CompleteRequest;aError=%d", (TInt)aError );

	// Set our flag to say that we no longer have an outstanding request.
	iCurrentRequest.iClientPtr = NULL;
	if(ENotifyDataAvailable==iCurrentRequest.iRequestType)
		{
		OstTrace1( TRACE_NORMAL, CACMREADER_COMPLETEREQUEST_DUP1, 
				"CAcmReader::CompleteRequest;\tcalling NotifyDataAvailableCompleted with error %d", aError );
		iPort.NotifyDataAvailableCompleted(aError);	
		}
	else // read and readoneormore
		{
		OstTrace1( TRACE_NORMAL, CACMREADER_COMPLETEREQUEST_DUP2, 
					"CAcmReader::CompleteRequest;\tcalling ReadCompleted with error %d", aError );
		iPort.ReadCompleted(aError);
		}
	OstTraceFunctionExit0( CACMREADER_COMPLETEREQUEST_EXIT );
	}

void CAcmReader::IssueRead()
/**
 * Issues a read request for N bytes, where N is the minimum of iLengthToGo, 
 * the LDD's limit on Read requests, and how far we are from the end of our 
 * buffer. Used when trying to satisfy an RComm Read without terminators.
 * We enforce that the buffer is empty, so we don't have to worry about the 
 * buffer being wrapped and the consequent risk of overwriting.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_ISSUEREAD_ENTRY );
	CheckBufferEmptyAndResetPtrs();

	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREAD, "CAcmReader::IssueRead;iBufSize=%d", (TInt)iBufSize );
	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREAD_DUP1, "CAcmReader::IssueRead;\tiInPtr = %d", (TInt)(iInPtr - iBufStart) );
	const TUint lenBeforeWrap = iBufStart + iBufSize - iInPtr;
	
	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREAD_DUP2, "CAcmReader::IssueRead;\tiLengthToGo = %d", (TInt)iLengthToGo );
	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREAD_DUP3, "CAcmReader::IssueRead;\tlenBeforeWrap = %d", (TInt)lenBeforeWrap );

	const TUint limit = Min(static_cast<TInt>(iLengthToGo), 
		static_cast<TInt>(lenBeforeWrap));
	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREAD_DUP4, "CAcmReader::IssueRead;\tissuing read for %d bytes", (TInt)limit );
	iInBuf.Set(iInPtr, 0, limit);
	if (!iPort.Acm())
		{
		OstTrace0( TRACE_FATAL, CACMREADER_ISSUEREAD_DUP5, "CAcmReader::IssueRead;iPort.Acm() is null" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iPort.Acm()->Read(*this, iInBuf, limit);
	OstTraceFunctionExit0( CACMREADER_ISSUEREAD_EXIT );
	}

void CAcmReader::IssueReadOneOrMore()
/**
 * Issues a read request for N bytes, where N is the minimum of iLengthToGo, 
 * and how far we are from the end of our buffer. Used when trying to satisfy 
 * an RComm ReadOneOrMore.
 * We enforce that the buffer is empty, so we don't have to worry about the 
 * buffer being wrapped and the consequent risk of overwriting.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_ISSUEREADONEORMORE_ENTRY );
	CheckBufferEmptyAndResetPtrs();

	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREADONEORMORE, "CAcmReader::IssueReadOneOrMore;iBufSize=%d", (TInt)iBufSize );
	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREADONEORMORE_DUP1, 
			"CAcmReader::IssueReadOneOrMore;\tiInPtr = %d",(TInt)(iInPtr - iBufStart) );
	const TUint lenBeforeWrap = iBufStart + iBufSize - iInPtr;
	
	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREADONEORMORE_DUP2, "CAcmReader::IssueReadOneOrMore;iLengthToGo=%d", (TInt)iLengthToGo );
	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREADONEORMORE_DUP3, "CAcmReader::IssueReadOneOrMore;lenBeforeWrap=%d", (TInt)lenBeforeWrap );

	const TUint limit1 = Min(static_cast<TInt>(lenBeforeWrap), iLengthToGo);
	OstTrace1( TRACE_DUMP, CACMREADER_ISSUEREADONEORMORE_DUP4, 
			"CAcmReader::IssueReadOneOrMore;\tissuing read one or more for %d bytes", (TInt)limit1 );

	iInBuf.Set(iInPtr, 0, limit1);
	if (!iPort.Acm())
		{
		OstTrace0( TRACE_FATAL, CACMREADER_ISSUEREADONEORMORE_DUP5, "CAcmReader::IssueReadOneOrMore;iPort.Acm() is null" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iPort.Acm()->ReadOneOrMore(*this, iInBuf, limit1);
	OstTraceFunctionExit0( CACMREADER_ISSUEREADONEORMORE_EXIT );
	}

TInt CAcmReader::FindTerminator() const
/**
 * This function searches the circular buffer for one of a number of 
 * termination characters.
 * The search is conducted between the read and write pointers. The function 
 * copes with the wrapping of the buffer.
 *
 * @return If positive: the number of bytes between where iOutPtr is pointing 
 * at and where the terminator was found inclusive. Takes wrapping into 
 * account. If negative: error.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_FINDTERMINATOR_ENTRY );
	
	TInt pos = 0;
	TInt ret = KErrNone;
	if ( !BufWrap() )
		{
		ret = PartialFindTerminator(iOutPtr, iInPtr, pos);
		if ( !ret )
			{
			// Buffer wasn't wrapped, terminator found.
			ret = pos;
			}
		}
	else
		{
		ret = PartialFindTerminator(iOutPtr, iBufStart+iBufSize, pos);
		if ( !ret )
			{
			// Buffer was wrapped, but terminator was found in the section 
			// before the wrap.
			ret = pos;
			}
		else
			{
			ret = PartialFindTerminator(iBufStart, iInPtr, pos);
			if ( !ret )
				{
				// Buffer was wrapped, terminator was found in the wrapped 
				// section.
				const TUint lenBeforeWrap = iBufStart + iBufSize - iOutPtr;
				ret = pos + lenBeforeWrap;
				}
			}
		}

	// Check we're returning what we said we would.
	if (ret == KErrNone)
		{
		OstTrace0( TRACE_FATAL, CACMREADER_FINDTERMINATOR_DUP1, "CAcmReader::FindTerminator;ret == KErrNone" );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	
	OstTrace1( TRACE_NORMAL, CACMREADER_FINDTERMINATOR, "CAcmReader::FindTerminator;ret=%d", ret );
	OstTraceFunctionExit0( CACMREADER_FINDTERMINATOR_EXIT );
	return ret;
	}

TInt CAcmReader::PartialFindTerminator(TUint8* aFrom, 
									   TUint8* aTo, 
									   TInt& aPos) const
/**
 * This function searches the buffer for one of a number of termination 
 * characters. The search is conducted between the pointers given. The 
 * function only searches a continuous buffer space, and does not respect the 
 * circular buffer wrap.
 *
 * @param aFrom The pointer at which to start searching.
 * @param aTo The pointer one beyond where to stop searching.
 * @param aPos The number of bytes beyond (and including) aFrom the terminator 
 * was found. That is, if the terminator was found adjacent to aFrom, aPos 
 * will be 2- the client can take this as meaning that 2 bytes are to be 
 * written back to the RComm client (aFrom and the one containing the 
 * terminator).
 * @return KErrNone- a terminator was found. KErrNotFound- no terminator was 
 * found.
 */
	{
	OstTraceFunctionEntry0( CACMREADER_PARTIALFINDTERMINATOR_ENTRY );
	
	aPos = 1;
	OstTraceExt2( TRACE_NORMAL, CACMREADER_PARTIALFINDTERMINATOR, 
			"CAcmReader::PartialFindTerminator;\taFrom=%d, aTo=%d", (TInt)(aFrom-iBufStart), (TInt)(aTo-iBufStart) );
	
	for ( TUint8* p = aFrom ; p < aTo ; p++, aPos++ )
		{
		for ( TUint i = 0 ; i < iTerminatorCount ; i++ )
			{
			if (!p)
				{
				OstTraceExt1( TRACE_FATAL, CACMREADER_PARTIALFINDTERMINATOR_DUP3, "CAcmReader::PartialFindTerminator;p=%p", p );
				__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
				}
			if ( *p == iTerminator[i] )
				{
				OstTraceExt2( TRACE_FLOW, CACMREADER_PARTIALFINDTERMINATOR_DUP1, 
						"CAcmReader::PartialFindTerminator;\tterminator %d found at aPos %d", (TInt)iTerminator[i], (TInt)aPos );
				OstTraceFunctionExit0( CACMREADER_PARTIALFINDTERMINATOR_EXIT );
				return KErrNone;
				}
			}
		}
	
	OstTrace0( TRACE_NORMAL, CACMREADER_PARTIALFINDTERMINATOR_DUP2, "CAcmReader::PartialFindTerminator;\tno terminator found" );
	OstTraceFunctionExit0( CACMREADER_PARTIALFINDTERMINATOR_EXIT_DUP2 );
	return KErrNotFound;
	}

//
// End of file
