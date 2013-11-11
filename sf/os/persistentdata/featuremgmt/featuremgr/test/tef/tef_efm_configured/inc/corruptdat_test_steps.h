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
// EFM test steps for checking behaviour with corrupted feature set data files.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CORRUPTDATTESTSTEPS__
#define __CORRUPTDATTESTSTEPS__

#include <test/testexecutestepbase.h>

_LIT(KCorruptFeatDatTest, "CorruptFeatDatTest");
class CCorruptFeatDatFileStep : public CTestStep
   {
public:
   CCorruptFeatDatFileStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KMissingDatFileStep, "MissingDatFileTest");
class CMissingDatFileStep : public CTestStep
   {
public:
   CMissingDatFileStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KMissingDSRDatFileStep, "MissingDSRDatFileTest");
class CMissingDSRDatFileStep : public CTestStep
   {
public:
   CMissingDSRDatFileStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KOverlappingDSRFeatDatTest, "OverlappingDSRFeatDatTest");
class COverlappingDSRFeatDatFileStep : public CTestStep
   {
public:
   COverlappingDSRFeatDatFileStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KCorruptRuntimeFeaturesStep,"CorruptRuntimeFeaturesStep");
class CCorruptRuntimeFeaturesStep : public CTestStep
	{
public:
	CCorruptRuntimeFeaturesStep();
	~CCorruptRuntimeFeaturesStep();
	virtual TVerdict doTestStepL();	
	};



          
#endif // __CORRUPTDATTESTSTEPS__

