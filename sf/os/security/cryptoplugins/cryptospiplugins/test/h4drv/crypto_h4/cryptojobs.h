/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/**
 @file
 @internalComponent
 @released
*/
#ifndef __CRYPTOJOBS__
#define __CRYPTOJOBS__
#include <e32cmn.h>
#include <e32ver.h>
#include <e32def.h>
#include "cryptodriver.h"

#define TRACE_FUNCTION(funcName) do{ static TraceFunction tf( __FILE__ , funcName ); tf.Inc(); }while(0)

class TraceFunction
	{
public:
	TraceFunction(const char *aFileName, const char *aFunctionName)
		: iFileName(aFileName),
		  iFunctionName(aFunctionName),
		  iHitCount(0),
		  iNext(HeadRef())
		{
		HeadRef() = this;
		}
	
	void Inc()
		{
		++iHitCount;
		}

static void DumpCounts()
		{
		Kern::Printf("TraceFunction::DumpCounts\n");
		TraceFunction *p = HeadRef();
		while(p)
			{
			Kern::Printf("%d\t%s:%s\n", p->iHitCount, p->iFileName, p->iFunctionName);
			p->iHitCount = 0;
			p = p->iNext;
			}
		}

IMPORT_C static TraceFunction *&HeadRef();

private:
	const char *iFileName;
	const char *iFunctionName;
	TUint32 iHitCount;

	TraceFunction *iNext;
	};


class CryptoJob;
class MCryptoJobCallbacks;
class DCryptoJobScheduler : public DBase
	{
public:
	IMPORT_C DCryptoJobScheduler();
	IMPORT_C ~DCryptoJobScheduler();

	/**
	   ScheduleJob

	   If job is already queued, and is the current job, and is not
	   already running, attempt to schedule it and return.  Otherwise,
	   if already in the queue, just return.

	   If the job is not in the queue, then add it - If the queue is
	   empty it is added at the front and we attempt to run it,
	   otherwise we add it second in the queue for later scheduling.
	 */
	IMPORT_C void ScheduleJob(CryptoJob *aJob);

	/**
	   Abort the job synchronously

	   In the EReady state call DoReleaseHw derived function which
	   should, before returning, abort the job.

	   Changes the state to Idle.
	   
	   Removes from the scheduler.
	   
	   The caller is expected to complete any outstanding user request.
	*/
	IMPORT_C void DeScheduleJob(CryptoJob *aJob);

	/**
	   SliceComplete

	   Called by a job when the current slice is complete. The job
	   should update its internal state before calling us.
	   
	   The specified job MUST be the current job.
	 */
	IMPORT_C void SliceComplete(CryptoJob *aJob, TInt aStatus);

	/**
	   JobComplete

	   Called by a job when the final slice is complete.
	   
	   The specified job MUST be the current job.
	 */
	IMPORT_C void JobComplete(CryptoJob *aJob, TInt aStatus);
private:
	/**
	   Schedule
	   
	   If current job is not running make it run
	 */
	void Schedule(TBool aReschedule);

	CryptoJob *iJobList;
	};


class DCryptoLddChannel;
class CryptoJob
	{
public:
	IMPORT_C CryptoJob();
	IMPORT_C virtual ~CryptoJob();
	virtual void SetDfcQ(TDfcQue *aDfcQue) = 0;

	IMPORT_C void Stalled();  // This job MUST be the current job
	IMPORT_C void Resume();
	IMPORT_C void DeScheduleJob();
	IMPORT_C void SetRunning(TBool aRunning);

	virtual void GetToPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore) = 0;
	virtual void BytesWrittenToPdd(TUint32 aBytes) = 0;

	virtual void GetFromPddBuffer(TUint8 * &aBuf, TUint32 &aBufLen, TBool &aMore) = 0;
	virtual void BytesReadFromPdd(TUint32 aBytes) = 0;

	enum CryptoJobState 
	{
		ECreated, 			// Not registered with LDD factory
		EReadyForFirstRun, 	// Not running, but could run next for the first time
		ERunning,			// H/w is currently working on this job
		EReady,			  	// Is the current job (h/w is setup), but h/w is idle
		EReadyNoSavedState,	// Ready to run
		EReadySavedState,	// Ready to run, but requires state restore
		// EStalled
		//
		// Three cases:-
		//
		// 1) Already got enough data but LDD hasn't read it yet.
		// 2) No space for more h/w output
		// 3) No more data to feed to h/w
		//
		// Ultimately recoverd by the LDD reading/writing data and/or
		// deleting the job.
		EStalled
	};

 	IMPORT_C CryptoJobState State() const;
	
private:
	friend class DCryptoJobScheduler;
	/**
	   
	 */
	virtual void DoSlice(TBool aFirstSlice) = 0;

	/**
	   DoSaveState

	   Save the current job h/w state so that the job can be later
	   restored when we are re-scheduled.

	   If there is state to save it should return ETrue (in which case
	   DoRestoreState will be called before our DoSlice function is
	   next called).
	   
	   If the h/w state does not require saving/restore it should
	   return EFalse.
	*/
	virtual TBool DoSaveState() = 0;

	/**
	   DoRestoreState
	   
	   Restore the h/w state. The caller will almost certainly call
	   DoSlice just after this to continue the job.
	*/
	virtual void DoRestoreState() = 0;

	/**
	   DoReleaseHw
	   
	   Only called in state EReady or ERunning

	   Should abort the operation and release h/w (unhook ISRs etc)
	   before returning.

	   Later the caller will probably complete the user request...
	 */
	virtual void DoReleaseHw() = 0;
	
	CryptoJobState iState;

protected:
	DCryptoJobScheduler *iJobScheduler;
	MCryptoJobCallbacks *iCallbacks;
private:

	CryptoJob *iNext;
	TBool iInJobList;
	};

class MCryptoJobCallbacks
	{
public:
	/**
	   DataRequired

	   PDD is requesting more data to process.

	   Normally the LDD will write one or more bytes into the supplied
	   buffer and return the number of bytes written. It is also legal
	   for the LDD to not write any bytes and return 0.

	   If not enough bytes are supplied, the PDD will underrun and
	   stall (this function might not be re-called).

	   The LDD should provide more data to the h/w via the
	   GetToPddBuffer/BytesWrittenToPdd functions.

	   @return KErrNone or error code
	*/
	virtual TInt DataRequired() = 0;

	/**
	   DataAvailable

	   PDD has output data available for copying to the user.

	   This function will be called when the PDD is running low out
	   space for storing output data of if all input data has been
	   processed.

	   Normally the LDD would copy all the supplied data to the user.

	   It is legal for the LDD to copy less bytes, but this may cause
	   and the PDD to overrun and stall (this function might not be
	   re-called).

	   The LDD should use the GetFromPddBuffer/BytesReadFromPdd
	   functions to read data from the PDD.

	   @return KErrNone or error code

	*/
	virtual TInt DataAvailable() = 0;

	/**
	   JobComplete

	   The job scheduler has declared this job complete (maybe with an
	   error), and calls this function to tell the LDD to complete the
	   user request.
	*/
	virtual void JobComplete(TInt aResult) = 0;
	};


class CryptoJobRandom : public CryptoJob
	{
public:
	/**
	   SetDetails
	   
	   Setup and start job. Results later available via ResultsBuffer()

	   @param Ptr to the DCryptoJobScheduler for this type of job
	   @param Ptr to the LDD object for MCryptoJobCallbacks
	   @param aNumOfBytes Total number of random numbers the user requires
	*/
	virtual void SetDetails(DCryptoJobScheduler *aJobScheduler, 
							MCryptoJobCallbacks *aCallbacks,
							TUint32 aNumOfBytes) = 0;
	};

class CryptoJobAes : public CryptoJob
	{
public:
	/**
	   GetKeyBuffer

	   Get ptr to KEY buffer to copy user data into. Max key length is
	   32 bytes (256 bits)

	   @param Pointer to key buffer
	*/
	virtual TUint8 *GetKeyBuffer() = 0;

	/**
	   GetIVBuffer

	   Get ptr to IV buffer to copy user IV data into. 16 bytes long.

	   @param Pointer to IV buffer
	*/
	virtual TUint8 *GetIVBuffer() = 0;

	/**
	   MaxBytes

	   @param Length of buffer returned by Buffer().
	*/
	virtual TUint32 MaxBytes() const = 0;

	/**
	   Buffer

	   Get ptr to PDD buffer to copy user data into and results from
	   Buffer length <= MaxBytes().

	   @param Pointer to key buffer
	*/
	virtual TUint8 *GetIOBuffer() = 0;

	/**
	   SetDetails

	   @param Ptr to the DCryptoJobScheduler for this type of job
	   @param Ptr to the LDD object for MCryptoJobCallbacks
	   @param aEncrypt True requests encryption, false decryption
	   @param aKeyLength Length of key in bytes
	   @param aMode See RCryptoDriver::TChainingMode
	   @return KErrNone or error code
	*/
	virtual TInt SetDetails(DCryptoJobScheduler *aJobScheduler, 
							MCryptoJobCallbacks *aCallbacks,
							TBool aEncrypt, 
							TInt aKeyLengthBytes,
							RCryptoDriver::TChainingMode aMode) = 0;

	virtual void NotifyReadRequestLength(TUint32 aReadRequestLength) = 0;
	virtual void HwPerfCheck() = 0;
	};

#endif
