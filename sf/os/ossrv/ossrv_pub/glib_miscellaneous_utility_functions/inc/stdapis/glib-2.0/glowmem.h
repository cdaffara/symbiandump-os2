/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __G_LOW_MEM_H__
#define __G_LOW_MEM_H__
 
#include <unistd.h> 
#include <setjmp.h>

/*-------------------------- Data Structure Decleration ----------------*/
typedef struct _cleanUpStack cleanUpStack ;
typedef struct _mem_info mem_info ;

struct _cleanUpStack
{
	void *ptr[1000];
	int top;
};

struct _mem_info
{
	int is_setjmp_called;
	jmp_buf buf;
	cleanUpStack stack;
};

 
/*--------------------Function declerations------------------*/ 
#ifdef __cplusplus
extern "C"
{
#endif    /* __cplusplus */

IMPORT_C mem_info * _get_thread_specific_data();
IMPORT_C int _set_thread_specific_data(mem_info *m);
IMPORT_C int _push(cleanUpStack *cs,void *ptr);
IMPORT_C void *_pop(cleanUpStack *cs);
IMPORT_C void _findAndDestroy(cleanUpStack *cs,void *ptr);
IMPORT_C void _destroCleanUpStack(cleanUpStack *cs);
IMPORT_C void _clearCleanUpStack(cleanUpStack *cs);
IMPORT_C void * _pAlloc(size_t size);
IMPORT_C void _dummy1();
IMPORT_C void _dummy2();
IMPORT_C void _dummy3();

#ifdef __cplusplus
}
#endif    /* __cplusplus */

/*--------------------MACRO declerations------------------*/

#define SET_LOW_MEMORY_TRAP_VOID()	\
gboolean did_i_set = FALSE;\
{\
	mem_info *m = _get_thread_specific_data();\
	if(m == NULL)\
	{\
		m = (mem_info *)_pAlloc(sizeof(mem_info));\
		if(!m)\
			return;\
		m->is_setjmp_called = FALSE;\
		_clearCleanUpStack(&(m->stack));\
		if(_set_thread_specific_data(m))\
			return;\
	}\
	if(!m->is_setjmp_called)\
	{\
  		if(setjmp(m->buf) > 0)\
  		{\
  			m->is_setjmp_called = FALSE;\
  			_destroCleanUpStack(&(m->stack));\
  			return ;\
  		}\
  		m->is_setjmp_called = TRUE;\
  		did_i_set = TRUE;\
  	}\
}

#define SET_LOW_MEMORY_TRAP(failure_value)	\
gboolean did_i_set = FALSE;\
{\
	mem_info *m = _get_thread_specific_data();\
	if(m == NULL)\
	{\
		m = (mem_info *)_pAlloc(sizeof(mem_info));\
		if(!m)\
			return failure_value;\
		m->is_setjmp_called = FALSE;\
		_clearCleanUpStack(&(m->stack));\
		if(_set_thread_specific_data(m))\
			return failure_value;\
	}\
	if(!m->is_setjmp_called)\
	{\
  		if(setjmp(m->buf) > 0)\
  		{\
  			m->is_setjmp_called = FALSE;\
  			_destroCleanUpStack(&(m->stack));\
  			return failure_value;\
  		}\
  		m->is_setjmp_called = TRUE;\
  		did_i_set = TRUE;\
  	}\
}


#define REMOVE_LOW_MEMORY_TRAP()	{\
if(did_i_set)\
	{\
	mem_info *m = _get_thread_specific_data();\
	if(m)\
		m->is_setjmp_called = FALSE;\
		_clearCleanUpStack(&(m->stack));\
	}\
}

#endif /* __G_LOW_MEM_H__ */