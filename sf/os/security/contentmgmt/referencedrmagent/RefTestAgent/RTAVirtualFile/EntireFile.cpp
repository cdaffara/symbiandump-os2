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


#include "EntireFile.h"
#include <e32def.h>

using namespace ReferenceTestAgent;

EXPORT_C CEntireFile* CEntireFile::NewL(const TDesC& aFileName, TBool aCreateFile)
	{
	CEntireFile* self = new (ELeave) CEntireFile;
	CleanupStack::PushL(self);
	self->ConstructL(aFileName, aCreateFile);
	CleanupStack::Pop(self);
	return self;
	}

CEntireFile::CEntireFile()
	{
	}

void CEntireFile::ConstructL(const TDesC& aFileName, TBool aCreateFile)
	{
	User::LeaveIfError(iFs.Connect());
	if(aCreateFile)
		{
		User::LeaveIfError(iFile.Create(iFs, aFileName, EFileStream | EFileWrite | EFileShareAny));	
		}
	else
		{
		User::LeaveIfError(iFile.Open(iFs, aFileName, EFileRead | EFileStream | EFileShareReadersOnly));
		}
	}

CEntireFile::~CEntireFile()
	{
	iFile.Close();
	iFs.Close();
	}

TInt CEntireFile::Read(TDes8& aBuffer, TInt aLength)
	{
	return iFile.Read(aBuffer, aLength);
	}
	
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CEntireFile::Read(TInt64 aPos, TDes8& aBuffer, TInt aLength)
#else
TInt CEntireFile::Read(TInt aPos, TDes8& aBuffer, TInt aLength)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	return iFile.Read(aPos, aBuffer, aLength);
	}

TInt CEntireFile::Write(const TDesC8& aBuffer, TInt aLength)
	{
	return iFile.Write(aBuffer, aLength);	
	}
	
HBufC8* CEntireFile::ReadDescriptor8L()
	{
	return CVirtualFile::ReadDescriptor8L(iFile);
	}
	
void CEntireFile::WriteDescriptor8L(const TDesC8& aBuffer)
	{
	CVirtualFile::WriteDescriptor8L(iFile, aBuffer);
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CEntireFile::Seek(TSeek aMode, TInt64 &aPos)
#else
TInt CEntireFile::Seek(TSeek aMode, TInt &aPos)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	return iFile.Seek(aMode, aPos);
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
TInt CEntireFile::Size(TInt64& aSize)
#else
TInt CEntireFile::Size(TInt& aSize)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	return iFile.Size(aSize);
	}


#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
RFile64& CEntireFile::FileHandle()
#else
RFile& CEntireFile::FileHandle()
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	return iFile;
	}

