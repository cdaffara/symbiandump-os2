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
* Description:  Implements IPv6 bearer
*
*/

/*
* %version: 12 %
*/

#include <in_sock.h> // Header is retained, but in_sock.h is modified for ipv6
#include <in6_if.h>
#include <in_pkt.h>
#include <comms-infras/connectionsettings.h>
#include <nifmbuf.h>
#include "CLanIp6Bearer.h"
#include "WlanProto.h"
#include <comms-infras/es_protbinder.h>
#include "WlanProvision.h"

// From Wlan
#include <arp_hdr.h>
#include "am_debug.h"
#include "rwlmserver.h"

// Avoiding a warning in armv5 urel
#ifdef _DEBUG
_LIT(KCLanIp6BearerName,"EthWlan6");
#endif /* _DEBUG */

using namespace ESock;

// -----------------------------------------------------------------------------
// CLanIp6Bearer::ConstructL
// -----------------------------------------------------------------------------
//
void CLanIp6Bearer::ConstructL()
{
	DEBUG("CLanIp6Bearer::ConstructL()");

	iIfName.Format(_L("wlan6[0x%08x]"), this);
   	UpdateMACAddr();
	// ReadCommDbLanSettingsL() moved from here to point when provisioning information received
}

// -----------------------------------------------------------------------------
// CLanIp6Bearer::Control
// -----------------------------------------------------------------------------
//
TInt CLanIp6Bearer::Control(TUint aLevel,TUint aName,TDes8& aOption)
{
	DEBUG("CLanIp6Bearer::Control()");

	if ((aLevel==KCOLInterface) || (aLevel==KSOLInterface))
   		{
		switch (aName)
      		{
			case KSoIfHardwareAddr:
			 	{
				typedef TPckgBuf<TSoIfHardwareAddr> THwAddr;
			    THwAddr& theHwAddrRef = static_cast<THwAddr&>(aOption);
			    theHwAddrRef().iHardwareAddr.SetFamily(1);
			    theHwAddrRef().iHardwareAddr.SetPort(KArpHarwareType_ETHERNET);
			    theHwAddrRef().iHardwareAddr.SetLength(sizeof(SSockAddr));
			    theHwAddrRef().iHardwareAddr.Append(Link()->MacAddress());
			    return KErrNone;
				}
		default:
			break;
		    }
	    }
	
	else if (aLevel==KSolInetIp)
		{
		switch (aName)
			{
			case KSoIp6LeaveGroup:
			case KSoIp6JoinGroup:
			    {
			    			    
			    // Convert multicast IPv6 address to multicast MAC address
			    TIp6Mreq& opt = *(TIp6Mreq*)aOption.Ptr();
			    TUint8* tmp = opt.iAddr.u.iAddr8;
			    TMacAddress mac;
			    mac.iMacAddress[0] = 0x33;
			    mac.iMacAddress[1] = 0x33;
			    mac.iMacAddress[2] = tmp[12];
			    mac.iMacAddress[3] = tmp[13];
			    mac.iMacAddress[4] = tmp[14];
			    mac.iMacAddress[5] = tmp[15];

			    TIp6Addr addr6 = opt.iAddr;
				TInetAddr addr(addr6, 0);
				TBuf<KCommsDbSvrMaxFieldLength> aa;
				addr.Output(aa);
			    			    
			    if(aName == KSoIp6JoinGroup)
	        		DEBUG1("CLanIp6Bearer::Control() - joining multicast address:%S", &aa);
			    else
	        		DEBUG1("CLanIp6Bearer::Control() - leaving multicast address:%S", &aa);
			    
			    TInt ret = KErrNone;
			    RWLMServer wlmServer;
			    ret = wlmServer.Connect();
			    if( ret )
			        {
			        DEBUG1("CLanIp6Bearer::Control()- ERROR connecting RWlmServer: %d", ret);
			        return ret;
			        }
			    else
			    	{
			    	ret = wlmServer.ConfigureMulticast( aName, mac );
			    	wlmServer.Close();
			    	return KErrNone;
			    	}
			    }
			default:
				break;
      		}
   		}
	return KErrNotSupported;
}

// -----------------------------------------------------------------------------
// CLanIp6Bearer::ResolveMulticastIp6
// -----------------------------------------------------------------------------
//
void CLanIp6Bearer::ResolveMulticastIp6(TDes8& aDstAddr, RMBufChain& aPdu)
    { 
    // We need to obtain the IPv6 header - i.e. the destination address.
	// Octets 12 to 15 of the destination address, should be copied to
	// Octets 2 to 5 of the aDstAddr.
	// Octets 0 and 1 should be 0x3333.

	DEBUG("CLanIp6Bearer::ResolveMulticastIp6()");
	
	const TUint8 KMulticastPrefix[2] = {0x33, 0x33};

	TIpv6Header* header = (TIpv6Header*)aPdu.First()->Next()->Ptr(); 

	TUint ix;
	for(ix=0; ix<2; ix++)
	    {
	    aDstAddr[ix]=KMulticastPrefix[ix];
	    }
	for(ix=2; ix<6; ix++)
	    {
	    aDstAddr[ix]=header->iDestAddrB[ix+10];
	    }
    }

// -----------------------------------------------------------------------------
// CLanIp6Bearer::Send
// -----------------------------------------------------------------------------
//
MLowerDataSender::TSendResult CLanIp6Bearer::Send(RMBufChain& aPdu)
{
	// Check that we are allowed to send data
	if (!Link()->BearerIsActive(this))
		{
			aPdu.Free();
			return ESendBlocked;
		}
	
	RMBufPktInfo* info = RMBufPacket::PeekInfoInChain(aPdu);
	TUint32 family = info->iDstAddr.Family();
	__ASSERT_DEBUG(family!=KAfInet, User::Panic(KCLanIp6BearerName, 0));

	TBuf8<KMACByteLength> destAddr;
	destAddr.SetMax();

	if(family == KAfInet6)
		{
		// Only multicast packets come here with address family KAfInet6,
		// generate the ethernet address as described in RFC 2464, section 7.
		ResolveMulticastIp6(destAddr, aPdu);
		}
	else if(family == 1)
		{
		// Address family == 1, ethernet address is found from info
		destAddr.Copy(info->iDstAddr.Mid(sizeof(SSockAddr), KMACByteLength));
		}
	else
		{
		aPdu.Free();
		return static_cast<MLowerDataSender::TSendResult>(1);
		}

	return static_cast<MLowerDataSender::TSendResult>(Link()->FrameSend(aPdu,destAddr,KIP6FrameType));
}


// -----------------------------------------------------------------------------
// CLanIp6Bearer::WantsProtocol
// -----------------------------------------------------------------------------
//
TBool CLanIp6Bearer::WantsProtocol(TUint16 aProtocolCode,const TUint8* aPayload)
{
	DEBUG("CLanIp6Bearer::WantsProtocol()");

	return (aProtocolCode==KIP6FrameType && (aPayload[0]>>4)==KIP6Protocol) ? ETrue:EFalse;
}

// -----------------------------------------------------------------------------
// CLanIp6Bearer::Process
// -----------------------------------------------------------------------------
//
void CLanIp6Bearer::Process(RMBufChain& aPdu)
{
	DEBUG("CLanIp6Bearer::Process()");

	if(iUpperReceiver)
		{
		iUpperReceiver->Process(aPdu);
		}
	else
		{
	  	aPdu.Free();
	  	}
}
// -----------------------------------------------------------------------------
// CLanIp6Bearer::UpdateMACAddr
// -----------------------------------------------------------------------------
//
void CLanIp6Bearer::UpdateMACAddr()
{
	DEBUG("CLanIp6Bearer::UpdateMACAddr()");

	TE64Addr myEuMac;
	myEuMac.SetAddrFromEUI48(Link()->MacAddress().Ptr());
	iEuiMac.SetAddress(myEuMac);
}

// -----------------------------------------------------------------------------
// CLanIp6Bearer::ReadCommDbLanSettingsL
// -----------------------------------------------------------------------------
//
void CLanIp6Bearer::ReadCommDbLanSettingsL()
{
	DEBUG("CLanIp6Bearer::ReadCommDbLanSettingsL()");

	ASSERT(iProvision);
	iPrimaryDns = iProvision->PrimaryDns();
	iSecondaryDns = iProvision->SecondaryDns();
}

// -----------------------------------------------------------------------------
// CLanIp6Bearer::GetConfig
// -----------------------------------------------------------------------------
//
TInt CLanIp6Bearer::GetConfig(TBinderConfig& aConfig)
	{
	DEBUG("CLanIp6Bearer::GetConfig()");
	
	TBinderConfig6& config = static_cast<TBinderConfig6&>(aConfig);

	config.iFamily = KAfInet6;
		
	config.iInfo.iFeatures = KIfNeedsND | KIfCanMulticast;
	config.iInfo.iMtu = Link()->Mtu();
	config.iInfo.iRMtu = Link()->Mtu();
	config.iInfo.iSpeedMetric = Link()->SpeedMetric();
	
	TEui64Addr& localId = TEui64Addr::Cast(config.iLocalId);
	localId = iEuiMac;

	// If required, configure static DNS addresses

	if (!iPrimaryDns.IsUnspecified())
		{
		config.iNameSer1.SetAddress(iPrimaryDns);
		if (!iSecondaryDns.IsUnspecified())
			config.iNameSer2.SetAddress(iSecondaryDns);
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLanIp6Bearer::ProtocolName
// -----------------------------------------------------------------------------
//
const TDesC8& CLanIp6Bearer::ProtocolName() const
/**
Return the protocol name of this bearer

Called from CLANLinkCommon
*/
	{
	DEBUG("CLanIp6Bearer::ProtocolName()");
	
	_LIT8(KProtocolName, "ip6");
	return KProtocolName();
	}
// -----------------------------------------------------------------------------
// CLanIp6Bearer::SetProvisionL
// -----------------------------------------------------------------------------
//
void CLanIp6Bearer::SetProvisionL(const Meta::SMetaData* aProvision)
	{
	DEBUG("CLanIp6Bearer::SetProvisionL()");
	
	if (iProvision == NULL)
		{
		iProvision = static_cast<const TLanIp6Provision*>(aProvision);
   		ReadCommDbLanSettingsL();
		}
	}
	
