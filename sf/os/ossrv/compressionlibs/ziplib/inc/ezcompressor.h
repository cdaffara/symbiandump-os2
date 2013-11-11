// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration for Compression class
// 
//

#ifndef __EZCOMPRESSOR_H__
#define __EZCOMPRESSOR_H__

#include <e32base.h>
#include <ezstream.h>
#include <ezbufman.h>

/**
The CEZCompressor class provides in-memory compression functions, including integrity checks of the uncompressed data.
This version of the library supports only one compression method (deflation).  Compression can be done in a single step
(using CompressL()) if the buffers are large enough (for example if an input file is mmap'ed), or can be done by repeated calls
of the DeflateL() function.  The source data is compressed to the target buffer (both source and target contained within 
the buffer manager argument), and various other arguments distinguish the different compression settings.

Note: In this version of the library a windowBits value of 8 is unsupported due to a problem with the window size being 
set to 256 bytes. Although a value of 8 will be accepted by the CEZCompressor constructors, as it is being changed 
internally by Zlib from 8 to 9, it will not be possible to use the same value for decompression. This is because the 
Zlib functions called by the CEZDecompressor constructors do not make the same change internally and as a result a 
KEZlibErrData is returned when calling InflateL(). It is therefore advised that for this version of the library 
windowBits of 9 is used in place of 8.

@publishedAll
@released
*/
class CEZCompressor : public CEZZStream
	{
public:
	/** Compression strategy - used to tune the compression algorithm */
	enum TStrategy
		{
		/** Use for normal data */		
		EDefaultStrategy = Z_DEFAULT_STRATEGY, 
		
		/** Use for data produced by a filter (or predictor) */
		EFiltered = Z_FILTERED, 
		
		/** Force Huffman encoding only (no string match) */
		EHuffmanOnly = Z_HUFFMAN_ONLY
		};

	/** Compression levels */
	enum 
		{
		EDefaultCompression = Z_DEFAULT_COMPRESSION,
		ENoCompression = Z_NO_COMPRESSION,
		EBestSpeed = Z_BEST_SPEED,
		EBestCompression = Z_BEST_COMPRESSION
		};

	/** Window Bits - the base two logarithm of the window size (the size of the history buffer) */
	enum
		{
		EMaxWBits = MAX_WBITS
		};

	/** Memory level - specifies how much memory should be allocated for the internal compression state */
	enum
		{
		EDefMemLevel = MAX_MEM_LEVEL
		};

	/** Compression panic values */
	enum
		{
		EDeflateInitlialiserError = EUnexpected + 1,
		EDeflateTerminated
		};

	public:
		~CEZCompressor();

		IMPORT_C static CEZCompressor* NewLC(MEZBufferManager& aInit, TInt aLevel = EDefaultCompression,
			TInt aWindowBits = EMaxWBits, TInt aMemLevel = EDefMemLevel, TStrategy aStrategy = EDefaultStrategy);
		IMPORT_C static CEZCompressor* NewL(MEZBufferManager& aInit, TInt aLevel = EDefaultCompression,
			TInt aWindowBits = EMaxWBits, TInt aMemLevel = EDefMemLevel, TStrategy aStrategy = EDefaultStrategy);
		IMPORT_C static CEZCompressor* NewLC(MEZBufferManager& aInit, const TDesC8 &aDictionary, 
			TInt aLevel = EDefaultCompression, TInt aWindowBits = EMaxWBits, TInt aMemLevel = EDefMemLevel, 
			TStrategy aStrategy = EDefaultStrategy);
		IMPORT_C static CEZCompressor* NewL(MEZBufferManager& aInit, const TDesC8 &aDictionary,  
			TInt aLevel = EDefaultCompression, TInt aWindowBits = EMaxWBits, TInt aMemLevel = EDefMemLevel, 
			TStrategy aStrategy = EDefaultStrategy);

		IMPORT_C void ResetL(MEZBufferManager& aInit);

		IMPORT_C TBool DeflateL();

		IMPORT_C static void CompressL(TDes8 &aDestination, const TDesC8 &aSource, TInt aLevel = EDefaultCompression);

	private:
		enum TDeflationState
			{
			ENoFlush,
			EFinish,
			EFinalize,
			ETerminated
			};

	private:
		CEZCompressor(MEZBufferManager* aInit);
		void ConstructL(TInt aLevel, const TUint8* aDictionary, TInt aLength, TInt aWindowBits, TInt aMemLevel, TStrategy aStrategy);
		void ConstructL(TInt aLevel, TInt aWindowBits, TInt aMemLevel, TStrategy aStrategy);

	private:
		MEZBufferManager* iBufferInit;
		TDeflationState iDeflationState;
	};

#endif


