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

#include "AcmWriter.h"
#include "AcmPort.h"
#include "AcmPanic.h"
#include "AcmUtils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "AcmWriterTraces.h"
#endif

CAcmWriter* CAcmWriter::NewL(CAcmPort& aPort, 
							 TUint aBufSize)
/**
 * Factory function.
 *
 * @param aPort Owning CAcmPort object.
 * @param aBufSize Required buffer size.
 * @return Ownership of a newly created CAcmWriter object
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_NEWL_ENTRY );
	CAcmWriter* self = new(ELeave) CAcmWriter(aPort, aBufSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	CLEANUPSTACK_POP(self);
	OstTraceFunctionExit0( CACMWRITER_NEWL_EXIT );
	return self;
	}

CAcmWriter::~CAcmWriter()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_CACMWRITER_DES_ENTRY );
	WriteCancel();
	delete iBuffer;
	OstTraceFunctionExit0( CACMWRITER_CACMWRITER_DES_EXIT );
	}

void CAcmWriter::Write(const TAny* aClientBuffer, TUint aLength)
/**
 * Queue a write.
 *
 * @param aClientBuffer pointer to the Client's buffer
 * @param aLength Number of bytes to write
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_WRITE_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CACMWRITER_WRITE, "CAcmWriter::Write;aClientBuffer=%p;aLength=%d", aClientBuffer, (TInt)aLength );

	// Check we're open to requests and make a note of interesting data.
	CheckNewRequest(aClientBuffer, aLength);

	// If the write size greater than the current buffer size then the
	// request will now complete over multiple operations. (This used to 
	// simply reject the write request with KErrNoMemory)
	
	// Get as much data as we can from the client into our buffer
	ReadDataFromClient();
	// ...and write as much as we've got to the LDD
	IssueWrite();
	OstTraceFunctionExit0( CACMWRITER_WRITE_EXIT );
	}

void CAcmWriter::WriteCancel()
/**
 * Cancel a write.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_WRITECANCEL_ENTRY );
	// Cancel any outstanding request on the LDD.
	if ( iPort.Acm() )
		{
		OstTrace0( TRACE_NORMAL, CACMWRITER_WRITECANCEL, "CAcmWriter::WriteCancel;\tiPort.Acm() exists- calling WriteCancel on it" );
		iPort.Acm()->WriteCancel();
		}

	// Reset our flag to say there's no current outstanding request. What's 
	// already in our buffer can stay there.
	iCurrentRequest.iClientPtr = NULL;
	OstTraceFunctionExit0( CACMWRITER_WRITECANCEL_EXIT );
	}

void CAcmWriter::ResetBuffer()
/**
 * Called by the port to clear the buffer.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_RESETBUFFER_ENTRY );
	// A request is outstanding- C32 should protect against this.

	if (iCurrentRequest.iClientPtr != NULL)
		{
		OstTrace1( TRACE_FATAL, CACMWRITER_RESETBUFFER, 
										"CAcmWriter::ResetBuffer;EPanicInternalError=%d", 
										(TInt)EPanicInternalError );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	// Don't have anything to do. There are no pointers to reset. This 
	// function may in the future (if we support KConfigWriteBufferedComplete) 
	// do work, so leave the above assertion in.
	OstTraceFunctionExit0( CACMWRITER_RESETBUFFER_EXIT );
	}

TInt CAcmWriter::SetBufSize(TUint aSize)
/**
 * Called by the port to set the buffer size. Also used as a utility by us to 
 * create the buffer at instantiation. 
 *
 * @param aSize The required size of the buffer.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_SETBUFSIZE_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMWRITER_SETBUFSIZE, "CAcmWriter::SetBufSize;aSize=%d", (TInt)aSize );

	if ( iCurrentRequest.iClientPtr )
		{
		// A request is outstanding. C32 does not protect us against this.
		OstTrace0( TRACE_NORMAL, CACMWRITER_SETBUFSIZE_DUP1, 
				"CAcmWriter::SetBufSize;\t***a request is outstanding- returning KErrInUse" );
		OstTraceFunctionExit0( CACMWRITER_SETBUFSIZE_EXIT );
		return KErrInUse;
		}

	// Create the new buffer.
	HBufC8* newBuf = HBufC8::New(static_cast<TInt>(aSize));
	if ( !newBuf )
		{
		OstTrace0( TRACE_NORMAL, CACMWRITER_SETBUFSIZE_DUP2, 
				"CAcmWriter::SetBufSize;\tfailed to create new buffer- returning KErrNoMemory" );
		OstTraceFunctionExit0( CACMWRITER_SETBUFSIZE_EXIT_DUP1 );
		return KErrNoMemory;
		}
	delete iBuffer;
	iBuffer = newBuf;
	iBuf.Set(iBuffer->Des());
	iBufSize = aSize;

	OstTraceFunctionExit0( CACMWRITER_SETBUFSIZE_EXIT_DUP2 );
	return KErrNone;
	}

CAcmWriter::CAcmWriter(CAcmPort& aPort, 
					   TUint aBufSize)
/**
 * Constructor.
 *
 * @param aPort The CPort parent.
 * @param aBufSize The size of the buffer.
 */
 :	iBufSize(aBufSize),
	iBuf(NULL,0,0),
	iPort(aPort)
	{
	OstTraceFunctionEntry0( CACMWRITER_CACMWRITER_CONS_ENTRY );
	OstTraceFunctionExit0( CACMWRITER_CACMWRITER_CONS_EXIT );
	}

void CAcmWriter::ConstructL()
/**
 * 2nd-phase constructor. 
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_CONSTRUCTL_ENTRY );
	// Create the required buffer.
	OstTrace0( TRACE_NORMAL, CACMWRITER_CONSTRUCTL, "CAcmWriter::ConstructL;\tabout to create iBuffer" );
	TInt err = SetBufSize(iBufSize);
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CACMWRITER_CONSTRUCTL_DUP1, "CAcmWriter::ConstructL;err=%d", err );
		User::Leave(err);
		}
	OstTraceFunctionExit0( CACMWRITER_CONSTRUCTL_EXIT );
	}

void CAcmWriter::WriteCompleted(TInt aError)
/**
 * This function is called when a write on the LDD has completed. 
 * This checks whether any data remains to be written, if so the 
 * read and write requests are re-issued until there in no data 
 * left or an error occurs.
 *
 * @param aError Error with which the write completed.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_WRITECOMPLETED_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMWRITER_WRITECOMPLETED, "CAcmWriter::WriteCompleted;aError=%d", (TInt)aError );					   	
	if(iLengthToGo == 0 || aError != KErrNone)
		{
		OstTrace1( TRACE_NORMAL, CACMWRITER_WRITECOMPLETED_DUP1, 
				"CAcmWriter::WriteCompleted;\tcompleting request with %d", aError );
		CompleteRequest(aError);	
		}
	else
		{
		//there is some data remaining to be read so reissue the Read & Write 
		//requests until there is no data left.
		ReadDataFromClient();
		IssueWrite();
		}
	OstTraceFunctionExit0( CACMWRITER_WRITECOMPLETED_EXIT );
	}

void CAcmWriter::ReadDataFromClient()
/**
 * Read data from the client space into the internal buffer, prior to writing.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_READDATAFROMCLIENT_ENTRY );
	TPtr8 ptr((TUint8*)iBuf.Ptr(),
			  0,
			  Min(iBuf.MaxLength(), iLengthToGo));

	TInt err = iPort.IPCRead(iCurrentRequest.iClientPtr,
			ptr,
			static_cast<TInt>(iOffsetIntoClientsMemory));
	OstTrace1( TRACE_NORMAL, CACMWRITER_READDATAFROMCLIENT, "CAcmWriter::ReadDataFromClient;\tIPCRead = %d", err );
	
	iBuf.SetLength(ptr.Length());
	if (err != 0)
		{
		OstTrace1( TRACE_FATAL, CACMWRITER_READDATAFROMCLIENT_DUP1, 
										"CAcmWriter::ReadDataFromClient;EPanicInternalError=%d", 
										(TInt)EPanicInternalError );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	static_cast<void>(err);
	// Increase our pointer (into the client's space) of already-read data.
	iOffsetIntoClientsMemory += iBuf.Length();
	OstTraceFunctionExit0( CACMWRITER_READDATAFROMCLIENT_EXIT );
	}



void CAcmWriter::CheckNewRequest(const TAny* aClientBuffer, TUint aLength)
/**
 * Utility function to check a new request from the port. 
 * Also checks that there isn't a request already outstanding. Makes a note of 
 * the relevant parameters and sets up internal counters.
 *
 * @param aClientBuffer Pointer to the client's memory space.
 * @param aLength Length to write.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_CHECKNEWREQUEST_ENTRY );
	
	if (aLength > static_cast<TUint>(KMaxTInt))
		{
		OstTrace1( TRACE_FATAL, CACMWRITER_CHECKNEWREQUEST, 
										"CAcmWriter::CheckNewRequest;aLength > static_cast<TUint>(KMaxTInt), EPanicInternalError=%d", 
										(TInt)EPanicInternalError );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	// Check we have no outstanding request already.
	if ( iCurrentRequest.iClientPtr )
		{
		OstTrace1( TRACE_FATAL, CACMWRITER_CHECKNEWREQUEST_DUP1, 
										"CAcmWriter::CheckNewRequest;iCurrentRequest.iClientPtr != NULL, EPanicInternalError=%d", 
										(TInt)EPanicInternalError );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	// Sanity check on what C32 gave us.
	if (aClientBuffer == NULL)
		{
		OstTrace1( TRACE_FATAL, CACMWRITER_CHECKNEWREQUEST_DUP2, 
										"CAcmWriter::CheckNewRequest;aClientBuffer == NULL, EPanicInternalError=%d", 
										(TInt)EPanicInternalError );
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	// Make a note of interesting data.
	iCurrentRequest.iLength = aLength;
	iCurrentRequest.iClientPtr = aClientBuffer;
	
	iLengthToGo = aLength;
	iOffsetIntoClientsMemory = 0;
	OstTraceFunctionExit0( CACMWRITER_CHECKNEWREQUEST_EXIT );
	}

void CAcmWriter::CompleteRequest(TInt aError)
/**
 * Utility to reset our 'outstanding request' flag and complete the client's 
 * request back to them. 
 * 
 * @param aError The error code to complete with.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_COMPLETEREQUEST_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMWRITER_COMPLETEREQUEST, 
			"CAcmWriter::CompleteRequest;CAcmWriter::CompleteRequest aError=%d", (TInt)aError );
	
	// Set our flag to say that we no longer have an outstanding request.
	iCurrentRequest.iClientPtr = NULL;
	OstTrace1( TRACE_NORMAL, CACMWRITER_COMPLETEREQUEST_DUP1, 
			"CAcmWriter::CompleteRequest;\tcalling WriteCompleted with %d", aError );
	
	iPort.WriteCompleted(aError);
	OstTraceFunctionExit0( CACMWRITER_COMPLETEREQUEST_EXIT );
	}

void CAcmWriter::IssueWrite()
/**
 * Writes a batch of data from our buffer to the LDD. Currently writes the 
 * entire load of buffered data in one go.
 */
	{
	OstTraceFunctionEntry0( CACMWRITER_ISSUEWRITE_ENTRY );
	
	OstTrace1( TRACE_NORMAL, CACMWRITER_ISSUEWRITE, "CAcmWriter::IssueWrite;\tissuing Write of %d bytes", (TInt)(iBuf.Length()) );
	if (iPort.Acm() == NULL)
		{
		OstTrace1( TRACE_FATAL, CACMWRITER_ISSUEWRITE_DUP1, 
					"CAcmWriter::IssueWrite;EPanicInternalError=%d", (TInt)EPanicInternalError);
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	iPort.Acm()->Write(*this, 
		iBuf, 
		iBuf.Length());
	
#ifdef DEBUG
	// A Zero Length Packet is an acceptable packet so iBuf.Length == 0 is acceptable, 
	// if we receive this and the request length > 0 then we may have a problem so check 
	// that the LengthToGo is also 0, if it is not then we may end up looping through this
	// code until a driver write error occurs which may never happen. 
	// This is not expected to occur but the test is in here just to be safe.
	if(iBuf.Length() == 0 && iCurrentRequest.Length() != 0 && iLengthToGo != 0)
		{
		OstTrace1( TRACE_FATAL, CACMWRITER_ISSUEWRITE_DUP2, 
					"CAcmWriter::IssueWrite;EPanicInternalError=%d", (TInt)EPanicInternalError);
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
#endif
	// Update our counter of remaining data to write. 
	iLengthToGo -= iBuf.Length();
	OstTraceFunctionExit0( CACMWRITER_ISSUEWRITE_EXIT );
	}

//
// End of file
