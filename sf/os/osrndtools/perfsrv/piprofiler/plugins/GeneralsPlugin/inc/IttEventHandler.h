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
* Description:  Event based ITT sampler skeleton copypasted from MemoryEventHandler.h
*
*/
#include "IttSamplerImpl.h"

#ifndef __PI_ITT_EVENT_HANDLER__
#define __PI_ITT_EVENT_HANDLER__

#include <piprofiler/ProfilerGenericClassesKrn.h>

// CONSTANTS
const TInt KITTBufferSize = 256;

class DProfilerSampleBuffer;

/*
 * ITT event handler for listaning kernel events 
 */
class DIttEventHandler : public DKernelEventHandler
    {
public:
    // constructor
    DIttEventHandler(DProfilerSampleBuffer*  aSampleBuffer, TProfilerGppSamplerData* aGppSamplerDataIn);
    TInt Create();
    ~DIttEventHandler();
    TInt Start();
    TInt Stop();
    TBool Tracking() {return iTracking;}
       
    TBool SampleNeeded();
    
private:
    static TUint EventHandler(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aThis);
    TUint HandleEvent(TKernelEvent aType, TAny* a1, TAny* a2);
    // handle code segments
    TBool HandleAddCodeSeg(DCodeSeg* aSeg);
    TBool HandleRemoveCodeSeg(DCodeSeg* aSeg);

private:
    /** Lock serialising calls to event handler */
        DMutex*     iLock;
        TBool       iTracking;
        DProfilerSampleBuffer*  iSampleBuffer;

        TUint32     iCount;        
        
        TUint8      iSample[KITTBufferSize];
        TPtr8       iSampleDescriptor;
        TProfilerGppSamplerData*     gppSamplerData;
    };

#endif  //__PI_ITT_EVENT_HANDLER__
