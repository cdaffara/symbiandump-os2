// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example file/test code to demonstrate how to develop a TestExecute Server
// Developers should take this project as a template and substitute their own
// for (WINS && !EKA2) versions will be xxxServer.Dll and require a thread to be started
// in the process of the client. The client initialises the server by calling the
// one and only ordinal.
//



/**
 @file te_ostv2integsuite_controller.cpp
 @internalTechnology
*/

#include "te_ostv2integsuite_controller.h"
#include "te_configfile.h"
#include "te_tracecontroldefaultconfig.h"
#include "te_tracecontrolclearconfig.h"
#include "te_tracecollectorclient.h"
#include "te_advancedfiltering.h"
#include "te_perf.h"
#include "te_tracecollectorfile.h"
#include "te_tracecollectorserial.h"
#include "te_buffersizing.h"
#include "te_buffermode.h"
#include "te_filepluginerr.h"
#include "te_perfsanity.h"
#include "te_perfoutputsanity.h"
#include "te_ostv2tracetestwrapper.h"


CDataWrapper* CTe_MCLSuiteController::CreateDataL(const TDesC& aData)
	{
	CDataWrapper* wrapper=NULL;
	//API functionality tests
	  if(aData == KTCMGroupIDFilteringWrapper)
		  wrapper = CTCMGroupIDFilteringWrapper::NewL();
	  else if(aData == KTCMComponentIDFilteringWrapper)
		  wrapper = CTCMComponentIDFilteringWrapper::NewL();
	  else if(aData == KTCMAdvancedGroupIDFilteringWrapper)
		  wrapper = CTCMAdvancedGroupIDFilteringWrapper::NewL();
	  else if(aData == KTCMAdvancedComponentIDFilteringWrapper)
		  wrapper = CTCMAdvancedComponentIDFilteringWrapper::NewL();	              
	  else if(aData == KTCMLoggingWrapper)
		  wrapper = CTCMLoggingWrapper::NewL();
	  else if(aData == KTCMKernelLoggingWrapper)
		  wrapper = CTCMKernelLoggingWrapper::NewL();
	  else if(aData == KTCMPrintfLoggingWrapper)
		  wrapper = CTCMPrintfLoggingWrapper::NewL();
	  else if(aData == KTCMKernelPrintfLoggingWrapper)
		  wrapper = CTCMKernelPrintfLoggingWrapper::NewL();
	  else if(aData == KTCMFilePluginWrapper)
		  wrapper = CTCMFilePluginWrapper::NewL();
	  else if(aData == KTCMSerialPluginWrapper)
		  wrapper = CTCMSerialPluginWrapper::NewL();
	  else if(aData == KTCMBufferSizingWrapper)
		  wrapper = CTCMBufferSizingWrapper::NewL();
	  else if(aData == KTCMBufferModeWrapper)
		  wrapper = CTCMBufferModeWrapper::NewL();
	  else if(aData == KClearTraceControlConfigWrapper)
		  wrapper = CClearTraceControlConfigWrapper::NewL();
	  else if(aData == KDefaultTraceControlConfigWrapper)
		  wrapper = CDefaultTraceControlConfigWrapper::NewL();
	  else if(aData == KPublicConfigFileWrapper)
		  wrapper = CPublicConfigFileWrapper::NewL();
	  else if(aData == KPrivateConfigFileWrapper)
		  wrapper = CPrivateConfigFileWrapper::NewL();
	  //negative tests (error code returned from an ulogger plugin) - server shouldn't panic
	  else if(aData == KTCMFilePluginErrWrapper)
		  wrapper = CTCMFilePluginErrWrapper::NewL();
	//Performance tests
	  else if(aData == KTCMPerformanceWrapper)
		  wrapper = CTCMPerformanceWrapper::NewL();
	  else if(aData == KPerformanceSanityWrapper)
		  wrapper = CPerformanceSanityWrapper::NewL();
	  else if(aData == KOutputCSVSanityWrapper)
		  wrapper = COutputCSVSanityWrapper::NewL(); 
	  else if (aData == KOstTraceTestWrapper)
		  wrapper = CTOstv2TraceTestWrapper::NewL();
	return wrapper;
	}
