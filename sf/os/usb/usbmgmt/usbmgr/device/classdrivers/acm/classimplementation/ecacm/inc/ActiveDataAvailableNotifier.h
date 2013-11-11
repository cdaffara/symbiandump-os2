/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef __ACTIVEDATAAVAILABLENOTIFIER_H__
#define __ACTIVEDATAAVAILABLENOTIFIER_H__

#include <e32base.h>
#ifndef __OVER_DUMMYUSBLDD__
#include <d32usbc.h>
#else
#include <dummyusblddapi.h>
#endif

class RDevUsbcClient;
class MNotifyDataAvailableObserver;

NONSHARABLE_CLASS(CActiveDataAvailableNotifier) : public CActive
/**
 * Active object used to notify the parent when there is data available 
 * to be read from the LDD.  Since the LDD does not support this functionality 
 * directly RDevUsbcClient::ReadOneOrMore() is used instead, specifying a 
 * zero byte read into a descriptor which is owned by the class.
 */
	{
public: 									   
	static CActiveDataAvailableNotifier* NewL(MNotifyDataAvailableObserver& aParent, 
		RDevUsbcClient& aLdd, TEndpointNumber aEndpoint);
	~CActiveDataAvailableNotifier();

public:
	void NotifyDataAvailable();

private:
	CActiveDataAvailableNotifier(MNotifyDataAvailableObserver& aParent, 
		RDevUsbcClient& aLdd, TEndpointNumber aEndpoint);

private: // from CActive
	virtual void DoCancel();
	virtual void RunL();

private: // unowned
	MNotifyDataAvailableObserver& iParent;
	RDevUsbcClient& iLdd;

private: // owned
	TEndpointNumber iEndpoint;
	TBuf8<1> iUnusedBuf;	// passed to the LDD in order to carry out the zero byte read (length must be > 0)
	};

#endif // __ACTIVEDATAAVAILABLENOTIFIER_H__
