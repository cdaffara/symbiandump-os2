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
// Name     : wsd_init.cpp
// Part of  : PThread library
// File for the writable static data in the library. 
// Version:
//



#include "threadglobals.h"

#ifdef __WINSCW__
#include <pls.h> // For emulator WSD API
const TUid KLibPthreadUid3 = {0x10275065};


#else
_global_data_t glbData;
#endif

_global_data_t::_global_data_t()
    {
    //Initialize global data structure
    start = NULL;
    threadCount = 0;  //First thread
    if (lockThreadTable.CreateLocal() != KErrNone)
        {
        THR_PRINTF("[pthread] FATAL :Mutex create failed");       
        }
    if (globalLockForMutex.CreateLocal() != KErrNone)
        {
        THR_PRINTF("[pthread] FATAL :Mutex create failed");
        }
    
    // Initialize all keys in pthread_key_list to NOT USED
    for (int loopvar = 0; loopvar < PTHREAD_KEYS_MAX; loopvar++)
        {
        // Set it to unused
        pthread_key_list[loopvar].destr = NULL;
        }
    
    for (int loopvar = 0; loopvar < STAT_FLAG_SIZE; loopvar++)
        {
        // Set it to unused
        statusflag[loopvar] = _KEY_UNUSED;
        }
    
    //Sem table initializations
    semStart = NULL;
    if (lockSemTable.CreateLocal() != KErrNone)
        {
        globalLockForMutex.Close();   
        lockThreadTable.Close();        
        THR_PRINTF("[pthread] FATAL :Mutex create failed");
        }
    }

// Destructor to supress warnings
_global_data_t::~_global_data_t()
	{
    globalLockForMutex.Close(); 
    lockThreadTable.Close();
    lockSemTable.Close();
	}
	
_global_data_t* GetGlobals()
    {
#ifdef __WINSCW__
    _global_data_t* glbDataPtr = Pls<_global_data_t>(KLibPthreadUid3);
    return glbDataPtr;
    
#else
    return &glbData;
#endif
    }
