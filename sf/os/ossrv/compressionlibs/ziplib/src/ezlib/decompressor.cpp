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

#include <ezdecompressor.h>
#include "libzcore.h"
#include <e32debug.h>

CEZDecompressor::~CEZDecompressor()
	{

	// Note inflateEnd may already have been called by zlib if an error occured during inflation.
	// However nothing bad will happen if it gets called twice.

	inflateEnd_r(&iStream);
	}

CEZDecompressor::CEZDecompressor(MEZBufferManager* aInit,const TUint8 *aDictionary, TInt aLength) : 
		iBufferInit(aInit), iDictionary(aDictionary), iDictionaryLength(aLength)
	{
	
	}

CEZDecompressor::CEZDecompressor(MEZBufferManager* aInit) : 
		iBufferInit(aInit), iDictionary(NULL), iDictionaryLength(-1)
	{
	
	}


void CEZDecompressor::ConstructL(TInt aWindowBits)
	{
	iStream.zalloc = Z_NULL;
	iStream.zfree = Z_NULL;
	iStream.opaque = Z_NULL;

	iBufferInit->InitializeL(*this);

	TInt err = inflateInit2_r(&iStream,aWindowBits);
	if (err == Z_VERSION_ERROR)
		User::Leave(KEZlibErrVersion);
	else if (err == Z_MEM_ERROR)
		User::LeaveNoMemory();

	iInflationState = ENoFlush;
	}

/**
Creates a new CEZDecompressor object and leaves it on the CleanupStack

@param aInit buffer manager to handle both input and output buffers
@param aWindowBits the base two logarithm of the window size (the size of the history buffer).  It should 
be in the range 8..15 for this version of the library. Larger values of this parameter result in better 
compression at the expense of memory usage.
@return the new CEZDecompressor object (on the CleanupStack)
*/
EXPORT_C CEZDecompressor*  CEZDecompressor::NewLC(MEZBufferManager& aInit, TInt aWindowBits)
	{
	CEZDecompressor *inf = new (ELeave) CEZDecompressor(&aInit);
	CleanupStack::PushL(inf);
	inf->ConstructL(aWindowBits);
	return inf;
	}

/**
Creates a new CEZDecompressor object

@param aInit buffer manager to handle both input and output buffers
@param aWindowBits the base two logarithm of the window size (the size of the history buffer).  It should 
be in the range 8..15 for this version of the library. Larger values of this parameter result in better 
compression at the expense of memory usage.
@return the new CEZDecompressor object
*/
EXPORT_C CEZDecompressor* CEZDecompressor::NewL(MEZBufferManager& aInit, TInt aWindowBits)
	{
	CEZDecompressor *inf = new (ELeave) CEZDecompressor(&aInit);
	CleanupStack::PushL(inf);
	inf->ConstructL(aWindowBits);
	CleanupStack::Pop();
	return inf;
	}

/**
Overload of CEZDecompressor constructor takes aDictionary argument

@param aInit buffer manager to handle both input and output buffers
@param aDictionary used to initialize the de-compression dictionary from the given byte sequence.  The compressor and 
decompressor must use exactly the same dictionary.
@param aWindowBits the base two logarithm of the window size (the size of the history buffer).  It should 
be in the range 8..15 for this version of the library. Larger values of this parameter result in better 
compression at the expense of memory usage.
@return the new CEZDecompressor object (on the CleanupStack)
*/
EXPORT_C CEZDecompressor* CEZDecompressor::NewLC(MEZBufferManager& aInit, const TDesC8& aDictionary, TInt aWindowBits)
	{
	CEZDecompressor *inf = new (ELeave) CEZDecompressor(&aInit,aDictionary.Ptr(),aDictionary.Size());
	CleanupStack::PushL(inf);
	inf->ConstructL(aWindowBits);
	return inf;
	}

/**
Overload of CEZDecompressor constructor takes aDictionary argument

@param aInit buffer manager to handle both input and output buffers
@param aDictionary used to initialize the de-compression dictionary from the given byte sequence.  The compressor and 
decompressor must use exactly the same dictionary.
@param aWindowBits the base two logarithm of the window size (the size of the history buffer).  It should 
be in the range 8..15 for this version of the library. Larger values of this parameter result in better 
compression at the expense of memory usage.
@return the new CEZDecompressor object
*/
EXPORT_C CEZDecompressor* CEZDecompressor::NewL(MEZBufferManager& aInit, const TDesC8& aDictionary, TInt aWindowBits)
	{
	CEZDecompressor *inf = new (ELeave) CEZDecompressor(&aInit,aDictionary.Ptr(),aDictionary.Size());
	CleanupStack::PushL(inf);
	inf->ConstructL(aWindowBits);
	CleanupStack::Pop();
	return inf;
	}

/**
Resets the current de-compression operation, with the new buffer manager

@param aInit new buffer manager to handle the new input and output buffers
@leave KEZlibErrStream There is a problem with the stream
@leave ... Any of the System wide error codes
*/
EXPORT_C void CEZDecompressor::ResetL(MEZBufferManager& aInit)
	{
	iBufferInit = &aInit;
	iBufferInit->InitializeL(*this);
	if (inflateReset_r(&iStream) == Z_STREAM_ERROR)
		User::Leave(KEZlibErrStream);
	iInflationState = ENoFlush;	
	}

/**
De-compress the data to the buffer in stages, return value indicates if the de-compression has finalised 
or if further calls are necessary

@leave KEZlibErrStream There is a problem with the stream
@leave KEZlibErrBuf There is a problem with the buffer
@leave KEZlibErrData There is a problem with the data
@leave KEZlibErrUnexpected Unexpected programming error
@leave ... Any of the System wide error codes
@return ETrue if the function must be called again, EFalse if compression is finalised
*/
EXPORT_C TBool CEZDecompressor::InflateL()
	{
	TInt err;
	TBool callAgain = ETrue;
	
	switch (iInflationState)
		{
	case ENoFlush:
		err = inflate_r(&iStream,Z_NO_FLUSH);
				
		switch (err)
			{
		case Z_STREAM_ERROR:			
			User::Leave(KEZlibErrStream);

			break;

		case Z_OK:
			if (iStream.avail_in == 0)
				iBufferInit->NeedInputL(*this);
				
			if (iStream.avail_out == 0)
				iBufferInit->NeedOutputL(*this);
			break;

		case Z_BUF_ERROR:  			
			User::Leave(KEZlibErrBuf);
			break;

		case Z_NEED_DICT:
			SetDictionaryL();
			break;

		case Z_STREAM_END:
			iInflationState = EFinalize;
			break;

		case Z_MEM_ERROR:
			User::LeaveNoMemory();
			break;
		
		case Z_DATA_ERROR:
			User::Leave(KEZlibErrData);
			break;

		default:
			
			// there's something wrong with this code if we get here !

			User::Leave(KEZlibErrUnexpected);
			break;
		}
		break;
	
	case EFinalize:
		iBufferInit->FinalizeL(*this);
		callAgain = EFalse;
		iInflationState = ETerminated;
		break;

	case ETerminated:		
		User::Leave(KEZlibErrInflateTerminated);
		}

	return callAgain;
	}

void CEZDecompressor::SetDictionaryL()
	{
	if(!iDictionary  || iDictionaryLength < 0)
		{
		User::Leave(KEZlibErrData);
		}

	TInt err = inflateSetDictionary_r(&iStream,STATIC_CAST(const Bytef*,iDictionary),iDictionaryLength);

	if (err == Z_DATA_ERROR)
		User::Leave(KEZlibErrInflateDictionary);
	else if (err == Z_STREAM_ERROR)
		User::Leave(KEZlibErrInflateDictionary);

	}

/**
De-compresses the data in the given buffer

@param aDestination the target buffer for the de-compressed data
@param aSource the buffer containing the compressed data
@leave KEZLibErrBuf There is a problem with the buffer
@leave KEZLIbErrData There is a problem with the data
@leave ... Any of the system wide error codes
*/
EXPORT_C void CEZDecompressor::DecompressL(TDes8 &aDestination, const TDesC8 &aSource)
	{
	uLongf dl = aDestination.MaxSize();
	Bytef *destinationBuffer = STATIC_CAST(Bytef* , CONST_CAST(TUint8* ,aDestination.Ptr()));
	const Bytef* sourceBuffer = STATIC_CAST(const Bytef* ,aSource.Ptr());
	TInt err = uncompress_r(destinationBuffer,&dl,sourceBuffer,aSource.Size());

	if (err == Z_MEM_ERROR) 
		User::LeaveNoMemory();
	else if (err == Z_BUF_ERROR)
		User::Leave(KEZlibErrBuf);
	else if (err == Z_DATA_ERROR)
		User::Leave(KEZlibErrData);

	aDestination.SetLength(dl);
	}



	
