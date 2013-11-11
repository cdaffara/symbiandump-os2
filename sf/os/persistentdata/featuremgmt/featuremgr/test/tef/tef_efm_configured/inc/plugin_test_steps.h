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
// EFM test steps for plugin testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __PLUGINTESTSTEPS__
#define __PLUGINTESTSTEPS__

#include <test/testexecutestepbase.h>

_LIT(KInvalidPluginTest, "InvalidPluginTest");
class CInvalidPluginTest : public CTestStep
	{
public:
	CInvalidPluginTest();
	virtual TVerdict doTestStepL();
	};
	
_LIT(KRefPluginsTest, "RefPluginsTest");
class CRefPluginsTest : public CTestStep
	{
public:
	CRefPluginsTest();
	virtual TVerdict doTestStepL();
	};
	
_LIT(KPluginCapabilitiesTest, "PluginCapabilitiesTest");
class CPluginCapabilitiesTest : public CTestStep
	{
public:
	CPluginCapabilitiesTest();
	virtual TVerdict doTestStepL();
	};

_LIT(KInvalidFlagsPluginTest, "InvalidFlagsPluginTest");
class CInvalidFlagsPluginTest : public CTestStep
   {
public:
   CInvalidFlagsPluginTest();
   virtual TVerdict doTestStepL(void);
   TInt GetInvalidFlag(TInt);
   };
   
_LIT(KCombinationNCPluginTest, "CombinationNCPluginTest");
class CCombinationNCPluginTestStep : public CTestStep
   {
public:
   CCombinationNCPluginTestStep();
   virtual TVerdict doTestStepL(void);
   };
   
_LIT(KCombinationNHPluginTest, "CombinationNHPluginTest");
class CCombinationNHPluginTestStep : public CTestStep
   {
public:
   CCombinationNHPluginTestStep();
   virtual TVerdict doTestStepL(void);
   };
   
_LIT(KSimpleEnhancedPluginTest, "SimpleEnhancedPluginTest");
class CSimpleEnhancedPluginTestStep : public CTestStep
   {
public:
   CSimpleEnhancedPluginTestStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KNormalPluginTest, "NormalPluginTest");
class CNormalPluginTestStep : public CTestStep
   {
public:
   CNormalPluginTestStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KHangingPluginTest, "HangingPluginTest");
class CHangingPluginTestStep : public CTestStep
   {
public:
   CHangingPluginTestStep();
   virtual TVerdict doTestStepL(void);
   };   
   
_LIT(KSlowStartPluginTest, "SlowStartPluginTest");
class CSlowStartPluginTestStep : public CTestStep
   {
public:
   CSlowStartPluginTestStep();
   virtual TVerdict doTestStepL(void);
   };   
   
_LIT(KCorruptPluginTest, "CorruptPluginTest");
class CCorruptPluginTestStep : public CTestStep
   {
   public:
      CCorruptPluginTestStep();
      virtual TVerdict doTestStepL(void);
   };  
   
_LIT(KSimpleMergePluginTestStep, "SimpleMergePluginTestStep");
class CSimpleMergePluginTestStep : public CTestStep
	{
	public:
		CSimpleMergePluginTestStep();
        virtual TVerdict doTestStepL(void);
    };  
          
#endif

