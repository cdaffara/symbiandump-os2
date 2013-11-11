// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//


/**
 @file te_perf.h
 @internalTechnology
*/

#if (!defined TE_ULOGGERMCLPERFSTEP_H)
#define TE_ULOGGERMCLPERFSTEP_H

#include <test/datawrapper.h>
#include "te_ostv2integsuite_basewrapper.h"
#include <hal.h>
#include <uloggerclient.h>
#include "te_ostv2integsuite_basewrapper.h"
#include "te_lightlogger.h"
#include "te_dataparameters.h"
#include "te_apirunconfig.h"

using namespace Ulogger; //CA:added so can use old ulogger api
_LIT8(KOutputPluginTest, "Output Plugin only");
_LIT8(KThroughputTest, "Full Throughput");

// is it possible to not use CBase at all and have this as a T class?
// Could we move this to a separate file?
class CPluginRunConfig : public CBase
	{
public:
	CPluginRunConfig();//initialise to zero
	~CPluginRunConfig();
	//TInt SetParametersL(const RArray<TInt>& aParameterArray); 
	//run parameters for outputplugin test
	TPtrC8 iPluginName;
	TPluginConfiguration iPluginConfiguration; 
	TPtrC8 iPluginKey;
	TPtrC8 iPluginValue;
	TPtrC8 iTestType; //this specifies whether we are testing the output plugin or full throughput
	TInt32 iDataSize; //this is the amount of data to be sent through the output plugin
	TInt iIteration; ///this can be used to calculate the latency - repeat test over these iterations to give latency over time
	
private:
//private run config members
	};


class CTCMPerformanceWrapper : public CTe_MCLSuiteBaseWrapper
	{
public:
	~CTCMPerformanceWrapper();
	static CTCMPerformanceWrapper* NewL();
	static CTCMPerformanceWrapper* NewLC();
	TAny* GetObject();
	TBool DoCommandL(	const TTEFFunction& aCommand,
						const TTEFSectionName& aSection, 
						const TInt aAsyncErrorIndex);
private:
	CTCMPerformanceWrapper();
	void ConstructL();
	TVerdict DoApiTestsL(const TTEFSectionName& aSection);
	TVerdict DoPluginTestsL(const TInt& aPluginOnly, const TTEFSectionName& aSection); //tried TBool - didn't work for some reason
	TVerdict DoBufferSizeTests();//to be implemented
	TVerdict DoBufferModeTests();//to be implemented
	TApiRunConfig 		iApiRunConfig;
	CPluginRunConfig* 	iPluginRunConfig;
	};

_LIT(KTCMPerformanceWrapper,"TCMPerformance");


#endif
