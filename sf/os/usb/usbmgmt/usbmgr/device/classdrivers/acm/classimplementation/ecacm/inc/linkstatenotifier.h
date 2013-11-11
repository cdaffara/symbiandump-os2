/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LINKSTATENOTIFIER_H__
#define __LINKSTATENOTIFIER_H__

#include <e32base.h> 

class RDevUsbcClient;


const TInt KMaxPacketTypeBulkFS = 64;
const TInt KMaxPacketTypeBulkHS = 512;

const TInt KDefaultMaxPacketTypeBulk = KMaxPacketTypeBulkFS;


class MLinkStateObserver
/**
 * Interface for notifications of link state events.
 */
	{
public:
	virtual void MLSOStateChange(TInt aPacketSize) = 0;
	};

NONSHARABLE_CLASS(CLinkStateNotifier) : public CActive
/**
 * Active object to request link state notifications.
 */
	{
public:
	static CLinkStateNotifier* NewL(MLinkStateObserver& aParent, 
		RDevUsbcClient& aUsb);
	~CLinkStateNotifier();
	
	void Start();

private: // from CActive
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

private:
	CLinkStateNotifier(MLinkStateObserver& aParent, RDevUsbcClient& aUsb);

private:
	void LinkUp();
	void LinkDown();

private: // unowned
	MLinkStateObserver& iParent; ///< Observer
	RDevUsbcClient& iUsb;		///< The Ldd to use

private: // owned
	TUint iUsbState;		///< Bitmask representing the endpoints
	TInt iPacketSize;
	};

#endif // __LINKSTATENOTIFIER_H__
