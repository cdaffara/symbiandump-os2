/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* TAGMA's text layout bytecode storage class.
*
*/


#include "TMSTD.H"

void CTmCode::AppendByteL(TUint8 aByte)
	{
	InsertByteL(aByte,Size());
	}

TInt CTmCode::AppendNumberL(TInt aNumber)
	{
	return InsertNumberL(aNumber,Size());
	}

TInt CTmCode::AppendRectL(const TRect& aRect)
	{
	return InsertRectL(aRect,Size());
	}

TInt CTmCode::Size() const
	{
	return iBuffer ? iBuffer->Size() : 0;
	}

void CTmCode::Delete(TInt aPos,TInt aLength)
	{
	if (iBuffer) iBuffer->Delete(aPos,aLength);
	}

void CTmCode::Reset()
	{
	delete iBuffer; iBuffer = NULL;
	}

void CTmCode::CreateBufferL()
	{
	if (!iBuffer) iBuffer = new(ELeave) CTmBufSeg(EExpandSize);
	}

void CTmCode::InsertByteL(TUint8 aByte,TInt aPos)
	{
	iBuffer->InsertL(aPos,&aByte,1);
	}

/*
Write a number in bytecode, using as few bytes as possible. Each byte assigns the low seven bits
to data and uses the high bit as a continuation bit. Return the position after the number.
*/
TInt CTmCode::InsertNumberL(TInt aNumber,TInt aPos)
	{
	TUint8 buffer[5];
	int bytes = WriteNumber(aNumber,buffer);
	iBuffer->InsertL(aPos,buffer,bytes);
	return aPos + bytes;
	}

// Write aNumber to aBuffer and return the number of bytes written (from 1 to 5).
TInt CTmCode::WriteNumber(TInt aNumber,TUint8* aBuffer)
	{
	int bytes;
	if (aNumber >= -64 && aNumber <= 63)
		bytes = 1;
	else if (aNumber >= -8192 && aNumber <= 8191)
		bytes = 2;
	else if (aNumber >= -1048576 && aNumber <= 1048575)
		bytes = 3;
	else if (aNumber >= -134217728 && aNumber <= 134217727)
		bytes = 4;
	else
		bytes = 5;
	int shift = 0;
	for (int i = 0; i < bytes; i++)
		{
		aBuffer[i] = (TUint8)((aNumber >> shift) & 0x7F);
		if (i > 0)
			aBuffer[i - 1] |= 0x80;
		shift += 7;
		}
	return bytes;
	}

// Write a rectangle in bytecode as left, top, width, height. Return the position after the rectangle.
TInt CTmCode::InsertRectL(const TRect& aRect,TInt aPos)
	{
	TUint8 buffer[20];
	int bytes = WriteNumber(aRect.iTl.iX,buffer);
	bytes += WriteNumber(aRect.iTl.iY,buffer + bytes);
	bytes += WriteNumber(aRect.Width(),buffer + bytes);
	bytes += WriteNumber(aRect.Height(),buffer + bytes);
	iBuffer->InsertL(aPos,buffer,bytes);
	return aPos + bytes;
	}

// Delete bytecode from aStart to aEnd and replace it with aNewCode, which is consumed by the operation.
void CTmCode::ChangeL(TInt aStart,TInt aEnd,CTmCode& aNewCode)
	{
	if (aStart <= 0 && aEnd >= Size())
		{
		delete iBuffer;
		iBuffer = aNewCode.iBuffer;
		aNewCode.iBuffer = NULL;
		}
	else
		{
		CreateBufferL();
		if (aEnd > aStart)
			iBuffer->Delete(aStart,aEnd - aStart);
		while (aNewCode.Size() > 0)
			{
			TPtrC8 p = aNewCode.Ptr(0);
			iBuffer->InsertL(aStart,p);
			aStart += p.Length();
			aNewCode.Delete(0,p.Length());
			}
		}
	}

TInt CTmBufSeg::MemoryUsed() const
	{
	TInt bytes = sizeof(*this) + iSize;
	TInt pos = 0;
	while (pos < iSize)
		{
		TPtr8 p = (CONST_CAST(CTmBufSeg*,this))->Ptr(pos);
		bytes += 8;
		pos += p.Length();
		}
	return bytes;
	}

// Return the amount of memory used in bytes, allowing 8 bytes overhead per heap object
TInt CTmCode::MemoryUsed() const
	{
	TInt bytes = sizeof(*this);
	if (iBuffer)
		bytes += iBuffer->MemoryUsed();
	return bytes;
	}

int TTmCodeReader::ReadNumber()
	{
	TUint byte = ReadByte();
	if (!(byte & 0xC0))
		return byte;

	int n = 0;
	int shift = 0;
	do
		{
		if (shift)
			byte = ReadByte();
		n |= ((byte & 0x7F) << shift);
		shift += 7;
		} while (byte & 0x80);
	if ((byte & 0x40) && shift < 32)	// if bit 6 of the last byte is set, the number is negative
		n |= (0xFFFFFFFF << shift);
	return n;
	}


TRect TTmCodeReader::ReadRect()
	{
	TRect r;
	r.iTl.iX = ReadNumber();
	r.iTl.iY = ReadNumber();
	r.iBr.iX = r.iTl.iX + ReadNumber();
	r.iBr.iY = r.iTl.iY + ReadNumber();
	return r;
	}

void TTmCodeReader::SetCodePtr()
	{
	if (iCodePos > iCodeEndPos)
		TmPanic(EBadCodePos);
	TPtrC8 p = CONST_CAST(CTmCode&,iCode).Ptr(iCodePos);
	iCodeSegPtr = p.Ptr();
	iCodeSegEndPtr = p.Ptr() + p.Length();
	}

void TTmCodeReader::SetCodePos(TInt aNewCodePos)
	{
	int bytes_to_skip = aNewCodePos - iCodePos;
	iCodePos = aNewCodePos;
	if (bytes_to_skip >= 0 && bytes_to_skip <= iCodeSegEndPtr - iCodeSegPtr)
		iCodeSegPtr += bytes_to_skip;
	else
		SetCodePtr();
	}
