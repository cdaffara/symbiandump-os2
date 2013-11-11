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

#ifndef __EFMFEATURENOTIFIERSTEPBASE__
#define __EFMFEATURENOTIFIERSTEPBASE__
             
#include <featmgr/featurenotifier.h>
#include <featmgr/featurecontrol.h>
#include <test/testexecutestepbase.h>
#include "efm_teststepbase.h"
#include "efm_test_consts.h"



class TWaitInfo
{
public:
    CPeriodic* iPeriodic;
    CActiveSchedulerWait* iWait;
};

class CFeatureNotifierStepBase : public CEFMConfigurableTestStepBase, public MFeatureObserver
{
public:
   virtual TVerdict doTestStepPreambleL(void);
   virtual TVerdict doTestStepPostambleL(void);
   void HandleNotifyChange( TFeatureChangeType aType, TFeatureEntry aFeature );
   void HandleNotifyError( TInt aError );
   void CheckNotifyRequestResultL(TUid aUid);
   void CheckNotifyCancelResultL(TUid aUid);   
   void CheckDeleteNotificationResultL(TUid aUid);   
protected:
	void WaitL( TInt aIntervalInMicorseconds );
protected:
   CActiveScheduler* iSched;
   CActiveSchedulerWait* iWait;
   TBool iNotifyCompleted;   
   RFeatureControl icontrol;
};




#endif
