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

#ifndef MEMSPYTHREADDATA_H
#define MEMSPYTHREADDATA_H

//#include <kernel/arm/arm_types.h>

class TMemSpyThreadData {
public:
	TMemSpyThreadData()
	: iId(0), iPid(0), iSID(0), iVID(0), iRequestCount(0), iThreadHandles(0), iProcessHandles(0), iThreadNumberUsing(0), 
	  iProcessNumberUsing(0), iAttributes(0), iCpuUse(0)
		{
		}
	
public:
    TThreadId iId;
    TProcessId iPid;    
    TFullName iName;    //name without formatting. Formatting to be moved to the Avkon UI part
    TInt iSID;
    TInt iVID;
    TThreadPriority iThreadPriority;
    TProcessPriority iProcessPriority;
    TInt iRequestCount;
    TInt iThreadHandles;
    TInt iProcessHandles;
    TInt iThreadNumberUsing;
    TInt iProcessNumberUsing;
    TInt iAttributes;
    TInt iCpuUse;        
    TExitType iExitType;
    RArray<TUint> iRegisters; //containts register contents of the thread (R00-R12, SP, LR, PC, Flags & DACR) - TUint for formating to HEX    
};

#endif
