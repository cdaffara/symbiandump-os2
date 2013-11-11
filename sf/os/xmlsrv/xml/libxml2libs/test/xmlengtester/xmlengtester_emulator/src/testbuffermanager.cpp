// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestBufferManager.h"
#include <s32mem.h>
#include <EZStream.h>


CTestBufferManager::CTestBufferManager(const TUint8 *aInput, TInt aInputLength, /*TPtr8 aOutput,*/ TInt aOutputLength)
	:	iInput(aInput), 
		iOutput(0,0),
		iOutputHBuf(NULL) ,
		iOutputLength(aOutputLength), 
		iInputLength(aInputLength),
		iInputDescriptor(NULL,0), 
		iOutputDescriptor(NULL,0)
	{
	}

CTestBufferManager::~CTestBufferManager()
	{
	delete[] iInputBuffer;
	delete[] iOutputBuffer;
	delete iOutputHBuf;
	}


CTestBufferManager *CTestBufferManager::NewLC(const TUint8 *aInput, TInt aInputLength, /*TPtr8 aOutput,*/ TInt aOutputLength, TInt aBufferSize)
	{
	CTestBufferManager *bm = new (ELeave) CTestBufferManager(aInput,aInputLength,aOutputLength);
	CleanupStack::PushL(bm);
	bm->ConstructL(aBufferSize);
	return bm;
	}

CTestBufferManager *CTestBufferManager::NewL(const TUint8 *aInput, TInt aInputLength, /*TPtr8 aOutput,*/ TInt aOutputLength, TInt aBufferSize)
	{
	CTestBufferManager *bm = new (ELeave) CTestBufferManager(aInput,aInputLength,aOutputLength);
	CleanupStack::PushL(bm);
	bm->ConstructL(aBufferSize);
	CleanupStack::Pop();
	return bm;
	}

void CTestBufferManager::ConstructL(TInt aBufferSize)
	{
	iBufferSize = aBufferSize;
	iInputBuffer = new (ELeave) TUint8[iBufferSize];
	iOutputBuffer = new (ELeave) TUint8[iBufferSize];
	iOutputHBuf = HBufC8::NewL(iOutputLength);
	iBufLength = iOutputLength;
	iOutput.Set(iOutputHBuf->Des());	
	}
	
void CTestBufferManager::InitializeL(CEZZStream &aZStream)
	{
	TInt remaining = iInputLength;
	if (remaining > iBufferSize)
		remaining = iBufferSize;
	Mem::Copy(iInputBuffer,iInput,remaining);
	iInputDescriptor.Set(iInputBuffer,remaining,iBufferSize);
	aZStream.SetInput(iInputDescriptor);
	iOutputDescriptor.Set(iOutputBuffer,0,iBufferSize);
	aZStream.SetOutput(iOutputDescriptor);
	iInput += remaining;
	}

void CTestBufferManager::NeedInputL(CEZZStream &aZStream)
	{
	TInt remaining;

	remaining = iInputLength - aZStream.TotalIn();
	if (remaining > iBufferSize)
		remaining = iBufferSize;
	Mem::Copy(iInputBuffer,iInput,remaining);
	
	iInputDescriptor.Set(iInputBuffer,remaining,iBufferSize);
	aZStream.SetInput(iInputDescriptor);
	iInput += remaining;
	}
	

void CTestBufferManager::NeedOutputL(CEZZStream &aZStream)
	{	
    iTotalOut = aZStream.TotalOut();
	TPtrC8 od = aZStream.OutputDescriptor();
	AppendDataL(od);
//	Mem::Copy(iOutput,iOutputBuffer,od.Size());
	aZStream.SetOutput(iOutputDescriptor);
	}

void CTestBufferManager::FinalizeL(CEZZStream &aZStream)
	{
	TInt copy = aZStream.OutputDescriptor().Size();

    if (copy > 0)
        {
        AppendDataL(aZStream.OutputDescriptor());
//        Mem::Copy(iOutput,iOutputBuffer,copy);
        }
		
	}
	
void CTestBufferManager::AppendDataL( const TDesC8& aDataPart )
{
	TInt curLen = iOutputHBuf->Length();
	TInt reqLen = curLen + aDataPart.Length();
    
	if( reqLen > iOutput.MaxLength() )
	{
	    TInt req = curLen+(2*aDataPart.Length());
//	    TInt req = 2*(iOutputHBuf->Length() + aDataPart.Length());
		TRAPD( error, ( iOutputHBuf = iOutputHBuf->ReAllocL( req ) ) );  // realloc a bit more data
		if( error != KErrNone ) User::Leave( KErrNoMemory );
		
		iOutput.Set(iOutputHBuf->Des());
//		iOutput.SetLength( curLen );
		iBufLength = req;
	}
	iOutput.Append(iOutputBuffer, aDataPart.Length());
}

