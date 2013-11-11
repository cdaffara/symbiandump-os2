/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#if !defined(__SYSSTARTPLUGIN_H__)
#define __SYSSTARTPLUGIN_H__

#include <e32base.h>
#include <f32file.h>

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
#include <ssm/ssmstateawaresession.h>

class TSsmState;

class CSysStartPlugin : public CBase, public MStateChangeNotificationSubscriber
#else
#include <domainmember.h>
class CSysStartPlugin : public CDmDomain
#endif
	{
public:
	virtual void WaitForSystemStartL();
	static CSysStartPlugin* NewL();
	virtual ~CSysStartPlugin();
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
public: // from MStateChangeNotificationSubscriber
	virtual void StateChanged(TSsmState aSsmState);
	
private:
	CSsmStateAwareSession* iStateAwareSession;
#else
	virtual void RunL();
#endif
protected:
	CSysStartPlugin();
	void ConstructL();
	};

#endif
