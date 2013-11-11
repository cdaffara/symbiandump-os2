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
* Name     : pthreadtypes.h
* Part of  : PThread library
* Exported thread data structures 
* Version:
*
*/



#ifndef PTHREADTYPES_H
#define PTHREADTYPES_H

// size of pthread_attr
#define _TOTALSIZEOF_PTHREAD_ATTR_T 32

typedef unsigned int pthread_t;

typedef struct
{
  char totalSize[_TOTALSIZEOF_PTHREAD_ATTR_T];
} pthread_attr_t;

/* Keys for thread-specific data */
typedef unsigned int pthread_key_t;

#endif //PTHREADTYPES_H
