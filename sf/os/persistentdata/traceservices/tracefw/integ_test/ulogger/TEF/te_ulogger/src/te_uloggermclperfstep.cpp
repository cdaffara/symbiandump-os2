// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Example CTestStep derived implementation
// 
//

/**
 @file te_uloggermclperfstep.cpp
 @internalTechnology
*/
#include "te_uloggermclperfstep.h"
#include "te_uloggermclsuitedefs.h"
#include <e32math.h>

using namespace Ulogger;

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//////////////////////////////////USER SIDE/////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

CULoggerMCLUserPerfStep::~CULoggerMCLUserPerfStep()
/**
 * Destructor
 */
	{
	}

CULoggerMCLUserPerfStep::CULoggerMCLUserPerfStep()
/**
 * Constructor
 */
	{
		SetTestStepName(KULoggerMCLUserPerfStep);
	}

TVerdict CULoggerMCLUserPerfStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Carrying out performance testing on user side tracing for file and serial plugins"));
		SetTestStepResult(EPass);
		return TestStepResult();
	}


TVerdict CULoggerMCLUserPerfStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  	if (TestStepResult()==EPass)
		{
		//  ************** Delete the Block, the block start ****************
			INFO_PRINTF1(_L("Benchmark trace logging for the file plug-in"));
			 __CREATE_LOG(true);
			 __HIRES_RESOLUTION;
			TInt fastTimerFreq;
			HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
			TReal ticksPerMicroSec = 1.0E-6 * fastTimerFreq;	
			TUint32 prevTime;
			TUint32 timeDiff;
			TReal timeSpentToTrace;
			bool ret=1;
			CUTraces Testtracer;
			TUTrace TestTraceObject(UTracePrimary, UTraceSecondary, schema, context, pc);
			TInt Result=0;
			/***Now set file plugins and filter settings****/
			RULogger logger;
			logger.Connect();
			CClearConfig configIni;
			configIni.ClearL(logger);
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(UTracePrimary);
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) UTraceSecondary);
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);
		
			///////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////
			/////////////////////////FILE//////////////////////////////
			///////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////
	
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step
			if(Result==0)	
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				Result=logger.ActivateOutputPlugin(mediaptr);
				if(Result==0||Result==-11)
				{	
					INFO_PRINTF1(_L("file set as active output plugin ok"));
					_LIT(KTextsetting, "output_path");
					_LIT(KTextvalue, "c:\\logs\\ULoggerPerfUserSideFileLogging.log");
					TPluginConfiguration setPluginConfigs;
					setPluginConfigs.SetKey(KTextsetting);
					setPluginConfigs.SetValue(KTextvalue);
					Result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
					if(Result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
			}
			else
				INFO_PRINTF1(_L("add file as output plugin failed"));
			/***Now carry out performance benchmarking and tracing for NonStatic SET method to file****/
			__LOG("Trace times for 10,000 user side nonstatic set traces logging to file plugin");
#ifdef FILE_PERF	
			for(int tracetag=0;tracetag!=NumberOfUTraceMacros;tracetag++)
			{
				logger.Start();
 			 	__LOG("NONStatic Trace ID: ")
 			 	__LOGNUM(tracetag)	 
				ret = 0;
				timeDiff=0;
				for(int i=0;i!=StatAverage;i++)
				{
					User::After(Math::Random()%100000);
					prevTime = User::FastCounter();
					for(int i=0;i!=UTrace_Cache_Count;i++)
						ret = Testtracer.DoTheSetTrace(tracetag, TestTraceObject);
					timeDiff += User::FastCounter() - prevTime;
				}
				timeSpentToTrace = timeDiff/ticksPerMicroSec/StatAverage;
				INFO_PRINTF5(_L("Utrace macro %d trace, size %d (%d trace(s))= %10.6lf microseconds"), tracetag, Testtracer.SizeOfTrace(tracetag), UTrace_Cache_Count, timeSpentToTrace);
				INFO_PRINTF2(_L("Time per trace = %10.6e microseconds"),timeSpentToTrace/UTrace_Count );
				logger.Stop();
			}
			if(ret==0)
				INFO_PRINTF1(_L("tracing failed from UTrace"));
			/***Now carry out performance benchmarking and tracing for Static method to file****/
			__LOG("Trace times for 10,000 user side static traces logging to file plugin");
			for(int tracetag=0;tracetag!=NumberOfUTraceMacros;tracetag++)
			{
				logger.Start();
 			 	__LOG("Static Trace ID: ")
 			 	__LOGNUM(tracetag)	 
				ret = 0;
				timeDiff=0;
				for(int i=0;i!=StatAverage;i++)
				{
					User::After(Math::Random()%100000);
					prevTime = User::FastCounter();
					for(int i=0;i!=UTrace_Cache_Count;i++)
						ret = Testtracer.DoTheStaticTrace(tracetag);
					timeDiff += User::FastCounter() - prevTime;
				}
				timeSpentToTrace = timeDiff/ticksPerMicroSec/StatAverage;
				INFO_PRINTF5(_L("Utrace macro %d trace, size %d (%d trace(s))= %10.6lf microseconds"), tracetag, Testtracer.SizeOfTrace(tracetag), UTrace_Cache_Count, timeSpentToTrace);
				INFO_PRINTF2(_L("Time per trace = %10.6e microseconds"),timeSpentToTrace/UTrace_Count );
				logger.Stop();
			}
			if(ret==0)
				INFO_PRINTF1(_L("tracing failed from UTrace"));
#endif	//file
	
			///////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////
			////////////////////////SERIAL/////////////////////////////
			///////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////
		
			INFO_PRINTF1(_L("Now benchmark trace logging for the serial plug-in"));
			// adding a plugin
			_LIT8(KSerialTextmedia,"uloggerserialplugin");
			TPtrC8 serialmediaptr(KSerialTextmedia);
			Result=logger.ActivateOutputPlugin(serialmediaptr);
			if(Result==0)			
			{	
			// test the added plugin can be activated
				if(Result==0||Result==-11)	
				{	
					INFO_PRINTF1(_L("Serial Plugin active"));
					_LIT(KSerialTextsetting, "output_port");	
					_LIT(KSerialTextvalue, "3");
					// test setting the plugin settings
					TPluginConfiguration setSerialPluginConfigs;
					setSerialPluginConfigs.SetKey(KSerialTextsetting);
					setSerialPluginConfigs.SetValue(KSerialTextvalue);				
					Result=logger.SetPluginConfigurations(serialmediaptr, setSerialPluginConfigs);
					if(Result==0)	
					{
						INFO_PRINTF1(_L("Serial Plugin Settings set correctly"));
					}
				}
				else
					INFO_PRINTF1(_L("Serial plugin not set as active plugin"));
			}
			__LOG("Trace times for 10,000 user side nonstatic traces logging to serial plugin");
			/***Now carry out performance benchmarking and tracing for nonstatic set method to serial****/
#ifdef SERIAL_PERF
#ifndef __WINSCW__			
			for(int tracetag=0;tracetag!=NumberOfUTraceMacros;tracetag++)
			{
				logger.Start();
 			 	__LOG("NONStatic Trace ID: ")
 			 	__LOGNUM(tracetag)	 
				ret = 0;
				timeDiff=0;
				for(int i=0;i!=StatAverage;i++)
				{
					User::After(Math::Random()%100000);
					prevTime = User::FastCounter();
					for(int i=0;i!=UTrace_Cache_Count;i++)
						ret = Testtracer.DoTheSetTrace(tracetag, TestTraceObject);
					timeDiff += User::FastCounter() - prevTime;
				}
				timeSpentToTrace = timeDiff/ticksPerMicroSec/StatAverage;
				INFO_PRINTF5(_L("Utrace macro %d trace, size %d (%d trace(s))= %10.6lf microseconds"), tracetag, Testtracer.SizeOfTrace(tracetag), UTrace_Cache_Count, timeSpentToTrace);
				INFO_PRINTF2(_L("Time per trace = %10.6e microseconds"),timeSpentToTrace/UTrace_Count );
				logger.Stop();
			}
			if(ret==0)
			INFO_PRINTF1(_L("tracing failed from UTrace"));
			__LOG("Trace times for 10,000 user side static traces logging to serial plugin");
				for(int tracetag=0;tracetag!=NumberOfUTraceMacros;tracetag++)
			{
				logger.Start();
 			 	__LOG("Static Trace ID: ")
 			 	__LOGNUM(tracetag)	 
				ret = 0;
				timeDiff=0;
				for(int i=0;i!=StatAverage;i++)
				{
					User::After(Math::Random()%100000);
					prevTime = User::FastCounter();
					for(int i=0;i!=UTrace_Cache_Count;i++)
						ret = Testtracer.DoTheStaticTrace(tracetag);
					timeDiff += User::FastCounter() - prevTime;
				}
				timeSpentToTrace = timeDiff/ticksPerMicroSec/StatAverage;
				INFO_PRINTF5(_L("Utrace macro %d trace, size %d (%d trace(s))= %10.6lf microseconds"), tracetag, Testtracer.SizeOfTrace(tracetag), UTrace_Cache_Count, timeSpentToTrace);
				INFO_PRINTF2(_L("Time per trace = %10.6e microseconds"),timeSpentToTrace/UTrace_Count );
				logger.Stop();
			}
			if(ret==0)
			INFO_PRINTF1(_L("tracing failed from UTrace"));
#endif //winscw
#endif //serial
			//  **************   Block end ****************
			if(ret==0||Result==1)
				SetTestStepResult(EFail);
			else
				SetTestStepResult(EPass);
		}
	 	return TestStepResult();
	}



TVerdict CULoggerMCLUserPerfStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Completed performance testing on user side tracing for file and serial plugins"));
		return TestStepResult();
	}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//////////////////////////////////KERNEL SIDE///////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


CULoggerMCLKernelPerfStep::~CULoggerMCLKernelPerfStep()
/**
 * Destructor
 */
	{
	}

CULoggerMCLKernelPerfStep::CULoggerMCLKernelPerfStep()
/**
 * Constructor
 */
	{
		SetTestStepName(KULoggerMCLKernelPerfStep);
	}

TVerdict CULoggerMCLKernelPerfStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Carrying out performance testing on kernel side tracing for file and serial plugins"));
		SetTestStepResult(EPass);
		return TestStepResult();
	}


TVerdict CULoggerMCLKernelPerfStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
		if (TestStepResult()==EPass)
		{
			//  ************** Delete the Block, the block start ****************
#ifndef __WINSCW__			
			INFO_PRINTF1(_L("Benchmark trace logging for the file plug-in"));
			 __CREATE_LOG(true);
			 __HIRES_RESOLUTION;
			TInt fastTimerFreq;
			HAL::Get(HALData::EFastCounterFrequency, fastTimerFreq);
			TReal ticksPerMicroSec = 1.0E-6 * fastTimerFreq;	
			TUint32 prevTime;
			TUint32 timeDiff;
			TReal timeSpentToTrace;	
			CUKernelTraces TestKerneltracer;
			TInt Result=0;
			/***Now set file plugins and filter settings****/
			RULogger logger;
			logger.Connect();
			CClearConfig configIni;	
			configIni.ClearL(logger);		
			CArrayFixFlat<TUint8> *setprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
			setprimfilter->AppendL(UTracePrimary);
			RArray<TUint32> setsecondfilter;
			setsecondfilter.Append((TUint32) UTraceSecondary);		
			logger.SetPrimaryFiltersEnabled(*setprimfilter,ETrue);
			logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);
			/**Load usb device**/
			TInt ret = KErrNone;
			ret =User::LoadLogicalDevice(_L("eusbc.ldd"));
			if(ret == KErrAlreadyExists)
			ret = KErrNone;	
			RDevUsbcClient usbDevice;
			usbDevice.Open(0);	
	
			///////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////
			/////////////////////////FILE//////////////////////////////
			///////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////
		
			INFO_PRINTF1(_L("about to set file as output plugin"));
			//NB: this of course assumes that the plugin functionality works which is tested more rigorously in the plugin test step
			if(Result==0)	
			{
				INFO_PRINTF1(_L("file set as output plugin ok"));
				_LIT8(KTextmedia,"uloggerfileplugin");
				TPtrC8 mediaptr(KTextmedia);
				Result=logger.ActivateOutputPlugin(mediaptr);				
				if(Result==0||Result==-11)
				{	
					INFO_PRINTF1(_L("file set as active output plugin ok"));
					_LIT(KTextsetting, "output_path");
					_LIT(KTextvalue, "c:\\logs\\ULoggerPerfKernelSideFileLogging.log");
					TPluginConfiguration setPluginConfigs;
					setPluginConfigs.SetKey(KTextsetting);
					setPluginConfigs.SetValue(KTextvalue);				
					Result=logger.SetPluginConfigurations(mediaptr, setPluginConfigs);
					if(Result==0)
						INFO_PRINTF1(_L("output plugin settings set ok"));
					else
						INFO_PRINTF1(_L("setting output plugin settings failed"));
				}
				else
					INFO_PRINTF1(_L("file not set as active output plugin - failed"));
			}
			else
				INFO_PRINTF1(_L("add file as output plugin failed"));
			/***Now carry out performance benchmarking and tracing for NonStatic SET method to file****/
			__LOG("Trace times for 10,000 kernel side nonstatic set traces logging to file plugin");
#ifdef FILE_PERF	
			for(int tracetag=0;tracetag!=NumberOfUTraceMacros;tracetag++)
			{
				logger.Start();
			 	__LOG("NONStatic Trace ID: ")
			 	__LOGNUM(tracetag)	 
				ret = 0;
				timeDiff=0;
				for(int i=0;i!=StatAverage;i++)
				{
					User::After(Math::Random()%100000);
					prevTime = User::FastCounter();
					for(int i=0;i!=UTrace_Cache_Count;i++)
						ret = TestKerneltracer.DoTheKernelSetTrace(tracetag, usbDevice);
					timeDiff += User::FastCounter() - prevTime;
				}
				timeSpentToTrace = timeDiff/ticksPerMicroSec/StatAverage;
				INFO_PRINTF5(_L("Utrace macro %d trace, size %d (%d trace(s))= %10.6lf microseconds"), tracetag, TestKerneltracer.SizeOfKernelTrace(tracetag), UTrace_Cache_Count, timeSpentToTrace);
				INFO_PRINTF2(_L("Time per trace = %10.6e microseconds"),timeSpentToTrace/UTrace_Count );
				logger.Stop();
			}
			if(ret==0)
				INFO_PRINTF1(_L("device driver returned error, but tracing should still be carried out"));
			/***Now carry out performance benchmarking and tracing for Static method to file****/
			__LOG("Trace times for 10,000 kernel side static traces logging to file plugin");
			for(int tracetag=0;tracetag!=NumberOfUTraceMacros;tracetag++)
			{
				logger.Start();
			 	__LOG("Static Trace ID: ")
			 	__LOGNUM(tracetag)	 
				ret = 0;
				timeDiff=0;
				for(int i=0;i!=StatAverage;i++)
				{
					User::After(Math::Random()%100000);
					prevTime = User::FastCounter();
					for(int i=0;i!=UTrace_Cache_Count;i++)
						ret = TestKerneltracer.DoTheKernelStaticTrace(tracetag, usbDevice);
					timeDiff += User::FastCounter() - prevTime;
				}
				timeSpentToTrace = timeDiff/ticksPerMicroSec/StatAverage;
				INFO_PRINTF5(_L("Utrace macro %d trace, size %d (%d trace(s))= %10.6lf microseconds"), tracetag, TestKerneltracer.SizeOfKernelTrace(tracetag), UTrace_Cache_Count, timeSpentToTrace);
				INFO_PRINTF2(_L("Time per trace = %10.6e microseconds"),timeSpentToTrace/UTrace_Count );
				logger.Stop();
			}
			if(ret==0)
				INFO_PRINTF1(_L("device driver returned error, but tracing should still be carried out"));
#endif	//file
				

		///////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////
		////////////////////////SERIAL/////////////////////////////
		///////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////
		
			INFO_PRINTF1(_L("Now benchmark trace logging for the serial plug-in"));
			_LIT8(KSerialTextmedia,"uloggerserialplugin");
			TPtrC8 serialmediaptr(KSerialTextmedia);
			Result=logger.ActivateOutputPlugin(serialmediaptr);				
			if(Result==0||Result==-11)			
			{	
				// test the added plugin can be activated			
				if(Result==0)	
				{	
					INFO_PRINTF1(_L("Serial Plugin active"));
					_LIT(KSerialTextsetting, "output_port");
					_LIT(KSerialTextvalue, "3");
					// test setting the plugin settings
					TPluginConfiguration setSerialPluginConfigs;
					setSerialPluginConfigs.SetKey(KSerialTextsetting);
					setSerialPluginConfigs.SetValue(KSerialTextvalue);
					Result=logger.SetPluginConfigurations(serialmediaptr, setSerialPluginConfigs);
					if(Result==0)	
					{
						INFO_PRINTF1(_L("Serial Plugin Settings set correctly"));
					}	
				}
				else
					INFO_PRINTF1(_L("Serial plugin not set as active plugin"));
			}
			else
				INFO_PRINTF1(_L("Serial plugin not added as output plugin"));
			/***Now carry out performance benchmarking and tracing for NonStatic SET method to serial****/
			__LOG("Trace times for 10,000 kernel side nonstatic set traces logging to serial plugin");
#ifdef SERIAL_PERF
			for(int tracetag=0;tracetag!=NumberOfUTraceMacros;tracetag++)
			{
				logger.Start();
			 	__LOG("NONStatic Trace ID: ")
			 	__LOGNUM(tracetag)	 
				ret = 0;
				timeDiff=0;
				for(int i=0;i!=StatAverage;i++)
				{
					User::After(Math::Random()%100000);
					prevTime = User::FastCounter();
					for(int i=0;i!=UTrace_Cache_Count;i++)
						ret = TestKerneltracer.DoTheKernelSetTrace(tracetag, usbDevice);
					timeDiff += User::FastCounter() - prevTime;
				}
				timeSpentToTrace = timeDiff/ticksPerMicroSec/StatAverage;
				INFO_PRINTF5(_L("Utrace macro %d trace, size %d (%d trace(s))= %10.6lf microseconds"), tracetag, TestKerneltracer.SizeOfKernelTrace(tracetag), UTrace_Cache_Count, timeSpentToTrace);
				INFO_PRINTF2(_L("Time per trace = %10.6e microseconds"),timeSpentToTrace/UTrace_Count );
				logger.Stop();
			}
			if(ret==0)
				INFO_PRINTF1(_L("device driver returned error, but tracing should still be carried out"));
			/***Now carry out performance benchmarking and tracing for Static method to file****/
			__LOG("Trace times for 10,000 kernel side static traces logging to serial plugin");
			for(int tracetag=0;tracetag!=NumberOfUTraceMacros;tracetag++)
			{
				logger.Start();
			 	__LOG("Static Trace ID: ")
			 	__LOGNUM(tracetag)	 
				ret = 0;
				timeDiff=0;
				for(int i=0;i!=StatAverage;i++)
				{
					User::After(Math::Random()%100000);
					prevTime = User::FastCounter();
					for(int i=0;i!=UTrace_Cache_Count;i++)
						ret = TestKerneltracer.DoTheKernelStaticTrace(tracetag, usbDevice);
					timeDiff += User::FastCounter() - prevTime;
				}
				timeSpentToTrace = timeDiff/ticksPerMicroSec/StatAverage;
				INFO_PRINTF5(_L("Utrace macro %d trace, size %d (%d trace(s))= %10.6lf microseconds"), tracetag, TestKerneltracer.SizeOfKernelTrace(tracetag), UTrace_Cache_Count, timeSpentToTrace);
				INFO_PRINTF2(_L("Time per trace = %10.6e microseconds"),timeSpentToTrace/UTrace_Count );
				logger.Stop();
			}
			if(ret==0)
				INFO_PRINTF1(_L("device driver returned error, but tracing should still be carried out"));
#endif	//serial
			//  **************   Block end ****************
			if(Result==0)
				SetTestStepResult(EPass);
			else
				SetTestStepResult(EFail);
#endif //winscw
#ifdef __WINSCW__
		INFO_PRINTF1(_L("Kernel tests not run on emulator"));			
#endif
		}
	  	return TestStepResult();
	}



TVerdict CULoggerMCLKernelPerfStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
		INFO_PRINTF1(_L("Completed performance testing on kernel side tracing for file and serial plugins"));
		return TestStepResult();
	}
