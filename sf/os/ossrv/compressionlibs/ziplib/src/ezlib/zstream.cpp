// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ezstream.h>

CEZZStream::CEZZStream() : iOutputPointer(NULL), iOutputBufferLength(0)
	{

	}

/**
Set the stream's input buffer

@param aInputData the input buffer for this stream
*/
EXPORT_C void CEZZStream::SetInput(const TDesC8& aInputData)
	{	
	iStream.avail_in = aInputData.Size();
	iStream.next_in = STATIC_CAST(Bytef* ,CONST_CAST(TUint8 *,aInputData.Ptr()));
	}

/**
Set the stream's output buffer

@param aOutputData the output buffer for this stream
*/
EXPORT_C void CEZZStream::SetOutput(TDes8& aOutputData)
	{
	iOutputPointer = STATIC_CAST(Bytef* ,CONST_CAST(TUint8 *,aOutputData.Ptr()));
	iOutputBufferLength = aOutputData.MaxSize();
	iStream.avail_out = iOutputBufferLength;
	iStream.next_out = iOutputPointer;
	}

/**
Return the progress of the current operation - that is the percentage of bytes written / read

@param aTotalLength the total number of bytes to read / write
@return the progress as a percentage - the number of bytes written / read out of the total target
*/
EXPORT_C TInt CEZZStream::Progress(TInt aTotalLength) const
	{
	return (aTotalLength == 0) ? 100 : ((iStream.total_in  * 100) / aTotalLength);
	}

/**
Return the total number of bytes output so far

@return the total number of bytes output so far
*/
EXPORT_C TInt CEZZStream::TotalOut() const
	{
	return iStream.total_out;
	}

/**
Return the total number of input bytes read so far

@return the total number of input bytes read so far
*/
EXPORT_C TInt CEZZStream::TotalIn() const
	{
	return iStream.total_in;
	}

/**
Return the value of the uncompressed data

@return the value of the uncompressed data
*/
EXPORT_C TInt32 CEZZStream::Adler32() const
	{
	return iStream.adler;
	}

/**
Return the number of bytes available at the next input byte

@return the number of bytes available at the next input byte
*/
EXPORT_C TInt CEZZStream::AvailIn() const
	{
	return iStream.avail_in;
	}

/**
Return the remaining free space at next output byte target

@return the remaining free space at next output byte target
*/
EXPORT_C TInt CEZZStream::AvailOut() const
	{
	return iStream.avail_out;
	}

/**
Return a decriptor pointer to the output buffer

@return a decriptor pointer to the output buffer
*/
EXPORT_C TPtrC8 CEZZStream::OutputDescriptor() const
	{
	return TPtrC8(iOutputPointer,iOutputBufferLength - iStream.avail_out);
	}

