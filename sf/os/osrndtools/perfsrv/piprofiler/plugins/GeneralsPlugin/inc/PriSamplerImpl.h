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


#ifndef PROFILER_PRI_SAMPLER_H
#define PROFILER_PRI_SAMPLER_H

#include "GeneralsConfig.h"

#include <kern_priv.h>

#include <piprofiler/ProfilerGenericClassesKrn.h>
#include <piprofiler/ProfilerTraces.h>
#include "GppSamplerImpl.h"

// defines the maximum thread amount that is assumed to
// be possible with PRI trace
const TInt KProfilerMaxThreadAmount = 512;

/*
 *	
 *	PRI sampler definition
 *	
 */

class DPriSamplerImpl //: public DBase
{
public:
	enum EProcessingState
	{
		EStartingToProcess,
		EProcessingNames,
		EProcessingData,
		ENothingToProcess
	};

	DPriSamplerImpl();
	~DPriSamplerImpl();

	TInt	CreateFirstSample();
	TInt	SampleImpl();
	TBool	SampleNeeded();
	void	Reset();
	TInt	ProcessChunks();

	TInt	EncodeChunkData(DThread& thread);
	TInt	EncodeChunkName(DThread& thread);
	
	TInt	EncodeNameCode();
	TInt	EncodeDataCode();

	TInt		iCountti;
	DThread*	threadsToSample[KProfilerMaxThreadAmount];
	DThread*	threadNamesToReport[KProfilerMaxThreadAmount];

	TInt		iThreadCount;
	TInt		iNewThreadCount;

	TInt		iProcessing;
	TInt		iPriSamplingPeriod;

	TUint8		sample[257];
	TPtr8		sampleDescriptor;
#ifdef __SMP__
    TInt        iCpuNumber;
    TUint32     iStartTime;
#endif   

};

template <int BufferSize>
class DProfilerPriSampler : public DProfilerGenericSampler<BufferSize>
{
public:
	DProfilerPriSampler(struct TProfilerGppSamplerData*, TInt id);
	~DProfilerPriSampler();

	void	Sample();
	TInt	Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset);
	TInt	PostSample();
	TBool	PostSampleNeeded();

private:
	DPriSamplerImpl				 	priSamplerImpl;
	struct TProfilerGppSamplerData* 	    gppSamplerData;
	TBool						 	sampleNeeded;
	TBool							sampleInProgress;
#ifdef __SMP__
    TInt   iCpuNumber;
#endif
};

/*
 *	
 *	PRI sampler implementation
 *	
 */
//#ifndef __SMP__
template <int BufferSize>
DProfilerPriSampler<BufferSize>::DProfilerPriSampler(struct TProfilerGppSamplerData* gppSamplerDataIn, TInt id) :
	DProfilerGenericSampler<BufferSize>(PROFILER_PRI_SAMPLER_ID)
    {
	LOGSTRING2("CProfilerPriSampler<%d>::CProfilerPriSampler",BufferSize);
	this->gppSamplerData = gppSamplerDataIn;
	this->sampleInProgress = false;
	this->iSamplingPeriod = 3000;	// set default setting
    }
//#else
//template <int BufferSize>
//DProfilerPriSampler<BufferSize>::DProfilerPriSampler(struct TProfilerGppSamplerData* gppSamplerDataIn, TInt id, TInt aCpuNumber) :
//    DProfilerGenericSampler<BufferSize>(PROFILER_PRI_SAMPLER_ID+(aCpuNumber*20)), iCpuNumber(aCpuNumber) 
//    {
//    LOGSTRING2("CProfilerGppSampler<%d>::CProfilerGppSampler",BufferSize);
//    }
//#endif

template <int BufferSize>
TInt DProfilerPriSampler<BufferSize>::Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset)
    {
	LOGSTRING2("CProfilerPriSampler<%d>::Reset - calling superclass reset",BufferSize);
	DProfilerGenericSampler<BufferSize>::Reset(aStream);
	LOGSTRING2("CProfilerPriSampler<%d>::Reset - called superclass reset",BufferSize);
	priSamplerImpl.Reset();

#ifdef __SMP__
//    this->priSamplerImpl.iCpuNumber = this->iCpuNumber;
//    
//    // set common start time for all CPU samplers
//    this->priSamplerImpl.iStartTime = aSyncOffset;
#endif
	this->priSamplerImpl.iPriSamplingPeriod = this->iSamplingPeriod;


	LOGSTRING3("CProfilerPriSampler<%d>::Reset - set pri sampling period to", 
							BufferSize,this->priSamplerImpl.iPriSamplingPeriod);

	TInt length(priSamplerImpl.CreateFirstSample());
	this->iSampleBuffer->AddSample(priSamplerImpl.sample,length);

	this->sampleInProgress = false;	
	this->sampleNeeded = false;

	return KErrNone;
    }

template <int BufferSize> 
TInt DProfilerPriSampler<BufferSize>::PostSample()
    {
	if(sampleNeeded)
	    {
		this->sampleNeeded = false;

		LOGSTRING3("CProfilerPriSampler<%d>::PostSample - state %d",BufferSize,this->sampleBuffer->GetBufferStatus());
		
		//TInt interruptLevel = NKern::DisableInterrupts(0);
		
		TInt length(this->priSamplerImpl.SampleImpl());
		if(length != 0)
		    {
			LOGSTRING("PRI sampler PostSample - starting to sample");

			// then, encode the sample to the buffer
			while(length > 0)
			    {
			    this->iSampleBuffer->AddSample(priSamplerImpl.sample,length);
				length = this->priSamplerImpl.SampleImpl();
				// indicate that the whole PRI sample ends by having a 0x00 in the end
				if(length == 0)
				    {
					TUint8 number(0);
					LOGSTRING("PRI sampler PostSample - all samples generated!");
					
					this->iSampleBuffer->AddSample(&number,1);
					LOGSTRING("PRI sampler PostSample - end mark added");
                    }
			    }
			LOGSTRING("PRI sampler PostSample - finished sampling");
		    }
		this->sampleInProgress = false;
		
		//NKern::RestoreInterrupts(interruptLevel);
	    }
	
	// finally perform superclass postsample
	TInt i(this->DProfilerGenericSampler<BufferSize>::PostSample());
	return i; 
    }

template <int BufferSize> 
TBool DProfilerPriSampler<BufferSize>::PostSampleNeeded()
    {
	LOGSTRING3("CProfilerPriSampler<%d>::PostSampleNeeded - state %d",BufferSize,this->sampleBuffer->GetBufferStatus());

	TUint32 status = this->iSampleBuffer->iBufferStatus;

	if(status == DProfilerSampleBuffer::BufferCopyAsap || status == DProfilerSampleBuffer::BufferFull || this->sampleNeeded == true)
	    {
		return true;
	    }
	
	return false;
    }


template <int BufferSize>
void DProfilerPriSampler<BufferSize>::Sample()
    {
	LOGSTRING2("CProfilerPriSampler<%d>::Sample",BufferSize);	
	
	if(priSamplerImpl.SampleNeeded() && this->sampleInProgress == false) 
	    {
		this->sampleInProgress = true;
		this->sampleNeeded = true;
		// start the PRI sample with the sample time
		TUint8 number(4);
		this->iSampleBuffer->AddSample(&number,1);
		this->iSampleBuffer->AddSample((TUint8*)&(gppSamplerData->sampleNumber),4);

		// leave the rest of the processing for PostSample()
	    }	

	LOGSTRING2("CProfilerPriSampler<%d>::Sample",BufferSize);
	return;
    }

template <int BufferSize>
DProfilerPriSampler<BufferSize>::~DProfilerPriSampler()
    {
	LOGSTRING2("CProfilerPriSampler<%d>::~CProfilerPriSampler",BufferSize);		
    }
#endif
