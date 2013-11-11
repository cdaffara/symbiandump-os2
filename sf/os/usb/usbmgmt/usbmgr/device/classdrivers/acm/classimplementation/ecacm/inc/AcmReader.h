/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ACMREADER_H__
#define __ACMREADER_H__

#include <e32std.h>
#include <d32comm.h>
#include "ReadObserver.h"
#include "ReadOneOrMoreObserver.h"
#include "NotifyDataAvailableObserver.h"

class CCdcAcmClass;
class CAcmPort;

NONSHARABLE_CLASS(CAcmReader) : public CBase,
					public MReadObserver,
					public MReadOneOrMoreObserver,
					public MNotifyDataAvailableObserver
/**
 * CAcmReader maintains the port's read buffer, fields requests from the port 
 * (its client) for data, and administers getting more data from the LDD. 
 * 
 * It presents an interface for requesting data- whether the required data is 
 * immediately available in the buffer or not, it eventually completes the 
 * client (port)'s request using CPort::IPCWrite and CPort::ReadCompleted.
 *
 * It uses the CCdcAcmClass to ask for more data from the host. Consequently 
 * it implements MReadObserver and MReadOneOrMoreObserver to be notified when 
 * data has come in.
 */
	{
public:
	static CAcmReader* NewL(CAcmPort& aPort, 
		TUint aBufSize);
	~CAcmReader();

public: // APIs for reading
	void Read(const TAny* aClientBuffer, TUint aMaxLen);
	void ReadOneOrMore(const TAny* aClientBuffer, TUint aMaxLen);
	void ReadCancel();
	void NotifyDataAvailable();
	void NotifyDataAvailableCancel();
	inline TBool IsNotifyDataAvailableQueryPending() const; 

public: // buffer APIs
	TUint BufLen() const;
	inline TUint BufSize() const;
	void ResetBuffer();
	TInt SetBufSize(TUint aSize);
	void SetTerminators(const TCommConfigV01& aConfig);

private: 
	CAcmReader(CAcmPort& aPort,
		TUint aBufSize);
	void ConstructL();

private: // from MReadObserver
	void ReadCompleted(TInt aError);

private: // from MReadOneOrMoreObserver
	void ReadOneOrMoreCompleted(TInt aError);

private: // from MNotifyDataAvailableObserver
	void NotifyDataAvailableCompleted(TInt aError);

private: // utilities
	inline TBool BufWrap() const;
	void CheckBufferEmptyAndResetPtrs();
	void CheckNewRequest(const TAny* aClientBuffer, TUint aMaxLen);
	void CheckForBufferedTerminatorsAndProceed();

	void WriteBackData(TUint aLength);
	void CompleteRequest(TInt aError);
		
	void ReadWithoutTerminators();
	void ReadWithTerminators();
	
	void IssueRead();
	void IssueReadOneOrMore();

	TInt FindTerminator() const;
	TInt PartialFindTerminator(TUint8* aFrom, TUint8* aTo, TInt& aPos) const;
	
private: // owned- information on the current request from the port (if any)

	enum TRequestType
			{
			ERead,
			EReadOneOrMore,
			ENotifyDataAvailable
			};

	NONSHARABLE_STRUCT(TRequestData)
		{
		// Pointer to the client's memory to put the data into. This is also 
		// used as a flag for whether we have an outstanding request or not.
		const TAny* iClientPtr; 
		// type of the current request
		TRequestType iRequestType;
		};
	// This struct is populated when a new client request comes in.
	TRequestData iCurrentRequest;

private: // owned- information on our attempt to satisfy the current request 
	// from the port (if any)

	// Starts as the amount of data requested by the client.
	// This may not be the same as the 
	// amount of data given back to the client when the request is 
	// completed, for instance if the read is a one-or-more, or if 
	// terminators are defined.
	// Will be the number of bytes remaining to be read before we can 
	// complete the client's request.
	TUint iLengthToGo; 

	// Offset into iClientPtr to write more data to using IPCWrite.
	TUint iOffsetIntoClientsMemory;

private: // owned- information on the buffer
	
	// The current actual size of the buffer.
	TUint iBufSize;

	// Our buffer of data.
	HBufC8* iBuffer;

	// These TUints are pointers to various places in iBuffer. 

	// Pointer to where data will be placed into our buffer (from the LDD).
	TUint8* iInPtr; 
	// Pointer to where data will leave our buffer (to go up to the client).
	TUint8* iOutPtr; 
	// Pointer to the start of the buffer.
	TUint8* iBufStart;
	
	// Descriptor pointer into iBuffer. We use this in Read and ReadOneOrMore 
	// calls to the LDD to get data written into iBuffer.
	TPtr8 iInBuf;

private: // owned- information on the terminator characters
	TUint iTerminatorCount;
	TFixedArray<TText8, KConfigMaxTerminators> iTerminator;

private: // unowned
	// Used to access the (RComm) client's memory and to complete their 
	// request. Also accesses the ACM class through the port. (The ACM class 
	// may come and go, following the lifetime of the registration port, not 
	// the ACM port.)
	CAcmPort& iPort; 
	};

TBool CAcmReader::BufWrap() const
/**
 * This function indicates whether the circular buffer has wrapped.
 *
 * @return TBool Indication of whether the buffer has wrapped.
 */
	{
	TBool ret = EFalse;
	if ( iOutPtr > iInPtr )
		{
		ret =  ETrue;
		}

	return ret;
	}

TUint CAcmReader::BufSize() const
/**
 * Accessor for current buffer size.
 *
 * @return The current buffer size.
 */
	{
	return iBufSize;
	}

TBool CAcmReader::IsNotifyDataAvailableQueryPending() const
/**
 * check if a NotifyDataAvailable query is in progress
 *
 * @return ETrue if a NotifyDataAvailable query is pending, EFalse if not.
 */
	{ 
	return ((iCurrentRequest.iClientPtr)&&(ENotifyDataAvailable==iCurrentRequest.iRequestType));
	}

#endif // __ACMREADER_H__
