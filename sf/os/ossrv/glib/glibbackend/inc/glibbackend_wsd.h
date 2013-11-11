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


 

#ifndef __GLIBBACKEND_WSD_H__
#define __GLIBBACKEND_WSD_H__

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include <pthread.h> 
#include <e32cmn.h> 

/* ---------------------- MACRO DEFINATIONS ---------------------------*/
#ifndef EMULATOR
#define EMULATOR (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#endif /* EMULATOR */

#define VARIABLE_NAME(var,filename) s_##filename##_##var

#define FUNCTION_NAME(var,filename) _s_##filename##_##var 

#define VARIABLE_DECL(var,filename,datatype)\
datatype VARIABLE_NAME(var,filename);

#define RETURN_VAL(var,filename) (Glibbackend_ImpurePtr()->VARIABLE_NAME(var,filename))

#define PLS(var,filename,type) \
type *FUNCTION_NAME(var,filename)() \
{ \
return &RETURN_VAL(var,filename);\
}
/* ---------------------- END MACRO DEFINATIONS ---------------------------*/
 
struct global_struct
{
	VARIABLE_DECL(key,lowmem,pthread_key_t)
	VARIABLE_DECL(key_once,lowmem,pthread_once_t)
	RHeap* 			_iPrivateHeap;
};

struct global_struct *Glibbackend_ImpurePtr();
 
#endif  
#endif /* __GLIBBACKEND_WSD_H__ */
