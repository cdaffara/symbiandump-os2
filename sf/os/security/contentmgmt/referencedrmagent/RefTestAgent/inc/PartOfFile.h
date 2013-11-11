/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#ifndef __PARTOFFILE_H__
#define __PARTOFFILE_H__

#include <f32file.h>
#include "virtualFile.h"

namespace ReferenceTestAgent
	{
	class CPartOfFile : public CVirtualFile
		{
	public:
		virtual ~CPartOfFile();	

		virtual TInt Read(TDes8& aBuffer, TInt aLength);
		virtual TInt Write(const TDesC8& aBuffer, TInt aLength);
		virtual HBufC8* ReadDescriptor8L();
		virtual void WriteDescriptor8L(const TDesC8& aBuffer);

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		IMPORT_C static CPartOfFile* NewL(RFile64& aSharedFile, RSemaphore& aSharedLock, TInt64 aOffset, TInt64 aSize);
		virtual TInt Read(TInt64 aPos, TDes8& aBuffer, TInt aLength);		
		virtual TInt Seek(TSeek aMode, TInt64 &aPos);
		virtual TInt Size(TInt64& aSize);
		virtual RFile64& FileHandle();
#else
		IMPORT_C static CPartOfFile* NewL(RFile& aSharedFile, RSemaphore& aSharedLock, TInt aOffset, TInt aSize);
		virtual TInt Read(TInt aPos, TDes8& aBuffer, TInt aLength);		
		virtual TInt Seek(TSeek aMode, TInt &aPos);
		virtual TInt Size(TInt& aSize);
		virtual RFile& FileHandle();
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	private:
		void Construct();
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		CPartOfFile(RFile64& aSharedFile, RSemaphore& aSharedLock, TInt64 aOffset, TInt64 aSize);
		TInt DoSeek(TSeek aMode, TInt64 &aPos);
#else
		CPartOfFile(RFile& aSharedFile, RSemaphore& aSharedLock, TInt aOffset, TInt aSize);
		TInt DoSeek(TSeek aMode, TInt &aPos);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	private:
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		RFile64&		iFile;
		const TInt64	iSize;
		const TInt64	iOffset;
		TInt64			iPos;
#else
		RFile&			iFile;
		const TInt		iSize;
		const TInt		iOffset;
		TInt			iPos;
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

		RSemaphore& iFileLock;
		};
	}

#endif

