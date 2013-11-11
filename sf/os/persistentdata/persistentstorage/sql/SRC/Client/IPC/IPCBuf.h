// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __IPCBUF_H__
#define __IPCBUF_H__

#include <s32buf.h>
#include "IpcDefs.h"

//Forward declarations
class TIpcArgs;
class RSqlDbSession;

/**
HIpcBuf class manages the "client to server" transfer of large data blocks.

It caches the data to be sent/retrieved on the client side and sychronises the content of the client-side cache
with the content of a similar server-side cache, sending appropriate read/write/sync commands to the server.

An instance of TIpcStreamBuf class is uased as a client-side cache.

The following code fragment shows how the class can be used to send to the server large blocks of data:

@code
//Step 1: Create the HIpcBuf instance.
HIpcBuf* ipcBuf = HIpcBuf::NewLC(params, function, args);

//Step 2: Create a stream instance. It will be used for sending the data to the server. All details about
//		  the data transfer will be performed by the ipcBuf object.
RWriteStream out(ipcBuf);

//Step 3: "TheObject" is some large data object which has to be sent to the server. Let's assume 
//		  "TheObject" has ExternalizeL() function which can be used to externalize the object into an output stream.
TheObject.ExternalizeL(out);

//Step 4: Commit the output stream. After the commit command the server will have all the data transferred.
out.CommitL();		

//Step 5: Cleanup.
CleanupStack::PopAndDestroy(ipcBuf);
@endcode

@see RSqlDbSession;
@see TIpcStreamBuf
@see RSessionBase

@internalComponent
*/
NONSHARABLE_CLASS(HIpcBuf) : public TStreamBuf
	{
public:
	static HIpcBuf* NewL(RSqlDbSession& aSession, TInt aFunction, TIpcArgs& aArgs);
	static HIpcBuf* NewLC(RSqlDbSession& aSession, TInt aFunction, TIpcArgs& aArgs);
	virtual ~HIpcBuf();
	
private:
	HIpcBuf(RSqlDbSession& aSession);
	void ConstructL(TInt aFunction, TIpcArgs& aArgs);
	
	// from TStreamBuf
	TInt UnderflowL(TInt aMaxLength);
	void OverflowL();
	void DoRelease();
	void DoSynchL();
	TInt DoReadL(TAny* aPtr, TInt aMaxLength);
	void DoWriteL(const TAny* aPtr, TInt aLength);
	TStreamPos DoSeekL(TMark aMark, TStreamLocation aLocation, TInt anOffset);
	
private:
	inline void SetPos(TRead, TInt aPos);
	inline void SetPos(TWrite, TInt aPos);
	inline TInt Pos(TRead) const;
	inline TInt Pos(TWrite) const;
	inline TInt MovePos(TRead, TInt aOffset);
	TInt IpcReadL(TAny* aPtr, TInt aMaxLength);
	void IpcWriteL(const TAny* aPtr, TInt aLength);
	TInt EndL();

	inline TInt Lag(TRead) const;
	inline TInt Lag(TWrite) const;
	inline TInt Mark(TRead) const;
	inline TInt Mark(TWrite) const;
	
private:
	RSqlDbSession& 	iSession;
	TInt			iHandle;
	TInt 			iRPos;
	TInt 			iWPos;
	TIpcStreamBuf	iBuf;
	};

#include "IPCBuf.inl"

#endif//__IPCBUF_H__
