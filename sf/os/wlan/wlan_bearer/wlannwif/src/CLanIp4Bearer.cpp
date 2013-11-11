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
* Description:  Implements IPv4 bearer
*
*/

/*
* %version: 13 %
*/

#include <in_sock.h> // Header is retained, but in_sock.h is modified for ipv6
#include <in_pkt.h>
#include <in6_if.h>
#include <nifvar.h>
#include <comms-infras/connectionsettings.h>
#include <nifmbuf.h>
#include "CLanIp4Bearer.h"
#include "arp_hdr.h"
#include <commdb.h>
#include <cdblen.h>
#include "WlanProto.h"
#include <comms-infras/es_protbinder.h>
#include "WlanProvision.h"

// From Wlan
#include "am_debug.h"
#include "rwlmserver.h"

// Avoiding a warning in armv5 urel
#ifdef _DEBUG
_LIT(KCLanIp4BearerName,"EthWlan4");
#endif /* _DEBUG */

using namespace ESock;

/**
Required ConstructL() method.  Performs the following:
"	Sets a unique name for this binder instance in the iIfName field - "wlan[0x%x]" where "%x"
    is the hexadecimal address of the "this" object.
"	Reads the IP Interface Settings using ReadIPInterfaceSettingsL().
*/
void CLanIp4Bearer::ConstructL()
{
    DEBUG("CLanIp4Bearer::ConstructL()");
 
	iIfName.Format(_L("wlan[0x%08x]"), this);
	iBroadcastConfigured = EFalse;
	iArpMsgNext = EFalse;
	// ReadCommDbLanSettingsL() moved from here to point when provisioning information received

}

TInt CLanIp4Bearer::Control(TUint aLevel, TUint aName, TDes8& aOption)
	{
    DEBUG("CLanIp4Bearer::Control()");
	
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
			    			    
			    // Convert multicast IPv4 address to multicast MAC address
			    TIp6Mreq& opt = *(TIp6Mreq*)aOption.Ptr();
			    TUint8* tmp = opt.iAddr.u.iAddr8;
			    TMacAddress mac;
			    mac.iMacAddress[0] = 0x01;
			    mac.iMacAddress[1] = 0x00;
			    mac.iMacAddress[2] = 0x5E;
			    mac.iMacAddress[3] = tmp[13];
			    mac.iMacAddress[3] &= 0x7F; // Most significant bit is set zero
			    mac.iMacAddress[4] = tmp[14];
			    mac.iMacAddress[5] = tmp[15];  
			    
			    TIp6Addr addr6 = opt.iAddr;
				TInetAddr addr(addr6, 0);
				TBuf<39> aa;
				addr.Output(aa);
			    			    
			    if(aName == KSoIp6JoinGroup)
	        		DEBUG1("CLanIp4Bearer::Control() - joining multicast address:%S", &aa);
			    else
	        		DEBUG1("CLanIp4Bearer::Control() - leaving multicast address:%S", &aa);
			    
			    TInt ret = KErrNone;
			    RWLMServer wlmServer;
			    ret = wlmServer.Connect();
			    if( ret )
			        {
	            	DEBUG1("CLanIp4Bearer::Control()- ERROR connecting RWlmServer: %d", ret);
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
// CLanIp4Bearer::Send
// -----------------------------------------------------------------------------
//
MLowerDataSender::TSendResult CLanIp4Bearer::Send(RMBufChain& aPdu)
	{
	DEBUG("CLanIp4Bearer::Send()");
	// Call down from the Protocol
	// Real Protocol data is in the second MBuf of the RMBufChain

	RMBufPktInfo* info = RMBufPacket::PeekInfoInChain(aPdu);
	TUint32 family = info->iDstAddr.Family();
	__ASSERT_DEBUG(family!=KAfInet6, User::Panic(KCLanIp4BearerName, 0));

	TBuf8<KMACByteLength> destAddr;
	const TUint8 etherBroadcastAddr[KMACByteLength] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	if(family == KAfInet)
		{
		// Only broadcast and multicast packets come here with address family KAfInet
		TInetAddr dest(info->iDstAddr);
		if( dest.IsMulticast() )
			{
			// multicast
			const TUint8 IpV4MulticastPrefix[KMACByteLength / 2] = {0x01, 0x00, 0x5e};
			destAddr.Copy(IpV4MulticastPrefix, KMACByteLength / 2);

			TUint32 mCastAdrrEnd = dest.Address() & 0x007fffff;
			BigEndian::Put32(reinterpret_cast<TUint8*>(&mCastAdrrEnd), mCastAdrrEnd);
			destAddr.Append(reinterpret_cast<TUint8*>(&mCastAdrrEnd) + 1, KMACByteLength / 2);
			}
        else
            {
            // broadcast
            destAddr.Copy(etherBroadcastAddr, KMACByteLength);
            }
		}
	else if(family == 1)
		{
		// Address family == 1, ethernet address is found from info
		destAddr.Copy(info->iDstAddr.Mid(sizeof(SSockAddr), KMACByteLength));
		}
	else
		{
		DEBUG1( "CLanIp4Bearer::Send() - unsupported family type: %u",
		    family );

		aPdu.Free();
		return static_cast<MLowerDataSender::TSendResult>(1);
		}

	TInt ret = Link()->FrameSend(aPdu, destAddr, (TUint)info->iProtocol == KProtocolArp ? KArpFrameType : KIPFrameType);

	return static_cast<MLowerDataSender::TSendResult>(ret);
}

// -----------------------------------------------------------------------------
// CLanIp4Bearer::StartSending
// -----------------------------------------------------------------------------
//
void CLanIp4Bearer::StartSending(CProtocolBase* aProtocol)
{
	DEBUG("CLanIp4Bearer::StartSending()");

	CLanxBearer::StartSending(aProtocol);
	
	if (!iBroadcastConfigured)
		{
		TMacAddress broadcastmac;
	    broadcastmac.iMacAddress[0] = 0xFF;
	    broadcastmac.iMacAddress[1] = 0xFF;
	    broadcastmac.iMacAddress[2] = 0xFF;
	    broadcastmac.iMacAddress[3] = 0xFF;
	    broadcastmac.iMacAddress[4] = 0xFF;
	    broadcastmac.iMacAddress[5] = 0xFF; 
	    
	    RWLMServer wlmServer;
	    if (wlmServer.Connect() != KErrNone)
	    	{
	    	DEBUG("CLanIp4Bearer::StartSending(), ERROR connecting to wlan engine");
	    	}
	    else
	    	{
	    	DEBUG("CLanIp4Bearer::StartSending(), Configuring umac to receive Broadcast frames");
	    	if( wlmServer.ConfigureMulticast( KSoIp6JoinGroup, broadcastmac ) )
	    		{
	    		DEBUG("CLanIp4Bearer::StartSending(),ERROR calling ConfigureMulticast");
	    		}
	    	wlmServer.Close();
	    	iBroadcastConfigured = ETrue;
	    	}	    
		}
}

// -----------------------------------------------------------------------------
// CLanIp4Bearer::WantsProtocol
// -----------------------------------------------------------------------------
//
TBool CLanIp4Bearer::WantsProtocol(TUint16 aProtocolCode,const TUint8* aPayload)
{
	DEBUG("CLanIp4Bearer::WantsProtocol()");

    iArpMsgNext = (aProtocolCode == KArpFrameType);
	return ((aProtocolCode==KIPProtocol && (aPayload[0]>>4)==KIP4Protocol) || (aProtocolCode==KArpFrameType))  ? ETrue:EFalse;
}

// -----------------------------------------------------------------------------
// CLanIp4Bearer::Process
// -----------------------------------------------------------------------------
//
void CLanIp4Bearer::Process(RMBufChain& aPdu)
{
	DEBUG("CLanIp4Bearer::Process()");
	
	if (iUpperReceiver)
		{
		RMBufPktInfo* info = RMBufPacket::PeekInfoInChain(aPdu);
		if (info)
			{
			info->iProtocol = iArpMsgNext ? KProtocolArp : KProtocolInetIp;
			}
		iUpperReceiver->Process(aPdu);
		}
	else
		{
	  	aPdu.Free();
	  	}
}

// -----------------------------------------------------------------------------
// CLanIp4Bearer::ReadCommDbLanSettingsL
// -----------------------------------------------------------------------------
//
void CLanIp4Bearer::ReadCommDbLanSettingsL()
{
	DEBUG("CLanIp4Bearer::ReadCommDbLanSettingsL()");

	ASSERT(iProvision);
	
	iLocalAddr = iProvision->LocalAddr();
	iNetMask = iProvision->NetMask();
	iDefGateway = iProvision->DefGateway();
	iPrimaryDns = iProvision->PrimaryDns();
	iSecondaryDns = iProvision->SecondaryDns();
}

// -----------------------------------------------------------------------------
// CLanIp4Bearer::GetConfig
// -----------------------------------------------------------------------------
//
TInt CLanIp4Bearer::GetConfig(TBinderConfig& aConfig)
	{
	DEBUG("CLanIp4Bearer::GetConfig()");
	
	TBinderConfig4& config = static_cast<TBinderConfig4&>(aConfig);
	
	config.iFamily = KAfInet;		/* KAfInet / KAfInet6 - selects TBinderConfig4/6 */

	config.iInfo.iFeatures = KIfCanBroadcast | KIfCanMulticast | KIfNeedsND;		/* Feature flags */
	config.iInfo.iMtu = Link()->Mtu();					/* Maximum transmission unit. */
	config.iInfo.iRMtu = Link()->Mtu();					/* Maximum transmission unit for receiving. */
	config.iInfo.iSpeedMetric = Link()->SpeedMetric();	/* approximation of the interface speed in Kbps. */
	
	config.iAddress.SetAddress(iLocalAddr);			/* Interface IP address. */
	config.iNetMask.SetAddress(iNetMask);			/* IP netmask. */
	config.iBrdAddr.SetAddress(iBroadcastAddr);		/* IP broadcast address. */
	config.iDefGate.SetAddress(iDefGateway);		/* IP default gateway or peer address (if known). */
	config.iNameSer1.SetAddress(iPrimaryDns);		/* IP primary name server (if any). */
	config.iNameSer2.SetAddress(iSecondaryDns);		/* IP secondary name server (if any). */

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CLanIp4Bearer::ProtocolName
// -----------------------------------------------------------------------------
//
const TDesC8& CLanIp4Bearer::ProtocolName() const
	{
	DEBUG("CLanIp4Bearer::ProtocolName()");
	
	_LIT8(KProtocolName, "ip");
	return KProtocolName();
	}
	
// -----------------------------------------------------------------------------
// CLanIp4Bearer::SetProvisionL
// -----------------------------------------------------------------------------
//
void CLanIp4Bearer::SetProvisionL(const Meta::SMetaData* aProvision)
	{
	DEBUG("CLanIp4Bearer::SetProvisionL()");
	
	if (iProvision == NULL)
		{
		iProvision = static_cast<const TLanIp4Provision*>(aProvision);
		ReadCommDbLanSettingsL();
		}
	}
	
