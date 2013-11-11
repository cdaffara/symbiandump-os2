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
* Description:  Implements WLAN meta connection provider
*
*/

/*
* %version: 8 %
*/

#include <comms-infras/ss_log.h>
#include <comms-infras/agentmcpractivities.h>
#include <comms-infras/coremcpractivities.h>
#include <comms-infras/ss_tiermanagerutils.h>
#include <comms-infras/ss_msgintercept.h>
#include "wlanmcpr.h"
#include "WlanProvision.h"
#include "am_debug.h"


using namespace ESock;
using namespace NetStateMachine;
using namespace MCprActivities;
using namespace WlanMCprStates;
using namespace Messages;



// No Bearer Activity
namespace WlanMCPRNoBearerActivity
{
DECLARE_DEFINE_NODEACTIVITY(ECFActivityNoBearer, WlanMCPRNoBearer, TCFControlProvider::TNoBearer)
	FIRST_NODEACTIVITY_ENTRY(CoreNetStates::TAwaitingNoBearer, MeshMachine::TNoTag)
	LAST_NODEACTIVITY_ENTRY(MeshMachine::KNoTag, CoreNetStates::TSendBearer)
NODEACTIVITY_END()
}



// Activity Map
namespace WlanMCprStates
{
DEFINE_EXPORT_ACTIVITY_MAP(stateMap)
	ACTIVITY_MAP_ENTRY(WlanMCPRNoBearerActivity, WlanMCPRNoBearer)
ACTIVITY_MAP_END_BASE(AgentMCprActivities, agentMCprActivities)
} // namespace WlanMCprStates

//-=========================================================
//
//CWlanMetaConnectionProvider implementation
//
//-=========================================================

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CWlanMetaConnectionProvider* CWlanMetaConnectionProvider::NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory, const TProviderInfo& aProviderInfo)
    {
    CWlanMetaConnectionProvider* self = new (ELeave) CWlanMetaConnectionProvider(aFactory, aProviderInfo, WlanMCprStates::stateMap::Self());
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::ConstructL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWlanMetaConnectionProvider::ConstructL()
	{
	CAgentMetaConnectionProvider::ConstructL();
	SetAccessPointConfigFromDbL();
	}

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::CWlanMetaConnectionProvider
// -----------------------------------------------------------------------------
//
EXPORT_C CWlanMetaConnectionProvider::CWlanMetaConnectionProvider(CMetaConnectionProviderFactoryBase& aFactory,
    								const ESock::TProviderInfo& aProviderInfo,
									const MeshMachine::TNodeActivityMap& aActivityMap)
:	CAgentMetaConnectionProvider(aFactory,aProviderInfo,aActivityMap)
	{
	
	}

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::~CWlanMetaConnectionProvider
// -----------------------------------------------------------------------------
//
EXPORT_C CWlanMetaConnectionProvider::~CWlanMetaConnectionProvider()
	{
	
	}

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::ReceivedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWlanMetaConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
    {
    DEBUG1("CWlanMetaConnectionProvider::ReceivedL, aCFMessage=%d", aMessage.MessageId().MessageId());

	//ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);
	
	MeshMachine::TNodeContext<CWlanMetaConnectionProvider> ctx(*this, aMessage, aSender, aRecipient);
    CCoreMetaConnectionProvider::Received(ctx);
    User::LeaveIfError(ctx.iReturn);
	}

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::SetAccessPointConfigFromDbL
// -----------------------------------------------------------------------------
//
void CWlanMetaConnectionProvider::SetAccessPointConfigFromDbL()
    {
    DEBUG("CWlanMetaConnectionProvider::SetAccessPointConfigFromDbL");
    
    RMetaExtensionContainer mec;
    static_cast<void>(mec.Open(AccessPointConfig()));
    CleanupClosePushL(mec);
    
    // Open an IAP specific view on CommsDat
	CCommsDatIapView* iapView = OpenIapViewLC();
	
	// Presumptions:
	// - none of the extensions can already exist in the AccessPointConfig array.  AppendExtensionL()
	//   is presumed to panic if adding the same extension a second time.
	// - if we have added several extensions to the AccessPointConfig array before getting a failure
	//   and leaving, it is presumed that the MCPr will be destroyed and AccessPointConfig destructor
	//   will clean up the extensions immediately afterwards.
	
    ProvisionNetworkConfigL(iapView, mec);
    
    CleanupStack::PopAndDestroy(); // CloseIapView();
    
    AccessPointConfig().Close();
    AccessPointConfig().Open(mec);
    CleanupStack::PopAndDestroy(&mec);
	}

// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::ProvisionNetworkConfigL
// -----------------------------------------------------------------------------
//
void CWlanMetaConnectionProvider::ProvisionNetworkConfigL(CCommsDatIapView* aIapView, ESock::RMetaExtensionContainer& aMec)
    {
	HBufC* buf = NULL;
    aIapView->GetTextL(KCDTIdLANIfNetworks, buf);
    CleanupStack::PushL(buf);
    if (buf->Length() == 0) 
        {
        User::Leave(KErrCorrupt);
        }

    TPtrC16 networks;
    networks.Set(*buf);
    TPtrC16 current;
    TUint32 order = 0;
    TInt commaPos = 0;
    while (commaPos != KErrNotFound) 
        {
        commaPos = networks.LocateF(',');
        if (commaPos == KErrNotFound) 
            {
            // take all of string
            current.Set(networks);
            }
        else
            {
            current.Set(networks.Ptr(), commaPos);
            }
        
        if (!current.CompareF(_L("ip"))) 
            {
            ProvisionIp4ConfigL(aIapView, order, aMec);
            }
        else if (!current.CompareF(_L("ip6"))) 
            {
            ProvisionIp6ConfigL(aIapView, order, aMec);
            }
        else 
            {
            User::Leave(KErrCorrupt);
            }
        
        order++;
        networks.Set(networks.Mid(commaPos+1));
        }
    
    CleanupStack::PopAndDestroy(buf);
    }
    
// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::ProvisionIp4ConfigL
// -----------------------------------------------------------------------------
//
void CWlanMetaConnectionProvider::ProvisionIp4ConfigL(CCommsDatIapView* aIapView, TUint32 aOrder, ESock::RMetaExtensionContainer& aMec)
    {
	TLanIp4Provision* ip4Provision = new (ELeave) TLanIp4Provision;
	CleanupStack::PushL(ip4Provision);
	
	ip4Provision->SetOrder(aOrder);
	
	// Read IP address configuration parameters
	TBool serverRequired;
	aIapView->GetBoolL(KCDTIdLANIpAddrFromServer, serverRequired);

	TUint32 addr;
	TInt err;

	ip4Provision->SetLocalAddr(KInetAddrNone);
	ip4Provision->SetNetMask(KInetAddrNone);
	ip4Provision->SetDefGateway(KInetAddrNone);
	
	if (!serverRequired)
		{
		GetIp4AddrL(aIapView, KCDTIdLANIpAddr, addr);
  		ip4Provision->SetLocalAddr(addr);
		GetIp4AddrL(aIapView, KCDTIdLANIpNetMask, addr);
		ip4Provision->SetNetMask(addr);
		
		err = GetIp4Addr(aIapView, KCDTIdLANIpGateway, addr);
		if (err == KErrNone)
		    {
		    ip4Provision->SetDefGateway(addr);
		    }
		else if (err == KErrNotFound)
		    {
		    ip4Provision->SetDefGateway(ip4Provision->LocalAddr());
		    }
		else
		    {
		    User::Leave(err);
		    }

		// Because CommDB doesn't define a Broadcast Address field, we must
		// calculate the broadcast address. This is based on the localAddr
		// and the netMask.

		TInetAddr localAddr(ip4Provision->LocalAddr(), 0);
		TInetAddr netMask(ip4Provision->NetMask(), 0);
		TInetAddr broadcast;
		broadcast.SubNetBroadcast(ip4Provision->LocalAddr(), ip4Provision->NetMask());
		ip4Provision->SetBroadcastAddr(broadcast.Address());
   		}

	ip4Provision->SetPrimaryDns(KInetAddrNone);
	ip4Provision->SetSecondaryDns(KInetAddrNone);
	
	aIapView->GetBoolL(KCDTIdLANIpDNSAddrFromServer, serverRequired);
  	if (!serverRequired)
  		{
  		err = GetIp4Addr(aIapView, KCDTIdLANIpNameServer1, addr);
  		if (err == KErrNone) 
  		    {
  		    ip4Provision->SetPrimaryDns(addr);
		    err = GetIp4Addr(aIapView, KCDTIdLANIpNameServer2, addr);
		    if (err == KErrNone)
		        {
		        ip4Provision->SetSecondaryDns(addr);
		        }
  		    }
  		
  		if (err != KErrNone && err != KErrNotFound) 
  		    {
  		    User::Leave(err);
  		    }
     	}	
	
	// Append the provisioning object to the CAccessPointConfig array
	aMec.AppendExtensionL(ip4Provision);
	CleanupStack::Pop(ip4Provision);
    }
    
// -----------------------------------------------------------------------------
// CWlanMetaConnectionProvider::ProvisionIp6ConfigL
// -----------------------------------------------------------------------------
//    
void CWlanMetaConnectionProvider::ProvisionIp6ConfigL(CCommsDatIapView* aIapView, TUint32 aOrder, ESock::RMetaExtensionContainer& aMec)
    {
	TLanIp6Provision* ip6Provision = new (ELeave) TLanIp6Provision;
	CleanupStack::PushL(ip6Provision);
	
	ip6Provision->SetOrder(aOrder);
	
	// Determine whether static DNS configuration is required.
	TBool dynamicDns = ETrue;

	// By default, Ensure that static DNS addresses are set as unspecified,
	// so they are not used in Control(KSoIfConfig).
	ip6Provision->SetPrimaryDns(KInet6AddrNone);
	ip6Provision->SetSecondaryDns(KInet6AddrNone);
	
	// Ignore any errors from reading this field - default to dynamicDns = ETrue
	(void) aIapView->GetBool(KCDTIdLANIp6DNSAddrFromServer, dynamicDns);

	if (!dynamicDns)
		{
		// Read static DNS addresses
		TInt err;
		TIp6Addr addr6;
  		err = GetIp6Addr(aIapView, KCDTIdLANIpNameServer1, addr6);
  		if (err == KErrNone) 
  		    {
  		    ip6Provision->SetPrimaryDns(addr6);
		    err = GetIp6Addr(aIapView, KCDTIdLANIpNameServer2, addr6);
		    if (err == KErrNone)
		        {
		        ip6Provision->SetSecondaryDns(addr6);
		        }
  		    }
  		
  		if (err != KErrNone && err != KErrNotFound) 
  		    {
  		    User::Leave(err);
  		    }
		}
	
	// Append the provisioning object to the CAccessPointConfig array
	aMec.AppendExtensionL(ip6Provision);
	CleanupStack::Pop(ip6Provision);
    }

    
