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
 @internalTechnology
 @released
*/
#ifndef __IPCBUF_H__
#define __IPCBUF_H__

#include <e32base.h>
#include <s32buf.h>

_LIT(KIpcBufPanic, "IpcBuf");

class RIpcBuf : public MStreamBuf
	{
public:
IMPORT_C void Open(const RMessagePtr2& aMessage, TInt aMessageSlot);
	
protected:
IMPORT_C TStreamPos DoSeekL(TMark aMark, TStreamLocation aLocation, TInt anOffset);

IMPORT_C TInt DoReadL(TAny *aPtr, TInt aMaxLength);
IMPORT_C void DoWriteL(const TAny* aPtr,TInt aLength);
	
private:
	enum TIpcBufPanic
		{
		EIpcBufSeekUnknownLocation,
		EIpcBufSeekBadOffset
		};
	
private:
	TInt iReadPos;
	TInt iWritePos;
	RMessagePtr2 iMessage;
	TInt iMessageSlot;
	};



#endif /* __IPCBUF_H__ */
