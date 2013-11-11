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
// EFM test steps for component testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __COMPONENTSTEP__
#define __COMPONENTSTEP__

#include "efm_teststepbase.h"
#include <featmgr/featurenotifier.h>
#include "efm_featurenotifierstepbase.h"

/** Multiconnection test - we provide 2 test steps that 
 *  will be executed concurrentlly to test multiple connections 
 *  from multiple threads.
*/
_LIT(KMulticonnectionTest1, "MulticonnectionTest1");     
class CMulticonnectionTestStep1: public CTestStep
   {
public:
   CMulticonnectionTestStep1();
   virtual TVerdict doTestStepL(void);   
   };

_LIT(KMulticonnectionTest2, "MulticonnectionTest2");     
class CMulticonnectionTestStep2: public CTestStep
   {
public:
   CMulticonnectionTestStep2();
   virtual TVerdict doTestStepL(void);   
   };   
   
_LIT(KPluginIgnoringTest, "PluginIgnoringTest");     
class CPluginIgnoringTestStep: public CTestStep
   {
public:
   CPluginIgnoringTestStep();
   virtual TVerdict doTestStepL();   
   };
   
/*This class is used in the CFeatMgrStressTest test case*/
class CNotifierObserver : public MFeatureObserver
   {
public:
   virtual void HandleNotifyChange( TFeatureChangeType, TFeatureEntry);
   virtual void HandleNotifyError( TInt );
   };
         	
_LIT(KFeatMgrStressTest, "FeatMgrStressTest");
class CFeatMgrStressTest : public CTestStep
   {
public:
   struct TThreadTestInfo
   {
      TInt iId;
      TInt iStartServerErr;
      TInt iAddFeatErr;
      TInt iSetFeatErr;
      TInt iDetailsFeatErr;	
      TInt iDeleteFeatErr;
      TInt iRequestNotif;
      TInt iSetFeatNotif;
      TInt iNotifReceived;
         		
      TThreadTestInfo()
      	 {
         iId = KErrArgument;
         iStartServerErr = KErrArgument;
         iAddFeatErr = KErrArgument;
         iSetFeatErr = KErrArgument;
         iDetailsFeatErr = KErrArgument;	
         iDeleteFeatErr = KErrArgument;
         iRequestNotif = KErrArgument;
         iSetFeatNotif = KErrArgument; 
         iNotifReceived = KErrArgument;
         }
   };	

public:
   CFeatMgrStressTest();
   ~CFeatMgrStressTest();
   virtual TVerdict doTestStepL();
private:	
   static TInt ThreadeEntryPoint( TAny* );
   void DoThreadActionL();
private:
   RArray<RThread> iThreadsArr;
   RArray<TThreadTestInfo> iThreadsInfoArr;
   TInt iThreadsCount;
   RCriticalSection iSection;
   };
       
/* Multiple notification test - there are 2 classes that implement
 * the HandleNotifyChange from MFeatureObserver to demonstrate that
 * each object of the two classes gets its HandleNotifyChange
 * implementation called in the right behavior.
*/


class MNotificationOrderVerifier
	{
public:	
    virtual void OnNotification(TInt aObjectID, TInt aFeatureID) = 0;
	};

class CNotificationHandler : public MFeatureObserver
	{
public:
	enum{ EHandler1, EHandler2 };
	CNotificationHandler(TInt aID, MNotificationOrderVerifier* aNotificationOrderVerifier);	
    virtual void HandleNotifyChange( TFeatureChangeType aType, TFeatureEntry);
    virtual void HandleNotifyError( TInt aError ); 
private:
	MNotificationOrderVerifier* iNotificationOrderVerifier;
	TInt iID;
	};

_LIT( KMultipleNotificationRequests, "MultipleNotificationRequests" );
class CMultipleNotificationRequestsStep : public CTestStep, public MNotificationOrderVerifier
	{
public:
	CMultipleNotificationRequestsStep();
    virtual TVerdict doTestStepPreambleL(void);
    virtual TVerdict doTestStepPostambleL(void);
    virtual TVerdict doTestStepL(void);
    virtual void OnNotification(TInt aObjectID, TInt aFeatureID);
private:
	CNotificationHandler* notificationHandler1;
	CNotificationHandler* notificationHandler2;
	TInt iNotificationCounter;
    };
    
 _LIT(KFeatmgrStartEndInstall, "FeatmgrStartEndInstall");     
 class CFeatmgrStartEndInstall: public CTestStep
    {
 public:
    CFeatmgrStartEndInstall();
    virtual TVerdict doTestStepL(void);
    };
    
#endif
