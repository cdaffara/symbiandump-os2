
// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tcapclockblocks.cpp
// 
//

#include "tcapclock.h"

// -----------------------------------------------------------------------------
// CTesttrapsignal::Testcapclocksettime1()
// 
// API tested: clock_settime()
// Description: Capability test for clock_settime()
// -----------------------------------------------------------------------------

TInt Ctestcapclock::Testcapclocksettime1()
	{
	int ret, ret1 = KErrGeneral;
	clockid_t clockid;
	struct timespec stmspec, gtmspec;
	ret = clock_getcpuclockid(0,&clockid);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to retrieve the clock id of the calling process and errno is %d"),errno);
	 	goto close;	
		}
	stmspec.tv_sec = 2;
	stmspec.tv_nsec = 100000;
	ret = clock_settime(clockid,&stmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	return errno;	
		}	
	INFO_PRINTF1(_L("clock_settime() successfully able to set the time") );
	ret = clock_gettime(clockid,&gtmspec);
	if (ret != 0)
		{
	 	ERR_PRINTF2(_L("Failed to get the time of specified clock id and errno is %d"),errno);
	 	goto close;	
		}
	if (gtmspec.tv_sec < stmspec.tv_sec)
		{
	 	ERR_PRINTF2(_L("Failed to retrieve resolution of the clock id specified and errno is %d"),errno);
	 	goto close;			
		}
	INFO_PRINTF1(_L("clock_gettime() successfully able to get the time") );
	ret1 = KErrNone;
	
	close:
	return ret1;	
	}

//End of file
