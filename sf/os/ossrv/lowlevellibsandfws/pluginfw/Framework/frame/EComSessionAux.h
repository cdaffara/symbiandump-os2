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

#ifndef __ECOMSESSIONAUX_H__
#define __ECOMSESSIONAUX_H__

#include <e32cmn.h>
#include <startup.hrh>

/**
Codes used for processing ESetGetParameters messages.
Note: The constants are intended for component testing of
EcomServer.
@internalComponent
*/
enum TEComParameterOptions
	{
#ifdef __ECOM_SERVER_TESTABILITY__
	//these enumerated values are used for ecom component testing
	EChangeStartupState,
	EProcessStartupState,
	EGetStartupState,
#endif
#ifdef __ECOM_SERVER_PERFORMANCE__
	//this enumerated value is used for ecom performance testing
	EGetStartupStateTimerResult,
	EGetAccumulatedClientRequestsTimerResult,
	EGetRegistryCounts,
	EResetStartupStateTimerCounts,
	EGetEComPerfTimeRecord,
	EResetEComPerfTimeRecords,
	EGetEComServerHeapResult,
#endif 
	ELastOption
	};

#ifdef __ECOM_SERVER_TESTABILITY__

void ChangeStartupStateL(TInt aState);
	
void ProcessCurrentStartupStateL();
	
TInt GetCurrentStartupStateL();

#endif

#ifdef __ECOM_SERVER_PERFORMANCE__

//==================== For Startup State Time Results ===================
/**
@internalComponent
Class for storing start and end timer results for a state.
*/
class TStartupStateTimerResult
	{
public:
	/** State for which to store the timer result */
	TInt iState;
	/** Timer result reading in [ms] at the beginning of the state */
	TReal iStartTime;
	/** Timer result reading in [ms] at the end of the state */
	TReal iEndTime;
	};

/**
@internalComponent
Class for storing start and end timer results for a state and client request type.
*/
class TStartupStateTimerEntry
	{
public:
	/** State in which the ECom client request was made */
	TInt iState;
	/** Timer result reading in [ms] at the beginning of the request */
	TUint32 iTimerResult;
	};

class RStartupStateTimerResults
	{
public: // public methods
	void GetAllTimerResults();
	void Close();
	TInt Count();
	TStartupStateTimerResult& At(TInt aIndex);
	TInt FindInOrderTimerResult(TInt aState);
	TInt FindTimerResult(TInt aState);
	void ResetTimerCountL();
private: // private methods
	TInt GetTimerResult(TInt aTimerIdx, TUint32& aTimerResult, TInt& aState);
private: // private member variables
	RArray<TStartupStateTimerResult> iTimerResults;
	};

//==================== For Client Requests Time Results ===================
/**
@internalComponent
Enumeration of ECom client request types
*/
enum TEComClientRequestType
	{
	EEComCreateRequestType,
	EEComListRequestType,
	EEComNotifyOnChangeRequestType,
	EEComCancelNotifyOnChangeRequestType,
	EEComCollectImplementationsRequestType,
	EEComListExtendedInterfacesRequestType,
	EEComNumClientRequestTypes
	};

/**
@internalComponent
Class for storing start and end timer results for a state and client request type.
*/
class TClientRequestTimerEntry
	{
public:
	/** State in which the ECom client request was made */
	TInt iState;
	/** Timer result reading in [ms] at the beginning of the request */
	TUint iStartTime;
	/** Timer result reading in [ms] at the end of the request */
	TUint iEndTime;
	/** Whether the client request was a list request or a create request */
	TEComClientRequestType iClientRequestType;
	};

/**
@internalComponent
Class to retrieve timing for ECom client requests during the different stages of start-up
*/
class RClientRequestTimerResults
	{
public: // public methods
	void RetrieveResultsL();
	void Close();
	TReal GetAccumulatedClientRequestTime(TUint& aNumRequests);
	TReal GetAccumulatedClientRequestTime(TInt aState, TUint& aNumRequests);
	TReal GetAccumulatedClientRequestTime(TEComClientRequestType aRequestType, TUint& aNumRequests);
	TReal GetAccumulatedClientRequestTime(TInt aState, TEComClientRequestType aRequestType, TUint& aNumRequests);
private: // private methods
	static TInt GetTimerResult(TInt aTimerIdx, TClientRequestTimerEntry& aTimerEntry);
private: // private data
	/** List of all timer results */
	RArray<TClientRequestTimerEntry> iResults;
	};

/**
@internalComponent
Class to retrieve ECom plugin counts
*/
class RegistryCounts
	{
public:
	// Struct for storing registry counts.
	struct TRegistryCounts
		{
		enum TRegistryCountDriveType
			{
			ERoInternal,
			ENonRoInternal,
			EAll
			};
		/** The number of drives found */
		TInt iDrives;
		/** The number of plugins found */
		TInt iDlls;
		/** The number of interfaces found */
		TInt iInterfaces;
		/** The number fo implementations found */
		TInt iImplementations;
		};

public: // public methods
	static void GetRegistryCountsL(TRegistryCounts::TRegistryCountDriveType aType, TRegistryCounts& aCounts);
private: // private methods
private: // private data
	};

//==================== For ECom Performance Time Results ===================
/**
@internalComponent
ECom performance time record types. Time records can be retrieved by type.
*/
enum TEComPerfTimeRecordType
	{
	// Used only for validation, not used by any real types
	ENullType,
	// Time record types, distinguished by functions it exams
	ECDiscovererNotifierRunL,
	ECDiscovererTimerRunL,
	ECDiscovererRediscoveryScanDirectoryL,
	};

/**
@internalComponent
Class for storing a fast count result with type and other information.
*/
class TEComPerfTimeRecordEntry
	{
public:
	// Fast counter result.
	TUint iTime;
	// The type of time record.
	TEComPerfTimeRecordType iType;
	// Additional inforamtion attached with the record. Eg. Drive number of a notifier
	TInt iInfo;
	};

/**
@internalComponent
Class for storing a complete time measurement result (in ms) with type and other information.
*/
class TEComPerfRealTimeResult
	{
public:
	// Time result reading in [ms] at the beginning of the measurement.
	TReal iStartTime;
	// Time result reading in [ms] at the end of the measurement.
	TReal iEndTime;
	// Time result type, indicating which function/method it measures.
	TEComPerfTimeRecordType iType;
	// Additional information if needed.
	TInt iInfo;
	};

/**
@internalComponent
Class for storing the heap usage at various startup state
*/
class TEComPerfHeapUsage
	{
public:
	/** State for which to store the heap usage */
	TInt iState;
	/** Current committed heap reading */
	TInt iHeapSize;	
	};

/**
@internalComponent
An R Class to retrieve the heap record from the server side
*/
class REComHeapUsageRecords
	{
public:
	void OpenL();
	void GetAllHeapUsageResultsL();
	TInt GetHeapUsageAtState(TInt aState);
	void Close();
private:
	TInt GetHeapRecordEntry(TInt aHeapIdx,TEComPerfHeapUsage& aHeapEntry);
private:
	// this will store the actual heap usage at different state
	RArray<TEComPerfHeapUsage> iHeapRecords;	
	};

/**
@internalComponent
An R Class to retrieve time records from server side and process them into real time results. 
 APIs of the class are used by test cases.
*/
class REComPerfTimeRecords
	{
public: // public methods
	void OpenL();
	TInt Count();
	void Reset();
	void ResetRecordsOnServerL();
	void RetrieveResultsByTypeL(TEComPerfTimeRecordType aType, RArray<TEComPerfRealTimeResult>& aTimeResults);
	void RetrieveResultsInfoByTypeL(TEComPerfTimeRecordType aType, RArray<TInt>& aInfos);
	void Close();
private: // private methods
	TInt GetTimeRecordEntry(TInt aTimerIdx, TEComPerfTimeRecordEntry& aTimerEntry);
private: // private member variables
	// Local duplicate of time records on server, filled by Open()
	RArray<TEComPerfTimeRecordEntry> iTimeRecords;
	};

//===========================================================
TReal FastCountToMilliseconds(TInt aFastCount);

#endif

#endif // __ECOMSESSIONAUX_H__
