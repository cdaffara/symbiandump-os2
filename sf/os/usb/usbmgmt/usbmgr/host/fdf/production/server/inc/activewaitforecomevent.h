/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ACTIVEWAITFORECOMEVENT_H
#define ACTIVEWAITFORECOMEVENT_H

#include <e32base.h>
#include <ecom/ecom.h>

NONSHARABLE_CLASS(MEComEventObserver)
	{
public:
	virtual void EComEventReceived() = 0;
	};

NONSHARABLE_CLASS(CActiveWaitForEComEvent) : public CActive
	{
public:
	static CActiveWaitForEComEvent* NewL(MEComEventObserver& aObserver);
	~CActiveWaitForEComEvent();

public:
	void Wait();

private:
	void ConstructL();
	CActiveWaitForEComEvent(MEComEventObserver& aObserver);

private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	

private: 
	REComSession 		iEComSession;
	MEComEventObserver& iObserver; 
	};

#endif // ACTIVEWAITFORECOMEVENT_H
