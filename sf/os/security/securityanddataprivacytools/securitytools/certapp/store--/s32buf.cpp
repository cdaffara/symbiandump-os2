#include <s32buf.h>
#include "logger.h"/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


void MStreamBuf::Close()
{
	Release();
}

void MStreamBuf::DoRelease()
{
}

void MStreamBuf::Release()
{
	DoRelease();
}

void MStreamBuf::SynchL()
{
}


TInt MStreamBuf::ReadL(TAny *aPtr,TInt aMaxLength)
{
	return DoReadL(aPtr, aMaxLength);
}

void MStreamBuf::WriteL(const TUint8* aPtr,TInt aLength)
{
	DoWriteL(aPtr, aLength);
}

void MStreamBuf::SeekL(TMark aMark,TStreamPos aPos)
{
	DoSeekL(aMark,EStreamBeginning,aPos);
}

TStreamPos MStreamBuf::SeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
{
	return DoSeekL(aMark,aLocation,anOffset);
}

TStreamPos MStreamBuf::SeekL(TRead,TStreamLocation aLocation,TInt anOffset)
{
	return DoSeekL(ERead,aLocation,anOffset);
}

TStreamPos MStreamBuf::SeekL(TWrite,TStreamLocation aLocation,TInt anOffset)
{
	return DoSeekL(EWrite,aLocation,anOffset);
}

TStreamPos MStreamBuf::SeekL(TRead,TInt anOffset)
{
	return DoSeekL(ERead,EStreamMark,anOffset);
}

TStreamPos MStreamBuf::SeekL(TWrite,TInt anOffset)
{
	return DoSeekL(EWrite,EStreamMark,anOffset);
}

TStreamPos MStreamBuf::TellL(TRead) const
{
	return const_cast<MStreamBuf*>(this)->DoSeekL(ERead,EStreamMark,0);
}

TStreamPos MStreamBuf::TellL(TWrite) const
{
	return const_cast<MStreamBuf*>(this)->DoSeekL(EWrite,EStreamMark,0);
}




TStreamPos MStreamBuf::DoSeekL(TMark,TStreamLocation,TInt)
{
	dbg << Log::Indent() << "MStreamBuf::DoSeekL not supported" << Log::Endl();
	FatalError();
	return -1;
}




// End of file
