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
// EZLib: DECOMPRESSOR.H
// Declaration for Decompression class
// 
//

#ifndef __EZLIB_EZDECOMPRESSOR_H__
#define __EZLIB_EZDECOMPRESSOR_H__

#include <e32base.h>
#include "OldEZstream.h"
#include "OldEZBufman.h"

/**
The CEZDecompressor class provides in-memory de-compression functions, including integrity checks of the compressed data.
This version of the library supports only one compression / de-compression method (deflation / inflation).  De-compression
can be done in a single step (using DecompressL()) if the buffers are large enough (for example if an input file is mmap'ed),
or can be done by repeated calls of the InflateL() function.  The source data is de-compressed to the target buffer (both source 
and target contained within the buffer manager argument).

@publishedAll
@released
*/
namespace TOLDEZLIB
{
	
class CEZDecompressor : public CEZZStream
	{
public:
	/** Decompression panic values */
	enum 
		{
		EInflateInitlialiserError = EUnexpected + 1,
		EInflateVersionError,
		EInflateTerminated,
		EInflateDictionaryError
		};
		
	/** Window Bits - the base two logarithm of the window size (the size of the history buffer) */
	enum
		{
		EMaxWBits = MAX_WBITS
		};

public:
	~CEZDecompressor();

	IMPORT_C static CEZDecompressor* NewLC(MEZBufferManager& aInit, TInt aWindowBits = EMaxWBits);
	IMPORT_C static CEZDecompressor* NewL(MEZBufferManager& aInit, TInt aWindowBits = EMaxWBits);

	IMPORT_C static CEZDecompressor* NewLC(MEZBufferManager& aInit, const TDesC8& aDictionary, TInt aWindowBits = EMaxWBits);
	IMPORT_C static CEZDecompressor* NewL(MEZBufferManager& aInit, const TDesC8& aDictionary, TInt aWindowBits = EMaxWBits);


	IMPORT_C void ResetL(MEZBufferManager& aInit);
	IMPORT_C TBool InflateL();

	IMPORT_C static void DecompressL(TDes8 &aDestination, const TDesC8 &aSource);

	private:
		enum TInflationState
			{
			ENoFlush,
			EFinalize,
			ETerminated
			};

	private:
		void SetDictionaryL();
		CEZDecompressor(MEZBufferManager* aInit);
		CEZDecompressor(MEZBufferManager* aInit, const TUint8 *aDictionary, TInt aLength);
		void ConstructL(TInt aWindowBits);

	private:
		MEZBufferManager* iBufferInit;
		TInflationState iInflationState;
		const TUint8* iDictionary;
		TInt iDictionaryLength;
	};

} //namespace TOLDEZLIB
#endif


