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

#ifndef __LOGVIEWCHANGEOBSERVER_H__
#define __LOGVIEWCHANGEOBSERVER_H__

// System includes
#include <logwrap.h>

/**
This class allows a client of the log engine to observe specific change events within
a given view.
@publishedAll 
@released
*/
class MLogViewChangeObserver
	{
public:

	/**Handle a change corresponding to a view addition
	
	This method is called when a log event is added to a view. The event maybe 
	recently added,	or have existed for some time but only now meet the 
	criteria of this view (i.e satisfy the filter) due to some other change.
	
	@param aId The id of the log event which has been added to the view.
	@param aViewIndex The position within the view at which the event has been 
	added.
	@param aChangeIndex	In a series of changes, this is the index of the 
	currently processing change. The change index will be greater than (or 
	equal to) 0, and less than the total change count.
	@param aTotalChangeCount This is the total count of all changes which will 
	be processed in this batch.
	 */
	virtual void HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount) = 0;

	/** Handle a change corresponding to a view change
	
	This method is called when a log event changes within a view.
	
	@param aId The id of the log event which has changed within the view.
	@param aViewIndex The position within the view where the change occurred.
	@param aChangeIndex	In a series of changes, this is the index of the 
	currently processing change. The change index will be greater than (or 
	equal to) 0, and less than the total change count.
	@param aTotalChangeCount This is the total count of all changes which will 
	be processed in this batch.
	 */
	virtual void HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount) = 0;

	/** Handle a change corresponding to a view deletion
	
	This method is called when a log event is removed from a view.
	
	@param aId The id of the log event which has been removed from the view
	@param aViewIndex The position within the view which the event previously 
	held.
	@param aChangeIndex In a series of changes, this is the index of the 
	currently processing change. The change index will be greater than (or 
	equal to) 0, and less than the total change count.
	@param aTotalChangeCount This is the total count of all changes which 
	will be processed in this batch
	 */
	virtual void HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount) = 0;
	};


#endif
