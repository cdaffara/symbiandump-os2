
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
// Name        : tipc_captest.cpp
// 
//

 
#include "tipc.h"
#include <sys/ipc.h>
#include <e32err.h>
#include <sys/stat.h>
#include <stdio.h>

#define MAX_LEN  20

// ============================ MEMBER FUNCTIONS ===============================


/*
-------------------------------------------------------------------------------
Function Name		: CTestipc::ftokprivate
API Tested			: ftokprivate for capability

-------------------------------------------------------------------------------
*/

TInt CTestipc::ftokprivate()
	{
	INFO_PRINTF1(_L("ftokprivate"));
    key_t i;
    int proj_id = 100;
    i=ftok("z:\\private\\10202be9\\temp2", proj_id);
    
    if  (i < 0) 
		{
		
			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestipc::ftoksys
API Tested			: ftoksys for capability

-------------------------------------------------------------------------------
*/

TInt CTestipc::ftoksys()
	{
	INFO_PRINTF1(_L("ftoksys"));
    key_t i;
    int proj_id = 100;
 	i=ftok("z:\\sys\\temp2", proj_id);
 	
	if  (i < 0) 
		{

			INFO_PRINTF2(_L("errno=%d\n"),errno);
         	return errno;
		}
	return KErrNone;
	}
	
































