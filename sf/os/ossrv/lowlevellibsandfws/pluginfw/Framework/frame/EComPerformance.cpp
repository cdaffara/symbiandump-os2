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
// Implementation of static class to log ECom performance
// 
//

/**
 @internalComponent
 @file
*/

#include <e32debug.h>
#include "EComPerformance.h"

#ifdef __ECOM_SERVER_PERFORMANCE__

TUint32 EComPerformance::iEComStartupStateTimerResults[KEComStartupStateTimerResultMaxCount];
TInt EComPerformance::iEComStartupStateTimerResultCount = 0;
TClientRequestTimerEntry EComPerformance::iAccumulatedClientRequestTimerResults[KAccumulatedClientRequestTimerResultMaxCount];
TInt EComPerformance::iAccumulatedClientRequestTimerResultCount = 0;
TEComPerfTimeRecordEntry EComPerformance::iEComPerfTimeRecords[KEComPerfTimerRecordMaxCount];
TInt EComPerformance::iEComPerfTimeRecordCount = 0;
TEComPerfHeapUsage EComPerformance::iEComStartupStateHeapResults[KEComStartupStateHeapResultMaxCount];
TInt EComPerformance::iEComStartupStateHeapResultCount=0;

/**
 Retrieves startup state timing data from the given index
 @param aTimerIdx The index of the timing entry to retrieve	
 @param aTimerResult On return contains the timing (using FastCounter) at the given index
 @param aState The state the timing data is for
 */
TInt EComPerformance::GetStartupStateTimerResult(TInt aTimerIdx, TUint32& aTimerResult, TInt& aState)
	{
	TInt resultIdx = aTimerIdx * 2;
	aTimerResult = 0;
	aState = -1;
	
	if(iEComStartupStateTimerResultCount > resultIdx + 1)
		{
		aTimerResult = iEComStartupStateTimerResults[resultIdx];
		aState = iEComStartupStateTimerResults[resultIdx+1];
		}
	else
		{
		RDebug::Print(_L("Exceeded size of %d held by KEComStartupStateTimerResults array\n"), iEComStartupStateTimerResultCount);
		return KErrOverflow;
		}
	return KErrNone;
	}

/**
 Records timing data for the given state that can be retrieved using GetStartupStateTimerResult
 @param aState The state the timing data is for
 */
void EComPerformance::RecordStartupStateTimerResult(TInt aState)
	{
	if(iEComStartupStateTimerResultCount+1<KEComStartupStateTimerResultMaxCount)
		{
		iEComStartupStateTimerResults[iEComStartupStateTimerResultCount++] = User::FastCounter();
		iEComStartupStateTimerResults[iEComStartupStateTimerResultCount++] = aState;
		}
	}
	
void EComPerformance::ResetStartupStateTimerResult()
	{
	iEComStartupStateTimerResultCount = 0;
	}
/**
 Retrieves timing data for the client request indexed by aTimerIdx
 Note that this data does not include timing for requests where an exception occurred during processing
 @param aTimerIdx The index of the timing entry to retrieve	
 @param aTimerEntry On return contains the timing data for the given index
 @return KErrNone if the timing data was retrieved successfully, KErrOverflow if the requested index is greater than the number of timing entries	
*/
TInt EComPerformance::GetAccumulatedClientRequestTimerResult(TInt aTimerIdx, TClientRequestTimerEntry& aTimerEntry)
	{
	if(aTimerIdx < iAccumulatedClientRequestTimerResultCount)
		{
		aTimerEntry = iAccumulatedClientRequestTimerResults[aTimerIdx];
		}
	else
		{
		return KErrOverflow;
		}
	return KErrNone;
	}

/**
 Records timing data for the start of the processing of a client request.
 @param aClientRequestType The type of client request
 @param aState The current start-up state
*/
void EComPerformance::RecordStartClientRequestTimerResult(TEComClientRequestType aClientRequestType, TInt aState)
	{
	if(iAccumulatedClientRequestTimerResultCount < KAccumulatedClientRequestTimerResultMaxCount)
		{
		iAccumulatedClientRequestTimerResults[iAccumulatedClientRequestTimerResultCount].iStartTime = User::FastCounter();
		iAccumulatedClientRequestTimerResults[iAccumulatedClientRequestTimerResultCount].iClientRequestType = aClientRequestType;
		iAccumulatedClientRequestTimerResults[iAccumulatedClientRequestTimerResultCount].iState = aState;
		}
	}
	
/**
 Records timing data for the end of the processing of the client request associated with the last call to RecordStartClientRequestTimerResult
 @pre RecordStartClientRequestTimerResult must have been called before this method is called
*/
void EComPerformance::RecordEndClientRequestTimerResult()
	{
	if (iAccumulatedClientRequestTimerResultCount < KAccumulatedClientRequestTimerResultMaxCount)
		{
		iAccumulatedClientRequestTimerResults[iAccumulatedClientRequestTimerResultCount].iEndTime = User::FastCounter();
		iAccumulatedClientRequestTimerResultCount++;
		}
	}

/**
 Retrieves ECom performance timing record by aTimeIdx
@param aTimerIdx the index of the record to get (a zero indexed value)
@param aTimeRecord the record readings
*/
TInt EComPerformance::GetEComPerfTimeRecord(TInt aTimeIdx, TEComPerfTimeRecordEntry& aTimeEntry)
	{
	if(aTimeIdx < KEComPerfTimerRecordMaxCount)
		{
		aTimeEntry = iEComPerfTimeRecords[aTimeIdx];
		}
	else
		{
		return KErrOverflow;		
		}
	return KErrNone;
	}

/**
 Records ECom performance timing record with its type an additional information
 @param aType The type of the record
 @param aInfo The additional inforamtion attached
*/
void EComPerformance::RecordEComPerfTime(TEComPerfTimeRecordType aType, TInt aInfo)
	{
	if(iEComPerfTimeRecordCount < KEComPerfTimerRecordMaxCount)
		{
		iEComPerfTimeRecords[iEComPerfTimeRecordCount].iTime = User::FastCounter();
		iEComPerfTimeRecords[iEComPerfTimeRecordCount].iType = aType;
		iEComPerfTimeRecords[iEComPerfTimeRecordCount].iInfo = aInfo;
		++iEComPerfTimeRecordCount;
		}
	}

/**
 Cleans up ECom performance timing record array and reset counter
*/
void EComPerformance::ResetEComPerfTimeRecords()
	{
	for(TInt idx = 0; idx <= iEComPerfTimeRecordCount; ++idx)
		{
		iEComPerfTimeRecords[idx].iTime = NULL;
		iEComPerfTimeRecords[idx].iType = ENullType;
		iEComPerfTimeRecords[idx].iInfo = NULL;
		}
	iEComPerfTimeRecordCount = 0;
	}

/**
Get the heap size at this startup state
*/	
void EComPerformance::RecordEComHeapSize(TInt aState)
	{
	if (iEComStartupStateHeapResultCount < KEComStartupStateHeapResultMaxCount)
		{
		iEComStartupStateHeapResults[iEComStartupStateHeapResultCount].iState=aState;
		iEComStartupStateHeapResults[iEComStartupStateHeapResultCount].iHeapSize=User::Heap().Size();
		++iEComStartupStateHeapResultCount;		
		}
	}
	
TInt EComPerformance::GetEComHeapSize(TInt aHeapIdx,TEComPerfHeapUsage& aHeapRecord)
	{
	if (aHeapIdx < KEComStartupStateHeapResultMaxCount)
		{
		aHeapRecord=iEComStartupStateHeapResults[aHeapIdx];
		}
	else
		{
		return KErrOverflow;
		}
	return KErrNone;
	}

#endif //__ECOM_SERVER_PERFORMANCE__
