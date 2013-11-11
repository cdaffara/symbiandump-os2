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

#ifndef __LOGSERVOPERATIONMANAGER_H__
#define __LOGSERVOPERATIONMANAGER_H__

#include "LogServDefs.h"
#include "LogCliServShared.h"

// Classes referenced
class CLogServOperationBase;


/**
An interface class offering methods for adding/removing operations to/from the operations queue.
The LogEng server maintains an operations queue where all pending tasks such as adding/changing events 
are kept and scheduled later for execution.

@see CLogServOperationBase
@see LogServFactory 

@internalComponent
*/
class MLogServOperationManager
	{
public:

	/**
	 *
	 */
	virtual void OMOperationQueueAdd(CLogServOperationBase& aOperation) = 0;
	virtual void OMOperationQueueRemove(CLogServOperationBase& aOperation) = 0;

	/**
	 * Get the result of a specific operation
	 */
	virtual void OMGetResultL(TLogOperationId aId, 
                              TLogServSessionId aSessionId, const RMessage2& aMessageToWriteTo) = 0;

	/**
	 * Cancel a specific operation
	 */
	virtual void OMCancel(TLogOperationId aId, TLogServSessionId aSessionId, TBool aCompleteRequest) = 0;
	virtual void OMCancel(TLogServSessionId aSessionId, TBool aCompleteRequest) = 0;
	};


#endif

