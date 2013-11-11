// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_EComPluginReDiscoverPerfTest.h
// Declaration for class to test the performance of ECom plugin re-discovery.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef	TE_ECOMPLUGINREDISCOVERPERFTESTSTEP_H
#define	TE_ECOMPLUGINREDISCOVERPERFTESTSTEP_H

#include <e32cmn.h>
#include "Te_EComPerfTestStep.h"
#include "EComSessionAux.h"

_LIT(KEComPluginReDiscoverPerfTest, "EComPluginReDiscoverPerfTest");

class CEComPluginReDiscoverPerfTest : public CEComPerfTestBase
	{
public:
	CEComPluginReDiscoverPerfTest();
	~CEComPluginReDiscoverPerfTest();
	virtual TVerdict doTestStepL(void);
	// doTestStepPreamble and doTestStepPostamble are implemented in CEComPerfTestBase, 
	// which performs initialization common to all ECom performance tests	
private:
#ifdef __ECOM_SERVER_PERFORMANCE__
	void SetBenchmarksL();
	TBool CheckTimeResults(TReal aTimeLimit, RArray<TEComPerfRealTimeResult>& aTimeResults);
	void CopyPluginCR629TestRSCOnDrive(TUint aDriveNum, TInt aNumOfCopies);
	void DeleteRSCFilesOnDrive(TUint aDriveNum);
	void DeleteRSCFolderOnDrive(TUint aDriveNum);
#endif //__ECOM_SERVER_PERFORMANCE__
	};
	
#endif //TE_ECOMSTARTUPSTATEPERFTESTSTEP_H
