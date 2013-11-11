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


#ifndef PROFILER_GPP_SAMPLER_H
#define PROFILER_GPP_SAMPLER_H

#include "GeneralsConfig.h"

#include <e32cmn.h>

#include <piprofiler/ProfilerGenericClassesKrn.h>
#include <piprofiler/ProfilerTraces.h>

// for RPropertyRef
#include <sproperty.h> 
#include <e32cmn.h>

/*
 *	
 *	GPP sampler definition
 *	
 */
class DGppSamplerImpl //: public DBase
{
public:

	DGppSamplerImpl();
	~DGppSamplerImpl();

	TUint8* EncodeTag(TUint8* aPtr);
	TUint8* EncodeInt(TUint8* aPtr,TInt aValue);
	TUint8* EncodeUint(TUint8* aPtr,TUint aValue);
	TUint8* EncodeText(TUint8* aPtr, const TDesC& aDes);

	TUint8* EncodeName(TUint8* aPtr, DObject& aObject,TUint32 id);
	TUint8* EncodeThread(TUint8* aPtr, DThread& aThread);

	TBool	IsaTaskKnown(TUint8 task);
	TUint8* EncodeIsaTask(TUint8* aPtr, TUint task);
	TUint8* EncodeIsaName(TUint8* aPtr, TUint task,TBool process);
	
	TUint8* EncodeRepeat(TUint8* aPtr);

	TInt	CreateFirstSample();
	TInt	SampleImpl();
#ifdef __SMP__
    TSpinLock* LockPtr();
#endif
    TInt    iGppSamplingPeriod;
	TUint8	tempBuf[512];
	TUint	iLastPc;
	
	TBool	iIsaSample;
	TInt	knownIsaTasks[256];
	TUint8	knownIsaTaskCount;
	TInt*	isaOsTaskRunningAddr;
	void	Reset();
    TUint32 iSampleCount;
#ifdef __SMP__
    TInt        iCpuNumber;
    TUint32     iStartTime;
#endif	   
	PROFILER_DEFINE_ISA_TASK_NAME_ARRAY

private:
	enum TState {EStop,ERunning,EStopping};

	const TUint* iInterruptStack;
	
	TUint		iLastThread;
	TUint		iRepeat;
#ifndef __SMP__
	TUint32 	iStartTime;
#endif
	RPropertyRef iIsaStartAddr;
	RPropertyRef iIsaEndAddr;
	RPropertyRef iIsaPluginStatus;
	RPropertyRef iIsaOsTaskRunning;
	TInt 		iIsaStatus;
	TInt 		iIsaStart;
	TInt 		iIsaEnd;
	TUint32     iMask;
	TUint32     iCpuSelector;
};

struct TProfilerGppSamplerData
{
//public:
	TUint32		lastPcValue;
	TUint32		sampleNumber;
    TInt        samplingPeriod;
};

template <int BufferSize>
class DProfilerGppSampler : public DProfilerGenericSampler<BufferSize>
{
public:
#ifndef __SMP__
	DProfilerGppSampler();
#else
    DProfilerGppSampler(TInt aCpuNumber);
#endif
	~DProfilerGppSampler();

	struct TProfilerGppSamplerData* GetExportData();
	void	Sample();
	TInt	Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset);
	TInt 	GetPeriod();
	
private:
	DGppSamplerImpl gppSamplerImpl;
	struct TProfilerGppSamplerData exportData;
#ifdef __SMP__
	TInt   iCpuNumber;
#endif
};

/*
 *	
 *	GPP sampler implementation
 *	
 */
#ifndef __SMP__
template <int BufferSize>
DProfilerGppSampler<BufferSize>::DProfilerGppSampler() :
	DProfilerGenericSampler<BufferSize>(PROFILER_GPP_SAMPLER_ID) 
    {
	LOGSTRING2("CProfilerGppSampler<%d>::CProfilerGppSampler",BufferSize);
    }
#else
template <int BufferSize>
DProfilerGppSampler<BufferSize>::DProfilerGppSampler(TInt aCpuNumber) :
    DProfilerGenericSampler<BufferSize>(PROFILER_GPP_SAMPLER_ID+(aCpuNumber*20)), iCpuNumber(aCpuNumber) 
    {
    LOGSTRING2("CProfilerGppSampler<%d>::CProfilerGppSampler",BufferSize);
    }
#endif

template <int BufferSize>
DProfilerGppSampler<BufferSize>::~DProfilerGppSampler()
    {
	LOGSTRING2("CProfilerGppSampler<%d>::~CProfilerGppSampler",BufferSize);		
    }

template <int BufferSize>
TInt DProfilerGppSampler<BufferSize>::Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset)
        {
	LOGSTRING2("CProfilerGppSampler<%d>::Reset - calling superclass reset",BufferSize);
	DProfilerGenericSampler<BufferSize>::Reset(aStream, 0);
	LOGSTRING2("CProfilerGppSampler<%d>::Reset - called superclass reset",BufferSize);

	this->gppSamplerImpl.Reset();
	
#ifdef __SMP__
	this->gppSamplerImpl.iCpuNumber = this->iCpuNumber;
	
	// set common start time for all CPU samplers
	this->gppSamplerImpl.iStartTime = aSyncOffset;
#endif
	this->gppSamplerImpl.iGppSamplingPeriod = this->iSamplingPeriod;
	this->gppSamplerImpl.iSampleCount = 0;
	this->exportData.sampleNumber = 0;
	this->exportData.lastPcValue = 0;
    this->exportData.samplingPeriod = this->gppSamplerImpl.iGppSamplingPeriod;

	TInt length = gppSamplerImpl.CreateFirstSample();
	LOGSTRING3("CProfilerGPPSampler<%d>::Reset - got first sample, size %d",BufferSize,length);	
	
	this->iSampleBuffer->AddSample(gppSamplerImpl.tempBuf,length);

	// check if sampling period > 1 ms
	// NOTE: feature supported in Performance Investigator 2.01 and above
	if(this->gppSamplerImpl.iGppSamplingPeriod > 1)
	    {
        // For Address/Thread (GPP) version 2.01 or above, the first word is the sampling period in milliseconds
        TUint8* w = gppSamplerImpl.tempBuf;
        
        (*w++) = (this->gppSamplerImpl.iGppSamplingPeriod >> 24) & 0xFF;
        (*w++) = (this->gppSamplerImpl.iGppSamplingPeriod >> 16) & 0xFF;
        (*w++) = (this->gppSamplerImpl.iGppSamplingPeriod >>  8) & 0xFF;
        (*w++) = (this->gppSamplerImpl.iGppSamplingPeriod) & 0xFF;
        
        this->iSampleBuffer->AddSample(gppSamplerImpl.tempBuf,4);
	    }
	
	LOGSTRING2("CProfilerGPPSampler<%d>::Reset finished",BufferSize);
	return KErrNone;
    }

template <int BufferSize>
void DProfilerGppSampler<BufferSize>::Sample()
    {
	LOGSTRING2("CProfilerGppSampler<%d>::Sample",BufferSize);
//	if(this->gppSamplerImpl.iSampleCount % 1000 == 0) 
//	    {
//#ifdef __SMP__
//	    if(this->iCpuNumber == 0)  // print sample tick only from CPU 0 context
//	        {
//#endif
//	        Kern::Printf(("PIPROF SAMPLE TICK, #%d"),exportData.sampleNumber);
//#ifdef __SMP__
//	        }
//#endif
//	    }
	
	TInt length(gppSamplerImpl.SampleImpl());

    this->gppSamplerImpl.iSampleCount++;
	this->exportData.sampleNumber += this->gppSamplerImpl.iGppSamplingPeriod;
	this->exportData.lastPcValue = gppSamplerImpl.iLastPc;

	if(length > 0)
        {
        this->iSampleBuffer->AddSample(gppSamplerImpl.tempBuf,length);
        }

	LOGSTRING3("CProfilerGppSampler<%d>::Sample - length %d",BufferSize,length);

	return;
    }


template <int BufferSize>
struct TProfilerGppSamplerData* DProfilerGppSampler<BufferSize>::GetExportData()
    {
	LOGSTRING2("CProfilerGppSampler<%d>::GetExportData",BufferSize);
	return &(this->exportData);	
    }

template <int BufferSize>
TInt DProfilerGppSampler<BufferSize>::GetPeriod()
    {
	return this->gppSamplerImpl.iGppSamplingPeriod;
    }

#endif
