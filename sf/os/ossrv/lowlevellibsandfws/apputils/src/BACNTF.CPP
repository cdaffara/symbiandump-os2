// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by BLB, October 1996
// Active object for tracking changes in TLocale
// 
//

#include <bacntf.h>

inline CEnvironmentChangeNotifier::CEnvironmentChangeNotifier(TInt aPriority)
	: CActive(aPriority)
	{__DECLARE_NAME(_S("CEnvironmentChangeNotifier"));}


EXPORT_C CEnvironmentChangeNotifier* CEnvironmentChangeNotifier::NewL(TInt aPriority,const TCallBack& aCallBack)
/** Constructs a new environment change notifier object with the specified active 
object priority and callback function.

The function requires a priority value for this active object and a reference 
to a TCallBack object encapsulating a pointer to the call back function which 
is to run when change events occur.

As part of its implementation, the function:

creates a Kernel side change notifier and opens a handle (an RChangeNotifier) 
to it.

adds this active object to the current active scheduler.

Note that construction of the environment change notifier does not issue any 
requests for change events.

@param aPriority The priority of this active object. Priority values determine 
the order in which an active scheduler handles completed active object requests.
@param aCallBack A reference to a callback object which the caller must construct 
to encapsulate the callback function. 
@return A pointer to the new environment change notifier object.
@see CEnvironmentChangeNotifier::Start()
@see CActive::TPriority */
	{
	CEnvironmentChangeNotifier* This=new(ELeave) CEnvironmentChangeNotifier(aPriority);
	This->iChangeNotifier.Create();
	This->Set(aCallBack);
	CActiveScheduler::Add(This);
	return(This);
	}

EXPORT_C CEnvironmentChangeNotifier::~CEnvironmentChangeNotifier()
/** Destructor. Frees all resources owned by the object, prior to its destruction.

In particular, it cancels any outstanding request to the Kernel side change 
notifier before closing the handle to it. */
	{
	Cancel();
	iChangeNotifier.Close();
	}

EXPORT_C void CEnvironmentChangeNotifier::Start()
/** Issues a request for change events.

The request completes when change events occur, as signalled by the Kernel 
side change notifier service. The request may also complete if it is cancelled 
by calling the Cancel() member function of this active object.

When change events occur, the callback function is called.

Note that after the first call to this function, the callback function is 
called immediately; this is because of the way the underlying change notifier 
is implemented. The changes reported are all those defined by the TChanges 
enum.

@see CActive::Cancel()
@see TChanges */
	{
	SetActive();
	iChangeNotifier.Logon(iStatus);
	}

EXPORT_C TInt CEnvironmentChangeNotifier::Set(const TCallBack& aCallBack)
/** Sets the callback function.

A callback is normally set when this active object is constructed through 
the NewL() function. This function replaces any existing callback object with 
the specified callback object.

@param aCallBack A reference to the call back object encapsulating the call 
back function. 
@return KErrNone if successful, KErrInUse if this active object currently has 
an outstanding request for change events, or another of the system-wide error-codes. */
	{
	if (IsActive())
		return(KErrInUse);
	iCallBack=aCallBack;
	return(KErrNone);
	}

void CEnvironmentChangeNotifier::RunL()
//
//	Active framework: Call the callback function
//
	{
	iChange=iStatus.Int();
	Start();
	if (iChange!=0)
		{
		iCallBack.CallBack();
		iChange=0;
		}
	}

void CEnvironmentChangeNotifier::DoCancel()
//
//	Active framework: Cancel the current request
//	
	{
	iChangeNotifier.LogonCancel();
	}

