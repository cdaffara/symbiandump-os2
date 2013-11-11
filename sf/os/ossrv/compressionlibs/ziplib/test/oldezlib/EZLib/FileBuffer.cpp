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

#include "OldEZFilebuffer.h"
#include "OldEZstream.h"

using namespace TOLDEZLIB;

_LIT(KFileBuffer,"FileBuffer");

void CEZFileBufferManager::ConstructL(TInt aBufferSize)
	{
	__ASSERT_ALWAYS(aBufferSize > 0, User::Panic(KFileBuffer, EBadInitialization));

	iBufferSize = aBufferSize;

	iInputBuffer = new (ELeave) TUint8[iBufferSize];
	iOutputBuffer = new (ELeave) TUint8[iBufferSize];

	iInputDescriptor.Set(iInputBuffer,0,iBufferSize);
	iOutputDescriptor.Set(iOutputBuffer,0,iBufferSize);
	}

CEZFileBufferManager::~CEZFileBufferManager()
	{
	delete[] iInputBuffer;
	delete[] iOutputBuffer;
	}

CEZFileBufferManager::CEZFileBufferManager(RFile &aInput, RFile &aOutput) : 
	iInputFile(aInput), iOutputFile(aOutput), iInputDescriptor(NULL,0), iOutputDescriptor(NULL,0)
	{

	}

/**
Creates a new CEZFileBufferManager object and leave it on the CleanupStack

@param aInput the input file
@param aOutput the output file
@param aBufferSize the required size of the buffers
@return the new CEZFileBufferManager object, on the CleanupStack
*/
EXPORT_C CEZFileBufferManager* CEZFileBufferManager::NewLC(RFile &aInput, RFile &aOutput, TInt aBufferSize)
	{
	CEZFileBufferManager *fb = new (ELeave) CEZFileBufferManager(aInput,aOutput);
	CleanupStack::PushL(fb);
	fb->ConstructL(aBufferSize);
	return fb;
	}

/**
Creates a new CEZFileBufferManager object

@param aInput the input file
@param aOutput the output file
@param aBufferSize the required size of the buffers
@return the new CEZFileBufferManager object
*/
EXPORT_C CEZFileBufferManager* CEZFileBufferManager::NewL(RFile &aInput, RFile &aOutput, TInt aBufferSize)
	{
	CEZFileBufferManager *fb = new (ELeave) CEZFileBufferManager(aInput,aOutput);
	CleanupStack::PushL(fb);
	fb->ConstructL(aBufferSize);
	CleanupStack::Pop();
	return fb;
	}


void CEZFileBufferManager::InitializeL(CEZZStream &aZStream)
	{
	User::LeaveIfError(iInputFile.Read(iInputDescriptor));
	aZStream.SetInput(iInputDescriptor);
	aZStream.SetOutput(iOutputDescriptor);
	}

void CEZFileBufferManager::NeedInputL(CEZZStream &aZStream)
	{
	User::LeaveIfError(iInputFile.Read(iInputDescriptor));
	aZStream.SetInput(iInputDescriptor);
	}

void CEZFileBufferManager::NeedOutputL(CEZZStream &aZStream)
	{
	TPtrC8 od = aZStream.OutputDescriptor();
	User::LeaveIfError(iOutputFile.Write(od));
	aZStream.SetOutput(iOutputDescriptor);
	}

void CEZFileBufferManager::FinalizeL(CEZZStream &aZStream)
	{
	TPtrC8 od = aZStream.OutputDescriptor();
	User::LeaveIfError(iOutputFile.Write(od));
	}
