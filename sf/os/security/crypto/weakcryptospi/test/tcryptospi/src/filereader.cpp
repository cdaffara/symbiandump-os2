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
#include "filereader.h"

#include "cryptospi/cryptospidef.h"
#include <e32std.h>
#include <e32def.h>
#include <f32file.h>

// a simple utility class to read from a text file given the path in its 
// NewLC / NewL 

CFileReader* CFileReader::NewL(TPtrC aFilePath)
	{
	CFileReader* self=CFileReader::NewLC(aFilePath);
	CleanupStack::Pop(self);
	return self;				
	}

CFileReader* CFileReader::NewLC(TPtrC aFilePath)
	{
	CFileReader* self=new(ELeave) CFileReader();
	CleanupStack::PushL(self);
	self->ConstructL(aFilePath);
	return self;				
	}
	
	
CFileReader* CFileReader::NewL(TPtrC aFilePath, TInt aBlockSize)
	{
	CFileReader* self=CFileReader::NewLC(aFilePath, aBlockSize);
	CleanupStack::Pop(self);
	return self;				
	}

CFileReader* CFileReader::NewLC(TPtrC aFilePath, TInt aBlockSize)
	{
	CFileReader* self=new(ELeave) CFileReader();
	CleanupStack::PushL(self);
	self->ConstructL(aFilePath, aBlockSize);
	return self;				
	}	
	
	
CFileReader::~CFileReader()
	{
	// nulled in constructor so safe to call
	delete iFileContents;	
	
	delete iFilePath;
	
	iFile.Close();
	iRfs.Close();
	}
	
CFileReader::CFileReader() : iFileContents(0), iFilePath(0)
	{
	}

TBool CFileReader::ReadBlockL()
	{
	TInt moreData = EFalse;

	if(iBlockSize)
		{
		delete iFileContents;
		iFileContents = HBufC8::NewL(iBlockSize);
		}
	else
		{
		TInt length;
		iFile.Size(length);
		delete iFileContents;
		iFileContents = HBufC8::NewL(length);
		}
		
					
	TPtr8 ptr = iFileContents->Des();

	// use appropriate read method depending on whether
	// we are block reading or reading the whole file;
	if(iBlockSize)
		{
		iFile.Read(iFileOffset, ptr, iBlockSize);

		TInt size;
		iFile.Size(size);

		if(iFileOffset < size)
			moreData = ETrue;

		iFileOffset+=iBlockSize;
		}
	else
		{
		iFile.Read(ptr);
		}
	
	
	return moreData;
	
	}
	
void CFileReader::ConstructL(TPtrC aFilePath)	
	{
	iFilePath = HBufC::NewL(aFilePath.Length());
	
	iFilePath->Des().Copy(aFilePath);
	
	iFileOffset = 0;
	
	iBlockSize = 0;

	User::LeaveIfError(iRfs.Connect());
	User::LeaveIfError(iFile.Open(iRfs, iFilePath->Des(), EFileRead));
	
	ReadBlockL();
	}

void CFileReader::ConstructL(TPtrC aFilePath, TInt aBlockSize)	
	{
	iFilePath = HBufC::NewL(aFilePath.Length());
	
	iFilePath->Des().Copy(aFilePath);
	
	iFileOffset = 0;
	
	iBlockSize = aBlockSize;
	
	User::LeaveIfError(iRfs.Connect());
	User::LeaveIfError(iFile.Open(iRfs, iFilePath->Des(), EFileRead));
	}

CFileReader::operator const TPtrC8()
	{
	return iFileContents->Des();
	}	
	
TInt CFileReader::NumBlocks()
	{
	TInt numBlocks;
	TInt fileSize;
	TInt mod;

	iFile.Size(fileSize);

	numBlocks = fileSize / iBlockSize;
	
	mod = fileSize % iBlockSize;
	
	if(mod) 
		{
		numBlocks++;	
		}
	
	return numBlocks;
	}
	
RInteger CFileReader::ParseIntegerL()
    {
	HBufC8* buf = ParseBinaryL(iFileContents->Des());
	CleanupStack::PushL(buf);
    RInteger result = RInteger::NewL(*buf);
    CleanupStack::PopAndDestroy(buf);

    return result;
    }

HBufC8* CFileReader::ParseBinaryL(const TDesC8& aDes)
	{
    __ASSERT_ALWAYS(aDes.Length() % 2 == 0, User::Panic(_L("ParseBinaryL"), KErrArgument));
	TInt length = aDes.Length() / 2;
	HBufC8* buf = HBufC8::NewL(length);
    TPtr8 ptr = buf->Des();
	ptr.SetLength(length);

    for (TInt i = 0 ; i < aDes.Length() ; i += 2)
        {
        TUint8 tmp;
        tmp=(TUint8)(aDes[i]-(aDes[i]>'9'?('A'-10):'0'));
        tmp*=16;
        tmp|=(TUint8)(aDes[i+1]-(aDes[i+1]>'9'?('A'-10):'0'));
		ptr[i / 2] = tmp;
        }

	return buf;
	}

