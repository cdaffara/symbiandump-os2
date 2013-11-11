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


#ifndef PROFILER_GFC_SAMPLER_H
#define PROFILER_GFC_SAMPLER_H


#include "GeneralsConfig.h"

#include <kern_priv.h>

#include <piprofiler/ProfilerGenericClassesKrn.h>
#include <piprofiler/ProfilerTraces.h>
#include "GppSamplerImpl.h"

extern void UsrModLr(TUint32*);


/*
 *	
 *	GFC sampler definition
 *	
 */

template <int BufferSize>
class DProfilerGfcSampler : public DProfilerGenericSampler<BufferSize>
{
public:
	DProfilerGfcSampler(struct TProfilerGppSamplerData* );
	~DProfilerGfcSampler();

	void	Sample();
	TInt	Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset);
private:
	struct TProfilerGppSamplerData* gppSamplerData;
	TUint32 gfcSample[3];

	TUint8	encodedSample[15];
	TUint32	repeat;
};

/*
 *	
 *	GFC sampler implementation
 *	
 */

template <int BufferSize>
DProfilerGfcSampler<BufferSize>::DProfilerGfcSampler(struct TProfilerGppSamplerData* gppSamplerDataIn) :
	DProfilerGenericSampler<BufferSize>(PROFILER_GFC_SAMPLER_ID)
{
	this->gppSamplerData = gppSamplerDataIn;
	LOGSTRING2("CProfilerGfcSampler<%d>::CProfilerGfcSampler",BufferSize);	
}

template <int BufferSize>
TInt DProfilerGfcSampler<BufferSize>::Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset)
{
	LOGSTRING2("CProfilerGfcSampler<BufferSize> - entry",BufferSize);
	
	this->repeat = 0;
	for(TInt i(0);i<3;i++)
	{
		this->gfcSample[i] = 0;
	}

	LOGSTRING2("CProfilerGfcSampler<%d>::Reset - calling superclass reset",BufferSize);
	DProfilerGenericSampler<BufferSize>::Reset(aStream);
	LOGSTRING2("CProfilerGfcSampler<%d>::Reset - called superclass reset",BufferSize);
	
	// add the first sample, indicating the gfc trace version
	TUint8 firstSample[33];
	TPtr8 firstSampleDesc(&(firstSample[1]),32);
	firstSampleDesc.Zero();

	firstSampleDesc.Append(_L8("Bappea_V"));
	firstSampleDesc.Append(PROFILER_GFC_SAMPLER_VERSION);
	firstSampleDesc.Append(_L8("_GFC"));
	firstSample[0] = firstSampleDesc.Size();

	this->iSampleBuffer->AddSample(firstSample,(firstSample[0]+1));

	LOGSTRING2("CProfilerGfcSampler<BufferSize> - exit",BufferSize);

	return KErrNone;
}

template <int BufferSize>
void DProfilerGfcSampler<BufferSize>::Sample()
{
	LOGSTRING2("CProfilerGfcSampler<%d>::Sample",BufferSize);	

	TUint32 usr_mod_link_reg;

	UsrModLr(&usr_mod_link_reg);

	TUint32 pc(gppSamplerData->lastPcValue);
	TUint32 lr(usr_mod_link_reg);
	TUint32 sa(gppSamplerData->sampleNumber);

	if(pc == gfcSample[0] && lr == gfcSample[1] && sa == gfcSample[2]+1)
	{
		// encode repeat
		repeat++;
		gfcSample[2] = sa;
		LOGSTRING2("CProfilerGfcSampler<%d>::Sample - repeat",BufferSize);
		return;
	}
	else if(repeat > 0)
	{
		TUint8 repeatSample[3];
		repeatSample[0] = 0xff;
		repeatSample[1] = (TUint8)(repeat>>8);
		repeatSample[2] = (TUint8)repeat;
		this->iSampleBuffer->AddSample(repeatSample,3);

		LOGSTRING3("CProfilerGfcSampler<%d>::Sample - Encoded repeat %d",BufferSize,repeat);

		repeat = 0;
	}

	TInt ptr(3);

	// encode program counter value
	if(pc>=gfcSample[0]) 
	{
		pc = (pc-gfcSample[0]);
		encodedSample[0] = 0x80;
	}
	else 
	{
		pc = (gfcSample[0]-pc);
		encodedSample[0] = 0x00;
	}

	if(pc <= (TUint32)0xff) 
	{
		encodedSample[0] |= 1;
		encodedSample[ptr] = (TUint8)pc;ptr++;
	}
	else if(pc <= (TUint32)0xffff) 
	{
		encodedSample[0] |= 2;
		encodedSample[ptr] = (TUint8)pc;ptr++;
		encodedSample[ptr] = (TUint8)(pc>>8);ptr++;
	}
	else if(pc <= (TUint32)0xffffff) 
	{
		encodedSample[0] |= 3;
		encodedSample[ptr] = (TUint8)pc;ptr++;
		encodedSample[ptr] = (TUint8)(pc>>8);ptr++;
		encodedSample[ptr] = (TUint8)(pc>>16);ptr++;
	}
	else 
	{
		encodedSample[0] |= 4;
		encodedSample[ptr] = (TUint8)pc;ptr++;
		encodedSample[ptr] = (TUint8)(pc>>8);ptr++;
		encodedSample[ptr] = (TUint8)(pc>>16);ptr++;
		encodedSample[ptr] = (TUint8)(pc>>24);ptr++;
	}

	// encode link register value
	if(lr>=gfcSample[1]) 
	{
		lr = (lr-gfcSample[1]);
		encodedSample[1] = 0x80;
	}
	else 
	{
		lr = (gfcSample[1]-lr);
		encodedSample[1] = 0x00;
	}

	if(lr <= (TUint32)0xff) 
	{
		encodedSample[1] |= 1;
		encodedSample[ptr] = (TUint8)lr;ptr++;
	}
	else if(lr <= (TUint32)0xffff) 
	{
		encodedSample[1] |= 2;
		encodedSample[ptr] = (TUint8)lr;ptr++;
		encodedSample[ptr] = (TUint8)(lr>>8);ptr++;
	}
	else if(lr <= (TUint32)0xffffff) 
	{
		encodedSample[1] |= 3;
		encodedSample[ptr] = (TUint8)lr;ptr++;
		encodedSample[ptr] = (TUint8)(lr>>8);ptr++;
		encodedSample[ptr] = (TUint8)(lr>>16);ptr++;
	}
	else 
	{
		encodedSample[1] |= 4;
		encodedSample[ptr] = (TUint8)lr;ptr++;
		encodedSample[ptr] = (TUint8)(lr>>8);ptr++;
		encodedSample[ptr] = (TUint8)(lr>>16);ptr++;
		encodedSample[ptr] = (TUint8)(lr>>24);ptr++;
	}
	
	// endcode sample number difference
	if( (sa - gfcSample[2]) < (TUint8)0xff) 
	{
		encodedSample[2] = (sa-gfcSample[2]);
	}
	else
	{
		encodedSample[2] = 0xff;
		encodedSample[ptr] = (TUint8)sa;ptr++;
		encodedSample[ptr] = (TUint8)(sa>>8);ptr++;
		encodedSample[ptr] = (TUint8)(sa>>16);ptr++;
		encodedSample[ptr] = (TUint8)(sa>>24);ptr++;
	}

	// store the values for the next sample
	gfcSample[0] = gppSamplerData->lastPcValue;
	gfcSample[1] = usr_mod_link_reg;
	gfcSample[2] = gppSamplerData->sampleNumber;
	
	this->iSampleBuffer->AddSample(encodedSample,ptr);

	LOGSTRING3("CProfilerGfcSampler<%d>::Sample Size %d",BufferSize,ptr);

	return;

}

template <int BufferSize>
DProfilerGfcSampler<BufferSize>::~DProfilerGfcSampler()
{
	LOGSTRING2("CProfilerGfcSampler<%d>::~CProfilerGfcSampler",BufferSize);		
}
#endif
