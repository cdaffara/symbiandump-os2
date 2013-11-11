// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Ost Performance Tests stack info routine caller
// containing the definition of the functions contained in te_stackinfo.s
//



/**
 @file te_perfstackinfo.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTSTACKINFO_H
#define TE_UPTSTACKINFO_H

#include <e32def.h>


#ifndef __WINSCW__ 
//only call stack info if on hardware as it doesn't make sense on emulator

//include the assembler methods
#include <e32cmn.h>
extern "C"
{	  
  TUint32 Te_Fill_Remaining_Stack(const TUint32 aStackBase); //passes into r0
  TUint32 Te_Get_Stack_Size(const TUint32 aStackBase);
}


//kernel side implementation only
#ifdef __KERNEL_MODE__ 
#include <kern_priv.h>
#include <platform.h>


#define __MARK_INITIAL_STACK(aDoStack, aStackBase, aBefore) \
{ \
if(aDoStack) \
{ \
aStackBase = (TUint32)((DThread&)Kern::CurrentThread()).iSupervisorStack; \
aBefore = Te_Fill_Remaining_Stack(aStackBase); \
} \
}

//user side implementation only
#else 
#include <e32std.h>

//user side implementation
#define __MARK_INITIAL_STACK(aDoStack, aStackBase, aBefore) \
{ \
if(aDoStack) \
{ \
TThreadStackInfo myStackInfo; \
RThread().StackInfo(myStackInfo); \
aStackBase = myStackInfo.iLimit; \
aBefore = Te_Fill_Remaining_Stack(aStackBase); \
} \
}

#endif

//User and kernel side implementation, on hardware only
#define __RETURN_STACK_SIZE(aDoStack, aStackBase, aBefore, aApiStackUsage) \
{\
if(aDoStack)\
{\
aApiStackUsage = aBefore - Te_Get_Stack_Size(aStackBase);\
}\
}

#else //emulator implementation only

//empty calls for both user and kernel side for emulator
#define __MARK_INITIAL_STACK(aDoStack, aStackBase, aBefore)
#define __RETURN_STACK_SIZE(aDoStack, aStackBase, aBefore, aApiStackUsage)

#endif  // __WINSCW__


#endif // TE_UPTSTACKINFO_H


