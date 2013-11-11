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
* Description:  Defines WLAN meta connection provider
*
*/

/*
* %version: 5 %
*/

#ifndef __WLANMCPR_H__
#define __WLANMCPR_H__

#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/agentmcpr.h>


namespace WlanMCprStates
    {
	class TSendProvision;
    DECLARE_EXPORT_ACTIVITY_MAP(stateMap)	
    }
    
class CWlanMetaConnectionProviderFactory;

class CWlanMetaConnectionProvider : public CAgentMetaConnectionProvider
    {
    friend class WlanMCprStates::TSendProvision;

public:
    typedef CWlanMetaConnectionProviderFactory FactoryType;

	IMPORT_C static CWlanMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const ESock::TProviderInfo& aProviderInfo);
	IMPORT_C virtual ~CWlanMetaConnectionProvider();

protected:
    IMPORT_C CWlanMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory,
							   const ESock::TProviderInfo& aProviderInfo,
							   const MeshMachine::TNodeActivityMap& aActivityMap);    
    
    void SetAccessPointConfigFromDbL();
	IMPORT_C virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	IMPORT_C void ConstructL();

private:
	void ProvisionNetworkConfigL(ESock::CCommsDatIapView* aIapView, ESock::RMetaExtensionContainer& aMec);
    void ProvisionIp4ConfigL(ESock::CCommsDatIapView* aIapView, TUint32 aOrder, ESock::RMetaExtensionContainer& aMec);
    void ProvisionIp6ConfigL(ESock::CCommsDatIapView* aIapView, TUint32 aOrder, ESock::RMetaExtensionContainer& aMec);
    };

#endif // __WLANMCPR_H__
