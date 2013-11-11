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
* Description:  Event based ITT sampler skeleton copypasted from MemoryEventHandler.cpp
*
*/

#include <e32def.h>
#include <e32cmn.h>
#include <arm.h>
#include <kernel.h>
#include <kern_priv.h>
#include <nk_trace.h>

#include "IttEventHandler.h"

/*
 * Constructor
 * 
 * @param DProfilerSampleBuffer*    pointer to sample buffer
 */
DIttEventHandler::DIttEventHandler(DProfilerSampleBuffer* aSampleBuffer, TProfilerGppSamplerData* aGppSamplerDataIn)
    :   DKernelEventHandler(EventHandler, this),
        iSampleBuffer(aSampleBuffer),
        iSampleDescriptor(&(this->iSample[1]),0,KITTBufferSize),
        gppSamplerData(aGppSamplerDataIn)
    {
    //Kern::Printf("DIttEventHandler::DIttEventHandler()");

    }

/*
 * DIttEventHandler::Create()
 */
TInt DIttEventHandler::Create()
    {
    Kern::Printf("DIttEventHandler::Create()");

    TInt err(Kern::MutexCreate(iLock, _L("IttEventHandlerLock"), KMutexOrdDebug));
    if (err != KErrNone)
        return err;
    
    return Add();
    }

/*
 * Destructor
 */
DIttEventHandler::~DIttEventHandler()
    {
    //Kern::Printf("DIttEventHandler::~DIttEventHandler()");

    if (iLock)
        iLock->Close(NULL);
    }


TInt DIttEventHandler::Start()
    {
    //Kern::Printf("DIttEventHandler::Start()");

    iTracking = ETrue;
    return KErrNone;
    }


TInt DIttEventHandler::Stop()
    {
    //Kern::Printf("DIttEventHandler::Stop()");

    iTracking = EFalse;
    return KErrNone;
    }

TBool DIttEventHandler::SampleNeeded()
    {
    LOGTEXT("DIttEventHandler::SampleNeeded()");
    
    // increase the counter by one on each round
    iCount++;
    
    // check if event handler was not running
    if(!iTracking)
       return false;
    
    return true;
    }


TUint DIttEventHandler::EventHandler(TKernelEvent aType, TAny* a1, TAny* a2, TAny* aThis)
    {
    //Kern::Printf("DIttEventHandler::EventHandler()");
    return ((DIttEventHandler*)aThis)->HandleEvent(aType, a1, a2);
    }



TUint DIttEventHandler::HandleEvent(TKernelEvent aType, TAny* a1, TAny* a2)
    {
    //Kern::Printf("DIttEventHandler::HandleEvent()");
    //Kern::Printf("New kernel event received, %d", aType);
    
    if (iTracking/* && iCount != iPreviousCount*/)
        {
        switch (aType)
            {
            
            case EEventAddCodeSeg:
                //Kern::Printf("DCodeSeg added: 0x%08x", (DCodeSeg*)a1);
                HandleAddCodeSeg((DCodeSeg*)a1);
                break;
                
            case EEventRemoveCodeSeg:
                //Kern::Printf("DCodeSeg deleted: 0x%08x", (DCodeSeg*)a1);
                HandleRemoveCodeSeg((DCodeSeg*)a1);
                break;
   
            default:
                break;
            }
        }
//    else if(iTracking && iCount == iPreviousCount)
//        {
//        // if time stamp is not updated profiling has stopped
//        Stop();
//        }
    return DKernelEventHandler::ERunNext;
    }

/*
 * 
 */
TBool DIttEventHandler::HandleAddCodeSeg(DCodeSeg* aSeg)
    {    
    iSampleDescriptor.Zero();
    //Kern::Printf("DLL ADD: NM %S : RA:0x%x SZ:0x%x SN:0x%x",aSeg->iFileName,aSeg->iRunAddress,aSeg->iSize, this->gppSamplerData->sampleNumber);

    iSample[0] = aSeg->iFileName->Length();
    iSampleDescriptor.Append(*aSeg->iFileName);
    iSampleDescriptor.Append((TUint8*)&(aSeg->iRunAddress),4);
    iSampleDescriptor.Append((TUint8*)&(aSeg->iSize),4);
    iSampleDescriptor.Append((TUint8*)&(this->gppSamplerData->sampleNumber),4);
    iSample[0] = iSampleDescriptor.Size();
    
    iSampleBuffer->AddSample(iSample,iSampleDescriptor.Size()+1);
    return ETrue;
    }

TBool DIttEventHandler::HandleRemoveCodeSeg(DCodeSeg* aSeg)
    {
    iSampleDescriptor.Zero();
    //Kern::Printf("DLL REM: NM %S : RA:0x%x SZ:0x%x SN:0x%x",aSeg->iFileName,aSeg->iRunAddress,aSeg->iSize, this->gppSamplerData->sampleNumber);

    iSample[0] = aSeg->iFileName->Length();
    iSampleDescriptor.Append(*aSeg->iFileName);
    iSampleDescriptor.Append((TUint8*)&(aSeg->iRunAddress),4);
    iSampleDescriptor.Append((TUint8*)&(aSeg->iSize),4);
    iSampleDescriptor.Append((TUint8*)&(this->gppSamplerData->sampleNumber),4);
    iSample[0] = iSampleDescriptor.Size();
    
    iSampleBuffer->AddSample(iSample,iSampleDescriptor.Size()+1);
    return ETrue;
    }
// end of file
