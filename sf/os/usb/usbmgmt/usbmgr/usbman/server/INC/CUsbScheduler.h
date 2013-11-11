/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Implements an Active Scheduler for the server to use
* 
*
*/



/**
 @file
*/

#ifndef __CUSBSCHEDULER_H__
#define __CUSBSCHEDULER_H__

#include <e32base.h>

class CUsbServer;

/**
 * The CUsbScheduler class
 *
 * Implements an Active Scheduler for the server to use. This is necessary
 * in order to provide an Error() function which does something useful instead
 * of panicking.
 */
NONSHARABLE_CLASS(CUsbScheduler) : public CActiveScheduler
	{
public:
	static CUsbScheduler* NewL();
	~CUsbScheduler();

	void SetServer(CUsbServer& aServer);

private:
	inline CUsbScheduler() {};
	// from CActiveScheduler
	void Error(TInt aError) const;

public:
	CUsbServer* iServer;
	};

#endif //__CUSBSCHEDULER_H__
