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
* 
*/

#ifndef MEMSPYAPIMEMORYTRACKINGCYCLE_H_
#define MEMSPYAPIMEMORYTRACKINGCYCLE_H_

#include <e32base.h>

class TMemSpyMemoryTrackingCycleData;

NONSHARABLE_CLASS( CMemSpyApiMemoryTrackingCycle )
	{
public: // API
	IMPORT_C TInt CycleNumber() const;
	IMPORT_C const TTime& Time() const;
	IMPORT_C const TDesC& Caption() const;
	IMPORT_C const TInt64& FreeMemory() const;
	IMPORT_C TInt64 MemoryDelta() const;
	IMPORT_C TInt64 PreviousCycleDiff() const;
	IMPORT_C TInt ChangeCount() const;
	
public:
	IMPORT_C ~CMemSpyApiMemoryTrackingCycle();
	
	static CMemSpyApiMemoryTrackingCycle* NewL(const TMemSpyMemoryTrackingCycleData& aData);

	static CMemSpyApiMemoryTrackingCycle* NewLC(const TMemSpyMemoryTrackingCycleData& aData);
	
private:
	CMemSpyApiMemoryTrackingCycle();
	
	void ConstructL(const TMemSpyMemoryTrackingCycleData& aData);
	
private:
	TMemSpyMemoryTrackingCycleData* iData;
	
	};

#endif /* MEMSPYAPIMEMORYTRACKINGCYCLE_H_ */
