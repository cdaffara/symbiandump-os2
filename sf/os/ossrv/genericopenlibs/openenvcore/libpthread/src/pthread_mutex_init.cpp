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
// Name        : pthread_mutex_init.cpp
// Part of     : pthread
// Interface   : POSIX, pthread
// POSIX implementation of mutexes on Symbian
// Version     :
//



#include "mutextypes.h"

extern int _mutexCreate (pthread_mutex_t * mutex, 
                  const pthread_mutexattr_t* attr);
EXPORT_C int pthread_mutex_init (pthread_mutex_t * mutex, 
                                 const pthread_mutexattr_t* attr)
{

    pthread_mutexattr_t defaultattributes = 
                           PTHREAD_MUTEX_ATTRIBUTE_DEFAULT_INITIALIZER;
    if(!attr)
    {
        attr = &defaultattributes;
    }

	//coverity[var_compare_op]  
    if( !mutex || !VALID_MUTEX_ATTR(attr) || mutex->iState == _EInitialized)
    {
        return EINVAL;
    }
  
    //recursively call pthread_mutex_init under _InitOnce harness to 
    //diambiguate race to create this mutex if this is a statically 
    //intialiazied mutex ignore passed pthread_mutexattr_t attribute 
    //in this case

	//coverity[var_deref_op]
  
    if( (mutex->iState == _ENeedsNormalInit    || 
         mutex->iState == _ENeedsRecursiveInit || 
         mutex->iState == _ENeedsErrorCheckInit   )  && 
         mutex->iReentry != 0xBABA)
    { 
        if( _InitOnce(mutex) )
        {
            return 0;
        }
    }
    
    return _mutexCreate(mutex,attr);
}

// End of File
