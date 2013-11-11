// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LogDynBuf.h"

/**
Constructs RLogDynBuf object with the specified granularity.

@param aGranularity RLogDynBuf granularity. 
@endcode

@leave KErrNoMemory Out of memory.
*/
void RLogDynBuf::CreateLC(TInt aGranularity)
	{
	iGranularity = aGranularity;
	iBuf.CreateL(aGranularity);
	CleanupClosePushL(*this);
	}

/**
Destroys the buffer
*/
void RLogDynBuf::Close()
	{
	iBuf.Close();	
	}

/**
Attempts to append aStr to the end of the buffer. If the buffer has not enough available space,
AppendL() will try to expand the buffer before the append operation.

@param aStr 16-bit desrpiptor to be appended to the end of the buffer.

@leave KErrNoMemory Out of memory.
*/	
void RLogDynBuf::AppendL(const TDesC& aStr)
	{
	DoAllocL(aStr.Length());
	iBuf.Append(aStr);
	}

/**
If aLen is bigger than the available buffer space, the buffer will be expanded.

@param aLen The new size of the buffer, in bytes.  

@leave KErrNoMemory Out of memory.
*/
void RLogDynBuf::DoAllocL(TInt aLen)
	{
	__ASSERT_DEBUG(aLen >= 0, User::Invariant());
	const TInt len = iBuf.Length();
	TInt maxLen = iBuf.MaxLength();
	TInt available = maxLen - len;
	TInt overflow = aLen - available;
	if(overflow > 0)
		{
		iBuf.ReAllocL(maxLen + (overflow / iGranularity + 1) * iGranularity);
		}
	}
