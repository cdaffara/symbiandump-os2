// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client observer class
// 
//

#include "UD_STD.H"

// Class RDbNotifier

EXPORT_C TInt RDbNotifier::Open(RDbDatabase& aDatabase)
/** Opens a DBMS change notifier on a database.

Note that a notifier cannot be opened on a client side database.

@param aDatabase The database to be observed.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TRAPD(r,iNotifier=aDatabase.iDatabase->NotifierL());
	return r;
	}
 
EXPORT_C void RDbNotifier::Close()
/** Closes a DBMS change notifier. Any outstanding notification request is automatically 
cancelled.

Note that this function can be called on a DBMS change notifier that has already 
been closed. */
	{
	iNotifier.Close();
	}

//LOCAL_C void Notify(RDbHandle<CDbNotifier>& aNotifier,TRequestStatus& aStatus,CDbNotifier::TType aType)
//	{
//	aNotifier->Notify(aType,aStatus);
//	}

//EXPORT_C void RDbNotifier::NotifyUnlock(TRequestStatus& aStatus)
///** Makes an asynchronous request for notification of a database event. All events, 
//including the release of all read locks, are reported.
//
//When a database event occurs, the request completes and the TRequestStatus 
//objectcontains one of the enumerators defined by the TEvent enumeration member.
//
//Alternatively, if an outstanding request is cancelled by a call to the Cancel() 
//member function of this class, then the request completes with KErrCancel.
//
//Only one notification request can be outstanding on this notifier at any one 
//time.
//
//Note that if further database events occur while a client is handling a request 
//completion, the notifier records the most significant database event and this 
//is signalled as soon as the client issues the next NotifyUnlock() or NotifyChange() 
//request.
//
//@param aStatus The request status object. If the request is cancelled, this 
//is set to KErrCancel. If the request completes normally, this is set to one 
//of the enumerator values defined by the TEvent enumeration member. */
//	{
//	::Notify(iNotifier,aStatus,CDbNotifier::EUnlock);
//	}

//EXPORT_C void RDbNotifier::NotifyChange(TRequestStatus& aStatus)
///** Makes an asynchronous request for notification of changes to the database.
//
//All events which indicate a change to the database are reported; in practice, 
//this means all database events except RDbNotifier::EUnlock.
//
//When a change event occurs, the request completes and the TRequestStatus object 
//contains one of the appropriate enumerators defined by the TEvent enumeration 
//member.
//
//Alternatively, if an outstanding request is cancelled by a call to the Cancel() 
//member function of this class, then the request completes with KErrCancel.
//
//Only one notification request can be outstanding on this notifier at any one 
//time.
//
//Note that if further database events occur while a client is handling a request 
//completion, the notifier records the most significant database event and this 
//is signalled as soon as the client issues the next NotifyUnlock() or NotifyChange() 
//request.
//
//@param aStatus The request status object. If the request is cancelled, this 
//is set to KErrCancel. If the request completes normally, this is set to one 
//of the enumerator values defined by the TEvent enumeration member. */
//	{
//	::Notify(iNotifier,aStatus,CDbNotifier::EChange);
//	}

//EXPORT_C void RDbNotifier::Cancel()
///** Cancels any outstanding notification request to this DBMS notifier.
//
//The outstanding request completes with a KErrCancel. */
//	{
//	iNotifier->Cancel();
//	}
