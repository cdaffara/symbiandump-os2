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


#ifndef __PLUGINSAMPLER_H__
#define __PLUGINSAMPLER_H__

/*
 * The user-interface to the sampling device driver sued by the profiling engine
 */
//	#include <piprofiler/ProfilerConfig.h>
//	#include <piprofiler/ProfilerVersion.h>	// versions removed from ProfilerConfig.h

#ifndef __KERNEL_MODE__
#include <utf.h>
#endif

/*
 *	Forward declarations
 */
class TBapBuf;

/**
 * A minimal capabilities class for the Sampling device.
 * It just returns the version number.
 */
class TCapsSamplerV01
	{
public:
	TVersion	iVersion;
	};


const TInt KMinDelay = 1;
const TInt KMaxDelay = 10000;

/**
 * The user device driver class for controlling the plugin sampler.
 */

class RPluginSampler :  public RBusLogicalChannel
{	
	friend class DPluginDriver;
	public:
		enum TControl
		{
			EOutputSettingsForTrace,
			EMarkTraceActive,
			EMarkTraceInactive,
			ESample,
			EStartSampling,
			EGetSampleTime,
			EGetSamplerVersion,
			EStopAndWaitForEnd,
			ESetSamplingPeriod,
			EAdditionalTraceSettings,
			EAdditionalTraceSettings2,
			EAdditionalTraceSettings3,
			EAdditionalTraceSettings4,
			//EPrintTraces,
			ECancelStreamRead,
			ERequestFillThisStreamBuffer,
			ERequestPrintStreamBuffer,
			ETest
		};

	public:
		#ifndef __KERNEL_MODE__
		
		inline ~RPluginSampler();
			
		/**
		 * 
		 * Methods for controlling the sampler device
		 * 
		 **/
		/** Open a channel to the sampling device **/
		virtual TInt Open() = 0;

		/** Set sampling period for sampler */
		inline void SetSamplingPeriod(TInt samplerId, TInt settings);
		
		/** Set additional trace settings **/
		inline void AdditionalSettingsForTrace(TInt samplerId,TInt settings);
		inline void AdditionalSettingsForTrace2(TInt samplerId,TInt settings);
		inline void AdditionalSettingsForTrace3(TInt samplerId,TInt settings);
		inline void AdditionalSettingsForTrace4(TInt samplerId,TInt settings);
		
		/** Mark trace active **/
		inline void MarkTraceActive(TUint32 id);

		/** Mark trace inactive **/
		inline void MarkTraceInactive(TUint32 id);

		/** Sample command to kernel LDD**/
		inline void Sample();
		
		/** Start tracing **/
		inline void StartSampling();

		/** Stop tracing **/
		inline void StopSampling();

		/** Get current sampling time **/
		inline TUint32 GetSampleTime();

		/** Get sampler version as descriptor **/
		virtual void GetSamplerVersion(TDes*);

		/** Get next filled buffer in stream mode **/
		void GetNextbuffer( );

		/** Request stream read **/
		void FillThisStreamBuffer(TBapBuf* aBuffer, TRequestStatus& aStatus);

		/** Request print buffer**/
		void PrintStreamBuffer(TDesC16* aBuffer);
		
		/** Run test case **/
		inline void Test(TUint32 testCase);

		#endif	// !__KERNEL_MODE__	
};


#ifndef __KERNEL_MODE__
inline RPluginSampler::~RPluginSampler()
{
}

inline void RPluginSampler::AdditionalSettingsForTrace(TInt samplerId,TInt settings)
	{
	DoControl(EAdditionalTraceSettings,
			reinterpret_cast<TAny*>(samplerId),
			reinterpret_cast<TAny*>(settings));
	}

inline void RPluginSampler::AdditionalSettingsForTrace2(TInt samplerId,TInt settings)
	{
	DoControl(EAdditionalTraceSettings2,
			reinterpret_cast<TAny*>(samplerId),
			reinterpret_cast<TAny*>(settings));
	}

inline void RPluginSampler::AdditionalSettingsForTrace3(TInt samplerId,TInt settings)
	{
	DoControl(EAdditionalTraceSettings3,
			reinterpret_cast<TAny*>(samplerId),
			reinterpret_cast<TAny*>(settings));
	}

inline void RPluginSampler::AdditionalSettingsForTrace4(TInt samplerId,TInt settings)
	{
	DoControl(EAdditionalTraceSettings4,
			reinterpret_cast<TAny*>(samplerId),
			reinterpret_cast<TAny*>(settings));
	}

inline void RPluginSampler::SetSamplingPeriod(TInt samplerId, TInt settings)
	{
	DoControl(ESetSamplingPeriod, 
			reinterpret_cast<TAny*>(samplerId),
			reinterpret_cast<TAny*>(settings));
	}


inline void RPluginSampler::MarkTraceActive(TUint32 id)
{
		DoControl(EMarkTraceActive, reinterpret_cast<TAny*>(id));
}

inline void RPluginSampler::MarkTraceInactive(TUint32 id)
{
		DoControl(EMarkTraceInactive, reinterpret_cast<TAny*>(id));
}

inline void RPluginSampler::Sample()
	{
	DoControl(ESample);
	}

inline void RPluginSampler::StartSampling()
{
	// at first, start the kernel side samplers
	DoControl(EStartSampling);
}

inline void RPluginSampler::StopSampling()
{
	// stop the device driver and the kernel mode samplers
	TRequestStatus status;
	status = KRequestPending;
	DoRequest(EStopAndWaitForEnd,status);
	User::WaitForRequest(status);
}

inline TUint32 RPluginSampler::GetSampleTime()
{
	TUint32* sampleTime = new TUint32;

	DoControl(EGetSampleTime,reinterpret_cast<TAny*>(sampleTime));
	TUint32 value = *sampleTime;
	delete sampleTime;

	return value;
}

inline void RPluginSampler::GetSamplerVersion(TDes* aDes)
{
	TBuf8<16> buf;
	DoControl(EGetSamplerVersion,(TAny*)&buf);
	CnvUtfConverter::ConvertToUnicodeFromUtf8(*aDes,buf);
}

inline void RPluginSampler::PrintStreamBuffer(TDesC16* aDes) 
{
	DoControl(ERequestPrintStreamBuffer,reinterpret_cast<TAny*>(aDes));
}

inline void RPluginSampler::FillThisStreamBuffer(TBapBuf* aBuffer, TRequestStatus& aStatus)
{
	aStatus = KRequestPending;
	DoRequest(ERequestFillThisStreamBuffer,aStatus,(TAny*)aBuffer);

}

inline void RPluginSampler::Test(TUint32 testCase)
{
	DoControl(ETest,reinterpret_cast<TAny*>(testCase));
}

#endif
#endif
