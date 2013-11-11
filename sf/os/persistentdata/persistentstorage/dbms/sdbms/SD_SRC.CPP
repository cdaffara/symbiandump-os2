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
// DBMS server: data source management and sharing classes
// 
//

#include "SD_STD.H"

extern const TDbDriver KBuiltinDriver;

// Class CDbsObserver::HObserver

// The server side of a RDbNotifier object
// All "observers" on the same database are held in a list on the
// primary CDbsObserver, which tracks the single notifier object
// on the data source.

inline CDbsObserver::HObserver::HObserver()
	:iPending(0)
	{}

//
// Complete the outstanding request, and reset the observer status
//
void CDbsObserver::HObserver::Complete(TInt aStatus)
	{
	iPending=0;
	iMessage.Complete(aStatus);
	}

//
// Notification request from the client
// Int0() has the notification type (CDbNotifier::TType)
//
void CDbsObserver::HObserver::Notify(const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(iPending>=0,Panic(EDbsObserverRequestPending));
	iMessage=aMessage;
	if (iPending>RDbNotifier::EUnlock)
		Complete(iPending);				// report any missed event first
	else if (iLink.iNext==&iLink)
		Complete(RDbNotifier::EClose);	// report a "closed" event
	else
		iPending=aMessage.Int0();		// wait for an event
	}

//
// Cancel the notification request (if pending)
//
void CDbsObserver::HObserver::Cancel()
	{
	if (iPending<0)
		Complete(KErrCancel);
	}

//
// An event occurs on the database
//
void CDbsObserver::HObserver::Event(TInt aEvent)
	{
	if (aEvent==RDbNotifier::EClose)
		{	// detach the observer when closed
		iLink.Deque();
		iLink.iPrev=iLink.iNext=&iLink;
		}
	TInt pending=iPending;
	if (pending<0)
		{	// request is pending
		if (aEvent==RDbNotifier::EUnlock && pending==CDbNotifier::EChange)
			;	// don't report unlock events to "change" requests
		else
			Complete(aEvent);
		}
	else if (aEvent>pending)
		iPending=aEvent;	// store more significant event
	}

//
// Client notifer is closed
//
CDbsObserver::HObserver::~HObserver()
	{
	Cancel();
	iLink.Deque();
	}

// Class CDbsObserver

// The central server-side observer active object for the database notifiers
// This maintains a list of all notifiers, and propogates events from the
// database source.

inline CDbsObserver::CDbsObserver(CDbsSource& aSource)
	:CActive(10),iSource(aSource),iQueue(_FOFF(HObserver,iLink))
	{}

CDbsObserver* CDbsObserver::NewL(CDbsSource& aSource)
	{
	CDbsObserver* self=new(ELeave) CDbsObserver(aSource);
	CleanupStack::PushL(self);
	self->iNotifier=aSource.Source().NotifierL();
	CActiveScheduler::Add(self);
	CleanupStack::Pop();	// self
	return self;
	}

//
// Used by the source to destroy the observer only when all client
// notifiers have been closed
//
CDbsObserver* CDbsObserver::Collect(CDbsObserver* aObserver)
	{
	if (!aObserver)
		return aObserver;
	if (!aObserver->iQueue.IsEmpty())
		return aObserver;
	delete aObserver;
	return 0;
	}

CDbsObserver::~CDbsObserver()
	{
	__ASSERT(iQueue.IsEmpty());
	CDbObject::Destroy(iNotifier);	// will cancel any request
	Cancel();
	}

//
// Create and initialise a new client-observer object
//
CDbsObserver::HObserver* CDbsObserver::ObserverL()
	{
	HObserver* observer=new(ELeave) HObserver;
	iQueue.AddLast(*observer);
	if (!IsActive())
		Queue();		// start receiving events
	return observer;
	}

//
// Request an event from the database
//
void CDbsObserver::Queue()
	{
	SetActive();
	iNotifier->Notify(CDbNotifier::EUnlock,iStatus);
	}

//
// Dispatch the event to all observers and re-queue
//
void CDbsObserver::RunL()
	{
	TDblQueIter<HObserver> iter(iQueue);
	for (HObserver* ob;(ob=iter++)!=0;)
		ob->Event(iStatus.Int());
	if (!iQueue.IsEmpty())
		Queue();
	else if (iStatus.Int()==RDbNotifier::EClose)
		iSource.Closed();	// disconnect and destroy on a close event
	}

//
// Provided fo CActive: should never have to do anything as called only
// via the d'tor which destroys the notifier first, completing the message
//
void CDbsObserver::DoCancel()
	{
	__ASSERT(iStatus!=KRequestPending);
	}

// Class CDbsDatabaseStub

// This class is used as a stub object between the two phases of
// authentication on a secure database

CDbsDatabaseStub* CDbsDatabaseStub::NewL()
	{
	return new(ELeave) CDbsDatabaseStub;
	}

//
// Authenticate the access, on success destroy this object and return the
// real database interface object
//
CDbDatabase* CDbsDatabaseStub::AuthenticateL()
	{
	CDbSource& src=CDbsConnection::Source(*this).Source();
	CDbDatabase* db=src.AuthenticateL();
	Attach(db);
	Destroy(this);
	return db;
	}

// Class CDbsConnection

// The context for all interface objects residing in the server, which
// owns a reference on the data source.

CDbsConnection::~CDbsConnection()
	{
	if (iSource)
		iSource->Close();
	}


// Class CDbsSource

// The sharing point for databases in the server, this maintains access
// to the CDbSource interface object, allowing multiple connections to the
// same database.

//
// Something has closed. Check if we are still needed, and delete if not
//
void CDbsSource::Closed()
	{
	__ASSERT(iConnections==0);
	iObserver=CDbsObserver::Collect(iObserver);
	if (!iObserver)
		delete this;
	}

//
// A connection has been removed
//
void CDbsSource::Close()
	{
	__ASSERT(iConnections>0);
	__ASSERT(iSource);
	if (--iConnections==0)
		{	// last connection is closed
		CDbSource* s=iSource;
		iSource=0;
		CDbObject::Destroy(s);
		iLink.Deque();			// cannot connect this source again
		Closed();
		}
	}

CDbsSource::~CDbsSource()
	{
	__ASSERT(!iSource);
	__ASSERT(!iObserver);
	delete iName;
	}

//
// Construct a new source object from a database using the driver discovery
//
CDbsSource* CDbsSource::NewL(RFs& aFs,const TDesC& aSource)
	{
    //The following two statements are here to check the file path and raise (if have to) 
    //the same errors as they were raised before by the previous (DbDriver related) source code.
	::TEntry fileEntry;
	__LEAVE_IF_ERROR(aFs.Entry(aSource, fileEntry));
		
	const TDbFormat& fmt=KBuiltinDriver.iFormats[0];
	
	CDbsSource* self=new(ELeave) CDbsSource(fmt);
	CleanupStack::PushL(self);
	self->iName=aSource.AllocL();
	self->iSource=fmt.OpenL(aFs,aSource,TDbFormat::EReadWrite);
	CleanupStack::Pop();	// self
	return self;
	}

//
// [Create and] return the active observer of the data source
//
CDbsObserver::HObserver* CDbsSource::ObserverL()
	{
	__ASSERT(iSource);
	CDbsObserver* observer=iObserver;
	if (!observer)
		iObserver=observer=CDbsObserver::NewL(*this);
	return observer->ObserverL();
	}

//
// Test to find an identical source, in order to share it
//
TBool CDbsSource::Is(const TDesC& aSource) const
	{
	if (iName->CompareF(aSource)!=0)
		return EFalse;
	
	  return ETrue;
	}


// Class RDbsSources

// The collection of all shared sources in the server

//
// Open a source for sharing
//
CDbsConnection* RDbsSources::OpenLC(RFs& aFs,const TDesC& aSource,const TDesC& /*aFormat*/)
	{
	CDbsConnection* connect=new(ELeave) CDbsConnection;
	CleanupStack::PushL(connect);

  // try and find the source already open
  	TIter iter(iSources);
  	CDbsSource* src;
  	for (;;)
  		{
  		src=iter++;
  		if (src==0)
  			{	// not already open, have to open a new source
  			src=CDbsSource::NewL(aFs,aSource);
  			iSources.AddFirst(*src);
  			break;
  			}
  		if (src->Is(aSource))
  			break;	// share this source
  		}
  // we have a source
	connect->Set(*src);
	return connect;
	}

