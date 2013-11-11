// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32buf.h>
#include "SqlAssert.h"
#include "IPCStream.h"
#include "SqlSrvResourceProfiler.h"

/**
Reads data from the stream buffer and sends them to the client.

@param aMessage The client message.

@return The number of bytes sent.

Usage of the IPC call arguments:
Arg 0:            not used
Arg 1: [in]       from which position to read
Arg 2: [in/out]   IPC buffer
Arg 3: [in]       max length of the requested data
*/
TInt HIpcStream::ReadL(const RMessage2& aMessage)
	{
	TInt pos=aMessage.Int1();
	if (pos!=iRPos)
		iHost.SeekL(iHost.ERead,EStreamBeginning,pos);
	iRPos=-1;
	TInt len=aMessage.Int3();
	pos+=len;
	TInt tfr=len;
	for (;;)
		{
		TUint8 buf[KIpcStreamSize];
		TInt read=iHost.ReadL(buf,Min(tfr,KIpcStreamSize));
		if (read==0)
			break;
		aMessage.WriteL(2,TPtrC8(buf,read),len-tfr);
		SQLPROFILER_REPORT_IPC(ESqlIpcWrite, read);
		tfr-=read;
		if (tfr==0)
			break;
		if (read<KIpcStreamSize)
			break;
		}
	iRPos=pos-tfr;
	return len-tfr;
	}

/**
Reads data from the client message and stores the data in the stream buffer.

@param aMessage The client message.

@return The number of bytes read and stored in the stream buffer.


Usage of the IPC call arguments:
Arg 0:            not used
Arg 1: [in]       from which position to write
Arg 2: [in/out]   IPC buffer
*/
void HIpcStream::WriteL(const RMessage2& aMessage)
	{
	TInt pos=aMessage.Int1();
	if (pos!=iWPos)
		iHost.SeekL(iHost.EWrite,EStreamBeginning,pos);
	iWPos=-1;
	TInt offset=0;
	TBuf8<KIpcStreamSize> buf;
	for (;;)
		{
		aMessage.ReadL(2,buf,offset);
		TInt len=buf.Length();
		SQLPROFILER_REPORT_IPC(ESqlIpcRead, len);
		if (len==0)
			break;
		iHost.WriteL(buf.Ptr(),len);
		offset+=len;
		if (len<KIpcStreamSize)
			break;
		}
	iWPos=pos+offset;
	}

/**
Destroys HIpcReadBuf object.
*/
void HIpcReadBuf::DoRelease()
	{
	delete this;
	}
