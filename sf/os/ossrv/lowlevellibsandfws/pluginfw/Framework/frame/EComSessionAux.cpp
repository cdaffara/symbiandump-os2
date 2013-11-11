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

#include <ecom/ecom.h>
#include <hal.h>
#include "EComSessionAux.h"

#ifdef __ECOM_SERVER_TESTABILITY__

/**
@internalComponent
A helper function to use SetGetParametersL to set the next
state to be used by the ServerStartupManager of EComServer
@param aState the next state to be used by the EComServer
*/
void ChangeStartupStateL(TInt aState)
	{
	TIpcArgs params = TIpcArgs(EChangeStartupState, aState);
	REComSession::SetGetParametersL(params);
	}
	
/**
@internalComponent
A helper function to use SetGetParametersL to force EComServer
to process the next state.
*/
void ProcessCurrentStartupStateL()
	{
	TIpcArgs params = TIpcArgs(EProcessStartupState);
	REComSession::SetGetParametersL(params);
	}
	
/**
@internalComponent
A helper function to use SetGetParametersL to get the current 
startup state of EComServer
@return current startup state of EComServer
*/
TInt GetCurrentStartupStateL()
	{
	TInt currentState = 0;
	TPckg<TInt> statePckg(currentState);
	TIpcArgs params = TIpcArgs(EGetStartupState, &statePckg);
	REComSession::SetGetParametersL(params);
	
	return currentState;
	}

#endif

#ifdef __ECOM_SERVER_PERFORMANCE__

//==================== For Startup State Time Results ===================
/** Method used for TLinearOrder for TTimerResult arrays*/
TInt CompareTimerResults(const TStartupStateTimerResult& aTimerResult1, const TStartupStateTimerResult& aTimerResult2)
	{
	if(aTimerResult1.iState < aTimerResult2.iState)
		{
		return -1;
		}
	else if(aTimerResult1.iState > aTimerResult2.iState)
		{
		return 1;
		}
		
	return 0;
	}
	
/** Method used for TIdentityRelation for TTimerResult arrays*/
TBool MatchOnState(const TStartupStateTimerResult& aKey, const TStartupStateTimerResult& aIndexItem)
	{
	return aIndexItem.iState == aKey.iState;
	}

TInt RStartupStateTimerResults::FindTimerResult(TInt aState)
	{
	TIdentityRelation<TStartupStateTimerResult> identity(MatchOnState);
	TStartupStateTimerResult key;
	key.iState = aState;
	return iTimerResults.Find(key, identity);
	}

/**
Method that returns the idx of the timer result at requested state
@param aTimerResults the sorted timer results array to search for the 
timer value
@param aState the state of the requested timer result 
@return The index of the matching timer result KErrNotFound, if no 
matching object can be found
*/	
TInt RStartupStateTimerResults::FindInOrderTimerResult(TInt aState)
	{
	TLinearOrder<TStartupStateTimerResult> compareFunc(CompareTimerResults);
	TStartupStateTimerResult key;
	key.iState = aState;
	return iTimerResults.FindInOrder(key, compareFunc);
	}

/**
@internalComponent
A helper function to use SetGetParametersL to get the timer recordings
for performance testing. If the specified aTimerIdx is beyond the boundaries
of the recorded values then aTimerResult would return a value of 0 and aState
would return a value of -1. 
@param aTimerIdx the index of the timer to use
@param aTimerResult the timer readings
@param aState the state of the server when timer was read (a zero indexed value)
*/
TInt RStartupStateTimerResults::GetTimerResult(TInt aTimerIdx, TUint32& aTimerResult, TInt& aState)
	{
	TInt returnedStatus;
	TStartupStateTimerEntry result;

	TPckg<TInt> statusPckg(returnedStatus);
	TPckg<TStartupStateTimerEntry> resultPckg(result);

	TIpcArgs params = TIpcArgs(EGetStartupStateTimerResult, aTimerIdx, &statusPckg, &resultPckg);
	TRAPD(err, REComSession::SetGetParametersL(params));
	if (err != KErrNone)
		{
		return err;	
		}
	
	aTimerResult = result.iTimerResult;
	aState = result.iState;
	
	return returnedStatus;
	}

/**
@internalComponent
A helper function to use SetGetParametersL to get all of the timer recordings
for performance testing.
@param aTimerResults a reference to an array to store sorted timer results.
*/
void RStartupStateTimerResults::GetAllTimerResults()
	{
	TInt counter = 0;
	while(ETrue)
		{
		TUint32 time = 0;
		TInt state = -1;
		TReal realTime = 0.0;
		
		// get state start info
		TInt err = GetTimerResult(counter++, time, state);
		if(err || (time == 0 && state == -1))
			{
			break;
			}
		
		realTime = FastCountToMilliseconds(time);
		
		//search state
		TInt idx = FindTimerResult(state);
		if(idx == KErrNotFound) //if it has not been entered to the array make a new entry
			{
			TStartupStateTimerResult timerResult;
			timerResult.iState = state;
			timerResult.iStartTime = realTime;
			timerResult.iEndTime = 0;
			
			iTimerResults.Append(timerResult);
			}
		else //if it has already been entered update the end time
			{
			TStartupStateTimerResult& timerResult = iTimerResults[idx];
			timerResult.iEndTime = realTime;
			}
		}
	
	//the array is populated, sort it for faster search.
	TLinearOrder<TStartupStateTimerResult> compareFunc(CompareTimerResults);
	iTimerResults.Sort(compareFunc);
	}

void RStartupStateTimerResults::ResetTimerCountL()
	{
	
	TIpcArgs params = TIpcArgs(EResetStartupStateTimerCounts);
	REComSession::SetGetParametersL(params);
	}
	
/**
 Frees any allocated resources
*/
void RStartupStateTimerResults::Close()
	{
	iTimerResults.Close();
	}

/**
 Startup state timer results must have been populated by a call to RetrieveResults before this method is called
 @param aIndex The index of the timing entry to retrieve
 @return The timer result entry for the given index
*/
TStartupStateTimerResult& RStartupStateTimerResults::At(TInt aIndex)
	{
	return iTimerResults[aIndex];
	}

/**
 @return The number of timer results retrieved
*/
TInt RStartupStateTimerResults::Count()
	{
	return iTimerResults.Count();
	}

//==================== For Client Requests Time Results ===================
/**
 Retrieves a single client request timer entry from the ECom server
 @param aTimerIdx The index of the timing entry to retrieve
 @return KErrNone if successful, KErrOverflow if aTimerIdx is greater than the number of timing entries
*/
TInt RClientRequestTimerResults::GetTimerResult(TInt aTimerIdx, TClientRequestTimerEntry& aTimerEntry)
	{
	TInt returnedStatus;
	TPckg<TInt> statusPckg(returnedStatus);
	TPckg<TClientRequestTimerEntry> resultPckg(aTimerEntry);
	
	TIpcArgs params = TIpcArgs(EGetAccumulatedClientRequestsTimerResult, aTimerIdx, &statusPckg, &resultPckg);
	TRAPD(err, REComSession::SetGetParametersL(params));
	if (err != KErrNone)	
		{
		return err;	
		}
	
	return returnedStatus;
	}
	
/**
Retrieves all client request timer entries from the ECom server
*/
void RClientRequestTimerResults::RetrieveResultsL()
	{
	TClientRequestTimerEntry timerEntry;
	TInt err = KErrNone;
	for (TInt i = 0; err == KErrNone; i++)
		{
		err = GetTimerResult(i, timerEntry);
		if (err == KErrNone)
			{
			iResults.Append(timerEntry);
			}
		else if (err != KErrOverflow)
			{
			User::Leave(err);
			}
		}
	}
	
/**
 Determines the total time taken by ECom server servicing client requests with the given state and request type
 by going through each of the retrieved timer entries and adding up their associated times
 @pre RetrieveResultsL must have been called before this method is called
 @param aState The startup state to retrieve timer results for
 @param aRequestType The type of client request to retrieve timer results for
 @param aNumRequests On return contains the number of client requests matching aState and aRequestType
 @return The total time taken servicing client requests matching startup state aState and aRequestType
*/
TReal RClientRequestTimerResults::GetAccumulatedClientRequestTime(TInt aState, TEComClientRequestType aRequestType, TUint& aNumRequests)
	{
	TUint32 accumulatedTicks = 0;
	
	aNumRequests = 0;
	for (TInt i = 0; i < iResults.Count(); i++)
		{
		if (iResults[i].iState == aState && iResults[i].iClientRequestType == aRequestType)
			{
			TUint netTime = iResults[i].iEndTime - iResults[i].iStartTime;
			accumulatedTicks += netTime;
			aNumRequests++;
			}
		}
		
	return FastCountToMilliseconds(accumulatedTicks);
	}

/**
 Determines the total time taken by ECom server servicing client requests with the given state
 by going through each of the retrieved timer entries and adding up their associated times
 Client request timer results must have been populated by a call to RetrieveResults before this method is called
 @param aState The startup state to retrieve timer results for
 @param aNumRequests On return contains the number of client requests matching aState
 @return The total time taken servicing client requests matching startup state aState
*/
TReal RClientRequestTimerResults::GetAccumulatedClientRequestTime(TInt aState, TUint& aNumRequests)
	{
	TUint32 accumulatedTicks = 0;
	
	aNumRequests = 0;
	for (TInt i = 0; i < iResults.Count(); i++)
		{
		if (iResults[i].iState == aState)
			{
			TUint netTime = iResults[i].iEndTime - iResults[i].iStartTime;
			accumulatedTicks += netTime;
			aNumRequests++;
			}
		}
		
	return FastCountToMilliseconds(accumulatedTicks);
	}

/**
 Determines the total time taken by ECom server servicing client requests with the given request type
 by going through each of the retrieved timer entries and adding up their associated times
 Client request timer results must have been populated by a call to RetrieveResults before this method is called
 @param aRequestType The type of client request to retrieve timer results for
 @param aNumRequests On return contains the number of client requests matching aRequestType
 @return The total time taken servicing client requests matching startup state aRequestType
*/
TReal RClientRequestTimerResults::GetAccumulatedClientRequestTime(TEComClientRequestType aRequestType, TUint& aNumRequests)
	{
	TUint32 accumulatedTicks = 0;
	
	aNumRequests = 0;
	for (TInt i = 0; i < iResults.Count(); i++)
		{
		if (iResults[i].iClientRequestType == aRequestType)
			{
			TUint netTime = iResults[i].iEndTime - iResults[i].iStartTime;
			accumulatedTicks += netTime;
			aNumRequests++;
			}
		}
		
	return FastCountToMilliseconds(accumulatedTicks);
	}

/**
 Determines the total time taken by ECom server servicing all client requests
 by going through each of the retrieved timer entries and adding up their associated times
 Client request timer results must have been populated by a call to RetrieveResults before this method is called
 @param aNumRequests On return contains the number of client requests
 @return The total time taken servicing client requests
*/
TReal RClientRequestTimerResults::GetAccumulatedClientRequestTime(TUint& aNumRequests)
	{
	TUint32 accumulatedTicks = 0;
	
	aNumRequests = 0;
	for (TInt i = 0; i < iResults.Count(); i++)
		{
		TUint netTime = iResults[i].iEndTime - iResults[i].iStartTime;
		accumulatedTicks += netTime;
		aNumRequests++;
		}
		
	return FastCountToMilliseconds(accumulatedTicks);
	}

/**
Frees any resources
*/
void RClientRequestTimerResults::Close()
	{
	iResults.Reset();
	}

/**	
@internalComponent
A helper function to use SetGetParametersL to get the number of
drives, plugins, interfaces, implementation for performance testing.
@param aCounts a reference to a struct to store results.
*/
void RegistryCounts::GetRegistryCountsL(TRegistryCounts::TRegistryCountDriveType aType, TRegistryCounts& aCounts)
	{
	TPckg<TRegistryCounts> registryCountPckg(aCounts);
	TIpcArgs params = TIpcArgs(EGetRegistryCounts, aType, &registryCountPckg);
	REComSession::SetGetParametersL(params);
	}


//==================== For ECom Performance Time Results ===================
/**
@internalComponent
A helper function to use SetGetParametersL to get the time recordings
for performance testing. If the specified aTimerIdx is beyond the size
of the valid recorded values then KErrOverFlow returns from the server side
@param aTimerIdx the index of the time record to use
@param aTimeEntry the fast time record entry
*/
TInt REComPerfTimeRecords::GetTimeRecordEntry(TInt aTimeIdx, TEComPerfTimeRecordEntry& aTimeEntry)
	{
	TInt returnedStatus;

	TPckg<TInt> statusPckg(returnedStatus);
	TPckg<TEComPerfTimeRecordEntry> resultPckg(aTimeEntry);

	TIpcArgs params = TIpcArgs(EGetEComPerfTimeRecord, aTimeIdx, &statusPckg, &resultPckg);
	TRAPD(err, REComSession::SetGetParametersL(params));
	if (err != KErrNone)
		{
		return err;	
		}
		
	return returnedStatus;
	}

/**
Get all ECom performance time records from server and fill up local array for further use
*/
void REComPerfTimeRecords::OpenL()
	{
	TEComPerfTimeRecordEntry timeEntry;
	TInt err = KErrNone;
	
	iTimeRecords.Reset();
	
	// Get the first record from server
	TInt idx = 0;
	err = GetTimeRecordEntry(idx, timeEntry);
	while(err == KErrNone)
		{
		// If it is a valid record entry, append it to local record array, 
		//  and get the next entry from server.
		if (timeEntry.iType != ENullType)
			{
			iTimeRecords.Append(timeEntry);
			idx++;
			err = GetTimeRecordEntry(idx, timeEntry);
			}
		// Otherwise finished
		else
			break;
		}
			
	if (err != KErrOverflow)
		{
		User::LeaveIfError(err);
		}
	}

/**
Transform raw fast count records into real time results (in mSecs), and collect the results by specified type.
*/
void REComPerfTimeRecords::RetrieveResultsByTypeL(TEComPerfTimeRecordType aType, RArray<TEComPerfRealTimeResult>& aTimeResults)
	{
	TEComPerfRealTimeResult timeResult;
	TBool start = ETrue;
	TInt count = iTimeRecords.Count();

	if(count)
		{
		for (TInt i = 0; i < count; i++)
			{
			// If the record has correct type
			if(iTimeRecords[i].iType == aType)
				{
				// If the record is the first of the couple, it should be the start time record
				if(start)
					{
					timeResult.iStartTime = FastCountToMilliseconds(iTimeRecords[i].iTime);
					timeResult.iType = iTimeRecords[i].iType;
					timeResult.iInfo = iTimeRecords[i].iInfo;
					start = EFalse;
					}
				// Otherwise it should be the end time record.
				else
					{
					timeResult.iEndTime = FastCountToMilliseconds(iTimeRecords[i].iTime);
					aTimeResults.Append(timeResult);
					start = ETrue;
					}
				}
			}
		}
	else // Leave if there's no records to retrieve
		{
		User::Leave(KErrNotFound);
		}
	}


/**

*/
void REComPerfTimeRecords::RetrieveResultsInfoByTypeL(TEComPerfTimeRecordType aType, RArray<TInt>& aInfos)
	{
	TInt count = iTimeRecords.Count();
	if(count)
		{
		for (TInt i = 0; i < count; i++)
			{
			// If the record has correct type, insert the info into aInfos array. No duplicate infos are recorded.
			if(iTimeRecords[i].iType == aType)
				{
				TInt err = aInfos.InsertInOrder(iTimeRecords[i].iInfo);
				
				if(err!=KErrAlreadyExists)
					User::LeaveIfError(err);
				}
			}
		}
	else // Leave if there's no records to retrieve
		{
		User::Leave(KErrNotFound);
		}
	}

// Reset all fast count records on server
void REComPerfTimeRecords::ResetRecordsOnServerL()
	{
	TIpcArgs params = TIpcArgs(EResetEComPerfTimeRecords);
	REComSession::SetGetParametersL(params);
	}

TInt REComPerfTimeRecords::Count()
	{
	return iTimeRecords.Count();
	}

// Empty local time result array
void REComPerfTimeRecords::Reset()
	{
	iTimeRecords.Reset();
	}

// Release resources
void REComPerfTimeRecords::Close()
	{
	iTimeRecords.Close();
	}

//==================== For ECom Performance Heap Usage Results ===================
/**
@internalComponent
*/
void REComHeapUsageRecords::OpenL()
	{
	iHeapRecords.Reset();
	
	// Get the first record from server
	TInt idx = 0;
	TEComPerfHeapUsage heapEntry;
	TInt err=GetHeapRecordEntry(idx,heapEntry);
	while(err == KErrNone && heapEntry.iState!=0)
		{
		//check existing array for a start item
		TBool startFound=EFalse;
		for (TInt i=0;i<iHeapRecords.Count();i++)
			{
			//if can find one this entry must be a start entry so update the heap usage
			if (iHeapRecords[i].iState==heapEntry.iState)
				{
				iHeapRecords[i].iHeapSize=heapEntry.iHeapSize-(iHeapRecords[i].iHeapSize);
				startFound=ETrue;
				break;
				}
			}
		//only append the entry if it is a new start entry
		if (!startFound)
			iHeapRecords.Append(heapEntry);
		idx++;
		err=GetHeapRecordEntry(idx,heapEntry);
		}
			
	if (err != KErrOverflow)
		{
		User::LeaveIfError(err);
		}	
	}

TInt REComHeapUsageRecords::GetHeapUsageAtState(TInt aState)
	{
	for (TInt i=0;i<iHeapRecords.Count();i++)
		{
		if (iHeapRecords[i].iState==aState)
			{
			return iHeapRecords[i].iHeapSize;
			}
		}
	return KErrNotFound;
	}

void REComHeapUsageRecords::Close()
	{
	iHeapRecords.Reset();
	iHeapRecords.Close();
	}

TInt REComHeapUsageRecords::GetHeapRecordEntry(TInt aHeapIdx,TEComPerfHeapUsage& aHeapEntry)
	{
	TInt returnedStatus;
	TPckg<TInt> statusPckg(returnedStatus);
	TPckg<TEComPerfHeapUsage> resultPckg(aHeapEntry);
	TIpcArgs params = TIpcArgs(EGetEComServerHeapResult, aHeapIdx,&statusPckg,&resultPckg);
	TRAPD(err, REComSession::SetGetParametersL(params));
	return err;
	}

//===========================================================
/**
Converts time retrieved using FastCounter to milliseconds
 @param aFastCount The time to convert to milliseconds, retrieved using User::FastCounter
 @return The time in milliseconds corresponding to aFastCount
*/
TReal FastCountToMilliseconds(TInt aFastCount)
	{
	TInt freqInHz;
	HAL::Get(HAL::EFastCounterFrequency, freqInHz);
	TReal freqInkHz = freqInHz / 1000;
	return (TReal)aFastCount / freqInkHz;
	}
	
#endif
