/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <e32base.h>
#include <s32strm.h>
#include "cafutils.h"

using namespace ContentAccess;

HBufC* TCafUtils::ReadDescriptor16L(RReadStream& aStream)
	{
	// Read the unicode data from the stream
	TInt dataLength = aStream.ReadInt32L();
	HBufC* iData = HBufC::NewL(dataLength);
	CleanupStack::PushL(iData);
	if(dataLength > 0)
		{
		TPtr dataPtr = iData->Des();
		aStream.ReadL(dataPtr, dataLength);
		}
	CleanupStack::Pop(iData);
	return iData;
	}

void TCafUtils::WriteDescriptor16L(RWriteStream& aStream, const TDesC& aDescriptor)
	{
	// Write unicode data to the stream
	aStream.WriteInt32L(aDescriptor.Length());
	aStream.WriteL(aDescriptor);
	}	

HBufC8* TCafUtils::ReadDescriptor8L(RReadStream& aStream)
	{
	// Read the 8 bit data from a stream
	TInt dataLength = aStream.ReadInt32L();
	HBufC8* iData = HBufC8::NewL(dataLength);
	CleanupStack::PushL(iData);
	if(dataLength > 0)
		{
		TPtr8 dataPtr = iData->Des();
		aStream.ReadL(dataPtr, dataLength);
		}
	CleanupStack::Pop(iData);
	return iData;
	}

void TCafUtils::ReadDescriptor8L(RReadStream& aStream, TDes8& aBuffer)
	{
	aBuffer.SetLength(0);
	
	// Read the 8 bit data from a stream
	TInt dataLength = aStream.ReadInt32L();
	if(dataLength > aBuffer.MaxLength())
		{
		User::Leave(KErrOverflow);
		}
	if(dataLength > 0)
		{
		aStream.ReadL(aBuffer, dataLength);
		}
	}

void TCafUtils::WriteDescriptor8L(RWriteStream& aStream, const TDesC8& aDescriptor)
	{
	// Write 8 bit data to a stream
	aStream.WriteInt32L(aDescriptor.Length());
	aStream.WriteL(aDescriptor);
	}	

// DLL entry point - only for EKA1

