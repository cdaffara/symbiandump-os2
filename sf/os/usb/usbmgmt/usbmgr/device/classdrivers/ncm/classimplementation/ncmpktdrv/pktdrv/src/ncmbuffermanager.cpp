/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@internalComponent
*/


#include "ncmbuffermanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ncmbuffermanagerTraces.h"
#endif




// if there are KCongestionCount buffers will notify networking to stop sending
// if there are at least KUnCongestionCount, will notify networking to start sending
const TInt KCongestionCount = 1;
const TInt KUnCongestionCount = 3;



// ======== MEMBER FUNCTIONS ========
//

CNcmBufferManager* CNcmBufferManager::NewL()
    {
    OstTraceFunctionEntry0( CNCMBUFFERMANAGER_NEWL_ENTRY );
    CNcmBufferManager *self = new (ELeave) CNcmBufferManager();
    OstTraceFunctionExit0( CNCMBUFFERMANAGER_NEWL_EXIT );
    return self;
    }

CNcmBufferManager::~CNcmBufferManager()
    {
    OstTraceFunctionEntry1( CNCMBUFFERMANAGER_CNCMBUFFERMANAGER_ENTRY, this );
    iFreeQueue.Reset();
    OstTraceFunctionExit1( CNCMBUFFERMANAGER_CNCMBUFFERMANAGER_EXIT, this );
    }

//
// get a buffer block from manager
//
TInt CNcmBufferManager::GetBuffer(TNcmBuffer& aBuffer)
    {
	OstTraceFunctionEntry1( CNCMBUFFERMANAGER_GETBUFFER_ENTRY, this );
	
    TInt count = iFreeQueue.Count();
    TInt ret = KErrNone;

    if (count == 0)
        {
        OstTraceFunctionExit1( CNCMBUFFERMANAGER_GETBUFFER_EXIT, this );
        return KErrNoBuffer;
        }
    else if (count == KCongestionCount)
        {
        iIsCongestion = ETrue;
        ret = KErrCongestion;
        }
    aBuffer = iFreeQueue[0];
    iFreeQueue.Remove(0);
    OstTraceFunctionExit1( CNCMBUFFERMANAGER_GETBUFFER_EXIT_DUP1, this );
    return ret;
    }

//
// free a buffer block to manager
//
void CNcmBufferManager::FreeBuffer(const TNcmBuffer& aBuffer)
    {
    OstTraceFunctionEntry1( CNCMBUFFERMANAGER_FREEBUFFER_ENTRY, this );
    iFreeQueue.Append(aBuffer);  
    int count = iFreeQueue.Count();
    iFreeQueue[count-1].iLen = 0;
    if (count == KUnCongestionCount)
        {
        iIsCongestion = EFalse;
        }
    OstTraceFunctionExit1( CNCMBUFFERMANAGER_FREEBUFFER_EXIT, this );
    }

//
// set the whole buffer area to create buffer blocks
//
void CNcmBufferManager::InitBufferArea(TAny* aBuf, TInt aLength)
    {
    OstTrace1( TRACE_NORMAL, CNCMBUFFERMANAGER_INITBUFFERAREA, "CNcmBufferManager::InitBufferArea aLength=%d", aLength );    
    iBuf = (TUint8*)aBuf;
    iLen = aLength;
    }

//
// set the size of a buffer block, create buffer blocks on the buffer area
//
TInt CNcmBufferManager::SetBufferCellSize(TInt aSize)
    {
    OstTraceFunctionEntry1( CNCMBUFFERMANAGER_SETBUFFERCELLSIZE_ENTRY, this );

//    buffer block size must be aligned with KAlignSize to make the each buffer start align with KAlignSize 
    iCellSize = (aSize+iAlignSize-1)&~(iAlignSize-1);
    OstTraceExt2( TRACE_NORMAL, CNCMBUFFERMANAGER_SETBUFFERCELLSIZE, "CNcmBufferManager::SetBufferCellSize aSize=%d, iCellSize=%d", aSize, iCellSize );

//   buffer area got from share chunk LDD may not align, make it align to KAlignSize first
    TUint32 buf = (TUint)iBuf;
    buf = (buf+iAlignSize-1)&~(iAlignSize-1);
    TUint8* alignbuf = (TUint8*)buf;
    TUint32 offset = buf - (TUint)iBuf;
    
    int count = (iLen-offset)/iCellSize;
    iFreeQueue.Reset();
    if (count < KUnCongestionCount)
        {        
        OstTrace1( TRACE_FATAL, CNCMBUFFERMANAGER_SETBUFFERCELLSIZE1, "the buffer cell size is too big and create too less buffers %d", count );
        OstTraceFunctionExit1( CNCMBUFFERMANAGER_SETBUFFERCELLSIZE_EXIT, this );
        return KErrUnknown;
        }

    TNcmBuffer ncmbuf;
    for (TInt i=0 ; i<count ; i++)
        {
        ncmbuf.iPtr = alignbuf+i*iCellSize;
        ncmbuf.iLen = 0;
        ncmbuf.iMaxLength = iCellSize;
        iFreeQueue.Append(ncmbuf);
        }
    iIsCongestion = EFalse;
    OstTraceFunctionExit1( CNCMBUFFERMANAGER_SETBUFFERCELLSIZE_EXIT_DUP1, this );
    return KErrNone;
    }

CNcmBufferManager::CNcmBufferManager()
    {
    iIsCongestion = EFalse;
    }

//
// if there is not enough buffer blocks, return ETrue and will start flow control in networking
//

TBool CNcmBufferManager::IsCongestion()
    {
    OstTrace1( TRACE_NORMAL, CNCMBUFFERMANAGER_ISCONGESTION, "CNcmBufferManager::FreeBufferCount %d", iFreeQueue.Count() );
    return iIsCongestion;
    }

TInt CNcmBufferManager::RequiredBufferCount()
    {
    return KUnCongestionCount;
    }


