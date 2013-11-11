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

#ifndef __IPCSTREAM_H__
#define __IPCSTREAM_H__

#include <s32mem.h>
#include "IpcDefs.h"

/**
IPC, server-side class.

HIpcStream class is part of the server-side IPC framework, used for transferring large blocks of 
data to/from the client side.

@internalComponent
*/
NONSHARABLE_CLASS(HIpcStream)
	{
public:
	inline HIpcStream(MStreamBuf* aHost, TInt aReadPos);
	~HIpcStream();

	TInt ReadL(const RMessage2& aMessage);	
	void WriteL(const RMessage2& aMessage);	
	inline TInt SizeL();
	void SynchL();
	
private:
	MStreamBuf& 			iHost;
	TInt 					iRPos;
	TInt 					iWPos;
	
	};

/**
IPC, server-side class.

HIpcReadBuf class is part of the server-side IPC framework, used for transferring large blocks of 
data to the client side.

The difference between HIpcReadBuf and HIpcBufBuf is that HIpcReadBuf does not allocate memory for
the data. It keeps just a pointer to the data which needs to be sent to the client side.

@see HIpcStream

@internalComponent
*/
NONSHARABLE_CLASS(HIpcReadBuf) : public TMemBuf
	{
public:
	inline static HIpcReadBuf* NewL(const TDesC8& aDes);
	
private:
	inline HIpcReadBuf(const TDesC8& aDes);
	virtual void DoRelease();
	
	};

#include "IPCStream.inl"

#endif//__IPCSTREAM_H__
