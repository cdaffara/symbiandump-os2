/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ACMWRITER_H__
#define __ACMWRITER_H__

#include <e32base.h>
#include "WriteObserver.h"

class CAcmPort;

NONSHARABLE_CLASS(CAcmWriter) : public CBase, 
					public MWriteObserver
/**
 * CAcmWriter presents an interface for writing data to the bulk endpoint on 
 * the LDD.
 * Note that we reject write requests bigger than the buffer. We also don't 
 * support KConfigWriteBufferedComplete, so client requests are turned into 
 * one write request on the LDD. When the LDD request completes, we complete 
 * to the client. 
 */
	{
public:
	static CAcmWriter* NewL(CAcmPort& aPort, TUint aBufSize);
	~CAcmWriter();

public: // APIs for writing
	void Write(const TAny* aClientBuffer, TUint aLength);
	void WriteCancel();

public: // buffer APIs
	void ResetBuffer();
	TInt SetBufSize(TUint aSize);

private:
	CAcmWriter(CAcmPort& aPort, TUint aBufSize);
	void ConstructL();

private: // from MWriteObserver
	void WriteCompleted(TInt aError);

private: // utility
	void ReadDataFromClient();
	void CheckNewRequest(const TAny* aClientBuffer, TUint aLength);
	void CompleteRequest(TInt aError);
	void IssueWrite();

private: // owned- information on the current request from the port (if any)

	NONSHARABLE_STRUCT(TRequestData)
		{
		// Pointer to the client's memory to get the data to write.
		const TAny* iClientPtr;

		// Number of bytes to write.
		TUint iLength;
		};
	// This struct is populated when a new client request comes in.
	TRequestData iCurrentRequest;

private: // owned- information on our attempt to satisfy the current request 
	// from the port (if any)

	// Offset into iClientPtr to get more data from using IPCRead.
	TUint iOffsetIntoClientsMemory;

	// Number of bytes remaining to be written before we can complete the 
	// client's request.
	TUint iLengthToGo;

private: // owned- information on the buffer

	// The current actual size of the buffer.
	TUint iBufSize; 

	// Our buffer of data. This is for temporarily storing RComm client data 
	// while we are writing it to the LDD.
	HBufC8* iBuffer;	
	
	// Descriptor pointer into iBuffer. We use this in Write calls to the LDD.
	TPtr8 iBuf;

private: // unowned
	// Used to access the (RComm) client's memory and to complete their 
	// request. Also accesses the ACM class through the port. (The ACM class 
	// may come and go, but not the ACM port.)
	CAcmPort& iPort; 
	};

#endif // __ACMWRITER_H__
