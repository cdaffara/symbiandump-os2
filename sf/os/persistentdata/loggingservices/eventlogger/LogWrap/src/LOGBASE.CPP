// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bautils.h>

#include <logwrap.h>
#include "LOGPANIC.H"

_LIT(KLogResourceFile,"\\resource\\logeng\\logwrap.rsc");

/** Sets the priority of this active object.

@param aPriority The priority of this active object. */
EXPORT_C CLogBase::CLogBase(TInt aPriority)
: CLogActive(aPriority), iResourceFile(NULL)
	{
	}

/** Fees all resources owned by the object prior to its destruction. In particular 
it closes the resource file reader. */
EXPORT_C CLogBase::~CLogBase()
	{
	  delete iResourceFile;
	}

#pragma BullseyeCoverage off

/** Adds an event to the log database. This is an asynchronous request.
	
	This function is overridden by log engine implementations.
	
	@param aEvent A reference to a log event detail object. This value is not 
	used.
	@param aStatus The request status. On completion of the asynchronous request, 
	it always contains KErrNotSupported.
	@capability Note For built-in event types, the required capability level is defined in
	the event type's write access policy. */
EXPORT_C void CLogBase::AddEvent(CLogEvent&, TRequestStatus& aStatus)
	{
	TRequestStatus* st = &aStatus;
	User::RequestComplete(st, KErrNotSupported);
	}

/** Gets the details of an existing event. This is an asynchronous request.
	
	This function is overridden by log engine implementations.
	
	@param aEvent A reference to a log event detail object. This value is not 
	used.
	@param aStatus The request status. On completion of the asynchronous request, 
	it always contains KErrNotSupported.
	@capability Note For built-in event types, the required capability level is defined in
	the event type's read access policy. */
EXPORT_C void CLogBase::GetEvent(CLogEvent&, TRequestStatus& aStatus)
	{
	TRequestStatus* st = &aStatus;
	User::RequestComplete(st, KErrNotSupported);
	}

/** Changes the details of an existing event. This is an asynchronous request.
	
	This function is overridden by log engine implementations.
	
	@param aEvent A reference to a log event detail object. This value is not 
	used.
	@param aStatus The request status. On completion of the asynchronous request, 
	it always contains KErrNotSupported.
	@capability Note For built-in event types, the required capability level is defined in
	the event type's write access policy. */
EXPORT_C void CLogBase::ChangeEvent(const CLogEvent&, TRequestStatus& aStatus)
	{
	TRequestStatus* st = &aStatus;
	User::RequestComplete(st, KErrNotSupported);
	}

/** Deletes an event from the log. This is an asynchronous request. 
	
	This function is overridden by log engine implementations.
	
	@param aId The unique event ID of the event to be deleted. This value is not 
	used.
	@param aStatus The request status. On completion of the asynchronous request, 
	it always contains KErrNotSupported..
	@capability Note For built-in event types, the required capability level is defined in
	the event type's write access policy. */
EXPORT_C void CLogBase::DeleteEvent(TLogId, TRequestStatus& aStatus)
	{
	TRequestStatus* st = &aStatus;
	User::RequestComplete(st, KErrNotSupported);
	}

/** Gets a standard string from the logwrap.dll resource file.
	
	This function is overridden by log engine implementations.
	
	@param aString A modifiable descriptor. The length of this descriptor is set 
	to zero.
	@param aId The resource ID for the string. This value is not used.
	@return KErrNotSupported.
	@see RResourceFile */
EXPORT_C TInt CLogBase::GetString(TDes& aString, TInt) const
	{
	aString.Zero();
	return KErrNotSupported;
	}

void CLogBase::DoRunL()
	{
	__ASSERT_DEBUG(ETrue, Panic(ELogNotImplemented));
	}

#pragma BullseyeCoverage on

EXPORT_C void CLogBase::LoadResourcesL(RFs& aFs)
	{
	// Find the resource file
    TFileName fileName;
    Dll::FileName(fileName);

    TParse parse;
    parse.Set(KLogResourceFile, &fileName, NULL);
	fileName = parse.FullName();

	// Get language of resource file
	BaflUtils::NearestLanguageFile(aFs, fileName);

	// Creating resource file member with 0 offset and 0 section size
	iResourceFile = CResourceFile::NewL(aFs, fileName, 0, 0);  
	}

#pragma BullseyeCoverage off

EXPORT_C void CLogBase::CLogBase_Reserved1()
	{
	}

#pragma BullseyeCoverage on
