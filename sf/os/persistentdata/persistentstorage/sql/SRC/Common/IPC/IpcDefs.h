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

#ifndef __IPCDEFS_H__
#define __IPCDEFS_H__

#include <e32std.h>

#ifdef _DEBUG

const TInt KIpcStreamSize = 8;
const TInt KIpcBufSize = 8;

#else //_DEBUG

const TInt KIpcStreamSize = 3 * 1024;
const TInt KIpcBufSize = KIpcStreamSize / 2;

#endif//_DEBUG

/**
IPC temporary data buffer.

HIpcBuf class uses the buffer as a temporary IPC data storage when a large block of data
has to be sent to the server. In this case the data will be split in chunks, each of 
TIpcStreamBuf size, and every chunk will be sent separately and the large data block will
reconstructed on the server side.

@see HIpcBuf

@internalComponent
*/
NONSHARABLE_CLASS(TIpcStreamBuf)
	{
public:
	enum {ESize = KIpcBufSize};
public:
	TInt	iExt;
	TUint8	iData[ESize];
	};

#endif//__IPCDEFS_H__
