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


#ifndef MEMSPYMEMORYTRACKINGCYCLEDATA_H_
#define MEMSPYMEMORYTRACKINGCYCLEDATA_H_

struct TMemSpyMemoryTrackingCycleData
	{
	TInt iCycleNumber;
	TTime iTime;
	TFullName iCaption;
	TInt64 iFreeMemory;
	TInt64 iMemoryDelta;
	TInt64 iPreviousCycleDiff;
	TInt iChangeCount;
	};

#endif /* MEMSPYMEMORYTRACKINGCYCLEDATA_H_ */
