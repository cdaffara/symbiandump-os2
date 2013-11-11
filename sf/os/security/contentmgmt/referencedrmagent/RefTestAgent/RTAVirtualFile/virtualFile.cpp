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


#include "virtualFile.h"
#include <caf/caferr.h>

using namespace ReferenceTestAgent;

CVirtualFile::CVirtualFile()
	{
	}

TInt CVirtualFile::Read(TDes8& aBuffer)
	{
	return Read(aBuffer, aBuffer.MaxLength());
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C 
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CVirtualFile::Write(const TDesC8& aBuffer)
	{
	return Write(aBuffer, aBuffer.Length());
	}

HBufC8* CVirtualFile::ReadDescriptor8L(RFile& aFile)
	{
	// Find the length of the descriptor
	TInt length;
	TPckg<TInt> lengthPckg(length);
	User::LeaveIfError(aFile.Read(lengthPckg,lengthPckg.Length()));
	//length = -30;
	//If the length of the descriptor is greater than the maximum permissible length or if it is a negative value then the contents of the file are corrupt.
	if(length >= (KMaxTInt/2) || length <= 0)
	{
	User::Leave(KErrCorrupt);
	}
			
	// read the data
	HBufC8* buffer = HBufC8::NewLC(length);
	TPtr8 bufferPtr = buffer->Des();
	User::LeaveIfError(aFile.Read(bufferPtr, length));
	CleanupStack::Pop(buffer);
	return buffer;
	}

void CVirtualFile::WriteDescriptor8L(RFile& aFile, const TDesC8& aBuffer)
	{
	// Write the length of the descriptor
	TInt length = aBuffer.Length();
	TPckg<TInt> lengthPckg(length);
	User::LeaveIfError(aFile.Write(lengthPckg,lengthPckg.Length()));
		
	// write the data
	User::LeaveIfError(aFile.Write(aBuffer, length));
	}


EXPORT_C TInt CVirtualFile::ReadIntL()
	{
	TInt value;
	TPckg<TInt> valuePckg(value);
	User::LeaveIfError(Read(valuePckg,valuePckg.Length()));
	return value;
	}

EXPORT_C void CVirtualFile::WriteIntL(TInt aValue)
	{
	TPckg<TInt> valuePckg(aValue);
	User::LeaveIfError(Write(valuePckg,valuePckg.Length()));
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt64 CVirtualFile::ReadInt64L()
	{
	TInt64 value;
	TPckg<TInt64> valuePckg(value);
	User::LeaveIfError(Read(valuePckg,valuePckg.Length()));
	return value;
	}

EXPORT_C void CVirtualFile::WriteInt64L(const TInt64& aValue)
	{
	TPckg<TInt64> valuePckg(aValue);
	User::LeaveIfError(Write(valuePckg,valuePckg.Length()));
	}
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

		
