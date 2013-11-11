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
#include "MemSamplerImpl.h"

#ifndef __PI_MEMORY_EVENT_HANDLER__
#define __PI_MEMORY_EVENT_HANDLER__

#include <piprofiler/ProfilerGenericClassesKrn.h>

// CONSTANTS
const TInt KEventBufferSize = 257;

class DProfilerSampleBuffer;

class DMemoryEventHandler : public DKernelEventHandler
    {
public:
    // constructor
    DMemoryEventHandler(DProfilerSampleBuffer*  aSampleBuffer);
    TInt Create();
    ~DMemoryEventHandler();
    TInt Start();
    TInt Stop();
    TBool Tracking() {return iTracking;}
       
    TBool SampleNeeded();
    
private:
    static TUint EventHandler(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aThis);
    TUint HandleEvent(TKernelEvent aType, TAny* a1, TAny* a2);
    // handle chunk activity
    TBool HandleAddChunk(DChunk* aChunk);
    TBool HandleUpdateChunk(DChunk* aChunk);
    TBool HandleDeleteChunk(DChunk* aChunk);
    // handle chunk activity
    TBool HandleAddThread(DThread* aThread);
    TBool HandleUpdateThread(DThread* aThread);
    TBool HandleDeleteThread(DThread* aThread);
    // handle chunk activity
    TBool HandleAddProcess(DProcess* aProcess);
    TBool HandleUpdateProcess(DProcess* aProcess);
    TBool HandleDeleteProcess(DProcess* aProcess);
    // handle library activity
    TBool HandleAddLibrary(DLibrary* aLibrary, DThread* aThread);
    TBool HandleDeleteLibrary(DLibrary* aLibrary);
    
    // data handling
    TInt AddHeader();
    TInt AddFooter();
    TInt EncodeTotalMemory();
    TInt EncodeNameCode();
    TInt EncodeNewCode();
    TInt EncodeUpdateCode();
    TInt EncodeRemoveCode();
    TInt EncodeChunkName(DThread& t);
    TInt EncodeChunkName(DChunk& c);
    TInt EncodeChunkName(DLibrary& l);
    TInt EncodeChunkData(DThread& t);
    TInt EncodeChunkData(DChunk& c);
    TInt EncodeChunkData(DLibrary& l, DThread& t);
private:
    /** Lock serialising calls to event handler */
    DMutex* iLock;
    TBool iTracking;

    DProfilerSampleBuffer*  iSampleBuffer;
    TInt iCounters[EEventLimit];
    
    TUint32     iCount;
    
    TUint8      iSample[KEventBufferSize];
    TPtr8       iSampleDescriptor;
    
    TUint32     iPreviousCount;
    };

#endif
