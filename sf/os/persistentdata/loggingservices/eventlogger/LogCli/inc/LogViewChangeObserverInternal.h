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

#ifndef __LOGVIEWCHANGEOBSERVERINTERNAL_H__
#define __LOGVIEWCHANGEOBSERVERINTERNAL_H__

// User includes
#include <logviewchangeobserver.h>

class MLogViewChangeObserverInternal : public MLogViewChangeObserver
/**
The internal change observer mechanism. Extended beyond the normal version
@internalComponent
*/
	{
public:

	/**
	 * Called internally within the log client when the log is cleared.
	 */
	virtual void HandleLogViewChangeEventLogClearedL() = 0;
	};


#endif
