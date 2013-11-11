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

#ifndef __ACTIVEREADONEORMOREREADER_H__
#define __ACTIVEREADONEORMOREREADER_H__

#include <e32base.h>
#ifndef __OVER_DUMMYUSBLDD__
#include <d32usbc.h>
#else
#include <dummyusblddapi.h>
#endif

class RDevUsbcClient;
class MReadOneOrMoreObserver;

NONSHARABLE_CLASS(CActiveReadOneOrMoreReader) : public CActive
/**
 * Active object to post ReadOneOrMore requests on the LDD.
 */
	{
public: 									   
	static CActiveReadOneOrMoreReader* NewL(MReadOneOrMoreObserver& aParent, 
		RDevUsbcClient& aLdd, TEndpointNumber aEndpoint);
	~CActiveReadOneOrMoreReader();

public:
	void ReadOneOrMore(TDes8& aDes, TInt aLength);

private:
	CActiveReadOneOrMoreReader(MReadOneOrMoreObserver& aParent, 
		RDevUsbcClient& aLdd, TEndpointNumber aEndpoint);

private: // from CActive
	virtual void DoCancel();
	virtual void RunL();

private: // unowned
	MReadOneOrMoreObserver& iParent;
	RDevUsbcClient& iLdd;

private: // owned
	TEndpointNumber iEndpoint;
	};

#endif // __ACTIVEREADONEORMOREREADER_H__
