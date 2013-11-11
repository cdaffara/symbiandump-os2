
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
// Name        : tutime_captest.cpp
// 
//

 
#include "tutime.h"
#include <utime.h>
#include <e32err.h>
#include <sys/stat.h>
#include <stdio.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================


/*
-------------------------------------------------------------------------------
Function Name		: CTestutime::utimeprivate
API Tested			: utimeprivate for capability

-------------------------------------------------------------------------------
*/

TInt CTestutime::utimeprivate()
	{
	INFO_PRINTF1(_L("utimeprivate"));
   	int i;
    utimbuf time;	
    time.actime=10000;
    time.modtime=10000;
    i=utime("z:\\private\\10202be9\\file4.txt",&time );
 
    if  (i < 0 ) 
		{
		INFO_PRINTF1(_L("Error in utime API"));
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
   	    return errno;
		}
	return KErrNone;
	}

	
/*
-------------------------------------------------------------------------------
Function Name		: CTestutime::utimesys
API Tested			: utimesys for capability

-------------------------------------------------------------------------------
*/

TInt CTestutime::utimesys()
	{
    int i;
    utimbuf time;	
    time.actime=10000;
    time.modtime=10000;
    i=utime("z:\\sys\\file4.txt",&time );
    
  	if  (i < 0 ) 
		{
	    INFO_PRINTF2(_L("errno=%d\n"),errno);
        return errno;
		}
	return KErrNone;
	}
	
































