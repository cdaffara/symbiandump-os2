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
*/
#include "ipcstream.h"

EXPORT_C RIpcReadStream::RIpcReadStream()
/**
Constructor
*/
: RReadStream(), iSource()
	{	
	}

EXPORT_C void RIpcReadStream::Open(const RMessagePtr2& aMessage, TInt aMessageSlot)
	{
	iSource.Open(aMessage, aMessageSlot);
	Attach(&iSource);
	}


EXPORT_C RIpcWriteStream::RIpcWriteStream()
/**
Constructor
*/
: RWriteStream(), iSource()
	{	
	}
	
EXPORT_C void RIpcWriteStream::Open(const RMessagePtr2& aMessage, TInt aMessageSlot)
	{
	iSource.Open(aMessage, aMessageSlot);
	Attach(&iSource);
	}

// End of file
