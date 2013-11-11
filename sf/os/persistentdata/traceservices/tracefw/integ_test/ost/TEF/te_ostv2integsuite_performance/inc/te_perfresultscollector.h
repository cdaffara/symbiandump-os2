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
// Ost Performance Tests Results Collector.
//



/**
 @file te_perfresultscollector.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTRESULTSCOLLECTOR_H
#define TE_UPTRESULTSCOLLECTOR_H

#include <e32base.h>
#include <f32file.h>
#include "te_perfapicall.h"
#include "te_perfplugin.h"


/**
This is a user-side utility class to store the api results, the output plugin results, the throughput results, 
the configuration settings for a particular set of results and to write results using the CCsvGenerator class  

@internalTechnology
@prototype
 */

class CUptResults: public CBase
	{
public:
	CUptResults();
	~CUptResults();
	
	void StoreApiResultsL(const TApiTestResult& aApiTest); 
	void StorePluginResultsL(const CPluginTestResult& aPluginTest);

	void WriteUptResultsL(const TDesC& aFileName, const TBool& aAppend, const TInt& aTestType);

	TInt Test(); 
private:
	RPointerArray<TApiTestResult> iApiTest;
	RPointerArray<CPluginTestResult> iPluginTest;
	
	};

#endif


