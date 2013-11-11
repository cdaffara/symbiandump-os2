/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Contains the declaration of  TMemSpyProcessData class
*/

#ifndef MEMSPYHEAPDATA_H
#define MEMSPYHEAPDATA_H

#include <memspy/driver/memspydriverobjectsshared.h>

// Constants
const TInt KBuf = 16;

// TMemSpyProcess data class holds data to be sent to the UI
class TMemSpyHeapData 
	{	
public:
	//constructor & destructor
	inline TMemSpyHeapData()		
	: iSize(0), iBaseAddress(0), iShared(EFalse), iChunkSize(0),
	iAllocationsCount(0), iFreeCount(0), iBiggestAllocation(0), iBiggestFree(0), iTotalAllocations(0),
    iTotalFree(0), iSlackFreeSpace(0), iFragmentation(0), iHeaderSizeA(0), iHeaderSizeF(0), iAllocationOverhead(0),
    iFreeOverhead(0), iTotalOverhead(0), iOverhead(0), iMinLength(0), iMaxLength(0), iDebugAllocatorLibrary(EFalse)
		{
		}
	
public:
	TBuf<KBuf> iType;
	TUint iSize;
	TLinAddr iBaseAddress;
	TBool iShared;
	TUint iChunkSize;
	TUint iAllocationsCount;
	TUint iFreeCount;
	TUint iBiggestAllocation;
	TUint iBiggestFree;
    TUint iTotalAllocations;
    TUint iTotalFree;
    TUint iSlackFreeSpace;
    TReal iFragmentation;
    TUint iHeaderSizeA;
    TUint iHeaderSizeF;
    TUint iAllocationOverhead;
    TUint iFreeOverhead;
    TUint iTotalOverhead;
    TReal iOverhead;
    TInt iMinLength;
    TInt iMaxLength;
    TBool iDebugAllocatorLibrary;
	};

#endif // MEMSPYHEAPDATA_H
