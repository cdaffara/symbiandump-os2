// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include <logcntmodel.h>
#include <logengevents.h>
#include "LOGADD.H"
#include "logservpanic.h"
#include "LOGDUP.H"
#include "LOGQUERY.H"
#include "LogServRecentList.h"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServResourceInterpreter.h"
#include "LogServCacheConfig.h"
#include "LogServDatabaseChangeInterface.h"
#include <centralrepository.h>
#include "LogServCacheStrings.h"
#include "LogServCacheTypes.h"
#include "LOGREPDEFS.H"

const TInt KMinimumNumberOfDigitsToMatch = 3;

////////////////////////////////////////////////////////////////////////////////////////////
// Local functions

#ifdef SYSLIBS_TEST

#pragma BullseyeCoverage off

static void LogStore32IntL(RFs& aFs, const TDesC& aFilePath, TInt aVal)
	{
	RFile file;
	User::LeaveIfError(file.Replace(aFs, aFilePath, EFileWrite));
	TPtrC8 p((const TUint8*)&aVal, sizeof(aVal));
	TInt err = file.Write(p);
	if(err == KErrNone)
		{
		err = file.Flush();
		}
	file.Close();
	User::LeaveIfError(err);
	}

static void LogStoreContactMatchCountAndNameFormatL(TInt aContactMatchCount, TLogContactNameFormat aContactNameFormat)
	{
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	_LIT(KTestDir, "c:\\test\\");
	TInt err = fs.MkDir(KTestDir);
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	
	_LIT(KLogengTestFileNameCount, "c:\\test\\test_logengconfig_count.ini");
	LogStore32IntL(fs, KLogengTestFileNameCount, aContactMatchCount);
	
	_LIT(KLogengTestFileNameFormat, "c:\\test\\test_logengconfig_format.ini");
	LogStore32IntL(fs, KLogengTestFileNameFormat, (TInt)aContactNameFormat);
	
	CleanupStack::PopAndDestroy(&fs);
	}

#pragma BullseyeCoverage on

#endif //SYSLIBS_TEST

//This function reads logeng repository file and returns the integer value of the given key. 
static TInt LogGetRepositoryValueL(CRepository& aRepository, TInt aKey)
	{		
	TInt val = -1;		
	User::LeaveIfError(aRepository.Get(aKey, val));
	return val;
	}

static void LogGetContactmatchCountAndNameFormatL(TInt& aContactMatchCount, TLogContactNameFormat& aContactNameFormat)
	{
	CRepository* repository = NULL;
	TRAPD(err, repository = CRepository::NewL(KUidLogengRepository));		
	if(err == KErrNone)
		{
		CleanupStack::PushL(repository);
		aContactMatchCount = LogGetRepositoryValueL(*repository, KContactMatchCountRepKey);
		aContactNameFormat = static_cast <TLogContactNameFormat> (LogGetRepositoryValueL(*repository, KContactNameFormatRepKey));
		CleanupStack::PopAndDestroy(repository);
		}
	else if(err == KErrCorrupt)
		{
		__ASSERT_DEBUG(!repository, User::Invariant());
		User::Leave(err);
		}
	else
		{
		__ASSERT_DEBUG(!repository, User::Invariant());
		aContactMatchCount = KLogContactMatchCount;
		aContactNameFormat = KLogContactNameFormat;
		}
	#ifdef SYSLIBS_TEST
	LogStoreContactMatchCountAndNameFormatL(aContactMatchCount, aContactNameFormat);
	#endif	
	}

////////////////////////////////////////////////////////////////////////////////////////////
// CLogAddEvent class

CLogAddEvent::CLogAddEvent(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority) :
	CLogActive(aPriority), 
	iDatabase(aDatabase)
	{
	}

CLogAddEvent::~CLogAddEvent()
	{
	Cancel();
	
	CloseContactsPlugin();

	delete iDuplicate;
	delete iDuplicateFilter;
	}

CLogAddEvent* CLogAddEvent::NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
	{
	CLogAddEvent* self = new(ELeave) CLogAddEvent(aDatabase, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLogAddEvent::ConstructL()
	{
	iDuplicate = CLogDuplicate::NewL(iDatabase, Priority());
	iDuplicateFilter = CLogFilter::NewL();
	::LogGetContactmatchCountAndNameFormatL(iContactMatchCount, iContactNameFormat);
	}

//This method will open contacts database (if not opened yet), only if the value of 
//iContactMatchCount is not 0.
//Se how iContactMatchCount data member is initialised.
TBool CLogAddEvent::PerformContactMatchL()
	{
	if (iContactMatchCount <= 0 || iEvent->Contact() != KLogNullContactId)
		return EFalse;
	
	if (iContactPlugin)
		return ETrue;
	
	// Attempt to load plugin
	TRAPD( err, iContactPlugin=CLogCntModel::NewL());

	// If plugin doesn't exist this is equivalent to matching being disabled so we don't leave
	// for KErrNotFound
	if(err==KEComErrNoInterfaceIdentified)
		{
		// Disable contacts matching so that we don't keep attempting to match
		iContactMatchCount = 0;
		// Plugin doesn't exist
		return EFalse;
		}

	User::LeaveIfError(err);
	
	// Open the DB
	OpenContactsL();
	
	return ETrue;
	}

void CLogAddEvent::StartL(CLogEvent& aEvent, const CLogServRecentList* aRecentList, TRequestStatus& aStatus, const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(ELogAlreadyActive1));

	LOGTEXT("CLogAddEvent::StartL()");

	// Store event details which were obtained from the client side
	iEvent = &aEvent;
	iState = ELogAddEvent;
	
	if (!iDatabase.DTIIsAllowed(EWriteOp, aMessage, iEvent->EventType()))
		{
		User::Leave(KErrPermissionDenied);
		}

	if (PerformContactMatchL())
		iState = ELogSetContactAndRemoteParty; // Go look for a matching contact

	iRecentList = aRecentList;
	iEventAdded = EFalse;

	// Setup the event's time (UTC)
	TTime time;
	time.UniversalTime();
	iEvent->SetTime(time);

	// Save the observer's request status and set it to KRequestPending
	Queue(aStatus);

	// Start this objects RunL chain
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}
	
void CLogAddEvent::SetEventContact()
    {
    // Start by converting the phone number text into a number
    // check we've got a long enough number to be worth checking
    if(iEvent->Number().Length() >= KMinimumNumberOfDigitsToMatch)
        {
        // now search for a contact by looking up the phone number
        TLogContactItemId contactId = KLogNullContactId;
        TRAPD(err, contactId = iContactPlugin->MatchPhoneNumberL(iEvent->Number(), iContactMatchCount));
        
        if(err == KErrNone)
            {
            // we have at least one match
            if(contactId != KLogNullContactId)
                {
                // we have a match so set the contact id
                iEvent->SetContact(contactId);
                }
            }
        }
    iEvent->SetFlags(KLogEventContactSearched);
    }

void CLogAddEvent::SetRemoteParty()
    {
    // Get the contact id
    TLogContactItemId  contactId = iEvent->Contact();
    if((contactId != KLogNullContactId) && (iEvent->RemoteParty().Length() == 0))
        {
        // Look it up and get the remote party info
        // Setup buffer to contain concatenated result
        TBuf<128> buf;
        // Go get the info
        TRAPD(err, iContactPlugin->ReadContactNameL(contactId, buf, iContactNameFormat));

        if(err == KErrNotFound)
            {
            // Couldn't find the contact with that id so set it to NULL
            iEvent->SetContact(KLogNullContactId);
            }
        else
            {
            // Found it so fill in remote party
            iEvent->SetRemoteParty(buf);
            }
        }
    }

void CLogAddEvent::GetConfigL()
    {
    iConfig = iDatabase.DTICacheConfig().Config();
    if(iConfig.iMaxLogSize == 0)
      {
      LOGTEXT("CLogAddEvent::DoRunL() - logging disabled");
      User::Leave(KErrNotSupported);
      }
    }

TLogTypeId CLogAddEvent::SetDescriptionL()
    {
    const TLogServCacheTypeEntry& entry = iDatabase.DTICacheTypes().FindByUid(iEvent->EventType());
    if(entry.iEventTypeId == KLogNullTypeId)
        {
        LOGTEXT("CLogAddEvent::DoRunL() - type not found");
        User::Leave(KErrNotFound);
        }
    if(!entry.iEventType->LoggingEnabled())
        {
        LOGTEXT("CLogAddEvent::DoRunL() - type not enabled");
        User::Leave(KErrNotSupported);
        }
    iEvent->SetDescription(entry.iEventType->Description());
    return entry.iEventTypeId;
    }
    
TBool CLogAddEvent::DetectDuplicateEventsL()
    {
    TBool rc = EFalse;
    if(iRecentList)
        {
        iRecentList->GetFilter(*iEvent, *iDuplicateFilter);
        rc = iDuplicate->StartL(iEvent->Id(), iRecentList->Id(), *iDuplicateFilter, iStatus);
        }
    return rc;
    }

void CLogAddEvent::DoRunL()
	{
	LOGTEXT3("CLogAddEvent::DoRunL(%d), state = %d", iStatus.Int(), iState);

	switch (iState)
		{
		case ELogSetContactAndRemoteParty:
			{
			SetEventContact();
			SetRemoteParty();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			iState = ELogAddEvent;
			SetActive();
			break;
			}
		case ELogAddEvent:
			{
			GetConfigL();
			TLogTypeId typeId = SetDescriptionL();
			RLogEventDbTable tbl;
			tbl.OpenLC(iDatabase.DTIDatabase());
			::LogPurgeMainL(iDatabase, tbl, iConfig.iMaxLogSize, 1);
			DoAddEventL(tbl, typeId, iDatabase.DTICacheStrings().GetIdL(iEvent->Direction()), iDatabase.DTICacheStrings().GetIdL(iEvent->Status()));
            CleanupStack::PopAndDestroy();//tbl
			iEventAdded = ETrue;
			if(DetectDuplicateEventsL())
			    {
                iState = ELogPurgeRecent;
                SetActive();
                break;
			    }
			iState = ELogPurgeRecent;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
			break;
			}
		case ELogPurgeRecent:
			{
			// Delete old recent events
			if (iRecentList)
				{
				RArray<TLogId> logIds;
				::LogGetRecentEventsLC(iDatabase, iRecentList->Id(), iConfig.iMaxRecentLogSize, logIds); 
				::LogPurgeRecentEventsL(iDatabase, logIds);
				CleanupStack::PopAndDestroy(&logIds);
				}
			}
			break;
		default:
			__ASSERT_DEBUG(ETrue, Panic(ELogBadState1));
            break;
		}

	LOGTEXT("CLogAddEvent::DoRunL() - end");
	}

void CLogAddEvent::DoCancel()
	{
	iDuplicate->Cancel();
	CLogActive::DoCancel();
	}

void CLogAddEvent::DoAddEventL(RLogEventDbTable& aTbl, TLogTypeId aTypeId, TLogStringId aDirectionId, TLogStringId aStatusId)
	{
	LOGTEXT("CLogAddEvent::DoAddEventL()");

	// Insert a new record
	aTbl.InsertL();

	aTbl.SetColL(RLogEventDbTable::iTypeColNo, (TUint32)aTypeId);

	if (iEvent->RemoteParty().Length() > 0)
		aTbl.SetColL(RLogEventDbTable::iRemotePartyColNo, iEvent->RemoteParty());

	if (iEvent->Direction().Length() > 0)
		aTbl.SetColL(RLogEventDbTable::iDirectionColNo, (TUint32)aDirectionId);

	aTbl.SetColL(RLogEventDbTable::iTimeColNo, iEvent->Time());
	aTbl.SetColL(RLogEventDbTable::iDurationTypeColNo, (TInt32)iEvent->DurationType());

	if (iEvent->DurationType() != KLogNullDurationType)
		aTbl.SetColL(RLogEventDbTable::iDurationColNo, iEvent->Duration());

	if (iEvent->Status().Length() > 0)
		aTbl.SetColL(RLogEventDbTable::iStatusColNo, (TUint32)aStatusId);

	if (iEvent->Subject().Length() > 0)
		aTbl.SetColL(RLogEventDbTable::iSubjectColNo, iEvent->Subject());

	if (iEvent->Number().Length() > 0)
		aTbl.SetColL(RLogEventDbTable::iNumberColNo, iEvent->Number());

	if (iEvent->Contact() != KLogNullContactId)
		aTbl.SetColL(RLogEventDbTable::iContactColNo, iEvent->Contact());

	if (iEvent->Link() != KLogNullLink)
		aTbl.SetColL(RLogEventDbTable::iLinkColNo, iEvent->Link());

	if (iEvent->Data().Length() > 0)
		aTbl.SetColL(RLogEventDbTable::iDataColNo, iEvent->Data());

	// Set the flags
	TInt bit = KLogFlagsCount;
	while(bit--)
		{
		aTbl.SetColL(RLogEventDbTable::iFlagColNo[bit], (TUint32)((iEvent->Flags() & 0x1 << bit) ? 1 : 0));
		}

	if (iRecentList)
		{
		__ASSERT_DEBUG(iRecentList->Id() != KLogNullRecentList, Panic(ELogNullRecentList));
		aTbl.SetColL(RLogEventDbTable::iRecentColNo, (TInt32)iRecentList->Id());
		}

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	if(iEvent->SimId() != KLogNullSimId)
		{
		aTbl.SetColL(RLogEventDbTable::iSimIdColNo, iEvent->SimId());
		}
	else
		{
		aTbl.SetColNullL(RLogEventDbTable::iSimIdColNo);
		}
#endif
	
	// Assign event id and end the rowset operation
	const TLogId newId = aTbl.ColInt32(RLogEventDbTable::iIdColNo);
	iEvent->SetId(newId);
	aTbl.PutL();

	// Tell change interface about the addition
	iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventAdded, newId);

	LOGTEXT("CLogAddEvent::DoAddEventL() - end");
	}

void CLogAddEvent::DoComplete(TInt& aStatus)
	{
	LOGTEXT2("CLogAddEvent::DoComplete(%d)", aStatus);

	if	(iDatabase.DTIInTransaction())
		{
		LOGTEXT2("CLogAddEvent::DoComplete() - in transaction: %d", aStatus);
		if (aStatus == KErrNone)
			aStatus = iDatabase.DTICommitAndEnd();

		LOGTEXT2("CLogAddEvent::DoComplete() - checking for need to rollback: %d", aStatus);
		if (aStatus < KErrNone)
			iDatabase.DTIRollBack();
		}
	else
		{
		if	(iEventAdded)
			aStatus = KErrNone;
		}

	LOGTEXT2("CLogAddEvent::DoComplete() - final status value is: %d", aStatus);
	}

//Opens the default contacts database.
void CLogAddEvent::OpenContactsL()
	{
	//Sometimes, after a sequence of OpenContactsL()/CloseContacts() calls the Contacts server crashes
	//and OpenContactsL() leaves with error -15. In order to avoid that the following delay has been added.
	//(something related to Contacts server state machine)
    User::After(1000);
	// Attempt to open DB
	TRAPD(err,iContactPlugin->OpenContactsL());
	if(KErrNone!=err)
		{
		if(err == KErrServerTerminated)
		    {
		    RDebug::Print(_L("+++LogEng, LogAdd.cpp, Contacts server crashed!\r\n"));
		    }
		// If DB doesn't open delete plugin
		delete iContactPlugin;
		iContactPlugin = NULL;
		User::Leave(err);
		}
	}

//Closes the default contacts database and deletes the plugin.
void CLogAddEvent::CloseContactsPlugin()
	{
	if(iContactPlugin)
		{
		iContactPlugin->CloseContacts();
		delete iContactPlugin;
		iContactPlugin = NULL;
		//REComSession::FinalClose() call moved here from logcntmodel.cpp.
		//The in-source documentation of REComSession::FinalClose() notes that: 
		//"It must never be called from within a plug-in implementations class 
		//destructor, especially following a DestroyImplementation() ". 
		//That was the case before the function call was moved here.
	    REComSession::FinalClose();
		}
	}
