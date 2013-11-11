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


#ifndef PROFILER_MEM_SAMPLER_H
#define PROFILER_MEM_SAMPLER_H

#include "GeneralsConfig.h"

#include <kern_priv.h>

#include <piprofiler/ProfilerGenericClassesKrn.h>
#include <piprofiler/ProfilerTraces.h>
#include "GppSamplerImpl.h"
#include "MemoryEventHandler.h"
#include <e32btrace.h>


// constants
// defines the maximum thread amount that is assumed to
// be possible with MEM trace
const TInt KSampleBufferSize = 257;
const TInt KProfilerMaxThreadsAmount = 512;
const TInt KProfilerMaxChunksAmount = 1024;
const TInt KProfilerMaxLibrariesAmount = 1024;
const TInt KProfilerTotalMemorySamplePeriod = 100;

// flags
#define MEM_EVENT_HANDLER
//#define MEM_EVENT_HANDLER_LIBRARY_EVENTS

/*
 *	
 *	MEM sampler definition
 *	
 */

class DMemoryEventHandler;

class DMemSamplerImpl //: public DBase
{
public:
	enum EProcessingState
	{
		EStartingToProcess,
		EProcessingNames,
		EProcessingData,
		ENothingToProcess
	};
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
	enum ESampleType
	{
	    ESampleChunks,
	    ESampleThreads,
	    ESampleLibraries
	};
#endif

	DMemSamplerImpl();
	~DMemSamplerImpl();

	TInt	CreateFirstSample();
	TInt	SampleImpl();
	TBool	SampleNeeded();
	void	Reset();
	TInt	ProcessChunks();
	TInt    ProcessThreads();
    TInt    GatherChunks();
    TInt    GatherThreads();

#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
    TInt    GatherLibraries();
    TInt    ProcessLibraries();
#endif
    
	TInt	EncodeChunkData(DChunk& chunk);
	TInt	EncodeChunkName(DChunk& chunk);
	TInt	EncodeChunkData(DThread& thread);
	TInt	EncodeChunkName(DThread& thread);
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
	TInt    EncodeChunkData(DLibrary& library);
	TInt    EncodeChunkName(DLibrary& library);
#endif

	TInt 	EncodeTotalMemoryName();
	TInt 	EncodeTotalMemory();
	
	TInt	EncodeNameCode();
	TInt	EncodeDataCode();

	DChunk*		heapChunksToSample[KProfilerMaxChunksAmount];
	DChunk*		heapChunkNamesToReport[KProfilerMaxChunksAmount];
	TInt		iCount;
	TInt		iChunkCount;
	TInt		iNewChunkCount;
	TBuf8<0x50> name;
	DThread*	threadsToSample[KProfilerMaxThreadsAmount];
	DThread*	threadNamesToReport[KProfilerMaxThreadsAmount];
	TInt		iThreadCount;
	TInt		iNewThreadCount;
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
	DLibrary*   librariesToSample[KProfilerMaxLibrariesAmount];
	DLibrary*   libraryNamesToReport[KProfilerMaxLibrariesAmount];
	TInt        iLibraryCount;
	TInt        iNewLibraryCount;
	TInt        iLibrariesProcessing;
#endif

	TInt		iChunksProcessing;
    TInt        iThreadsProcessing;
	TInt		iMemSamplingPeriod;
	TInt		iMemSamplingPeriodDiv2;
	TInt        iMemSamplingPeriodDiv3;
	
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
	ESampleType iSampleType;
#else
	TBool		iSampleThreads;
#endif	
	
	TBool       iTimeToSample;
	
	TBool 		iTotalMemoryOk;
	TBool		iTotalMemoryNameOk;

	TUint8		sample[KSampleBufferSize];
	TPtr8		sampleDescriptor;
	
	// test
#ifdef MEM_EVENT_HANDLER
//	DMemoryEventHandler*   iEventHandler;
	TBool      iChunksGathered;
	TBool      iThreadsGathered;
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
	TBool      iLibrariesGathered;
#endif
#endif
};


template <int BufferSize>
class DProfilerMemSampler : public DProfilerGenericSampler<BufferSize>
{
public:
	DProfilerMemSampler(struct TProfilerGppSamplerData*, TInt id);
	~DProfilerMemSampler();

	void	Sample();
	TInt	Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset);
	TInt	PostSample();
	TBool	PostSampleNeeded();
	
private:
#ifdef MEM_EVENT_HANDLER
    DMemoryEventHandler*               iEventHandler;
#endif
	DMemSamplerImpl			           memSamplerImpl;
	struct TProfilerGppSamplerData*    gppSamplerData;
	TBool                              sampleNeeded;
};

/*
 *	
 *	MEM sampler implementation
 *	
 */

template <int BufferSize>
DProfilerMemSampler<BufferSize>::DProfilerMemSampler(struct TProfilerGppSamplerData* gppSamplerDataIn, TInt id) :
	DProfilerGenericSampler<BufferSize>(PROFILER_MEM_SAMPLER_ID)
    {
    LOGSTRING2("DProfilerMemSampler<%d>::CProfilerMemSampler",BufferSize);
	this->gppSamplerData = gppSamplerDataIn;
	this->iSamplingPeriod = 3000;	// set default setting
    }

template <int BufferSize>
TInt DProfilerMemSampler<BufferSize>::Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset)
    {
//#ifdef MEM_EVENT_HANDLER
//    Kern::Printf("DProfilerMemSampler<%d>::Reset - calling superclass reset",BufferSize);
    
//#endif
    // check if reset called in stop (by driver)
    if(aSyncOffset != 999999)
        {
        DProfilerGenericSampler<BufferSize>::Reset(aStream);
        memSamplerImpl.Reset();

#ifdef MEM_EVENT_HANDLER
        // reset member variables
        this->memSamplerImpl.iThreadsGathered = false;
        this->memSamplerImpl.iChunksGathered = false;
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
        this->memSamplerImpl.iLibrariesGathered = false;
#endif
        
        // memory event handler
        if(iEventHandler)
            {
            // stop previous sampling if still running
//            Kern::Printf("Stopping DMemoryEventHandler");
            iEventHandler->Stop();
            iEventHandler->Close();
            iEventHandler = NULL;
            }
    
//        Kern::Printf("Initiating DMemoryEventHandler");
        iEventHandler = new DMemoryEventHandler(this->iSampleBuffer);
        if(iEventHandler)
            {
//            Kern::Printf("Creating DMemoryEventHandler");
            TInt err(iEventHandler->Create());
            if(err != KErrNone)
                {
                Kern::Printf("Error in creation of DMemoryEventHandler, error %d", err);
                return err;
                }
            }
        else
            {
            Kern::Printf("Could not initiate DMemoryEventHandler");
            return KErrGeneral;
            }
    
        // set first chunk&thread memory lookup at the 5 ms, should be enough
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
        this->memSamplerImpl.iMemSamplingPeriod = 45;
#else
        this->memSamplerImpl.iMemSamplingPeriod = 10;
#endif
        
#else
        this->memSamplerImpl.iMemSamplingPeriod = this->iSamplingPeriod;
#endif
        this->memSamplerImpl.iMemSamplingPeriodDiv2 = (TInt)(this->memSamplerImpl.iMemSamplingPeriod / 2);
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
        this->memSamplerImpl.iMemSamplingPeriodDiv3 = (TInt)(this->memSamplerImpl.iMemSamplingPeriod / 3);
#endif
	
        LOGSTRING3("CProfilerMemSampler<%d>::Reset - set mem sampling period to %d",
                                BufferSize,this->memSamplerImpl.iMemSamplingPeriod);
        }
	else
        {
        LOGSTRING2("DProfilerMemSampler<%d>::Reset - reset in stop", BufferSize);
#ifdef MEM_EVENT_HANDLER
        // destroy memory event handler
        if(iEventHandler)
            {
            // stop previous sampling if still running
//            Kern::Printf("Stopping DMemoryEventHandler");
            iEventHandler->Stop();
            iEventHandler->Close();
            iEventHandler = NULL;
            }
#endif
        return KErrNone;    // return if reset called in stop
        }

	// add MEM sample header
	TInt length(memSamplerImpl.CreateFirstSample());
	this->iSampleBuffer->AddSample(memSamplerImpl.sample,length);
	
	this->sampleNeeded = false;
	LOGSTRING("DProfilerMemSampler::Reset - exit");
	return KErrNone;
    }

template <int BufferSize> 
TInt DProfilerMemSampler<BufferSize>::PostSample()
    {
    this->sampleNeeded = false;

    LOGSTRING3("DProfilerMemSampler<%d>::PostSample - state %d",BufferSize,this->iSampleBuffer->GetBufferStatus());
    
#ifdef MEM_EVENT_HANDLER
    // check if all threads and chunks (and libraries) are gathered
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
    if(!memSamplerImpl.iThreadsGathered || !memSamplerImpl.iChunksGathered || !memSamplerImpl.iLibrariesGathered)
#else
    if(!memSamplerImpl.iThreadsGathered || !memSamplerImpl.iChunksGathered)
#endif
        {
#endif
        // disable interrupts for checking the kernel containers (EChunk, EThread)
//        TInt interruptLevel(NKern::DisableInterrupts(0));
        
        // first collect chunk data
        TInt length(this->memSamplerImpl.SampleImpl());
        if(length != 0)
            {
            // then, encode the sample to the buffer until no further data available
            while(length > 0)
                {
                this->iSampleBuffer->AddSample(memSamplerImpl.sample,length);
                length = this->memSamplerImpl.SampleImpl();
                
                // indicate that the whole MEM sample ends by having a 0x00 in the end
                if(length == 0)
                    {
                    TUint8 number(0);
                    LOGSTRING("MEM sampler PostSample - all samples generated!");
                    
                    this->iSampleBuffer->AddSample(&number,1);
                    LOGSTRING2("MEM sampler PostSample - end mark added, time: %d", gppSamplerData->sampleNumber);
                    }
                } 
            }
        // restore interrupts and continue normal execution
//        NKern::RestoreInterrupts(interruptLevel);
#ifdef MEM_EVENT_HANDLER
    }
    // check if all threads and chunks are gathered
#ifdef MEM_EVENT_HANDLER_LIBRARY_EVENTS
    if(memSamplerImpl.iThreadsGathered && memSamplerImpl.iChunksGathered && memSamplerImpl.iLibrariesGathered)
#else
    if(memSamplerImpl.iThreadsGathered && memSamplerImpl.iChunksGathered)
#endif
        {
        // start memory event tracking after checking the current memory status
        if(!iEventHandler->Tracking())
            {
            iEventHandler->Start();
//            Kern::Printf("DProfilerMemSampler<%d>::PostSample - memory event handler started",BufferSize);
            }
        
        }
#endif
    
    LOGSTRING2("MEM sampler PostSample - finished sampling, time: %d", gppSamplerData->sampleNumber);
    
    // finally perform superclass postsample
	TInt i(this->DProfilerGenericSampler<BufferSize>::PostSample());
	return i;
    }

template <int BufferSize> 
TBool DProfilerMemSampler<BufferSize>::PostSampleNeeded()
    {
	LOGSTRING3("DProfilerMemSampler<%d>::PostSampleNeeded - state %d",BufferSize,this->iSampleBuffer->GetBufferStatus());

	TUint32 status(this->iSampleBuffer->iBufferStatus);

	if(status == DProfilerSampleBuffer::BufferCopyAsap || status == DProfilerSampleBuffer::BufferFull || this->sampleNeeded == true)
	    {
		return true;
	    }
	
	return false;
    }

template <int BufferSize>
void DProfilerMemSampler<BufferSize>::Sample()
    {
    LOGSTRING2("DProfilerMemSampler<%d>::Sample",BufferSize);	

    // check if sample is needed, i.e. the sampling interval is met
	if(memSamplerImpl.SampleNeeded()) 
	    {
        // set the flag for post sampling
		this->sampleNeeded = true;

		// start the MEM sample with the sample time
		TUint8 number(4);    // mem sampler id
		this->iSampleBuffer->AddSample(&number,1);
		this->iSampleBuffer->AddSample((TUint8*)&(gppSamplerData->sampleNumber),4);

		// leave the rest of the processing for PostSample()
	    }	
	
#ifdef MEM_EVENT_HANDLER
	// call this to increase the time stamp
	else if(iEventHandler->SampleNeeded())
	    {
        // set the flag for post sampling
        this->sampleNeeded = true;
	    }
	
//	// check if time stamp is divibable with 
//	if((memSamplerImpl.iCount % KProfilerTotalMemorySamplePeriod) == 0 && 
//	        memSamplerImpl.iCount > 0)
//	    {
//        // sample total memory once per 100 ms 
//        memSamplerImpl.EncodeTotalMemory();
//
//        // add end mark
//        TUint8 number(0);
//        this->iSampleBuffer->AddSample(&number,1);
//	    }
#endif
	
	LOGSTRING2("CProfilerMemSampler<%d>::Sample",BufferSize);
	return;
    }

template <int BufferSize>
DProfilerMemSampler<BufferSize>::~DProfilerMemSampler()
    {
	LOGSTRING2("CProfilerMemSampler<%d>::~CProfilerMemSampler",BufferSize);		
#ifdef MEM_EVENT_HANDLER
    // memory event handler
     if(iEventHandler)
         {
         // stop previous sampling if still running
//         Kern::Printf("Stopping DMemoryEventHandler");
         iEventHandler->Stop();
         iEventHandler->Close();
         iEventHandler = NULL;
         }
#endif
    }
#endif
