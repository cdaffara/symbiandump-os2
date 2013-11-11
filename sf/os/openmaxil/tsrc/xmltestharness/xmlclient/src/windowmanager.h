/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@internalComponent
*/

#ifndef WINDOWMANAGER_H_
#define WINDOWMANAGER_H_

#include <e32msgqueue.h>
#include <w32std.h>

/**
 * Encapsulates the resources used to provide a window to render to via Graphic Surfaces.
 * Proxies certain methods so that they can be used by arbitary threads.
 */
class CWindowManager : public CActive
	{
private:
	enum TCommand
		{
		ERegisterSurface,
		ESetBackgroundSurface
		};
	
	struct TMessage
		{
		TCommand iCommand;
		TAny* iParam1;
		TInt iParam2;
		};
	
public:
	static CWindowManager* NewL();
	~CWindowManager();
	
	TSize ScreenSize() const;
	
	TInt RegisterSurface(TSurfaceId& aSurfaceId);
	TInt SetBackgroundSurface(TSurfaceConfiguration& aSurfaceConfig, TBool aTriggerRedraw);
	void Flush();
	
private:
	CWindowManager();
	void ConstructL();
	
	void RunL();
	void DoCancel();
	
	TInt RunCommand(TCommand aCommand, TAny* aParam1, TInt aParam2 = 0);
	TInt DoCommand(TCommand aCommand, TAny* aParam1, TInt aParam2);
	
private:
	RMsgQueue<TMessage> iMsgQueue;
	RSemaphore iSemaphore;
	TInt iReturnCode;
	TThreadId iCreatorThreadId;
	
	RWsSession iWsSession;
	CWsScreenDevice* iWsSd;		// Screen Device for this WServ session.
	RWindowGroup iWindowGroup;	// Window group of the AO windows.
	RWindow* iWindow;
	TSize iScreenSize;
	};

#endif /* WINDOWMANAGER_H_ */
