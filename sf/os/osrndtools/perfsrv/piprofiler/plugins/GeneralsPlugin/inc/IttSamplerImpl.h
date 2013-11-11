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

#ifndef PROFILER_ITT_SAMPLER_H
#define PROFILER_ITT_SAMPLER_H

#include "GeneralsConfig.h"

#include <kern_priv.h>

#include <piprofiler/ProfilerGenericClassesKrn.h>
#include <piprofiler/ProfilerTraces.h>
#include "GppSamplerImpl.h"
#include "IttEventHandler.h"

// CONSTANTS
const TInt KITTSampleBufferSize = 256;

// flags
#define ITT_EVENT_HANDLER   // enable event based ITT sampling

/*
 *	
 *	ITT sampler definition
 *	
 */
class DIttEventHandler;

/*
 * User side ITT sampler
 */
class IttSamplerImpl
{
public:
	IttSamplerImpl();
	~IttSamplerImpl();

	TInt	SampleImpl(TUint32 pc,TUint32 sampleNum);
	TBool	SampleNeeded(TUint32 sampleNum);
	TInt 	CreateFirstSample();
	void	Reset();
	TInt    ProcessEvent();
	
	TUint8*         itt_sample;
	TInt            iIttSamplingPeriod;
	TInt            iIttSamplingPeriodDiv2;
	TBool           iTimeToSample;
#ifdef ITT_EVENT_HANDLER
    TBool           iEventReceived;
    TBool           iFirstSampleTaken;
#endif
	
private:
#ifdef ITT_EVENT_HANDLER    
    TInt            iCount;
#endif
    TInt            currentLibCount;
    TInt            currentProcCount;
    
	TUint8          sample[KITTSampleBufferSize ];
	TPtr8           sampleDescriptor;
		
	TBuf8<64>		iVersionData;
	SDblQue* 		codeSegList;

};

/*
 * ITT sampler kernel part
 * 
 */
template <int BufferSize>
class DProfilerIttSampler : public DProfilerGenericSampler<BufferSize>
{
public:
	DProfilerIttSampler(struct TProfilerGppSamplerData* gppSamplerDataIn);
	~DProfilerIttSampler();

	void	Sample();
	TInt	Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset);
	TInt	PostSample();
	TBool	PostSampleNeeded();

private:
#ifdef ITT_EVENT_HANDLER
    DIttEventHandler*               iEventHandler;
#endif
	IttSamplerImpl					ittSamplerImpl;
	struct TProfilerGppSamplerData*     gppSamplerData;
	TBool							sampleInProgress;
	TBool							sampleNeeded;
};

/*  
 *	ITT sampler implementation
 *	
 */ 
template <int BufferSize>
DProfilerIttSampler<BufferSize>::DProfilerIttSampler(struct TProfilerGppSamplerData* gppSamplerDataIn) :
	DProfilerGenericSampler<BufferSize>(PROFILER_ITT_SAMPLER_ID)
{
	this->gppSamplerData = (struct TProfilerGppSamplerData*)gppSamplerDataIn;
	this->sampleInProgress = false;
	LOGSTRING2("CProfilerIttSampler<%d>::CProfilerIttSampler",BufferSize);	
}

/*
 *  DProfilerIttSampler::Reset()
 *  
 *  @param DProfilerSampleStream* sample stream
 *  @param TUint32 Offset 
 */
template <int BufferSize>
TInt DProfilerIttSampler<BufferSize>::Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset)
{
    Kern::Printf("DProfilerIttSampler<%d>::Reset - calling superclass reset",BufferSize);
    DProfilerGenericSampler<BufferSize>::Reset(aStream);

    // check if reset called in stop (by driver)
    if(aSyncOffset != 999999)
        {
#ifdef ITT_EVENT_HANDLER
        // Itt event handler
        if(iEventHandler)
            {
            // stop previous sampling if still running
            Kern::Printf("Stopping DIttEventHandler");
            iEventHandler->Stop();
            iEventHandler->Close();
            iEventHandler = NULL;
            }
    
        Kern::Printf("Initiating DIttEventHandler");
        iEventHandler = new DIttEventHandler(this->iSampleBuffer, this->gppSamplerData);
        if(iEventHandler)
            {
            Kern::Printf("Creating DIttEventHandler");
            TInt err(iEventHandler->Create());
            if(err != KErrNone)
                {
                Kern::Printf("Error in creation of DIttEventHandler, error %d", err);
                return err;
                }
            }
        else
            {
            Kern::Printf("Could not initiate DIttEventHandler");
            return KErrGeneral;
            }
    
        // set first sample at the 10 ms, should be enough
        this->ittSamplerImpl.iIttSamplingPeriod = 10;
#else
        this->ittSamplerImpl.iIttSamplingPeriod = this->iSamplingPeriod;
#endif
        this->ittSamplerImpl.iIttSamplingPeriodDiv2 = (TInt)(this->ittSamplerImpl.iIttSamplingPeriod / 2);
        LOGSTRING3("CProfilerIttSampler<%d>::Reset - set ITT sampling period to %d",
                                BufferSize,this->ittSamplerImpl.iIttSamplingPeriod);
        }
    else
        {
        LOGSTRING2("DProfilerIttSampler<%d>::Reset - reset in stop", BufferSize);
#ifdef ITT_EVENT_HANDLER
        // destroy memory event handler
        if(iEventHandler)
            {
            // stop previous sampling if still running
            iEventHandler->Stop();
            iEventHandler->Close();
            iEventHandler = NULL;
            }
#endif
        return KErrNone;    // return if reset called in stop
        }

    TInt length(ittSamplerImpl.CreateFirstSample());
    this->iSampleBuffer->AddSample((TUint8*)&length,1);
    this->iSampleBuffer->AddSample(ittSamplerImpl.itt_sample, length);
    this->sampleInProgress = false;
    this->sampleNeeded = false;
    //LOGSTRING("DProfilerIttSampler::Reset - exit");
	this->ittSamplerImpl.Reset();
    return KErrNone;

}

/*
 * DProfilerIttSampler::PostSample
 * 
 * Function for finishing sample
 */
template <int BufferSize> 
TInt DProfilerIttSampler<BufferSize>::PostSample()
{
#ifdef ITT_EVENT_HANDLER
    if(!ittSamplerImpl.iFirstSampleTaken)   // if we haven't read the initial state
    {
#endif
        if(sampleNeeded)
        {
            this->sampleNeeded = false;
            //LOGSTRING3("CProfilerIttSampler<%d>::PostSample - state %d",BufferSize,this->iSampleBuffer->GetBufferStatus());
            //Kern::Printf("DProfilerIttSampler<%d>::PostSample - state %d",BufferSize,this->iSampleBuffer->GetBufferStatus());
    
            TInt length = this->ittSamplerImpl.SampleImpl(this->gppSamplerData->lastPcValue,
                                                          this->gppSamplerData->sampleNumber);
            if(length != 0)
            {		
                LOGSTRING("ITT sampler PostSample - starting to sample");
    
                while(length > 0)
                {
                    this->iSampleBuffer->AddSample(ittSamplerImpl.itt_sample,length);
                    length = this->ittSamplerImpl.SampleImpl( this->gppSamplerData->lastPcValue,
                                                          this->gppSamplerData->sampleNumber );	
                    if(length == 0) 
                    {
                        LOGSTRING("MEM sampler PostSample - all samples generated!");
                    }
                }
                LOGSTRING("ITT sampler PostSample - finished sampling");
            }
            this->sampleInProgress = false;
        }
#ifdef ITT_EVENT_HANDLER
    }   
        if(!iEventHandler->Tracking())
            {
            iEventHandler->Start();
            Kern::Printf("DProfilerITTSampler<%d>::PostSample - ITT handler started",BufferSize);
            }

#endif    
	
    LOGSTRING2("ITT sampler PostSample - finished sampling, time: %d", gppSamplerData->sampleNumber);
    
	// finally perform superclass postsample
	TInt i(this->DProfilerGenericSampler<BufferSize>::PostSample());
	return i;
}

/*
 *  DProfilerIttSampler::PostSampleNeeded()
 *  
 *  Function for deciding if sample handling is needed 
 */
template <int BufferSize> 
TBool DProfilerIttSampler<BufferSize>::PostSampleNeeded()
{
	LOGSTRING3("CProfilerIttSampler<%d>::PostSampleNeeded - state %d",BufferSize,this->iSampleBuffer->GetBufferStatus());

	TUint32 status = this->iSampleBuffer->iBufferStatus;

	if(status == DProfilerSampleBuffer::BufferCopyAsap || status == DProfilerSampleBuffer::BufferFull || this->sampleNeeded == true)
	{
		return true;
	}
	
	return false;
}

/*
 * DProfilerIttSampler::Sample
 * 
 * Function for initiating sampling
 */
template <int BufferSize>
void DProfilerIttSampler<BufferSize>::Sample()
{
	LOGSTRING2("CProfilerIttSampler<%d>::Sample",BufferSize);	
	
	//#ifdef ITT_TEST
	LOGSTRING2("CProfilerIttSampler<%d>::Sample",BufferSize);	
	
	if(ittSamplerImpl.SampleNeeded(this->gppSamplerData->sampleNumber) && this->sampleInProgress == false) 
	{
		this->sampleInProgress = true;
		this->sampleNeeded = true;

		LOGSTRING2("CProfilerIttSampler<%d>::Sample - sample needed",BufferSize);	
	}	
#ifdef ITT_EVENT_HANDLER
    // call this to increase the time stamp
    else if(iEventHandler->SampleNeeded())
        {
        // set the flag for post sampling
        this->sampleNeeded = true;
        }
#endif

	LOGSTRING2("CProfilerIttSampler<%d>::Sample",BufferSize);
	return;
}

/*
 * Destructor
 */
template <int BufferSize>
DProfilerIttSampler<BufferSize>::~DProfilerIttSampler()
{
	LOGSTRING2("CProfilerIttSampler<%d>::~CProfilerIttSampler",BufferSize);
#ifdef ITT_EVENT_HANDLER
     if(iEventHandler)
         {
         // stop previous sampling if still running
         iEventHandler->Stop();
         iEventHandler->Close();
         iEventHandler = NULL;
         }
#endif
}
#endif
// end of file
