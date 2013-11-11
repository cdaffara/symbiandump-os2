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
* Contains the declaration of  CMemSpyApiHeap class 
*/

#ifndef MEMSPYAPIHEAP_H
#define MEMSPYAPIHEAP_H

// System includes
#include <e32base.h>

class TMemSpyHeapData;

NONSHARABLE_CLASS( CMemSpyApiHeap ) 
	{
public:
	IMPORT_C ~CMemSpyApiHeap();
	
	static CMemSpyApiHeap* NewL(const TMemSpyHeapData& aData);

	static CMemSpyApiHeap* NewLC(const TMemSpyHeapData& aData);
	
public: //API
	IMPORT_C TDesC& Type();
	IMPORT_C TInt Size();
	IMPORT_C TInt BaseAddress();
	IMPORT_C TBool Shared();
	IMPORT_C TInt ChunkSize();
	IMPORT_C TInt AllocationsCount();
	IMPORT_C TInt FreeCount();
	IMPORT_C TInt BiggestAllocation();
	IMPORT_C TInt BiggestFree();
	IMPORT_C TInt TotalAllocations();
	IMPORT_C TInt TotalFree();
	IMPORT_C TInt SlackFreeSpace();
	IMPORT_C TInt Fragmentation(); // Percentage value, iSize is 100% - value for calculation
	IMPORT_C TInt HeaderSizeA();
	IMPORT_C TInt HeaderSizeF();
	IMPORT_C TInt AllocationOverhead();
	IMPORT_C TInt FreeOverhead();
	IMPORT_C TInt TotalOverhead();
	IMPORT_C TInt Overhead(); //Percentage value, iSize is 100% - value for calculation
	IMPORT_C TInt MinLength();
	IMPORT_C TInt MaxLength();
	IMPORT_C TBool DebugAllocatorLibrary();
	
private:
	CMemSpyApiHeap();
	
	void ConstructL(const TMemSpyHeapData& aData);
		
private:
	TMemSpyHeapData *iHeapData;
};

	
#endif // MEMSPYAPIHEAP_H
