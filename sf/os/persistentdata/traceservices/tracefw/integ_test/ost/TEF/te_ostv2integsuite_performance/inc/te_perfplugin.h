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
// Ost Performance Tests Plugin Test Object
//



/**
 @file te_perfplugin.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTPLUGINTESTS_H
#define TE_UPTPLUGINTESTS_H

#include <e32base.h>
#include <f32file.h>
#include <uloggerclient.h>
#include "te_perf.h"
#include "te_tracecontrolcmds.h"




/**
This is a user-side utility class to test the performance of each of the output plugins (file, serial and usb)
and to store the test results  in the format of a struct
It is flexible enough for the user to be able to test their own plug-in
 

@internalTechnology
@prototype
 */

_LIT8(KOutputPluginHeader, "Plugin Type, Plugin Key, Plugin Value, Test Type, Data Size, Iteration, Latency, ");

class CPluginTestResult : public CBase
	{
	public:
	CPluginTestResult();
	~CPluginTestResult();
	TPtrC8 iPluginName;
	TPluginConfiguration iPluginConfiguration; 
	TPtrC8 iPluginKey;
	TPtrC8 iPluginValue;
	TPtrC8 iTestType;
	TInt32 iThroughput; //in bytes/sec, calculated from the amount of data sent divided by the// difference between the initial and final timestamps
	TInt32 iDataSize; //may not need
	TInt iIteration; 
	TInt64 iLatency; //in nanoseconds-> to be implemented once ULogger instrumented with testcode
	
	TInt Copy(const CPluginTestResult& aPluginTestResult);
	
	private:
	};

class CPluginTests: public CBase
	{
	public:
	CPluginTests();
	~CPluginTests();
	CPluginTestResult* DoTestL(const CPluginRunConfig& aPluginRunConfig);
	TInt Test(); 
	private:														
	//performance test functions
	CPluginTestResult* iPluginTestResult;
	
	};

#endif


