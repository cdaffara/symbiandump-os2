/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines WLAN CFP flow factory
*
*/

/*
* %version: 5 %
*/

#ifndef __WLANCFPROTO_H__
#define __WLANCFPROTO_H__

/** 
*	Header file declaring the WLAN CFProtocol ECOM factories
*/

#include <e32base.h>
#include <e32std.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_protflow.h>

/**
Wlan SubConnFlow Implementation UID
*/
const TInt KWlanFlowImplementationUid = 0x2001B2D3;


class CWlanSubConnectionFlowFactory : public ESock::CSubConnectionFlowFactoryBase
/**
Wlan Flow Factory
*/
	{
public:
	static CWlanSubConnectionFlowFactory* NewL(TAny* aConstructionParameters);
	virtual ESock::CSubConnectionFlowBase* DoCreateFlowL(ESock::CProtocolIntfBase* aProtocolIntf, ESock::TFactoryQueryBase& aQuery);
protected:
	CWlanSubConnectionFlowFactory(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer);
	};

#endif // __WLANCFPROTO_H__
