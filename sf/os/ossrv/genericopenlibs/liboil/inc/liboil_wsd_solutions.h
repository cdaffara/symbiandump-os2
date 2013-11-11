/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 


 
#ifndef __LIBOIL_WSD_SOLUTION_H__
#define __LIBOIL_WSD_SOLUTION_H__
#include "liboil_wsd_macro.h"
#include <liboil/liboiltypes.h>
#include <liboil/liboilfunction.h>
#include <stdio.h>



#if EMULATOR
#ifdef __cplusplus
extern "C" 
{
#endif

typedef struct ShutdownClosure ShutdownClosure;

/**
 * This struct represents a function to be called on shutdown.
 */
struct ShutdownClosure
{
  ShutdownClosure *next;     /**< Next ShutdownClosure */
//  DBusShutdownFunction func; /**< Function to call */
  void *data;                /**< Data for function */
};

struct liboil_global_struct
{

//      VARIABLE_DECL(buffers,g,gstcheck,GList*)
        VARIABLE_DECL(_class,g,abs_f32_f32,OilFunctionClass)
      /*END-global vars*/ 
    };

struct liboil_global_struct *liboil_ImpurePtr();
int liboil_Init(struct liboil_global_struct *);

#ifdef __cplusplus
}
#endif

#endif //EMULATOR
#endif //__LIBOIL_WSD_SOLUTION_H__

