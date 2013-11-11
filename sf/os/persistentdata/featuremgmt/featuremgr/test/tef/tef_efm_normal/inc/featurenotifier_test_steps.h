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
// Feature Notifier component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __FEATURENOTIFIERTESTSTEP__
#define __FEATURENOTIFIERTESTSTEP__

#include <test/testexecutestepbase.h>
#include <featmgr/featurecontrol.h>
#include <featmgr/featurenotifier.h>
#include "efm_featurenotifierstepbase.h"

   
_LIT(KFeatureNotifier_NewL, "FeatureNotifier_NewL");
class CFeatureNotifier_NewLStep : public CFeatureNotifierStepBase
   {
public:
   CFeatureNotifier_NewLStep();
   virtual TVerdict doTestStepL(void);
   };

_LIT(KFeatureNotifier_NotifyRequestL, "FeatureNotifier_NotifyRequestL");
class CFeatureNotifier_NotifyRequestLStep : public CFeatureNotifierStepBase
   {
public:
   CFeatureNotifier_NotifyRequestLStep();
   virtual TVerdict doTestStepL(void);
   };
_LIT(KFeatureNotifier_NotifyRequestArrayL, "FeatureNotifier_NotifyRequestArrayL");
class CFeatureNotifier_NotifyRequestArrayLStep : public CFeatureNotifierStepBase
   {
public:
   CFeatureNotifier_NotifyRequestArrayLStep();
   virtual TVerdict doTestStepL(void);
   };
      
   _LIT(KFeatureNotifier_NotifyCancelL, "FeatureNotifier_NotifyCancelL");
class CFeatureNotifier_NotifyCancelLStep : public CFeatureNotifierStepBase
   {
public:
   CFeatureNotifier_NotifyCancelLStep();
   virtual TVerdict doTestStepL(void);
   }; 
   
  _LIT(KFeatureNotifier_NotifyMiltipleCancelL, "FeatureNotifier_NotifyMultipleCancelL");
class CFeatureNotifier_NotifyMiltipleCancel : public CFeatureNotifierStepBase
   {
public:
   CFeatureNotifier_NotifyMiltipleCancel();
   virtual TVerdict doTestStepL(void);
   };      
   
_LIT(KFeatureNotifier_UnchangedFeatureTest, "FeatureNotifier_UnchangedFeatureTest");
class CFeatureNotifier_UnchangedFeatureTestStep : public CFeatureNotifierStepBase
      {
   public:
   CFeatureNotifier_UnchangedFeatureTestStep();
      virtual TVerdict doTestStepL(void);
      };
                                               
_LIT(KFeatureNotifier_CancelFeatureTwiceTest, "FeatureNotifier_CancelFeatureTwiceTest");
class CFeatureNotifier_CancelFeatureTwiceTestStep : public CFeatureNotifierStepBase
      {
    public:
    CFeatureNotifier_CancelFeatureTwiceTestStep();
    virtual TVerdict doTestStepL(void);
      };      

_LIT(KFeatureNotifier_NonExistingFeatureNotificationTest, "FeatureNotifier_NonExistingFeatureNotificationTest");
class CFeatureNotifier_NonExistingFeatureNotificationTestStep : public CFeatureNotifierStepBase
    {
     public:
     CFeatureNotifier_NonExistingFeatureNotificationTestStep();
     virtual TVerdict doTestStepL(void);
    };      

_LIT(KFeatureNotifier_NonExistingFeatureCancellationTest, "FeatureNotifier_NonExistingFeatureCancellationTest");
class CFeatureNotifier_NonExistingFeatureCancellationTestStep : public CFeatureNotifierStepBase
    {
    public:
    CFeatureNotifier_NonExistingFeatureCancellationTestStep();
    virtual TVerdict doTestStepL(void);
    };      

_LIT(KFeatureNotifier_Thread_1_TestStep, "FeatureNotifier_Thread_1_TestStep");
class CFeatureNotifier_Thread_1_TestStep : public CFeatureNotifierStepBase
    {
    public:
    CFeatureNotifier_Thread_1_TestStep();
    virtual TVerdict doTestStepL(void);
    ~CFeatureNotifier_Thread_1_TestStep();    
    virtual void HandleNotifyChange( TFeatureChangeType aType, TFeatureEntry);
    virtual void HandleNotifyError( TInt ); 
    private:
    TInt iNotificationTypeToExpect;
    TUid iUIDToExpect; 
	CFeatureNotifier* iNotif;
    };     

_LIT(KFeatureNotifier_Thread_2_TestStep, "FeatureNotifier_Thread_2_TestStep");
class CFeatureNotifier_Thread_2_TestStep : public CFeatureNotifierStepBase
    {
    public:
    CFeatureNotifier_Thread_2_TestStep();
    virtual TVerdict doTestStepL(void);
    ~CFeatureNotifier_Thread_2_TestStep();
    virtual void HandleNotifyChange( TFeatureChangeType aType, TFeatureEntry);
    virtual void HandleNotifyError( TInt ); 
    private:
    TInt iNotificationTypeToExpect;
    TUid iUIDToExpect; 
	CFeatureNotifier* iNotif;
    };    

_LIT(KFeatureNotifier_Thread_3_TestStep, "FeatureNotifier_Thread_3_TestStep");
class CFeatureNotifier_Thread_3_TestStep : public CFeatureNotifierStepBase
    {
    public:
    CFeatureNotifier_Thread_3_TestStep();
    virtual TVerdict doTestStepL(void);
    };  
        
#endif
