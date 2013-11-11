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

#include <f32file.h>
#include <bautils.h>
#include <startup.hrh>
#include "TestHarnessDomainMember.h"

const TInt KInitialTestState = EStartupStateUndefined + 1;
_LIT(KIniEcomTestBehaviourFileName, "c:\\EComTestBehaviour.ini");

/**
Constructor.
Adds this active object to the active scheduler. The priority of the active object
is the standard value, i.e. CActive::EPriorityStandard.
@param aHierarchyId The Id of the domain hierarchy to connect to.
@param aDomainId    The Id of the domain to connect to.
@see CActive
@see CActive::TPriority
*/
CDmDomainTestHarness::CDmDomainTestHarness(TDmHierarchyId aHierarchyId, TDmDomainId aDomainId):
	CDmDomain(aHierarchyId, aDomainId),
    iIsRequestSent(EFalse),
    iIsLastStateAcknowledged(ETrue),
	iState(KInitialTestState),
	iIsTestBehaviour(EFalse),
	iIsBehaviourSet(EFalse),
	iIsRequestCancelled(EFalse)
	{
	}
	
/**
Destructor.
Closes the session to the domain manager.
*/
CDmDomainTestHarness::~CDmDomainTestHarness()
	{
	
	}
	
/**
Requests notification when the domain's state changes.
RunL() will be called when this happens.
*/
void CDmDomainTestHarness::RequestTransitionNotificationL()
	{
	if(!IsTestBehaviour())
		{
		CDmDomain::RequestTransitionNotification();
		}
	else
		{
		if((iIsRequestSent)&& (!iIsRequestCancelled))
			{
			//If the request has already been sent and there is
			//another request there's a serious problem in the
			//production code.
			User::Leave(KErrGeneral);
			}

		iIsRequestSent = ETrue;
		}
	}
	
/**
Acknowledges the last state change.
An application must acknowledge that it has performed all actions required
by the last known state of the domain.
@param aError   The error to return to the domain manager. The client should
                set this to KErrNone if it successfully transitioned to the 
                new state or to one of the system-wide error codes.
*/
void CDmDomainTestHarness::AcknowledgeLastStateL(TInt aError)
	{
	if(!IsTestBehaviour())
		{
		CDmDomain::AcknowledgeLastState(aError);
		}
	else
		{
		if(!iIsLastStateAcknowledged)
			{
			//If the last state is acknowledged when it wasn't expected
			//in the first place there's a serious problem in the
			//production code.
			User::Leave(KErrGeneral);
			}
		iIsLastStateAcknowledged = ETrue;
		}
	}

/**
Gets the domain's state.
For this test implementation it only returns the member value
@return The connected domain's state.
*/
TDmDomainState CDmDomainTestHarness::GetState()
	{
	if(!IsTestBehaviour())
		{
		return CDmDomain::GetState();
		}
	else
		{
		return iState;
		}
	}
/**
it has to use Cancel() to invoke DoCancel() since the Active object
is not activated in Test Mode.
*/
void CDmDomainTestHarness::Cancel()
{
	if(!IsTestBehaviour())
		{
		CActive::Cancel();
		}
	else
		{
		DoCancel();
		}
}


/**
If not test behaviour passes the call to CDmDomain. Otherwise
disables the call.
*/
void CDmDomainTestHarness::DoCancel()
	{
	if(!IsTestBehaviour())
		{
		CDmDomain::DoCancel();	
		}
	else
		{
		iIsRequestCancelled = ETrue;
		}
	}

/**
If not test behaviour passes the call to CDmDomain. Otherwise
disables the call.
*/
void CDmDomainTestHarness::ConstructL()
	{
	if(!IsTestBehaviour())
		{
		CDmDomain::ConstructL();
		}
	}

/**
Sets the domain's state
@param aState the new state
*/
void CDmDomainTestHarness::ChangeStartupStateL(TInt aState)
	{
	if(!IsTestBehaviour())
		{
		User::Leave(KErrNotSupported);
		}
	else
		{
		iState = aState;
		}
	}

/**
Resets the iIsRequestSent member. The member is used as a flag
to check if RequestTransitionNotificationL() has been called.
*/
void CDmDomainTestHarness::ResetRequestTransitionNotificationL()
    {
	if(!IsTestBehaviour())
		{
		User::Leave(KErrNotSupported);
		}
	else
		{
	    iIsRequestSent = EFalse;
	    iIsRequestCancelled = EFalse;
		}
    }

/**
Checks if RequestTransitionNotificationL() has been called since 
iIsRequestSent member is last reset.
*/
TBool CDmDomainTestHarness::IsTransitionNotificationRequestedL()
	{
	if(!IsTestBehaviour())
		{
		User::Leave(KErrNotSupported);
		}
	return iIsRequestSent;
	}

/**
Resets the iIsLastStateAcknowledged member. The member is used as a flag
to check if AcknowledgeLastStateL() has been called.
*/
void CDmDomainTestHarness::ResetLastStateAcknowledgedL()
	{
	if(!IsTestBehaviour())
		{
		User::Leave(KErrNotSupported);
		}
	else
		{
		iIsLastStateAcknowledged = ETrue;
		}
	}
	
/**
Checks if AcknowledgeLastStateL() has been called since 
iIsLastStateAcknowledged member is last reset.
*/
TBool CDmDomainTestHarness::IsLastStateAcknowledgedL()
	{
	if(!IsTestBehaviour())
		{
		User::Leave(KErrNotSupported);
		}
	return iIsLastStateAcknowledged;
	}
	
/**
Checks to see if the class should follow test behaviour
or just act as a dummy intermediate class which passes
all the calls directly to the base class CDmDomain.
The nature of the behaviour is defined by the existence
of KIniEcomTestBehaviourFileName.
*/
TBool CDmDomainTestHarness::IsTestBehaviour()
	{
	if(!iIsBehaviourSet)	
		{
		RFs fsSession;
		TInt err = fsSession.Connect();
		//error in file sever connection.
		if(err)
			{
			iIsTestBehaviour = EFalse;
			}
		else
			{
			iIsTestBehaviour = BaflUtils::FileExists(fsSession, KIniEcomTestBehaviourFileName);
			fsSession.Close();
			}
		iIsBehaviourSet = ETrue;
		}
		
		return iIsTestBehaviour;
	}

/**
Checks if RequestTransitionNotificationL() has been cancelled since 
iIsRequestCancel member is last reset.
*/
TBool CDmDomainTestHarness::IsRequestNotificationCancelledL()
	{
	if(!IsTestBehaviour())
		{
		User::Leave(KErrNotSupported);
		}
	return iIsRequestCancelled;
	}
