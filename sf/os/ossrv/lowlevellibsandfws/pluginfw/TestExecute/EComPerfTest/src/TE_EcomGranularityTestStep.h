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
//

/**
 @file
 @internalComponent
*/
 
#ifndef	TE_ECOMGRANULARITYTESTSTEP_H
#define	TE_ECOMGRANULARITYTESTSTEP_H

#include <test/testexecutestepbase.h>

_LIT(KEComImplIndexPerfTest, "EComImplIndexPerfTest");
        
/** 
Teststep to measure ECOM discovery time at critical static stage
at various settings of granularities of ECOM's implementation
RArray.
*/
class CEComImplIndexPerfTest : public CTestStep
	{
public:
	CEComImplIndexPerfTest();
	~CEComImplIndexPerfTest();
	TVerdict doTestStepL();

private:
	TVerdict ReadConfigParameters();

private:
	TInt iMinInfUidIndexGranularity;
	TInt iMinIimplUidIndexGranularity;
	TInt iMaxInfUidIndexGranularity;
	TInt iMaxIimplUidIndexGranularity;
	TInt iGranStep;
	CActiveScheduler* iScheduler;
	RFs iFs;
	};
	
#endif //TE_ECOMGRANULARITYTESTSTEP_H
