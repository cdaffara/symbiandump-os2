// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file d32utrace.h
 @internalTechnology
*/

#ifndef D32UTRACE_H
#define D32UTRACE_H

#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif
#include <e32cmn.h>
#include <e32ver.h>

#include "te_tracerecord.h"

//needs to be the filename of the driver
_LIT(KUTraceDDDisabledFilename, "te_ostdd_disabled.ldd");
_LIT(KUTraceDDEnabledFilename, "te_ostdd_enabled.ldd");
_LIT(KUTraceDDSystemDisabledFilename, "te_utracedd_system_disabled.ldd");
_LIT(KUTraceDDSystemEnabledFilename, "te_utracedd_system_enabled.ldd");
_LIT(KUTraceDDEnabled, "UTraceDDEnabled");
_LIT(KUTraceDDDisabled, "UTraceDDDisabled");
_LIT(KUTraceDDSystemEnabled, "UTraceDDSystemEnabled");
_LIT(KUTraceDDSystemDisabled, "UTraceDDSystemDisabled");

struct TTimerResults
{
TUint32 iTime;
TUint32 iCount;
TInt	iError;
};

/**
Interface to the utrace api.

@internalTechnology
*/
class RUTrace : public RBusLogicalChannel
	{
public:
	/**
    Structure for holding driver capabilities information
    (Just a version number in this example.)
    */
    class TCaps
        {
    public:
        TVersion iVersion;
        };

	enum TControl
		{
		ETestTrace,
		ETimeTrace
		};

	inline static const TDesC& Name();
	inline static TVersion VersionRequired();

	/**
	Open channel to device driver.
	Must be called before any other methods are used.
	@return KErrNone or standard error code.
	*/
	TInt Open();
	TInt Close();
	TInt TestTraces(TTraceConfigs aTraceData);
	TInt TimeTraces(TTraceConfigs aTraceData, TUint32& aTime, TUint32& aCount);
	};

/**
  Returns the driver's name
*/
inline const TDesC& RUTrace::Name()
	{
#ifdef SYMBIAN_TRACE_SYSTEM_IS_INCLUDED
	#ifdef SYMBIAN_TRACE_EXECUTABLE_IS_INCLUDED
		return KUTraceDDEnabledFilename;
	#else
		return KUTraceDDDisabledFilename;
	#endif
#else
	#ifdef SYMBIAN_TRACE_EXECUTABLE_IS_INCLUDED
		return KUTraceDDSystemEnabledFilename;
	#else
		return KUTraceDDSystemDisabledFilename;
	#endif
#endif
	}
/**
  Returns the version number of the driver
*/
inline TVersion RUTrace::VersionRequired()
    {
    const TInt KMajorVersionNumber=1;
    const TInt KMinorVersionNumber=0;
    const TInt KBuildVersionNumber=3;
    return TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
    }

#ifndef __KERNEL_MODE__

TInt RUTrace::Open()
	{
	//this loads the driver in by using the filename
	//of the driver (adding the ldd extension if missing)
	TInt r = User::LoadLogicalDevice(Name());
	if(r!=KErrNone && r!=KErrAlreadyExists)
		return r;
	//this uses the name of the logical device
	//(the name of the LDD factory object, not the
	//filename of the LDD DLL)
	#ifdef SYMBIAN_TRACE_SYSTEM_IS_INCLUDED
		#ifdef SYMBIAN_TRACE_EXECUTABLE_IS_INCLUDED
			r = DoCreate(KUTraceDDEnabled, TVersion(), KNullUnit, NULL, NULL, EOwnerThread);
		#else
			r = DoCreate(KUTraceDDDisabled, TVersion(), KNullUnit, NULL, NULL, EOwnerThread);
		#endif
	#else
		#ifdef SYMBIAN_TRACE_EXECUTABLE_IS_INCLUDED
			r = DoCreate(KUTraceDDSystemEnabled, TVersion(), KNullUnit, NULL, NULL, EOwnerThread);
		#else
			r = DoCreate(KUTraceDDSystemDisabled, TVersion(), KNullUnit, NULL, NULL, EOwnerThread);
		#endif
	#endif

	return r;
	};

TInt RUTrace::TestTraces(TTraceConfigs aTraceRecord)
	{
	TInt result = KErrNone;
	TInt error = KErrNone;
	error = DoControl(ETestTrace,(TAny*)&aTraceRecord,(TAny*)&result);
	if(error != KErrNone)
		return error;
	return result;
	}

TInt RUTrace::TimeTraces(TTraceConfigs aTraceRecord, TUint32& aTime, TUint32& aCount)
	{
	
	TInt error = KErrNone;
   	TTimerResults timer;
	error = DoControl(ETimeTrace,(TAny*)&aTraceRecord,(TAny*)&timer);
   	aTime = timer.iTime;
   	aCount = timer.iCount;
   	return error;
	}

TInt RUTrace::Close()
	{
	RBusLogicalChannel::Close();
	return User::FreeLogicalDevice(RUTrace::Name());
	}
#endif //__KERNEL_MODE__

#endif
