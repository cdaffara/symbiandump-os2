// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TEST_HARNESS_DOMAIN_MEMBER_H__
#define __TEST_HARNESS_DOMAIN_MEMBER_H__

#include <e32base.h>
#include <domainmember.h>
/**
@internalTechnology

Test version of CDmDomain defined in domainmember.h
An class for interfacing to a domain managed by 
the domain manager or the test harness depending on whether
__ECOM_SERVER_TESTABILITY__ macro is defined or not.
This class acts either as a dummy intermediate class which 
passes all the calls directly to the base class CDmDomain or
a class which blocks all the calls to the CDmDomain. The
behaviour is defined based on the existence of 
KIniEcomTestBehaviourFileName
*/
class CDmDomainTestHarness : public CDmDomain
	{
public:
	CDmDomainTestHarness(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId);
	~CDmDomainTestHarness();

	void RequestTransitionNotificationL();
	void AcknowledgeLastStateL(TInt aError);
	TDmDomainState GetState();
	void ChangeStartupStateL(TInt aState);
	
	void ResetRequestTransitionNotificationL();
	TBool IsTransitionNotificationRequestedL();
	void ResetLastStateAcknowledgedL();
	TBool IsLastStateAcknowledgedL();
	TBool IsRequestNotificationCancelledL();
	void Cancel();
protected:
	// from CActive
	void DoCancel();
	void ConstructL();
private:
	TBool IsTestBehaviour();
    TBool iIsRequestSent;
    TBool iIsLastStateAcknowledged;
	TInt iState;
	TInt iIsTestBehaviour;
	TInt iIsBehaviourSet;
	TBool iIsRequestCancelled;
	};

#endif //__TEST_HARNESS_DOMAIN_MEMBER_H__

