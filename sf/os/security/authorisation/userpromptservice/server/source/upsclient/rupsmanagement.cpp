/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* RUpsManagement implementation.	 See class and function definitions
* for more detail.
*
*/


/**
 @file
*/


#include <ups/upsclient.h>
#include <s32mem.h>

#include "upscommon.h"
#include <scs/nullstream.h>

namespace UserPromptService
	{
EXPORT_C RUpsManagement::RUpsManagement()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
:	RScsClientBase(), iCreateViewFilterBuf(), iMatchLengthBuf()
	{
	iMatchLengthBuf() = 0;
	}

EXPORT_C TInt RUpsManagement::Connect()
/**
	Connect to the UPS server.

	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	TVersion v = UserPromptService::Version();
	TUidType serverFullUid = UserPromptService::ServerImageFullUid();

	TInt r = RScsClientBase::Connect(
		UserPromptService::KUpsServerName, v, UserPromptService::KServerImageName, serverFullUid);
	
	return r;
	}
	
EXPORT_C void RUpsManagement::Close()
/**
Cleanup and call RScsClientBase::Close
*/
	{
	RScsClientBase::Close();
	}

EXPORT_C void RUpsManagement::DeleteDatabaseL()
/**
	Deletes all stored UPS decisions.

	@capability WriteDeviceData 
*/
	{
	User::LeaveIfError(CallSessionFunction(EDeleteDatabase));
	}

EXPORT_C void RUpsManagement::CreateView(const CDecisionFilter& aFilter, TRequestStatus &aStatus)
/**
	Creates a view for records which match the supplied CDecisionFilter.

	If the CDecisionFilter is created using the constructor which specifies all fields (or all fields are set), then an exact match will be
	searched for.
	If the CDecisionFilter is created without parameters, then any fields which are not set will match any record.

	Only one view can be active in a single RUpsManagement session at any one time.

	Simultaneous database updates, either from this management session, another session or the main UPS 
	operation may cause the query to be aborted with KErrAbort.

	When you are finished with the view you should call CancelAndCloseView (otherwise you
	will not be able to create a new view).

	@param aFilter	Specifies the filter to be matched.
	@capability ReadDeviceData 
*/
{
	aStatus = KRequestPending;
	TRAPD(err,
		RNullWriteStream nullstream;
		nullstream << aFilter;
		nullstream.CommitL();
		TInt bytesWritten = nullstream.BytesWritten();

		iCreateViewFilterBuf.Close();
		iCreateViewFilterBuf.CreateL(bytesWritten);

		// Arg 0 - The CDecisionFilter
		// Externalize to iCreateViewFilterBuf
		RDesWriteStream desstream(iCreateViewFilterBuf);
		CleanupClosePushL(desstream);
		desstream << aFilter;
		desstream.CommitL();
		CleanupStack::PopAndDestroy(&desstream);

		// Arg 1 - TUint32 length of first match returned by server
		// Server writes into iMatchLengthBuf
		CallSessionFunction(ECreateView, TIpcArgs(&iCreateViewFilterBuf, &iMatchLengthBuf), aStatus);
		);
	if(err != KErrNone)
		{
		TRequestStatus *rs = &aStatus;
		User::RequestComplete(rs, err);
		}
}

EXPORT_C CDecisionRecord *RUpsManagement::NextMatchL()
/**
	Returns the next matching record in the view created with CreateViewL.

	Simultaneous database updates, either from this management session, another session or the main UPS 
	operation may cause the query to be aborted with KErrAbort.

	When you are finished with the view you should call CancelAndCloseView.

	@return record ptr, or 0 if there are no more matching records. Leaves for other errors.

	@capability ReadDeviceData 
*/
{
	if(iMatchLengthBuf() == 0)
		{
		return 0; // No more matches
		}

	CDecisionRecord *record = CDecisionRecord::NewLC();

	// Arg 0 - The CDecisionRecord buffer
	RBuf8 buf;
	CleanupClosePushL(buf);
	buf.CreateL(iMatchLengthBuf());

	// Arg 1 - TUint32 length of next match

	User::LeaveIfError(CallSessionFunction(ENextMatch, TIpcArgs(&buf, &iMatchLengthBuf)));

	RDesReadStream readStream(buf);
	CleanupClosePushL(readStream);

	readStream >> *record;

	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&buf);
	CleanupStack::Pop(record);
	return record;
}

EXPORT_C void RUpsManagement::CancelAndCloseView()
	/**
	Close down a view. This can also be used to cancel an outstanding CreateView request.

	@capability ReadDeviceData 
	*/
{
	CancelSessionFunction(ECreateView);
	(void) CallSessionFunction(ECloseView);
	iCreateViewFilterBuf.Close();
	iMatchLengthBuf() = 0;
}

EXPORT_C void RUpsManagement::RemoveDecisionsL(const CDecisionFilter& aFilter)
/**
	Removes all records which match the supplied CDecisionFilter.

	The filter can match/delete multiple records in one operation.
	
	@param aFilter	Specifies the records to be deleted.

	@capability WriteDeviceData 
*/
	{
	// Calculate the buffer length required to represent the filter object
	RNullWriteStream nullstream;
	nullstream << aFilter;
	nullstream.CommitL();
	TInt bytesWritten = nullstream.BytesWritten();

	// Create buffer for filter
	RBuf8 filterBuf;
	CleanupClosePushL(filterBuf);
	filterBuf.CreateL(bytesWritten);

	// Arg 0 - The CDecisionFilter
	// Externalize to iCreateViewFilterBuf
	RDesWriteStream desstream(filterBuf);
	CleanupClosePushL(desstream);
	desstream << aFilter;
	desstream.CommitL();
	CleanupStack::PopAndDestroy(&desstream);

	User::LeaveIfError(CallSessionFunction(ERemoveDecisions, TIpcArgs(&filterBuf)));

	CleanupStack::PopAndDestroy(&filterBuf);
	}

EXPORT_C void RUpsManagement::UpdateDecision(TUint32 aRecordId, TBool aAllow, TRequestStatus &aStatus)
/**
	Updates the single record which matches the unique record ID.

	This API ONLY updates the CDecisionRecord result field. The only legal values are ETrue (always) or EFalse (never)
	
	@param aRecordId	Specifies the single record to update.
	@param aAllow		Allow or reject the request.
	
	@capability AllFiles 
*/
	{
	TIpcArgs args(aRecordId, aAllow);
	CallSessionFunction(EUpdateDecision, args, aStatus);
	}

EXPORT_C void RUpsManagement::CancelUpdateDecision()
/**
	Cancel an outstanding UpdateDecision request.
	
	Normally this will not be used because an UpdateDecision call will complete very quickly, but
	internally the request is asynchronous and exposing the cancel API allows the cancel code path
	to be tested.

	@capability AllFiles 
*/
	{
	CancelSessionFunction(EUpdateDecision);
	}

EXPORT_C void RUpsManagement::DeleteDecisionsForExeL(const TSecureId& aExeSid)
/**
	Delete all decisions in the database for the specified executable.

	@param aExeSid	The SID of the executable which has been deleted.

	@capability SWI observer plugin SID only
*/
	{
	User::LeaveIfError(CallSessionFunction(EDeleteDecisionsForExe, TIpcArgs(aExeSid.iId)));
	}

EXPORT_C void RUpsManagement::NotifyPluginsMayHaveChangedL()
/**
	Notify the UPS that an ECOM plugin has been installed somewhere, which may be an evaluator.
	The UPS will reload all ECOM plugins ASAP.

	@capability SWI observer plugin SID only
*/
	{
	User::LeaveIfError(CallSessionFunction(ENotifyPluginsMayHaveChanged));
	}

EXPORT_C void RUpsManagement::NotifyPolicyFilesChanged(TRequestStatus &aStatus)
/**
	Policy files have been added, changed or deleted.

	The UPS server will rebuild its policy file cache and delete decisions which relate to policy files
	which are no longer active (ie. have been deleted, or eclipsed by a policy file with a different
	major version number).

	@capability SWI observer plugin SID only
*/
	{
	TIpcArgs args;
	CallSessionFunction(ENotifyPolicyFilesChanged, args, aStatus);

	}

EXPORT_C void RUpsManagement::CancelNotifyPolicyFilesChanged()
	/**
	Cancel a previous call to NotifyPolicyFilesChanged.

	Normally this functions should not be used, it is only present for testing the handling
	of abnormal events.

	@capability SWI observer plugin SID only
	*/
	{
	CancelSessionFunction(ENotifyPolicyFilesChanged);
	}



} // End of namespace UserPromptService

// End of file

