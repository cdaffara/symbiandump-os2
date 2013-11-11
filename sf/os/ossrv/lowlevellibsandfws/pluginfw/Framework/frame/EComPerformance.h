/**
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Macros for performance testing  
* 
*
*/



/**
 @file
 @internalComponent
*/
#include <e32def.h>
#include <f32file.h> 
#include "EComSessionAux.h"

#ifndef ECOM_PERFORMANCE_H
#define ECOM_PERFORMANCE_H

#ifdef __ECOM_SERVER_PERFORMANCE__

// Defines the sizes of the static arrays used to store fast count records.
static const TUint KEComStartupStateTimerResultMaxCount = 20;
static const TUint KAccumulatedClientRequestTimerResultMaxCount = 500;
static const TUint KEComPerfTimerRecordMaxCount = 200;
static const TUint KEComStartupStateHeapResultMaxCount=20;

/**
	This class is provided for performance testability. If the testing 
	compile	time flag __ECOM_SERVER_PERFORMANCE__ is not defined the methods
	are not accessible.
*/
class EComPerformance
	{
public:
	/**
	This method is an accessor to get the timer results stored
	during the startup of EComServer. If the requested timer value at aTimerIdx
	does not exist aTimerResult is set to 0 and aState is set to -1.
	@param aTimerIdx the index of the timer to use (a zero indexed value)
	@param aTimerResult the timer readings
	@param aState the state of the server when timer was read
	*/
	static TInt GetStartupStateTimerResult(TInt aTimerIdx, TUint32& aTimerResult, TInt& aState);
	
	/**
	This method is used to record timer values with the state that 
	EComServer is in when this method is called.
	@param aState the state of the server
	*/
	static void RecordStartupStateTimerResult(TInt aState);

	/**
	This method is used to reset timer counter iEComStartupStateTimerResultCount
	so that the code part for recording ECom Startup State Timer Results can be reused
	for re-discover performance test.
	*/
	static void ResetStartupStateTimerResult();

	/**
	This method is used to record timer values with the state that 
	EComServer is in as well as the type of client request it is processing
	@param aTimerIdx the index of the time entry to get (a zero indexed value)
	@param aTimerEntry the entry returns
	*/
	static TInt GetAccumulatedClientRequestTimerResult(TInt aTimerIdx, TClientRequestTimerEntry& aTimerEntry);
	/**
	This method is used to record timer values with the state that 
	EComServer is in as well as the type of client request it is processing
	@param aClientRequestType the type of the client request
	@param aState the state of the server
	*/
	static void RecordStartClientRequestTimerResult(TEComClientRequestType aClientRequestType, TInt aState);
	/**
	This method is used to end a timer values with the state that 
	EComServer is in as well as the type of client request it is processing
	*/
	static void RecordEndClientRequestTimerResult();

	/**
	This method is an accessor to get the fast count records stored on server side. The types of
	 the records are defined by TEComPerfTimeRecordType.
	If the requested aTimeIdx exceeds the size of valid records, KErrOverflow will be returned.
	@param aTimerIdx the index of the record to get (a zero indexed value)
	@param aTimeRecord the record readings
	*/
	static TInt GetEComPerfTimeRecord(TInt aTimeIdx, TEComPerfTimeRecordEntry& aTimeRecord);

	/**
	This method is used to record fast count values with an additional information
	as well as the type of the record to distinguish the functions it measures
	@param aType the type of the record
	@param aInfo an additional information to be held by the record
	*/
	static void RecordEComPerfTime(TEComPerfTimeRecordType aType, TInt aInfo);
	
	/**
	This method is used to cleanup iEComPerfTimeRecords and reset counter iEComPerfTimeRecordCount
	so that the array for recording ECom performance time records can be reused.
	*/
	static void ResetEComPerfTimeRecords();
	
	/**
	This method is used to record the commited heap size at various startup stage
	*/
	static void RecordEComHeapSize(TInt aState);
	
	/**
	This method is used to retrieve a heap entry at a specified index
	*/
	static TInt GetEComHeapSize(TInt aHeapIdx,TEComPerfHeapUsage& aHeapRecord);

private:
	static TUint32 iEComStartupStateTimerResults[KEComStartupStateTimerResultMaxCount];
	static TInt iEComStartupStateTimerResultCount;

	static TClientRequestTimerEntry iAccumulatedClientRequestTimerResults[KAccumulatedClientRequestTimerResultMaxCount];
	static TInt iAccumulatedClientRequestTimerResultCount;
	
	static TEComPerfTimeRecordEntry iEComPerfTimeRecords[KEComPerfTimerRecordMaxCount];
	static TInt iEComPerfTimeRecordCount;
	
	static TEComPerfHeapUsage iEComStartupStateHeapResults[KEComStartupStateHeapResultMaxCount];
	static TInt iEComStartupStateHeapResultCount;
	
	// Friend class for unit tests
	friend class CDiscovererTest;
	};

#define START_TIMER EComPerformance::RecordStartupStateTimerResult(-1);
#define RECORD_INITIALISE_RESULT EComPerformance::RecordStartupStateTimerResult(-1);
#define START_HEAP EComPerformance::RecordEComHeapSize(-1);
#define RECORD_INITIALISE_HEAP EComPerformance::RecordEComHeapSize(-1);
#define RECORD_STARTUP_STATE_TIMER_RESULT(aState) EComPerformance::RecordStartupStateTimerResult(aState);
#define RECORD_STARTUP_STATE_HEAP_RESULT(aState) EComPerformance::RecordEComHeapSize(aState);
#define RECORD_CLIENT_REQUEST_START_TIMER_RESULT(aRequestType, aState) EComPerformance::RecordStartClientRequestTimerResult(aRequestType, aState)
#define RECORD_CLIENT_REQUEST_END_TIMER_RESULT EComPerformance::RecordEndClientRequestTimerResult()

#define RECORD_START_NOTIFIER_RUNL_TIMER_RESULT(aDriveNum) EComPerformance::RecordEComPerfTime(ECDiscovererNotifierRunL, aDriveNum);
#define RECORD_END_NOTIFIER_RUNL_TIMER_RESULT(aDriveNum) EComPerformance::RecordEComPerfTime(ECDiscovererNotifierRunL, aDriveNum);
#define RECORD_START_TIMER_RUNL_TIMER_RESULT EComPerformance::RecordEComPerfTime(ECDiscovererTimerRunL, -1);
#define RECORD_END_TIMER_RUNL_TIMER_RESULT EComPerformance::RecordEComPerfTime(ECDiscovererTimerRunL, -1);
#define RECORD_START_REDISCOVERYSCANDIRECTORY_RESULT(aDriveNum) EComPerformance::RecordEComPerfTime(ECDiscovererRediscoveryScanDirectoryL, aDriveNum);
#define RECORD_END_REDISCOVERYSCANDIRECTORY_RESULT(aDriveNum) EComPerformance::RecordEComPerfTime(ECDiscovererRediscoveryScanDirectoryL, aDriveNum);
#else //__ECOM_SERVER_PERFORMANCE__

#define START_TIMER 
#define RECORD_INITIALISE_RESULT
#define START_HEAP
#define RECORD_INITIALISE_HEAP
#define RECORD_STARTUP_STATE_TIMER_RESULT(aState)
#define RECORD_STARTUP_STATE_HEAP_RESULT(aState) 
#define RECORD_CLIENT_REQUEST_START_TIMER_RESULT(aRequestType, aState)
#define RECORD_CLIENT_REQUEST_END_TIMER_RESULT

#define RECORD_START_NOTIFIER_RUNL_TIMER_RESULT(aDriveNum)
#define RECORD_END_NOTIFIER_RUNL_TIMER_RESULT(aDriveNum)
#define RECORD_START_TIMER_RUNL_TIMER_RESULT
#define RECORD_END_TIMER_RUNL_TIMER_RESULT
#define RECORD_START_REDISCOVERYSCANDIRECTORY_RESULT(aDriveNum)
#define RECORD_END_REDISCOVERYSCANDIRECTORY_RESULT(aDriveNum)
#endif //__ECOM_SERVER_PERFORMANCE__

#endif //ECOM_PERFORMANCE_H
