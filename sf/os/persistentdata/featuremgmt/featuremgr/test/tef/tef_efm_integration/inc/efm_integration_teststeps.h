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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
#ifndef __INTEGRATIONTESTS__
#define __INTEGRATIONTESTS__

#include "efm_teststepbase.h"

const TUid KTestSupportedROMUid = { 0x20279806 };
const TInt KDefaultData = { 0xabcd };

_LIT(KSingleRomSectionTest, "SingleRomSectionTest");
class CSingleRomSectionTestStep : public CTestStep
{
public:
   CSingleRomSectionTestStep();
   virtual TVerdict doTestStepL(void);	      
};   

_LIT(KMultipleRomSectionsTest, "MultipleRomSectionsTest");
class CMultipleRomSectionsTestStep : public CTestStep
{
public:
   CMultipleRomSectionsTestStep();
   virtual TVerdict doTestStepL(void);	      
};

_LIT(KFeatRegVerificationTest, "FeatRegVerificationTest");
class CFeatRegVerificationTestStep : public CTestStep
{
public:
	CFeatRegVerificationTestStep();
	virtual TVerdict doTestStepL(void);	      
};
#endif
