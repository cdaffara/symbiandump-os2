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
// EFM test steps for persistence testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __UNITSTEP__
#define __UNITSTEP__

#include <test/testexecutestepbase.h>

/** Persistence tests.
*/
_LIT(KPersistenceStoreSettingsTest, "PersistenceStoreSettingsTest");     
class CPersistenceStoreSettingsTestStep: public CTestStep
   {
public:
CPersistenceStoreSettingsTestStep();
   virtual TVerdict doTestStepL(void);   
   };
   
_LIT(KPersistenceLoadSettingsTest, "PersistenceLoadSettingsTest");        
class CPersistenceLoadSettingsTestStep: public CTestStep
   {
public:
   CPersistenceLoadSettingsTestStep();
   virtual TVerdict doTestStepL(void);   
   };
   
_LIT(KDeleteFeatureStep, "DeleteFeatureStep");
class CDeleteFeatStep : public CTestStep
   {
public:
   CDeleteFeatStep();	
   virtual TVerdict doTestStepL(void);
   };
#endif
