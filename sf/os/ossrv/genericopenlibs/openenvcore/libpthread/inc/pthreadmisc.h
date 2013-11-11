/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name     : pthreadmisc.h
* Part of  : PThread library
* Thread Miscellaneous functions prototypes.
* Version:
*
*/



#ifndef _PTHREADMISC_H
#define _PTHREADMISC_H

#define THREAD_NAME_WIDTH 8
#define THREAD_COUNT_ZERO 0
#define MAX_THREAD_CREATE_FAILURE_COUNT 20
#define MAX_THREAD_NAME_LEN 255
#define MAXLEN_RANDOM_NUM 20

void* _pthread_getTls() ;

void _pthread_deleteNode(_pthread_node_t *selfNodePtr, 
                           _global_data_t *glbPtr,
                           void ** retValPtr);
                           
void _pthread_destroyKeys(_global_data_t *glbPtr,
                            _pthread_node_t *selfNodePtr);

#endif //_PTHREADMISC_H

//End of File
