/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#include "TMSTD.H"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TmBufferBaseTraces.h"
#endif

#include "TMSTD.H"

CTmBufferBase::~CTmBufferBase()
	{
	User::Free(iData);
	}

void CTmBufferBase::Truncate(TInt aLength)
	{
	if (aLength > iLength || aLength < 0)
	    {
	    OstTrace0( TRACE_FATAL, CTMBUFFERBASE_TRUNCATE, "EBadTruncationLength" );
		TmPanic(EBadTruncationLength);
		}
	iLength = aLength;
	}

TAny* CTmBufferBase::ExtendL(TInt aSizeOfData)
	{
	if (iLength == iAllocated)
		GrowL(iExpandSize,aSizeOfData);
	TInt offset = iLength * aSizeOfData;
	iLength += 1;
	return REINTERPRET_CAST(TUint8*,iData) + offset;
	}

void CTmBufferBase::DoAppendL(const TAny* aItem,TInt aCount,TInt aSizeOfData)
	{
	int deficit = iLength + aCount - iAllocated;
	if (deficit > 0)
		{
		int n = deficit % iExpandSize;
		if (n)
			deficit += iExpandSize - n;
		GrowL(deficit,aSizeOfData);
		}
	Mem::Copy(REINTERPRET_CAST(TUint8*,iData) + iLength * aSizeOfData, aItem,
		aCount * aSizeOfData);
	iLength += aCount;
	}

void CTmBufferBase::GrowL(TInt aExtra,TInt aSizeOfData)
	{
	iData = User::ReAllocL(iData,(iAllocated + aExtra) * aSizeOfData);
	iAllocated += aExtra;
	}
