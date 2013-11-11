/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements WLAN meta connection provider factory
*
*/

/*
* %version: 4 %
*/

#include "wlanmcprfactory.h"
#include "wlanmcpr.h"
#include <ss_glob.h>
#include "am_debug.h"

#include <comms-infras/ss_msgintercept.h>


using namespace ESock;

//-=========================================================
//
// CWlanMetaConnectionProviderFactory methods
//
//-=========================================================	

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProviderFactory::NewL
// -----------------------------------------------------------------------------
// 
CWlanMetaConnectionProviderFactory* CWlanMetaConnectionProviderFactory::NewL(TAny* aParentContainer)
	{
	DEBUG("CWlanMetaConnectionProviderFactory::NewL");

	return new (ELeave) CWlanMetaConnectionProviderFactory(TUid::Uid(CWlanMetaConnectionProviderFactory::iUid), *(reinterpret_cast<CMetaConnectionFactoryContainer*>(aParentContainer)));
	}

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProviderFactory::CWlanMetaConnectionProviderFactory
// -----------------------------------------------------------------------------
// 
CWlanMetaConnectionProviderFactory::CWlanMetaConnectionProviderFactory(TUid aFactoryId, CMetaConnectionFactoryContainer& aParentContainer)
	: CMetaConnectionProviderFactoryBase(aFactoryId,aParentContainer)
	{
	}

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProviderFactory::DoCreateL
// -----------------------------------------------------------------------------
// 
ESock::ACommsFactoryNodeId* CWlanMetaConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery)
	{
	const TMetaConnectionFactoryQuery& query = static_cast<const TMetaConnectionFactoryQuery&>(aQuery);
	CMetaConnectionProviderBase* provider = CWlanMetaConnectionProvider::NewL(*this, query.iProviderInfo);
	
	ESOCK_DEBUG_REGISTER_GENERAL_NODE(iUid, provider);
	
	return provider;
	}
