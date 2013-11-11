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

#ifndef MEMSPYDRIVERLOGICALCHANHEAPDATAKERNEL_H
#define MEMSPYDRIVERLOGICALCHANHEAPDATAKERNEL_H

// User includes
#include "MemSpyDriverLogChanHeapDataBase.h"

NONSHARABLE_CLASS(DMemSpyDriverLogChanHeapDataKernel) : public DMemSpyDriverLogChanHeapDataBase
	{
public:    
    DMemSpyDriverLogChanHeapDataKernel( DMemSpyDriverDevice& aDevice, DThread& aThread );
    
private: // from DMemSpyDriverLogChanBase
	TInt Request( TInt aFunction, TAny* a1, TAny* a2 );
    TBool IsHandler( TInt aFunction ) const;

private: // Channel operation handlers
    TInt GetInfoData( TMemSpyDriverInternalHeapRequestParameters* aParams );
    TInt GetIsDebugKernel(TAny* aIsDebugKernel);
    TInt MakeKernelHeapCopy();
    void FreeKernelHeapCopy();

private: // Internal methods
    TInt GetInfoData(RMemSpyDriverRHeapBase& aHeap);
    TInt OpenKernelHeap( RMemSpyDriverRHeapKernelFromCopy& aHeap );   
    TInt GetHeapDataUser( TMemSpyDriverInternalHeapDataParams& aParams );
    TInt GetFullData( TMemSpyDriverInternalHeapDataParams& aParams );

private: // Data members	
	RMemSpyDriverRHeapKernelFromCopy iKernelHeap;
	};


#endif
