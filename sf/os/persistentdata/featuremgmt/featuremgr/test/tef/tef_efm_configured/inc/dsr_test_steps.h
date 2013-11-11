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
// EFM test steps for DSR feature handling
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __DSRTESTSTEPS__
#define __DSRTESTSTEPS__

#include <test/testexecutestepbase.h>

_LIT(KDSRTest, "DSRTest");
class CDSRTestStep : public CTestStep
   {
public:
   CDSRTestStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KDSRBoundsTest, "DSRBoundsTest");
class CDSRBoundsTestStep : public CTestStep
   {
public:
   CDSRBoundsTestStep();
   virtual TVerdict doTestStepL(void);
   };
          
#endif


                                        
