
// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : tsystime_captest.cpp
// 
//

#include "tsystime.h"
#include <sys/time.h>
#include <e32err.h>
#include <sys/stat.h>
#include <stdio.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================



/*
-------------------------------------------------------------------------------
Function Name		: CTestsystime::utimesys
API Tested			: utimesys for capability

-------------------------------------------------------------------------------
*/

TInt CTestsystime::utimessys()
	{
	INFO_PRINTF1(_L("utimessys"));
    int i;
    struct timeval times[2];	
    times[0].tv_sec=1000;
    times[0].tv_usec=1000;
    times[1].tv_sec=100;
    times[1].tv_usec=100;
 	i=utimes("z:\\sys\\file5.txt",times ); 

  	if  (i < 0 ) 
		 {
	     INFO_PRINTF2(_L("errno=%d\n"),errno);
         return errno;
		 }
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestsystime::utimesprivate
API Tested			: utimesprivate for capability

-------------------------------------------------------------------------------
*/

TInt CTestsystime::utimesprivate()
	{
	INFO_PRINTF1(_L("utimesprivate"));
    int i;
    struct timeval times[2];	
    times[0].tv_sec=1000;
    times[0].tv_usec=1000;
 	times[1].tv_sec=100;
  	times[1].tv_usec=100;
 	i=utimes("z:\\private\\10202be9\\file5.txt",times );

  	if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
