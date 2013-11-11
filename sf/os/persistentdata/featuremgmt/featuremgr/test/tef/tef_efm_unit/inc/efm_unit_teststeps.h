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
 @internalComponent
*/

#ifndef __UNITSTEP__
#define __UNITSTEP__

#include "efm_teststepbase.h"
#include "featmgrfeatureregistry.h"

class CServerBaseTestStep: public CEFMConfigurableTestStepBase, public MFeatMgrRegistryObserver
	{
public:
	virtual TVerdict doTestStepPreambleL(void);
	virtual TVerdict doTestStepPostambleL(void);
	virtual void HandleFeatureChange( TFeatureServerEntry& /*aFeature*/, TFeatureChangeType /*aType*/ ) 
		{
		}

protected:
	RFs ifs;
	CFeatMgrFeatureRegistry *ifeatureSet;
	};

_LIT(KServerDataTest, "ServerDataTest");
class CServerDataTestStep: public CTestStep
   {
public:
   CServerDataTestStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KServerFeatureLookupTest, "ServerFeatureLookupTest");
class CServerFeatureLookupTestStep: public CServerBaseTestStep
   {
public:
   CServerFeatureLookupTestStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KServerUpdateCodeTest, "ServerUpdateCodeTest");
class CServerUpdateCodeTestStep: public CServerBaseTestStep
   {
public:
	CServerUpdateCodeTestStep();
	virtual TVerdict doTestStepL(void);
   };

_LIT(KServerReadRuntimeFeaturesTest, "ServerReadRuntimeFeaturesTest");
class CServerReadRuntimeFeaturesTestStep: public CServerBaseTestStep
   {
public:
	CServerReadRuntimeFeaturesTestStep();
	virtual TVerdict doTestStepL(void);
   };

_LIT(KServerMergePluginFeaturesTest, "ServerMergePluginFeaturesTest");
class CServerMergePluginFeaturesTestStep: public CServerBaseTestStep
   {
public:
	CServerMergePluginFeaturesTestStep();
	virtual TVerdict doTestStepL(void);
	virtual TVerdict doTestStepPreambleL(void);
	virtual TVerdict doTestStepPostambleL(void);
	RFeatureArray irfa;
   };
   
_LIT(KServerMergePluginFeaturesTest2, "ServerMergePluginFeaturesTest2");
class CServerMergePluginFeaturesTestStep2: public CServerBaseTestStep
   {
public:
	CServerMergePluginFeaturesTestStep2();
	virtual TVerdict doTestStepL(void);
	virtual TVerdict doTestStepPreambleL(void);
	virtual TVerdict doTestStepPostambleL(void);
	RArray<FeatureInfoCommand::TFeature> ifc;
   };
      
/*
   This class is used to start the active scheduler installed by FeatmgrOomSWI
   testcase. The testcase requires the scheduler for Feature Manger timer Active 
   Object to work. 
   
   The timer introduced in this class expires after a set time (150000) which is 
   enough for Feature Manager to complete committing the features. The scheduler 
   will be stopped after that and the testcase continues to exit.
 */
class CControlASTimer : public CTimer 
	{
public:
   	static CControlASTimer* NewL();
   	~CControlASTimer();
   	void Start();
   		
private:
   	void RunL();
   	void DoCancel();
   	CControlASTimer();
   	void ConstructL();
   	};
   	
_LIT(KFeatmgrOomSWI, "FeatmgrOomSWI");     
class CFeatmgrOomSWI: public CServerBaseTestStep
    {
public:
   	CFeatmgrOomSWI();
	virtual TVerdict doTestStepPreambleL(void);
	virtual TVerdict doTestStepPostambleL(void);
    virtual TVerdict doTestStepL(void);   
       
private:
	TInt iIterations;
	RProcess iSWIProcess; 
	CControlASTimer* iAsTimer;
	CActiveScheduler* iActivescheduler;
    };   
    	
#endif
