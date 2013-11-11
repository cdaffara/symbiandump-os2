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
* Description:  Implements WLAN CFP flow factory
*
*/

/*
* %version: 4 %
*/

#include "WlanCfproto.h"
#include "WlanProto.h"

using namespace ESock;

// Wlan Flow Factory

// -----------------------------------------------------------------------------
// CWlanSubConnectionFlowFactory::NewL
// -----------------------------------------------------------------------------
//
CWlanSubConnectionFlowFactory* CWlanSubConnectionFlowFactory::NewL(TAny* aConstructionParameters)
/**
Constructs a Default SubConnection Flow Factory

@param aConstructionParameters construction data passed by ECOM

@returns pointer to a constructed factory
*/
	{
	CWlanSubConnectionFlowFactory* ptr = new (ELeave) CWlanSubConnectionFlowFactory(TUid::Uid(KWlanFlowImplementationUid), *(reinterpret_cast<CSubConnectionFlowFactoryContainer*>(aConstructionParameters)));
	return ptr;
	}

// -----------------------------------------------------------------------------
// CWlanSubConnectionFlowFactory::CWlanSubConnectionFlowFactory
// -----------------------------------------------------------------------------
//
CWlanSubConnectionFlowFactory::CWlanSubConnectionFlowFactory(TUid aFactoryId, CSubConnectionFlowFactoryContainer& aParentContainer)
	: CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
/**
Default SubConnection Flow Factory Constructor

@param aFactoryId ECOM Implementation Id
@param aParentContainer Object Owner
*/
	{
	}

// -----------------------------------------------------------------------------
// CWlanSubConnectionFlowFactory::DoCreateL
// -----------------------------------------------------------------------------
//
CSubConnectionFlowBase* CWlanSubConnectionFlowFactory::DoCreateFlowL(ESock::CProtocolIntfBase* aProtocolIntf, TFactoryQueryBase& aQuery)
	{
	const TDefaultFlowFactoryQuery& query = static_cast<const TDefaultFlowFactoryQuery&>(aQuery);
	return CLANLinkCommon::NewL(*this, query.iSCprId, aProtocolIntf);
	}


