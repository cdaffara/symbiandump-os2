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
#include "ipcbuf.h"


EXPORT_C void RIpcBuf::Open(const RMessagePtr2& aMessage, TInt aMessageSlot)
	{
	iMessage = aMessage;
	iMessageSlot = aMessageSlot;
	iReadPos = 0;
	iWritePos = 0;
	}

EXPORT_C TStreamPos RIpcBuf::DoSeekL(TMark aMark, TStreamLocation aLocation, TInt anOffset)
	{

	TInt size = iMessage.GetDesLengthL(iMessageSlot);

	switch (aLocation)
		{
	case EStreamBeginning:
		// do nothing
		break;
		
	case EStreamMark:
		if (aMark&ERead)
			{
			anOffset += iReadPos;
			}
		else if (aMark&EWrite)
			{
			anOffset += iWritePos;
			}
		break;
		
	case EStreamEnd:
		anOffset += size;
		break;
		
	default:
		User::Panic(KIpcBufPanic, EIpcBufSeekUnknownLocation);
		break;
		}
		
	// offset can't be negative or greater than size
	if (anOffset > size || anOffset < 0)
		{
		User::Panic(KIpcBufPanic, EIpcBufSeekBadOffset);
		}
		
	if (aMark&ERead)
		{
		iReadPos = anOffset;
		}
	else if (aMark&EWrite)
		{
		iWritePos = anOffset;
		}
		
	return TStreamPos(anOffset);
	}
	
EXPORT_C TInt RIpcBuf::DoReadL(TAny *aPtr, TInt aMaxLength)
	{
	TPtr8 ptr((TUint8*)aPtr, aMaxLength);
	iMessage.ReadL(iMessageSlot, ptr, iReadPos);
	TInt len = ptr.Length();
	iReadPos += len;
	return len;
	}
	
EXPORT_C void RIpcBuf::DoWriteL(const TAny* aPtr,TInt aLength)
	{
	TPtr8 ptr((TUint8*)aPtr, aLength, aLength);
	iMessage.WriteL(iMessageSlot, ptr, iWritePos);
	iWritePos += aLength;
	}
