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
// ood_test_steps.h
// EFM test steps for DSR feature handling
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __OODTESTSTEPS__
#define __OODTESTSTEPS__

#include <test/testexecutestepbase.h>

_LIT(KOODTest, "OODTest");
class COODTestStep : public CTestStep
   {
public:
   COODTestStep();
   virtual TVerdict doTestStepL(void);
   };
          
#endif


                                        
