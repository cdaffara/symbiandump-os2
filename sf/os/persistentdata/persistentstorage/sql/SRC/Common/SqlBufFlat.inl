// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// //               RSqlBufFlat class          //
// 
//

/**
 
 @return Flat buffer elements count
*/
inline TInt RSqlBufFlat::Count() const
	{
	SQLFLATBUF_INVARIANT();
	return iBuf->iCount;
	}

/**
@return Flat buffer size (this is not the max size. It refers only to the used part of the flat buffer)
*/
inline TInt RSqlBufFlat::Size() const
	{
	SQLFLATBUF_INVARIANT();
	return iBuf->iSize;
	}

/**
@return Flat buffer max size
*/
inline TInt RSqlBufFlat::MaxSize() const
	{
	SQLFLATBUF_INVARIANT();
	return iMaxSize;
	}

/**
@return A pointer to the beginning of the flat buffer header
*/
inline RSqlBufFlat::TCell* RSqlBufFlat::Header()
	{
	return reinterpret_cast <RSqlBufFlat::TCell*> (iBuf + 1);
	}

/**
@return A const pointer to the beginning of the flat buffer header
*/
inline const RSqlBufFlat::TCell* RSqlBufFlat::Header() const 
	{
	return reinterpret_cast <const RSqlBufFlat::TCell*> (iBuf + 1);
	}

/**
@return The avaliable space in the flat buffer
*/
inline TInt RSqlBufFlat::Available() const
	{
	return iMaxSize - iBuf->iSize;
	}

/**
This function returns a const reference to a descriptor object pointing to the internal buffer.
BufDes() guarantees to return a const descriptor, whose lifetime is the same as the flat buffer lifetime.
(useful when making asynchronous IPC calls)

@return A reference to a descriptor object pointing to the internal buffer.
*/
inline const TDesC8& RSqlBufFlat::BufDes() const
	{
	SQLFLATBUF_INVARIANT();
	iBufPtrC.Set(reinterpret_cast <const TUint8*> (iBuf), iBuf->iSize);
	return iBufPtrC;	
	}

/**
This function returns a modifiable reference to a descriptor object pointing to the internal buffer.
BufPtr() guarantees to return a modifiable descriptor, whose lifetime is the same as the flat buffer lifetime.
(useful when making asynchronous IPC calls)

@return A reference to a modifiable descriptor object pointing to the internal buffer.
*/
inline TPtr8& RSqlBufFlat::BufPtr()
	{
	SQLFLATBUF_INVARIANT();
	iBufPtr.Set(reinterpret_cast <TUint8*> (iBuf), iBuf->iSize, iMaxSize);
	return iBufPtr;	
	}

/**
@return The minimal allowed buffer size: sizeof(TBufFlat) + size of the header
*/
inline TInt RSqlBufFlat::SysDataSize() const
	{
	return iBuf->iHeaderSize + sizeof(RSqlBufFlat::TBufFlat);
	}
