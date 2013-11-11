/*
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
*
*/

#ifndef __ACTIVEREADER_H__
#define __ACTIVEREADER_H__

#include <e32base.h>
#ifndef __OVER_DUMMYUSBLDD__
#include <d32usbc.h>
#else
#include <dummyusblddapi.h>
#endif

class RDevUsbcClient;
class MReadObserver;

NONSHARABLE_CLASS(CActiveReader) : public CActive
/**
 * Active object to post read requests on the LDD.
 */
	{
public: 									  
	static CActiveReader* NewL(MReadObserver& aParent, RDevUsbcClient& aLdd, TEndpointNumber aEndpoint);
	~CActiveReader();

public:
	void Read(TDes8& aDes, TInt aLen);

private:
	CActiveReader(MReadObserver& aParent, RDevUsbcClient& aLdd, TEndpointNumber aEndpoint);

private: // from CActive
	virtual void DoCancel();
	virtual void RunL();

private: // unowned
	MReadObserver& iParent;
	RDevUsbcClient& iLdd;

private: // owned
	TEndpointNumber iEndpoint;
	};

#endif // __ACTIVEREADER_H__
