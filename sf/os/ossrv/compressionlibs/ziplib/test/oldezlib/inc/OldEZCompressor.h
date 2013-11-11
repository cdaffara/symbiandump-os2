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
// EZLib: COMPRESSOR.H
// Declaration for Compression class
// 
//

#ifndef __EZLIB_EZCOMPRESSOR_H__
#define __EZLIB_EZCOMPRESSOR_H__

#include <e32base.h>
#include "OldEZstream.h"
#include "OldEZBufman.h"

/**
The CEZCompressor class provides in-memory compression functions, including integrity checks of the uncompressed data.
This version of the library supports only one compression method (deflation).  Compression can be done in a single step
(using CompressL()) if the buffers are large enough (for example if an input file is mmap'ed), or can be done by repeated calls
of the DeflateL() function.  The source data is compressed to the target buffer (both source and target contained within 
the buffer manager argument), and various other arguments distinguish the different compression settings.

@publishedAll
@released
*/
namespace TOLDEZLIB
{
	
class CEZCompressor : public CEZZStream
	{
public:
	/** Compression strategy - used to tune the compression algorithm */
	enum TStrategy
		{
		/** Use for normal data */		
		EDefaultStrategy = Z_DEFAULT_STRATEGY, 
		
		/** Force Huffman encoding only (no string match) */
		EFiltered = Z_FILTERED, 
		
		/** Use for data produced by a filter (or predictor) */
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
} //namespace TOLDEZLIB
#endif


