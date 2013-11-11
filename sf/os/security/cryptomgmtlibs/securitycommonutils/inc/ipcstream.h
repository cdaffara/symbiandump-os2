/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Library to add s32strm support for IPC (ie. stream via multiple IPC read/writes instead of
* copying to a buffer and streaming to/from there.
*
*/


/**
 @file
 @publishedPartner
 @released
*/
#ifndef __IPCSTREAM_H__
#define __IPCSTREAM_H__

#include <e32base.h>
#include <s32strm.h>

#include <scs/ipcbuf.h>

class RIpcReadStream : public RReadStream
	{
public:
	IMPORT_C void Open(const RMessagePtr2& aMessage, TInt aMessageSlot);
	IMPORT_C RIpcReadStream();
private:
	RIpcBuf iSource;
	};
	
class RIpcWriteStream : public RWriteStream
	{
public:
	IMPORT_C void Open(const RMessagePtr2& aMessage, TInt aMessageSlot);
	IMPORT_C RIpcWriteStream();
private:
	RIpcBuf iSource;
	};

#endif /* __IPCSTREAM_H__ */
