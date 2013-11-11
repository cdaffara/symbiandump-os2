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


#ifndef PROFILERGENERICCLASSESKRN_H
#define PROFILERGENERICCLASSESKRN_H

	#include <piprofiler/ProfilerGenericClassesCommon.h>
	#include <piprofiler/ProfilerTraces.h>

	#include <e32cmn.h>

	#define PROFILER_KERNEL_MODE

#ifdef PROFILER_KERNEL_MODE
class DProfilerSampleBuffer;

class DProfilerSampleStream
    {
public:
		DProfilerSampleStream();
		~DProfilerSampleStream();

		void InsertCurrentClient(DThread* aClient);
		void AddSampleBuffer(TBapBuf* aBuffer,TRequestStatus* aStatus);
		void ReleaseIfPending();

		void AddSamples(DProfilerSampleBuffer& aBuffer, TInt aSamplerId);
		TInt EndSampling(DProfilerSampleBuffer& aBuffer,TInt aSamplerId);

		void PerformCopy(TUint8 aSamplerId,TUint8* aSrc,TPtr8* dst,TInt aOffset,TInt aAmount);

private:
		TBapBuf*			iCurrentBuffer;
		TRequestStatus*		iPendingRequest;
		DThread*			iClient;
		TInt				iAddingSamples;
    };

class DProfilerSampleBuffer : public DBase
    {
		friend class DProfilerSampleStream;
public:

			enum ProfilerBufferStatus 
			    {
				BufferOk,
				BufferCopyAsap,
				BufferBeingCopied,
				BufferFull,
				BufferDataEnd
			    };

			DProfilerSampleBuffer(TUint8* aBuffer, TUint8* aDblBuffer, TUint32 aSize);
			~DProfilerSampleBuffer();	
			TInt	AddSample(TUint8* aSample, TUint32 aLength);
			TUint32 GetBufferStatus();
			void	ClearBuffer();
			void	EndSampling();
			void	DataCopied();

			TUint32 iBufferStatus;
private:
			TUint32 iBytesWritten;
			TUint32 iDblBytesWritten;
			TUint32 iDblBytesRead;

			TUint32 iBufferDataSize;
			TUint32 iBufferRealSize;

			TProfilerSampleBufStruct* iDblBufStruct;
			TProfilerSampleBufStruct* iBufStruct;
    };

#endif


 
/*
 *	
 *	Abstract class CProfilerSamplerBase definition
 *	
 */

#ifdef PROFILER_KERNEL_MODE
class DProfilerSamplerBase : public DBase
    {
public:
	DProfilerSamplerBase();
			virtual					~DProfilerSamplerBase();

			virtual	TInt			Initialise() = 0;
			virtual	void			Sample() = 0;
			virtual TBool			PostSampleNeeded() = 0;
			virtual TInt			PostSample() = 0;
			virtual TInt			EndSampling() = 0;

			virtual TInt			Reset(DProfilerSampleStream* aStream = 0, TUint32 aSyncOffset = 0) = 0;

			virtual void			SetEnabledFlag(TBool aFlag) = 0;
			virtual TBool			GetEnabledFlag() = 0;
			virtual void			SetOutputCombination(TInt aSettings) = 0;
			virtual void 			SetSamplingPeriod(TInt aSettings) = 0;
			virtual void			SetAdditionalSettings(TInt aSettings) = 0;
			virtual void			SetAdditionalSettings2(TInt aSettings) = 0;
			virtual void			SetAdditionalSettings3(TInt aSettings) = 0;
			virtual void			SetAdditionalSettings4(TInt aSettings) = 0;

			TInt					iSamplerId;
			TInt					iOutputCombination;
			TBool					iEnabled;
    };

/*
 *	
 *	Template abstract class CProfilerGenericSampler definition
 *	
 */

// size parameter given defines the explicit buffer size in bytes for this sampler
template <int BufferSize> 
class DProfilerGenericSampler : public DProfilerSamplerBase
    {
public:
			DProfilerGenericSampler(TInt aSamplerId);
			virtual					~DProfilerGenericSampler();

			TInt					Initialise();
			virtual	void			Sample() = 0;
			TBool					PostSampleNeeded();
			TInt					PostSample();
			TInt					EndSampling();
			virtual TInt			Reset(DProfilerSampleStream* aStream = 0, TUint32 aSyncOffset = 0);


			void					SetEnabledFlag(TBool aFlag);
			TBool					GetEnabledFlag();
			void					SetOutputCombination(TInt aComb);
			void 					SetSamplingPeriod(TInt aSettings);
			void					SetAdditionalSettings(TInt aSettings);
			void					SetAdditionalSettings2(TInt aSettings);
			void					SetAdditionalSettings3(TInt aSettings);
			void					SetAdditionalSettings4(TInt aSettings);

			DProfilerSampleBuffer*	iSampleBuffer;
			DProfilerSampleStream*	iStream;

			// for right alignment
			TUint8					iBuffer[BufferSize+4];
			TUint8					iDblBuffer[BufferSize+4];
			
			TInt					iSamplingPeriod;
			TInt					iAdditionalSettings;
			TInt					iAdditionalSettings2;
			TInt					iAdditionalSettings3;
			TInt					iAdditionalSettings4;
    };

/*
 *	
 *	Template abstract class CProfilerGenericSampler implementation
 *
 */

template <int BufferSize>
DProfilerGenericSampler<BufferSize>::DProfilerGenericSampler(TInt aSamplerId)
    {
	iSamplerId = aSamplerId;
	iEnabled = false;
	iSampleBuffer = 0;
	iAdditionalSettings = 0;
	iAdditionalSettings2 = 0;
    iAdditionalSettings3 = 0;
    iAdditionalSettings4 = 0;
	iStream = 0;
	Initialise();
    }

template <int BufferSize> 
DProfilerGenericSampler<BufferSize>::~DProfilerGenericSampler()
    {
	LOGSTRING2("CProfilerGenericSampler<%d>::CProfilerGenericSampler",BufferSize);	
	
	if(iSampleBuffer != 0)
		delete iSampleBuffer;

    }
 
template <int BufferSize> 
TInt DProfilerGenericSampler<BufferSize>::Initialise()
    {
	LOGSTRING2("CProfilerGenericSampler<%d>::Initialise - chunk option",BufferSize);

	// stream is not used in chunk mode
	iStream = 0;

	// create the sample buffer object with the buffers
	if(iSampleBuffer == 0)
	    {
	    iSampleBuffer = new DProfilerSampleBuffer(iBuffer,iDblBuffer,BufferSize);
        }
	else
	   {
		LOGSTRING2("CProfilerGenericSampler<%d>::Initialise - ERROR 1",BufferSize);
        }

	return KErrNone;
    }



template <int BufferSize> 
TInt DProfilerGenericSampler<BufferSize>::Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset)
    {
	LOGSTRING4("CProfilerGenericSampler<%d>::Reset %d, sync offset %d",BufferSize,aStream,aSyncOffset);
	// reset the sample buffer and resolve the chunk again

	// CURRENT VERSION SUPPORTS ONLY STREAM MODE!
	LOGSTRING2("CProfilerGenericSampler<%d>::Reset - stream option",BufferSize);

	// initialise the sampler with the stream option
	iStream = aStream;

	// clear the sample buffer
	if(iSampleBuffer != 0)
	    {
	    iSampleBuffer->ClearBuffer();
        }
	else
	    {
		LOGSTRING2("CProfilerGenericSampler<%d>::Initialise - ERROR no buffer",BufferSize);
        }

	return KErrNone;

    }

template <int BufferSize> 
TBool DProfilerGenericSampler<BufferSize>::PostSampleNeeded()
    {
	LOGSTRING4("CProfilerGenericSampler<%d>::PostSampleNeeded - ID %d, state %d",iSamplerId,BufferSize,iSampleBuffer->GetBufferStatus());

	TUint32 status = iSampleBuffer->iBufferStatus;

	if(status == DProfilerSampleBuffer::BufferCopyAsap || status == DProfilerSampleBuffer::BufferFull)
	    {
		return true;
        }
	
	return false;
    }


template <int BufferSize> 
TInt DProfilerGenericSampler<BufferSize>::PostSample()
    {
	LOGSTRING4("CProfilerGenericSampler<%d>::PostSample - ID %d, state %d",iSamplerId,BufferSize,iSampleBuffer->GetBufferStatus());

	TUint32 status = iSampleBuffer->iBufferStatus;

	if(status == DProfilerSampleBuffer::BufferCopyAsap || status == DProfilerSampleBuffer::BufferFull)
	    {
        // write data to the stream
	    iStream->AddSamples(*iSampleBuffer,iSamplerId);
		}
	
	return KErrNone;
    }

template <int BufferSize>
TInt DProfilerGenericSampler<BufferSize>::EndSampling()
    {
	LOGSTRING3("CProfilerGenericSampler<%d>::EndSampling, ID %d",BufferSize,iSamplerId);

    // only if write to stream option is selected
    if(iStream->EndSampling(*iSampleBuffer,iSamplerId) == 0)
        {
        return KErrNone;
        }
    else 
        {
        // there is still data to copy
        return KErrNotReady;
        }
    }

template <int BufferSize>
void DProfilerGenericSampler<BufferSize>::SetEnabledFlag(TBool aFlag)
    {
	LOGSTRING2("CProfilerGenericSampler<%d>::SetEnabledFlag",BufferSize);
	iEnabled = aFlag;
    }

template <int BufferSize>
TBool DProfilerGenericSampler<BufferSize>::GetEnabledFlag()
    {
	LOGSTRING2("CProfilerGenericSampler<%d>::GetEnabledFlag",BufferSize);
	return iEnabled;
    }

template <int BufferSize>
void DProfilerGenericSampler<BufferSize>::SetOutputCombination(TInt aComb)
    {
	LOGSTRING2("CProfilerGenericSampler<%d>::SetOutputCombination",BufferSize);
	iOutputCombination = aComb;
    }

template <int BufferSize>
void DProfilerGenericSampler<BufferSize>::SetAdditionalSettings(TInt aSettings)
    {
	LOGSTRING3("CProfilerGenericSampler<%d>::SetAdditionalSettings to 0x%x",BufferSize,aSettings);
	iAdditionalSettings = aSettings;
    }

template <int BufferSize>
void DProfilerGenericSampler<BufferSize>::SetAdditionalSettings2(TInt aSettings)
    {
	LOGSTRING3("CProfilerGenericSampler<%d>::SetAdditionalSettings2 to 0x%x",BufferSize,aSettings);
	iAdditionalSettings2 = aSettings;
    }

template <int BufferSize>
void DProfilerGenericSampler<BufferSize>::SetAdditionalSettings3(TInt aSettings)
    {
	LOGSTRING3("CProfilerGenericSampler<%d>::SetAdditionalSettings3 to 0x%x",BufferSize,aSettings);
	iAdditionalSettings3 = aSettings;
    }

template <int BufferSize>
void DProfilerGenericSampler<BufferSize>::SetAdditionalSettings4(TInt aSettings)
    {
	LOGSTRING3("CProfilerGenericSampler<%d>::SetAdditionalSettings4 to 0x%x",BufferSize,aSettings);
	iAdditionalSettings4 = aSettings;
    }

template <int BufferSize>
void DProfilerGenericSampler<BufferSize>::SetSamplingPeriod(TInt aSettings)
    {
	LOGSTRING3("CProfilerGenericSampler<%d>::SetSamplingPeriod to 0x%x",BufferSize,aSettings);
	iSamplingPeriod = aSettings;
    }

/*
 *	
 *	Just a test class that is derived from CProfilerGenericSampler
 *	
 */

template <int BufferSize>
class DProfilerExampleSampler : public DProfilerGenericSampler<BufferSize>
    {
public:
	TUint32 iSampleNumber;

	DProfilerExampleSampler(TInt aSamplerId);
	~DProfilerExampleSampler();

	void Sample();
	void Sample(TInt aCount, TInt aLastPc);
    };


/*
 *	
 *	Just a test class that is derived from CProfilerGenericSampler
 *	
 */

template <int BufferSize>
DProfilerExampleSampler<BufferSize>::DProfilerExampleSampler(TInt aSamplerId) :
	DProfilerGenericSampler<BufferSize>(aSamplerId) 
    {
    iSampleNumber = 0;
	LOGSTRING2("CProfilerExampleSampler<%d>::CProfilerExampleSampler",BufferSize);	
    }

template <int BufferSize>
void DProfilerExampleSampler<BufferSize>::Sample()
    {
	LOGSTRING2("CProfilerExampleSampler<%d>::Sample",BufferSize);
	TBuf8<20>* testiBuf = new TBuf8<20>;

	testiBuf->AppendNum((TInt)iSampleNumber);
	iSampleNumber++;

	this->iSampleBuffer->AddSample((TUint8*)testiBuf->Ptr(),testiBuf->Length());
	delete testiBuf;
	return;
    }

template <int BufferSize>
void DProfilerExampleSampler<BufferSize>::Sample(TInt aCount, TInt aLastPc)
    {
	return;
    }

template <int BufferSize>
DProfilerExampleSampler<BufferSize>::~DProfilerExampleSampler()
    {
	LOGSTRING2("CProfilerExampleSampler<%d>::~CProfilerExampleSampler",BufferSize);		
    }

#include <piprofiler/ProfilerGenericClassesKrn.inl>

#endif


#endif
