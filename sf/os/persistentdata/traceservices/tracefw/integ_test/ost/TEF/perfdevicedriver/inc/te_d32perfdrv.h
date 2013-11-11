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
// te_d32btrace.h
//

#ifndef TE_D32UTRACE_H
#define TE_D32UTRACE_H

#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif
#include <e32cmn.h>
#include <e32ver.h>
#include "te_apirunconfig.h"

//needs to be the filename of the driver
_LIT(KUptUTraceDDDisabledFilename, "te_uptutracedddisabled.ldd");
_LIT(KUptUTraceDDEnabledFilename, "te_uptutraceddenabled.ldd");
_LIT(KUptUTraceDDEnabled, "UptUTraceDDEnabled");
_LIT(KUptUTraceDDDisabled, "UptUTraceDDDisabled");

/**
Interface to the utrace api.

@internalTechnology
*/
class RUptUTrace : public RBusLogicalChannel
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
		ETestUTrace,
		ETimeUTrace,
		ESanityTestTimer,
		ESanityTestLongTimer,
		ESanityUtraceTimer,
		EGetTraceWordPtr
		};

	inline static const TDesC& Name();
	inline static TVersion VersionRequired();

	/**
	Open channel to device driver.
	Must be called before any other methods are used.
	@return KErrNone or standard error code.
	*/
	TInt Open();//TBool aTraceBuiltIn);
	TInt Close();
	TInt TestUptUtrace(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults);
	TInt TimeUptUtrace(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults);
	
	/**
	 * Get the trace word pointer 
	 */
	TInt GetTraceWordPtr(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults);
	
	/**
	 * Just to make sure the timers work
	 */
	TBool SanityTestTimer(TUint32& aTestTime);
	TBool SanityTestLongTimer(TUint32& aTestTime);
	TBool SanityUtraceTestTimer(TUint32& aTestTime);
private:
	TBool DoSanityTimerTest(const TControl aTest, TUint32& aTestTime);
	};

	
/**
 * used for passing results back from the sanity testing
 */
struct TSanityResults
{
TUint32	iTime;
TUint32	iCount;
TBool	iPass;
};

/**
  Returns the driver's name
*/
inline const TDesC& RUptUTrace::Name()
	{
#ifdef TE_UPT_TRACE_ENABLED
	return KUptUTraceDDEnabledFilename;
#else
	return KUptUTraceDDDisabledFilename;
#endif

	}
/**
  Returns the version number of the driver
*/
inline TVersion RUptUTrace::VersionRequired()
    {
    const TInt KMajorVersionNumber=1;
    const TInt KMinorVersionNumber=0;
    const TInt KBuildVersionNumber=3;
    return TVersion(KMajorVersionNumber,KMinorVersionNumber,KBuildVersionNumber);
    }

#endif
