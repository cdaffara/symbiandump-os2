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
* Name     : threadcreate.h
* Part of  : PThread library
* Data structure needed for thread creatation.
* Version:
*
*/



#ifndef THREADCREATE_H
#define THREADCREATE_H

#include <e32std.h>
#include <sched.h>
#include "sysif.h"

#define MIN_RR_PRIORITY 0
#define MAX_RR_PRIORITY 255

// On any addition or deletion to this pthread_attr structure, update 
// _TOTALSIZEOF_PTHREAD_ATTR_T 
typedef struct 
{
  /* Scheduler parameters; Only priority.  */
  unsigned int policy;
  unsigned int stackSize;
  unsigned int detachState;
  unsigned int scope;
  struct sched_param sp;
  int priority;
  unsigned int reserved1;
  unsigned int reserved2;
}_pthread_attr;

typedef void *(*_START_ROUTINE) (void *);
typedef int (*_START_ROUTINE_SOS) (void *);

#ifdef __X86GCC__
// MinGW GCC compiler does not like typedef struct definitions with no tag
typedef struct _wrapperFunArgs_tag
#else
typedef struct 
#endif // __X86GCC__
{
    _START_ROUTINE begin_routine;
    void *args;
    void *nodePtr;
	/*******************************************************************
	Overloading new and delete operators so that they will
	allocate and deallocare memory from/to the private heap of backend
	********************************************************************/
	inline TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW
		{
		Mem::FillZ(aBase, aSize); return aBase;
		}
		
	inline TAny* operator new(TUint aSize) __NO_THROW
		{
		return Backend()->Alloc(aSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave)
		{
		TAny* ptr = Backend()->Alloc(aSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
		
	inline TAny* operator new(TUint aSize, TUint aExtraSize) __NO_THROW
		{
		return Backend()->Alloc(aSize + aExtraSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave, TUint aExtraSize)
		{
		TAny* ptr = Backend()->Alloc(aSize + aExtraSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
	
	inline void operator delete(TAny *aPtr) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
}_wrapperFunArgs;



#endif //THREADCREATE_H

//End of File
