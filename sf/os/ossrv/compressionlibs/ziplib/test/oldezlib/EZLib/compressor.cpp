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

#include "OldEZCompressor.h"

using namespace TOLDEZLIB;

CEZCompressor::CEZCompressor(MEZBufferManager* aInit) : 
		iBufferInit(aInit)
	{
	
	}

CEZCompressor::~CEZCompressor()
	{
		// Note that deflateEnd may have already been called by zlib if for example and alloc failure
		// occurred in deflateInit2.  However there is no harm in calling deflateEnd twice.

		deflateEnd(&iStream);		
	}

/**
Creates a new CEZCompressor object and leaves it on the CleanupStack

@param aInit buffer manager to handle both input and output buffers
@param aLevel compression levels
@param aWindowBits the base two logarithm of the window size (the size of the history buffer).  It should 
be in the range 8..15 for this version of the library. Larger values of this parameter result in better 
compression at the expense of memory usage.
@param aMemLevel specifies how much memory should be allocated for the internal compression state.  
memLevel=1 uses minimum memory but is slow and reduces compression ratio; memLevel=9 uses maximum memory
for optimal speed.
@param aStrategy compression strategy - used to tune the compression algorithm.  The strategy parameter only affects 
the compression ratio but not the correctness of the compressed output even if it is not set appropriately
@see TStrategy
@return the new CEZCompressor object (on the CleanupStack)
*/
EXPORT_C CEZCompressor *CEZCompressor::NewLC(MEZBufferManager& aInit, TInt aLevel, TInt aWindowBits, 
									 TInt aMemLevel, TStrategy aStrategy)
	{
	CEZCompressor* deflater = new (ELeave) CEZCompressor(&aInit);
	CleanupStack::PushL(deflater);
	deflater->ConstructL(aLevel,aWindowBits,aMemLevel,aStrategy);
	return deflater;
	}

/**
Creates a new CEZCompressor object

@param aInit buffer manager to handle both input and output buffers
@param aLevel compression levels
@param aWindowBits the base two logarithm of the window size (the size of the history buffer).  It should 
be in the range 8..15 for this version of the library. Larger values of this parameter result in better 
compression at the expense of memory usage.
@param aMemLevel specifies how much memory should be allocated for the internal compression state.  
memLevel=1 uses minimum memory but is slow and reduces compression ratio; memLevel=9 uses maximum memory
for optimal speed.
@param aStrategy compression strategy - used to tune the compression algorithm.  The strategy parameter only affects 
the compression ratio but not the correctness of the compressed output even if it is not set appropriately
@see TStrategy
@return the new CEZCompressor object
*/
EXPORT_C CEZCompressor* CEZCompressor::NewL(MEZBufferManager& aInit, TInt aLevel, TInt aWindowBits, 
									TInt aMemLevel, TStrategy aStrategy)
	{
	CEZCompressor* deflater = new (ELeave) CEZCompressor(&aInit);
	CleanupStack::PushL(deflater);
	deflater->ConstructL(aLevel,aWindowBits,aMemLevel,aStrategy);
	CleanupStack::Pop();
	return deflater;
	}

/**
Overload of CEZCompressor constructor takes aDictionary argument

@param aInit buffer manager to handle both input and output buffers
@param aDictionary used to initialize the compression dictionary from the given byte sequence
without producing any compressed output.  The compressor and decompressor must use exactly the same dictionary.  
The dictionary should consist of strings (byte sequences) that are likely to be encountered later in the data to be compressed, 
with the most commonly used strings preferably put towards the end of the dictionary. Using a dictionary is most useful 
when the data to be compressed is short and can be predicted with good accuracy; the data can then be compressed better than 
with the default empty dictionary.
@param aLevel compression level
@param aWindowBits the base two logarithm of the window size (the size of the history buffer).  It should 
be in the range 8..15 for this version of the library. Larger values of this parameter result in better 
compression at the expense of memory usage.
@param aMemLevel specifies how much memory should be allocated for the internal compression state.  
memLevel=1 uses minimum memory but is slow and reduces compression ratio; memLevel=9 uses maximum memory
for optimal speed.
@param aStrategy compression strategy - used to tune the compression algorithm.  The strategy parameter only affects 
the compression ratio but not the correctness of the compressed output even if it is not set appropriately
@see TStrategy
@return the new CEZCompressor object (on the CleanupStack)
*/
EXPORT_C CEZCompressor* CEZCompressor::NewLC(MEZBufferManager& aInit, const TDesC8& aDictionary, 
				TInt aLevel, TInt aWindowBits, TInt aMemLevel, TStrategy aStrategy)
	{
	CEZCompressor* deflater = new (ELeave) CEZCompressor(&aInit);
	CleanupStack::PushL(deflater);
	deflater->ConstructL(aLevel,aDictionary.Ptr(),aDictionary.Size(),aWindowBits,aMemLevel,aStrategy);
	return deflater;	
	}

/**
Overload of CEZCompressor constructor takes aDictionary argument

@param aInit buffer manager to handle both input and output buffers
@param aDictionary used to initialize the compression dictionary from the given byte sequence
without producing any compressed output.  The compressor and decompressor must use exactly the same dictionary.  
The dictionary should consist of strings (byte sequences) that are likely to be encountered later in the data to be compressed, 
with the most commonly used strings preferably put towards the end of the dictionary. Using a dictionary is most useful 
when the data to be compressed is short and can be predicted with good accuracy; the data can then be compressed better than 
with the default empty dictionary.   
@param aLevel compression level
@param aWindowBits the base two logarithm of the window size (the size of the history buffer).  It should 
be in the range 8..15 for this version of the library. Larger values of this parameter result in better 
compression at the expense of memory usage.
@param aMemLevel specifies how much memory should be allocated for the internal compression state.  
memLevel=1 uses minimum memory but is slow and reduces compression ratio; memLevel=9 uses maximum memory
for optimal speed.
@param aStrategy compression strategy - used to tune the compression algorithm.  The strategy parameter only affects 
the compression ratio but not the correctness of the compressed output even if it is not set appropriately
@see TStrategy
@return the new CEZCompressor object
*/
EXPORT_C CEZCompressor* CEZCompressor::NewL(MEZBufferManager& aInit, const TDesC8& aDictionary,  
			TInt aLevel, TInt aWindowBits, TInt aMemLevel, TStrategy aStrategy)
	{
	CEZCompressor* deflater = new (ELeave) CEZCompressor(&aInit);
	CleanupStack::PushL(deflater);
	deflater->ConstructL(aLevel,aDictionary.Ptr(),aDictionary.Size(),aWindowBits,aMemLevel,aStrategy);
	CleanupStack::Pop();
	return deflater;	
	}

/**
Resets the current compression operation, with the new buffer manager

@param aInit new buffer manager to handle the new input and output buffers
@leave ... Any of the system wide error codes
*/
EXPORT_C void CEZCompressor::ResetL(MEZBufferManager& aInit)
	{
	iBufferInit = &aInit;
	iBufferInit->InitializeL(*this);
	if (deflateReset(&iStream) == Z_STREAM_ERROR)		
		User::Leave(KEZlibErrStream);
	iDeflationState = ENoFlush;
	}

/**
Compress the data to the buffer in stages, return value indicates if the compression has finalised 
or if further calls are necessary

@leave KEZlibErrStream There is a problem with the stream
@leave KEZlibErrBuf There is a problem with the buffer
@leave KEZlibErrUnexpected Unexpected programming error
@leave ... Any of the System wide error codes
@return ETrue if the function must be called again, EFalse if compression is finalised
*/
EXPORT_C TBool CEZCompressor::DeflateL()
	{
	TInt err;
	TBool callAgain = ETrue;
	
	switch (iDeflationState)
		{
	case ENoFlush:
			err = deflate(&iStream,Z_NO_FLUSH);
		
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
				// this is probably ok we have just run out of input.

				iDeflationState = EFinish;
				break;

			default:

				// there's something wrong with this code if we get here !
				
				User::Leave(KEZlibErrUnexpected);
				break;
				}

			break;

	case EFinish:
		err = deflate(&iStream,Z_FINISH);
		
		switch (err)
			{
		case Z_STREAM_ERROR:				
				User::Leave(KEZlibErrStream);
				break;

		case Z_BUF_ERROR:
				User::Leave(KEZlibErrBuf);
				break;

		case Z_OK:
				if (iStream.avail_in == 0)
					iBufferInit->NeedInputL(*this);
				
				if (iStream.avail_out == 0)
					iBufferInit->NeedOutputL(*this);
				break;

		case Z_STREAM_END:
				iDeflationState = EFinalize;
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
		iDeflationState = ETerminated;
		break;

	case ETerminated:
		User::Leave(KEZlibErrDeflateTerminated);
		}

	return callAgain;
	}

void CEZCompressor::ConstructL(TInt aLevel, const TUint8 *aDictionary, TInt aLength, 
						   TInt aWindowBits, TInt aMemLevel, TStrategy aStrategy)
	{
	ConstructL(aLevel,aWindowBits,aMemLevel,aStrategy);
	if (deflateSetDictionary(&iStream,STATIC_CAST(const Bytef *,aDictionary),aLength) == 
			Z_STREAM_ERROR)
		User::Leave(KEZlibErrStream);  // This should never happen.
	}
	
void CEZCompressor::ConstructL(TInt aLevel, TInt aWindowBits, TInt aMemLevel, TStrategy aStrategy)
	{
	// don't need to assert the validity of aWindowBits, aMemLevel & aStrategy as deflateInit2 will
	// do this for us.

	iStream.zalloc = Z_NULL;
	iStream.zfree = Z_NULL;
	iStream.opaque = Z_NULL;

	iBufferInit->InitializeL(*this);

	TInt err = deflateInit2(&iStream,aLevel,Z_DEFLATED,aWindowBits,aMemLevel, STATIC_CAST(int,aStrategy));
	if (err == Z_STREAM_ERROR)
		User::Leave(KEZlibErrStream);
	else if (err == Z_MEM_ERROR)
		User::LeaveNoMemory();

	iDeflationState = ENoFlush;
	}

/**
Compresses the data in the given buffer

@param aDestination the target buffer for the compressed data
@param aSource the buffer containing the data to be compressed
@param aLevel the level of compression
@leave KEZLibErrBuf There is a problem with the buffer
@leave KEZLIbErrStream There is a problem with the stream
@leave ... Any of the system wide error codes
*/
EXPORT_C void CEZCompressor::CompressL(TDes8 &aDestination, const TDesC8 &aSource, 
										TInt aLevel)
	{
	Bytef* destinationBuffer = STATIC_CAST(Bytef* ,CONST_CAST(TUint8* ,aDestination.Ptr()));
	const Bytef* sourceBuffer = STATIC_CAST(const Bytef* ,aSource.Ptr());
	uLongf dl = aDestination.MaxSize();
	TInt err = compress2(destinationBuffer,&dl,sourceBuffer,aSource.Size(),aLevel);

	if (err == Z_MEM_ERROR) 
		User::LeaveNoMemory();
	else if (err == Z_BUF_ERROR)
		User::Leave(KEZlibErrBuf);
	else if (err == Z_STREAM_ERROR)
		User::Leave(KEZlibErrStream);

	aDestination.SetLength(dl);
	}
