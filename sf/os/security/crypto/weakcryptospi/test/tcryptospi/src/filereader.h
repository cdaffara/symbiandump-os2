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
*
*/




/**
 @file
 @internalTechnology
*/
#ifndef __FILEREADER__
#define __FILEREADER__

#include <e32cmn.h>
#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <f32file.h>
#include <bigint.h>

	class CFileReader : public CBase
		{
	public:
		static CFileReader* NewL(TPtrC aFileReader);
		static CFileReader* NewLC(TPtrC aFileReader);
		static CFileReader* NewL(TPtrC aFilePath, TInt aBlockSize);
		static CFileReader* NewLC(TPtrC aFilePath, TInt aBlockSize);

		operator const TPtrC8();
				
		virtual ~CFileReader();
		
		TBool ReadBlockL();
		
		TInt NumBlocks();
		
		/** Parse a hex string and return a new RInteger. */
    	RInteger ParseIntegerL();

    	/** Parse a hex string and return a new descriptor containing the binary data. */
		HBufC8* ParseBinaryL(const TDesC8& aDes);
		
	private:
		CFileReader();
	
		void ConstructL(TPtrC aFileReader);
		void ConstructL(TPtrC aFilePath, TInt aOffsetSize);

		HBufC8* iFileContents;
		HBufC*	iFilePath;
		TInt 	iFileOffset;
		TInt 	iBlockSize;
		
		RFs		iRfs;
		RFile	iFile;


		};
#endif
