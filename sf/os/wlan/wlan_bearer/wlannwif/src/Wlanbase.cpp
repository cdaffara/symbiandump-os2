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
* Description:  Implements functions for LAN interface
*
*/

/*
* %version: 24 %
*/

#include <f32file.h>
#include <e32std.h> // for TTime
#include <nifvar.h>
#include <nifutl.h>
#include <es_mbuf.h>
#include <nifmbuf.h>
#include <comms-infras/nifprvar.h>
#include <comms-infras/connectionsettings.h>  // for KSlashChar
#include <commdb.h>
#include "WlanProto.h"
#include "carddrv.h"
#include <in_sock.h> // Header is retained, but in_sock.h is modified for ipv6
#include "CLanIp4Bearer.h"
#include "CLanIp6Bearer.h"
#include <cdbcols.h>
#include <cdblen.h>
#include "CLanxBearer.h"
#include "WlanProvision.h"
#include <comms-infras/ss_metaconnprov.h> // for SAccessPointConfig
#include <comms-infras/linkmessages.h>
#include <elements/nm_messages_base.h>
#include <elements/nm_messages_child.h>

// From old wlan implementation
#include <ip4_hdr.h>
#include <ip6_hdr.h>
#include <centralrepository.h>
#include "am_debug.h"
#include "EtherCardApi.h"
#include "wlandevicesettingsinternalcrkeys.h"
#include "NifWLMServerIf.h"


using namespace ESock;
using namespace Messages;

_LIT8(KDescIp, "ip");
_LIT8(KDescIcmp, "icmp");
_LIT8(KDescIp6, "ip6");

const TInt KLanxBearerPtrArrayGranularity = 2;
const TInt KContinueSending = 1;
const TInt KMaxDSCPValues = 64;

/**
 * Mapping of user priority values to the corresponding access classes.
 */
const TWlmAccessClass KUPtoAC[E8021DUserPriorityMax] =
    {
    EWlmAccessClassBestEffort,      // E8021DUserPriorityBE
    EWlmAccessClassBackground,      // E8021DUserPriorityBK1
    EWlmAccessClassBackground,      // E8021DUserPriorityBK2
    EWlmAccessClassBestEffort,      // E8021DUserPriorityEE
    EWlmAccessClassVideo,           // E8021DUserPriorityCL
    EWlmAccessClassVideo,           // E8021DUserPriorityVI
    EWlmAccessClassVoice,           // E8021DUserPriorityVO
    EWlmAccessClassVoice,           // E8021DUserPriorityNC
    };

/**
 * Mapping of access class transitions on packet downgrade.
 */
const TWlmAccessClass KNextAC[EWlmAccessClassMax] =
    {
    EWlmAccessClassBackground,      // EWlmAccessClassBestEffort -> EWlmAccessClassBackground
    EWlmAccessClassMax,             // EWlmAccessClassBackground -> EWlmAccessClassMax
    EWlmAccessClassBestEffort,      // EWlmAccessClassVideo -> EWlmAccessClassBestEffort
    EWlmAccessClassVideo            // EWlmAccessClassVoice -> EWlmAccessClassVideo
    };

/**
 * Mapping of IP TOS (DSCP) bits to the corresponding user priority values.
 */
const T8021DPriority KDSCPtoUP[KMaxDSCPValues] =
    {
    /* DSCP bits as binary format, DSCP byte in IPv4 packet as HEX */
    
    /* 000000, 0x00 */E8021DUserPriorityBE,  /* 000001, 0x04 */E8021DUserPriorityBE,  
    /* 000010, 0x08 */E8021DUserPriorityBE,  /* 000011, 0x0C */E8021DUserPriorityBE,
    /* 000100, 0x10 */E8021DUserPriorityBE,  /* 000101, 0x14 */E8021DUserPriorityBE,  
    /* 000110, 0x18 */E8021DUserPriorityBE,  /* 000111, 0x1C */E8021DUserPriorityBE,
    /* 001000, 0x20 */E8021DUserPriorityBK1, /* 001001, 0x24 */E8021DUserPriorityBK1,  
    /* 001010, 0x28 */E8021DUserPriorityEE,  /* 001011, 0x2C */E8021DUserPriorityEE,
    /* 001100, 0x30 */E8021DUserPriorityEE,  /* 001101, 0x34 */E8021DUserPriorityEE,  
    /* 001110, 0x38 */E8021DUserPriorityEE,  /* 001111, 0x3C */E8021DUserPriorityEE,

    /* 010000, 0x40 */E8021DUserPriorityEE,  /* 010001, 0x44 */E8021DUserPriorityEE,  
    /* 010010, 0x48 */E8021DUserPriorityEE,  /* 010011, 0x4C */E8021DUserPriorityEE,
    /* 010100, 0x50 */E8021DUserPriorityEE,  /* 010101, 0x54 */E8021DUserPriorityEE,  
    /* 010110, 0x58 */E8021DUserPriorityEE,  /* 010111, 0x5C */E8021DUserPriorityEE,
    /* 011000, 0x60 */E8021DUserPriorityVI,  /* 011001, 0x64 */E8021DUserPriorityVI,  
    /* 011010, 0x68 */E8021DUserPriorityVI,  /* 011011, 0x6C */E8021DUserPriorityVI,
    /* 011100, 0x70 */E8021DUserPriorityVI,  /* 011101, 0x74 */E8021DUserPriorityVI,  
    /* 011110, 0x78 */E8021DUserPriorityVI,  /* 011111, 0x7C */E8021DUserPriorityVI,

    /* 100000, 0x80 */E8021DUserPriorityVI,  /* 100001, 0x84 */E8021DUserPriorityVI,  
    /* 100010, 0x88 */E8021DUserPriorityVI,  /* 100011, 0x8C */E8021DUserPriorityVI,
    /* 100100, 0x90 */E8021DUserPriorityVI,  /* 100101, 0x94 */E8021DUserPriorityVI,  
    /* 100110, 0x98 */E8021DUserPriorityVI,  /* 100111, 0x9C */E8021DUserPriorityVI,
    /* 101000, 0xA0 */E8021DUserPriorityCL,  /* 101001, 0xA4 */E8021DUserPriorityCL,  
    /* 101010, 0xA8 */E8021DUserPriorityCL,  /* 101011, 0xAC */E8021DUserPriorityCL,
    /* 101100, 0xB0 */E8021DUserPriorityCL,  /* 101101, 0xB4 */E8021DUserPriorityCL,  
    /* 101110, 0xB8 */E8021DUserPriorityVO,  /* 101111, 0xBC */E8021DUserPriorityVO,

    /* 110000, 0xC0 */E8021DUserPriorityNC,  /* 110001, 0xC4 */E8021DUserPriorityNC,  
    /* 110010, 0xC8 */E8021DUserPriorityNC,  /* 110011, 0xCC */E8021DUserPriorityNC,
    /* 110100, 0xD0 */E8021DUserPriorityNC,  /* 110101, 0xD4 */E8021DUserPriorityNC,  
    /* 110110, 0xD8 */E8021DUserPriorityNC,  /* 110111, 0xDC */E8021DUserPriorityNC,
    /* 111000, 0xE0 */E8021DUserPriorityNC,  /* 111001, 0xE4 */E8021DUserPriorityNC,  
    /* 111010, 0xE8 */E8021DUserPriorityNC,  /* 111011, 0xEC */E8021DUserPriorityNC,
    /* 111100, 0xF0 */E8021DUserPriorityNC,  /* 111101, 0xF4 */E8021DUserPriorityNC,  
    /* 111110, 0xF8 */E8021DUserPriorityNC,  /* 111111, 0xFC */E8021DUserPriorityNC
    };

/**
 * Inactivity timeouts per access class.
 */
const TUint KTsInactivityTime[EWlmAccessClassMax] =
    {
    600000000,    // 600 seconds for EWlmAccessClassBestEffort
    600000000,    // 600 seconds for EWlmAccessClassBackground
    120000000,    // 120 seconds for EWlmAccessClassVideo
    30000000,     // 30 seconds for EWlmAccessClassVoice
    };

EXPORT_C CLANLinkCommon::CLANLinkCommon(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConnId, CProtocolIntfBase* aProtocolIntf)
  : CSubConnectionFlowBase(aFactory, aSubConnId, aProtocolIntf), iMMState(EStopped)
    {
    DEBUG("CLANLinkCommon::CLANLinkCommon()");
    LOG_NODE_CREATE(KWlanLog, CLANLinkCommon);
    }


// -----------------------------------------------------------------------------
// CLANLinkCommon::LoadPacketDriverL
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::LoadPacketDriverL()
    {
    DEBUG("CLANLinkCommon::LoadPacketDriverL() - Creates only new class instance");

    iPktDrv = CPcCardPktDrv::NewL( this ); 
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CLANLinkCommon* CLANLinkCommon::NewL(CSubConnectionFlowFactoryBase& aFactory, const TNodeId& aSubConnId, CProtocolIntfBase* aProtocolIntf)
    {
    DEBUG("CLANLinkCommon::NewL()");
  
    CLANLinkCommon* s=new (ELeave) CLANLinkCommon(aFactory, aSubConnId, aProtocolIntf);
    CleanupStack::PushL(s);
    s->ConstructL();
    CleanupStack::Pop(s);
    return s;
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::ReadMACSettings
// -----------------------------------------------------------------------------
//
TBool CLANLinkCommon::ReadMACSettings()
    {
    DEBUG("CLANLinkCommon::ReadMACSettings()");

    TBuf8<KMACByteLength> drvMacAddr;
    TUint8* config=iPktDrv->GetInterfaceAddress();
    drvMacAddr.Append(&config[0],KMACByteLength); // First 3 bytes of config not part of address
    iMacAddr.Copy(drvMacAddr); // update Ethints copy of the MAC address
    TUint bearerLim = iBearers->Count();
    for(TUint index=0;index<bearerLim;index++)
        {
        CLanxBearer* bearer=(*iBearers)[index];
        bearer->UpdateMACAddr();
        }

    // check that the macaddress has been passed and is not 0;
    return CheckMac(iMacAddr); // wrong.
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::Mtu
// -----------------------------------------------------------------------------
//
TUint CLANLinkCommon::Mtu() const 
    {
    DEBUG("CLanLinkCommon::Mtu()");
    
    TInt err = KErrNone;
    CRepository* repository = NULL;
    TRAP( err, repository = CRepository::NewL( KCRUidWlanDeviceSettingsRegistryId ) );
    if( err != KErrNone )
        {
        DEBUG1( "Could not access repository (%d), using default value for mtu(1500)", err );
        return KEtherMaxDataLen;
        }
    // No need to use CleanupStack because no possibility to leave

    TInt temp = 0;
    err = repository->Get( KWlanMTU, temp );
    if( err == KErrNone ) 
        {
        delete repository;
        DEBUG1("MTU value from cenrep is:%d", temp);
        if( temp > KEtherMaxDataLen || temp <= 0 )
            {
            DEBUG("MTU from cenrep is out of range 1-1500, setting it to 1500");
            temp = KEtherMaxDataLen;
            }
        return temp;
        }
    else
        {
        DEBUG1( "Could not access repository (%d), using default value for mtu(1500)", err );
        delete repository;
        return KEtherMaxDataLen;
        } 
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::ConstructL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLANLinkCommon::ConstructL()
    {
    DEBUG("CLANLinkCommon::ConstructL()");

    // Initialise class members that do not need to be read from the database
    iBearers = new (ELeave) CLanxBearerPtrArray(KLanxBearerPtrArrayGranularity);
    iEtherType = EStandardEthernet;

    iOnlyThisBearer = NULL;
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::~CLANLinkCommon
// -----------------------------------------------------------------------------
//
EXPORT_C CLANLinkCommon::~CLANLinkCommon()
    {
    DEBUG("CLANLinkCommon::~CLANLinkCommon()");

    delete iPktDrv;
    ASSERT(0 == iBearers->Count());
    delete iBearers;
    delete iPeriodic;
    delete iWLMServerCommon;
	for( TUint idx( 0 ); idx < EWlmAccessClassMax; ++idx )
	    {
	    delete iAcArray[idx];	    
	    }

    LOG_NODE_DESTROY(KWlanLog, CLANLinkCommon);
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::CheckMac
// -----------------------------------------------------------------------------
//
TBool CLANLinkCommon::CheckMac(TDes8& aMacAddr)
    {
    DEBUG("CLANLinkCommon::CheckMac()");

    TUint MacBytes = 6;
    for (TUint i=0;i<MacBytes;i++)
        {
        if (aMacAddr[i] != 0)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::AcTrafficModeChanged
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::AcTrafficModeChanged(
    TWlmAccessClass aAccessClass,
    TWlmAcTrafficMode aMode )
    {
    DEBUG( "CLANLinkCommon::AcTrafficModeChanged()" );

    iAcArray[aAccessClass]->SetTrafficMode( aMode );
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::AcTrafficModeChanged
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::AcTrafficStatusChanged(
    TWlmAccessClass aAccessClass,
    TWlmAcTrafficStatus aStatus )
    {
    DEBUG( "CLANLinkCommon::AcTrafficStatusChanged()" );

    iAcArray[aAccessClass]->SetTrafficStatus( aStatus );
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::LinkLayerUp
// -----------------------------------------------------------------------------
//
/**
Link Layer Up
Called by the packet driver when it is happy
*/
EXPORT_C void CLANLinkCommon::LinkLayerUp()
    {
    DEBUG("CLANLinkCommon::LinkLayerUp()");
    
    //some drivers have a valid MAC only after negotiation finishes
    iValidMacAddr = ReadMACSettings();
    PostDataClientStartedMessage();

    /**
     * With the new comms framework, this KLinkLayerOpen always sent and then will be caught and swallowed
     * at the ipproto layer if a netcfgext is being used.
     */
  
    /**
     * We are forced to signal link layer open from here even though there may not
     * be a configured IP address.  This is a dirty fix because of politics...
     */
    PostProgressMessage(KLinkLayerOpen, KErrNone);

    TUint bearerLim = iBearers->Count();
    for(TUint index=0;index<bearerLim;index++)
        {
        CLanxBearer *bearer=(*iBearers)[index];
        bearer->StartSending((CProtocolBase*)this);
        }

	// Get current traffic status for access classes.
	TWlmAcTrafficStatusArray acArray;
	TInt ret = iWLMServerCommon->GetAcTrafficStatus( acArray );
	if( ret != KErrNone )
	    {
	    DEBUG1( "CLANLinkCommon::LinkLayerUp() - RWLMServer::GetAcTrafficStatus() failed with %d",
	        ret );

	    // Assume all access classes are admitted.
	    for( TUint idx( 0 ); idx < EWlmAccessClassMax; ++idx )
	         {
	         iAcArray[idx]->SetTrafficStatus( EWlmAcTrafficStatusAdmitted );
	         }
	    }
	else
	    {
	    // Update access class instances with the current status.
        for( TUint idx( 0 ); idx < EWlmAccessClassMax; ++idx )
             {
             iAcArray[idx]->SetTrafficStatus( acArray[idx] );
             }
	    }
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::LinkLayerDown
// -----------------------------------------------------------------------------
//
EXPORT_C void CLANLinkCommon::LinkLayerDown(TInt aError, TAction aAction)
/**
Link Layer Down
Called by the packet driver when the link has gone down

@param aError error code
@param aAction whether a reconnect is required or not
*/
    {
    DEBUG("CLANLinkCommon::LinkLayerDown()");
    
    // If a stop was requested from the SCpr then this LinkLayerDown()
    // was expected. This also means the SCpr has already been sent a
    // TDataClientStopped and the packet driver has been stopped.
    // If on the other hand the stop was not requested a TDataClientGoneDown
    // message needs to be sent to the SCpr and the packet driver still
    // needs a StopInterface call - usually this would mean that aAction
    // is EReconnect
    if (iStopRequested == EFalse && iMMState == EStarted)
        {
        PostProgressMessage(KLinkLayerClosed, aError);
        PostFlowGoingDownMessage(aError, (aAction == EReconnect) ? MNifIfNotify::EReconnect : MNifIfNotify::EDisconnect);
        iPktDrv->StopInterface();
        MaybePostDataClientIdle();
        iMMState = EStopped;
        }
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::StopCb
// -----------------------------------------------------------------------------
//
TInt CLANLinkCommon::StopCb(TAny* aThisPtr)
    {
    DEBUG("CLANLinkCommon::StopCb()");
    
    CLANLinkCommon* self = static_cast<CLANLinkCommon*>(aThisPtr);
    self->StopInterface();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::StopInterface
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::StopInterface()
    {
    DEBUG("CLANLinkCommon::StopInterface()");
    
    if(iPeriodic)
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        iPeriodic = NULL;
        }

    iPktDrv->StopInterface();
    PostProgressMessage(KLinkLayerClosed, iError);
    PostFlowDownMessage(iError);

    MaybePostDataClientIdle();
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::EtherFrame
// -----------------------------------------------------------------------------
//
TInt CLANLinkCommon::EtherFrame(RMBufChain &aPdu, TDesC8& aDestAddr, TUint16 aType, TUint8& aDscp)
    {
    RMBufChain ethHdr;
    TUint etherHeaderSize = (iEtherType==EStandardEthernet) ?
        KEtherHeaderSize : KEtherLLCHeaderSize;

    TRAPD(ret, ethHdr.AllocL(etherHeaderSize));
    if(ret != KErrNone)
        {
        return ret;
        }

    TEtherLLCFrame *hdrBuf = reinterpret_cast<TEtherLLCFrame*>(ethHdr.First()->Buffer());

    //Get the packet info:
    RMBufPktInfo* info = RMBufPacket::PeekInfoInChain(aPdu);

    // Fill in the dest, src Mac addresses:
    hdrBuf->SetDestAddr(aDestAddr);
    hdrBuf->SetSrcAddr(iMacAddr);

    switch(iEtherType)
        {
        case EStandardEthernet:
            BigEndian::Put16((TUint8*)&hdrBuf->iType,aType);
            break;
        case ELLCEthernet:
            // Several fields to fill in. According to nifmbuf.h,
            // info->iLength gives the actual length of the packet.
            BigEndian::Put16((TUint8*)&hdrBuf->iLen,static_cast<TInt16>(info->iLength));
            hdrBuf->iDSAP=KLLC_SNAP_DSAP;
            hdrBuf->iSSAP=KLLC_SNAP_SSAP;
            hdrBuf->iCtrl=KLLC_SNAP_CTRL;
            hdrBuf->SetOUI(0);
            hdrBuf->SetType(aType);
            break;
        default:
            // Can't handle any other kind of Ethernet header.
            return KErrGeneral;
        }

    //Remove the original first Packet - by trimming the info from the packet.
    aPdu.TrimStart(aPdu.Length() - info->iLength); // At this point, info becomes invalid.

    //Extract the DSCP value as aPdu points to IP packet now
    if (KIPFrameType == aType)
        {
        TInet6HeaderIP4 *ipv4hdr = (TInet6HeaderIP4 *)aPdu.First()->Ptr();
        TUint8 TOS = ipv4hdr->TOS();
        aDscp = (TOS >> 2) & 0xff;
        DEBUG2( "CLanLinkCommon::EtherFrame() - DSCP of the IPv4 packet: 0x%02X (%u)",
	        aDscp, aDscp );
        }
    else if (KIP6FrameType == aType)
        {
        TInet6HeaderIP *ipv6hdr = (TInet6HeaderIP *)aPdu.First()->Ptr();
        TUint8 TrafficClass = ipv6hdr->TrafficClass();
        aDscp = (TrafficClass >> 2) & 0xff;     
        DEBUG2( "CLanLinkCommon::EtherFrame() - DSCP of the IPv6 packet: 0x%02X (%u)",
	        aDscp, aDscp );
        }

    //Finally, add the saved packet buffer to the mac header
    aPdu.Prepend(ethHdr);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::FrameSend
// -----------------------------------------------------------------------------
//
TInt CLANLinkCommon::FrameSend(
    RMBufChain& aPdu,
    TDesC8& aDestAddr,
    TUint16 aType )
	{
	// Call down from the Protocol
	// The data begins in the second MBuf - because the first one
	// is the info. The upper layer must have filled in the destination
	// and source Mac addresses before this method is called and must
	// supply the ether header type in aType.
	// EtherFrame strips off the info and replaces it by the proper
	// ether header.

	TUint8 dscp( 0 );
	TInt err = EtherFrame( aPdu, aDestAddr, aType, dscp );
	if( err != KErrNone )
	    {
	    // drop the packet but _don't_ return an error (if we return
	    // an error from here, the stack thinks it is fatal...)
	    aPdu.Free();
	    return KContinueSending;
	    }

    // Map DSCP values to desired access class and user priority values.
	T8021DPriority userPriority( KDSCPtoUP[dscp] );
	TWlmAccessClass accessClass( KUPtoAC[userPriority] );	
	TBool dropPacket( EFalse );

    // ARP packets are treated as Voice traffic.
	if( aType == KArpFrameType )
	    {
        DEBUG( "CLanLinkCommon::FrameSend() - ARP packet" );

        userPriority = E8021DUserPriorityNC;
        accessClass = EWlmAccessClassVoice;
	    }

	if( iAcArray[accessClass]->IsAdmitted() )
	    {
	    DEBUG1( "CLanLinkCommon::FrameSend() - traffic admitted on AC %u",
	        accessClass );
	    }
	else
	    {
	    DEBUG1( "CLanLinkCommon::FrameSend() - traffic NOT admitted on AC %u",
	        accessClass );

	    // Find an admitted access class.
	    DownGradePacket( userPriority, dropPacket );
	    }

	if( !dropPacket )
	    {
	    //Prepend UP value to the packet
	    RMBufChain UPBuf;
	    TRAPD( allocRet, UPBuf.AllocL( sizeof(TUint8) ) );
	    if( allocRet != KErrNone )
	        {
	        // drop the packet but _don't_ return an error (if we return
	        // an error from here, the stack thinks it is fatal...)
	        aPdu.Free();
	        return KContinueSending;
	        }

	    (UPBuf.First())->Put((TUint8)userPriority);
	    aPdu.Prepend( UPBuf );

	    DEBUG2( "CLanLinkCommon::FrameSend() - sending packet on AC %u with UP %u",
	        KUPtoAC[userPriority],
	        userPriority );
	    TInt ret = iPktDrv->Send( aPdu );

	    // ARP packets will never triggers traffic stream creation.
	    if( aType == KArpFrameType )
	        {
	        return ret;
	        }

	    // Notify the per-AC handler.
	    iAcArray[accessClass]->OnFrameSend();

	    if( ret != KContinueSending )
	        {
	        // Suspend inactivity timer for all ACs if NIF Queue is full.
	        for( TUint idx( 0 ); idx < EWlmAccessClassMax; ++idx )
	            {
	            iAcArray[idx]->SuspendInactivityTimer();
	            }
	        }

	    return ret;
	    }
	else
	    {
	    DEBUG( "CLanLinkCommon::FrameSend() - no ACs admitted, dropping packet" );

	    aPdu.Free();

	    return KContinueSending;
	    }
	}

// -----------------------------------------------------------------------------
// CLANLinkCommon::DownGradePacket
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::DownGradePacket(
    T8021DPriority& aUP,
    TBool& aDrop )
    {
    DEBUG1( "CLanLinkCommon::DownGradePacket() - UP before downgrade: %u",
        aUP );

    /**
     * This method wouldn't be called if downgrade wasn't needed,
     * so we don't have to check Voice priority at all.   
     */
    TWlmAccessClass accessClass( KNextAC[KUPtoAC[aUP]] );
    while( accessClass != EWlmAccessClassMax )
        {
        if( iAcArray[accessClass]->IsAdmitted() )
            {
            aUP = KACtoUP[accessClass];
            aDrop = EFalse;
    
            DEBUG1( "CLanLinkCommon::DownGradePacket() - UP after downgrade: %u",
                aUP );

            return;
            }

        accessClass = KNextAC[accessClass];
        }

    DEBUG( "CLanLinkCommon::DownGradePacket() - packet will be dropped" );

    aDrop = ETrue;
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::GetProtocolType
// -----------------------------------------------------------------------------
//
TInt CLANLinkCommon::GetProtocolType(
    TUint8* aEtherHeader,
    TUint16& aEtherType )
    {
    TEtherFrame* etherFrame = reinterpret_cast<TEtherFrame*>(aEtherHeader);   
    TUint16 etherType = etherFrame->GetType();
  
    if( etherType >= KMinEtherType)
        {
        // type/length field >= 1536, this is Ethernet II frame
        aEtherType = etherType; 
        }
    else
        {
        // type/length field is something between KEtherLLCMaxLengthFieldValue
        // and KMinEtherType, this shouldn't happen...
        //__ASSERT_DEBUG(EFalse, User::Panic(_L("wlannif"), KErrGeneral));
        return KErrNotSupported;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::Process
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::Process(
    RMBufChain& aPdu, 
    TUint8* aEtherHeader, 
    TUint8 aUPValue )
	{
	TUint16 etherCode(0);
	TBool packetProcessed(EFalse);
	TInt ret=GetProtocolType( aEtherHeader, etherCode );
	if( ret==KErrNone )
		{  // It's an ethernet packet of some kind.
	    DEBUG2("CLANLinkCommon::Process() - UP: %u, ethertype: 0x%04X",
	        aUPValue, etherCode );

		TUint bearerLim = iBearers->Count();
		for( TUint index=0;index<bearerLim && !packetProcessed; index++ )
			{
			CLanxBearer *bearer=(*iBearers)[index];
		
			if( bearer->WantsProtocol(
			    etherCode, aPdu.First()->Next()->Buffer()) )
				{	
				bearer->Process( aPdu );
				packetProcessed=ETrue;
				}
			}
		}
	if( ret!=KErrNone || !packetProcessed )
	    {
	    aPdu.Free(); // Something strange about the packet - bin it
	    }
	else
	    {
        // Notify the per-AC handler.   
	    iAcArray[KUPtoAC[aUPValue]]->OnFrameReceive();
	    }
	}

// -----------------------------------------------------------------------------
// CLANLinkCommon::ResumeSending
// -----------------------------------------------------------------------------
//
EXPORT_C void CLANLinkCommon::ResumeSending()
    {
	DEBUG( "CLanLinkCommon::ResumeSending()" );
	
	TUint bearerLim = iBearers->Count();
	for( TUint index=0;index<bearerLim;index++ )
		{
		CLanxBearer *bearer=(*iBearers)[index];
		bearer->StartSending( (CProtocolBase*)this );
		}

    for( TUint idx( 0 ); idx < EWlmAccessClassMax; ++idx )
         {
         iAcArray[idx]->ResumeInactivityTimer();
         }
	}

/**

*/
EXPORT_C TInt CLANLinkCommon::ReadInt(const TDesC& /*aField*/, TUint32& /*aValue*/)
{
  return KErrNotSupported;
}
  
/**

*/
EXPORT_C TInt CLANLinkCommon::WriteInt(const TDesC& /*aField*/, TUint32 /*aValue*/)
{
  return KErrNotSupported;
}
  
/**

*/
EXPORT_C TInt CLANLinkCommon::ReadDes(const TDesC& /*aField*/, TDes8& /*aValue*/)
{
  return KErrNotSupported;
}
  
/**

*/
EXPORT_C TInt CLANLinkCommon::ReadDes(const TDesC& /*aField*/, TDes16& /*aValue*/)
{
  return KErrNotSupported;

}
  
/**

*/
EXPORT_C TInt CLANLinkCommon::WriteDes(const TDesC& /*aField*/, const TDesC8& /*aValue*/)
{
  return KErrNotSupported;
}
  
/**

*/
EXPORT_C TInt CLANLinkCommon::WriteDes(const TDesC& /*aField*/, const TDesC16& /*aValue*/)
{
  return KErrNotSupported;
}
  
/**

*/
EXPORT_C TInt CLANLinkCommon::ReadBool(const TDesC& /*aField*/, TBool& /*aValue*/)
{
  return KErrNotSupported;
}
  
/**

*/
EXPORT_C TInt CLANLinkCommon::WriteBool(const TDesC& /*aField*/, TBool /*aValue*/)
{
  return KErrNotSupported;
}
  
/**

*/
EXPORT_C void CLANLinkCommon::IfProgress(TInt aStage, TInt aError)
{
  PostProgressMessage(aStage, aError);
}
  
/**

*/
EXPORT_C void CLANLinkCommon::IfProgress(TSubConnectionUniqueId /*aSubConnectionUniqueId*/, TInt aStage, TInt aError)
{
  // Not quite correct - there is no subconnection id specific progress message.
  IfProgress(aStage, aError);
}
  
/**

*/
EXPORT_C void CLANLinkCommon::NifEvent(TNetworkAdaptorEventType /*aEventType*/, TUint /*aEvent*/, const TDesC8& /*aEventData*/, TAny* /*aSource*/)
{
}

// ======================================================================================
//
// from MFlowBinderControl

// -----------------------------------------------------------------------------
// CLANLinkCommon::GetControlL
// -----------------------------------------------------------------------------
//
EXPORT_C MLowerControl* CLANLinkCommon::GetControlL(const TDesC8& aName)
/**
Request from upper CFProtocol to retrieve our MLowerControl class.

@param aName Protocol Name.  Presently, only "ip" and "ip6" are supported.
@return pointer to our MLowerControl class instance.  We leave on an error, hence this routine should
never return NULL.
*/
  {
    DEBUG("CLANLinkCommon::GetControlL()");
  
  CLanxBearer* bearer=NULL;
    if (aName.CompareF(KDescIp6) == 0)
      {
        bearer = new(ELeave) CLanIp6Bearer(this);
        }
  else if (aName.CompareF(KDescIp) == 0 || aName.CompareF(KDescIcmp) == 0)
    {
        bearer = new(ELeave) CLanIp4Bearer(this);
        }
  else
    {
    User::Leave(KErrNotSupported);
    }
  
    CleanupStack::PushL(bearer);
    bearer->ConstructL();

  iBearers->AppendL(bearer);
  ProvisionBearerConfigL(aName);
    CleanupStack::Pop();
    return bearer;
  }

  
// -----------------------------------------------------------------------------
// CLANLinkCommon::BindL
// -----------------------------------------------------------------------------
//
EXPORT_C MLowerDataSender* CLANLinkCommon::BindL(const TDesC8& aProtocol, MUpperDataReceiver* aReceiver, MUpperControl* aControl)
/**
Request from upper CFProtocol to bind to this module.

@param aProtocol protocol name (e.g. "ip", "ip6" etc).
@param aReceiver pointer to MUpperDataReceiver instance of upper CFProtocol
@param aControl pointer to MUpperControl instance of upper CFProtocol
@return pointer to our MLowerDataSender instance (eventually passed to the upper CFProtocol)
*/
  {
    DEBUG("CLANLinkCommon::BindL()");
    
  TInt index = 0;
  CLanxBearer* bearer = FindBearerByProtocolName(aProtocol, index);
  if (bearer)
    {
    bearer->SetUpperPointers(aReceiver, aControl);
    if (iValidMacAddr)
      {
      aControl->StartSending();
      }
    }
  else
    {
    User::Leave(KErrNotSupported);
    }
  iSubConnectionProvider.RNodeInterface::PostMessage(Id(), TCFControlProvider::TActive().CRef()); 
  return bearer;
  }

  
// -----------------------------------------------------------------------------
// CLANLinkCommon::Unbind
// -----------------------------------------------------------------------------
//
EXPORT_C void CLANLinkCommon::Unbind(MUpperDataReceiver* /*aReceiver*/, MUpperControl* aControl)
/**
Request from upper CFProtocol to unbind from this module.

@param aReceiver pointer to data receiver class of upper CFProtocol (originally passed to it in downward BindL() request)
@param aControl pointer to control class of upper CFProtocol (originally passed to it in downward BindL() request)
*/

  {
    DEBUG("CLANLinkCommon::Unbind()");
    
  TInt index = 0;
  CLanxBearer* bearer = FindBearerByUpperControl(aControl, index);
  ASSERT(bearer);
  iBearers->Delete(index);
  delete bearer;
  
  // If no-one remains bound to us, signal data client idle to SCPR
  MaybePostDataClientIdle();
  }


//
// Utilities
//

// -----------------------------------------------------------------------------
// CLANLinkCommon::FindBearerByProtocolName
// -----------------------------------------------------------------------------
//
CLanxBearer* CLANLinkCommon::FindBearerByProtocolName(const TDesC8& aProtocol, TInt& aIndex) const
/**
Search the iBearers array searching for a match by protocol name.

@param aProtocol name of protocol (in)
@param aIndex on success, index of found entry (>=0), else -1.
@return pointer to CLanxBearer entry on success else NULL.
*/
  {
    DEBUG("CLANLinkCommon::FindBearerByProtocolName()");
  
  TInt count = iBearers->Count();
  for (TInt i = 0 ; i < count ; ++i)
    {
    CLanxBearer* bearer = iBearers->At(i);
    if (bearer->ProtocolName() == aProtocol)
      {
      aIndex = i;
      return bearer;
      }
    }
  aIndex = -1;
  return NULL;
  }


// -----------------------------------------------------------------------------
// CLANLinkCommon::FindBearerByUpperControl
// -----------------------------------------------------------------------------
//
CLanxBearer* CLANLinkCommon::FindBearerByUpperControl(const MUpperControl* aUpperControl, TInt& aIndex) const
/**
Search the iBearers array searching for a match by protocol name.

@param aProtocol name of protocol (in)
@param aIndex on success, index of found entry (>=0), else -1.
@return pointer to CLanxBearer entry on success else NULL.
*/
  {
    DEBUG("CLANLinkCommon::FindBearerByUpperControl()");
  
  TInt count = iBearers->Count();
  for (TInt i = 0 ; i < count ; ++i)
    {
    CLanxBearer* bearer = iBearers->At(i);
    if (bearer->MatchesUpperControl(aUpperControl))
      {
      aIndex = i;
      return bearer;
      }
    }
  aIndex = -1;
  return NULL;
  }
  
// =====================================================================================
//
// MCFNode

// -----------------------------------------------------------------------------
// CLANLinkCommon::ReceivedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLANLinkCommon::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
/**
Routine called on an incoming message from SCPR

@param aCFMessage incoming message
@return KErrNone, else a system wide error code.
*/
    {
    DEBUG1("CLANLinkCommon::ReceivedL(), aCFMessage=%d", aMessage.MessageId().MessageId());
    
    CSubConnectionFlowBase::ReceivedL(aSender, aRecipient, aMessage);
   
    if (TEBase::ERealmId == aMessage.MessageId().Realm())
        {
        switch (aMessage.MessageId().MessageId())
            {
        case TEBase::TError::EId :
            SubConnectionError(static_cast<TEBase::TError&>(aMessage).iValue);
            break;
        case TEBase::TCancel::EId :
            CancelStartFlow();
            break;                
        default:
            ASSERT(EFalse);
            }
        }
    else if (TEChild::ERealmId == aMessage.MessageId().Realm())
        {
        switch (aMessage.MessageId().MessageId())
            {
        case TEChild::TDestroy::EId :
            Destroy();
            break;
        default:
            ASSERT(EFalse);
            }
        }
    else if (TCFDataClient::ERealmId == aMessage.MessageId().Realm())
        {
        switch (aMessage.MessageId().MessageId())
            {
        case TCFDataClient::TStart::EId :
            StartFlowL();
            break;
        case TCFDataClient::TProvisionConfig::EId:
            ProvisionConfig(static_cast<TCFDataClient::TProvisionConfig&>(aMessage).iConfig);
            break;
        case TCFDataClient::TStop::EId :
            StopFlow(static_cast<TCFDataClient::TStop&>(aMessage).iValue);
            break;
        case TCFDataClient::TBindTo::EId :
      {
            TCFDataClient::TBindTo& bindToReq = message_cast<TCFDataClient::TBindTo>(aMessage);
            if (!bindToReq.iNodeId.IsNull())
                {
                User::Leave(KErrNotSupported);
                }
            RClientInterface::OpenPostMessageClose(Id(), aSender, TCFDataClient::TBindToComplete().CRef());
            break;
      }
    default:
            ASSERT(EFalse);
            }
        }
    else if (TLinkMessage::ERealmId == aMessage.MessageId().Realm())
        {
        switch (aMessage.MessageId().MessageId())
            {
            case TLinkMessage::TAgentToFlowNotification::EId:
                {                    
              TLinkMessage::TAgentToFlowNotification& msg = message_cast<TLinkMessage::TAgentToFlowNotification>(aMessage);
              if(msg.iValue != EAgentToNifEventVendorSpecific)
                {
                User::Leave(KErrNotSupported);
                }
              else //EAgentToNifEventVendorSpecific
                {
                //User::LeaveIfError((iPktDrv) ? iPktDrv->Notification(
                //    static_cast<TAgentToNifEventType>(msg.iValue), NULL) : KErrNotSupported);
                DEBUG("CLANLinkCommon::ReceivedL(), rcvd disconnect notification from Agent");
                iError = KErrDisconnected;
                StopInterface();
                }         
                }
                break;
            default:
                ASSERT(EFalse);             
            }; //endswitch   
        }
    else
        {
        ASSERT(EFalse);
        }
    }

//
// Methods for handling incoming SCPR messages
//

// -----------------------------------------------------------------------------
// CLANLinkCommon::StartFlowL
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::StartFlowL()
  {
    DEBUG("CLANLinkCommon::StartFlowL()");
    
    if ( !iPktDrv )
        LoadPacketDriverL();
  
  iStopRequested = EFalse;
  
  ASSERT(iMMState == EStopped);

  if (iSavedError != KErrNone)
    {
    // If there were errors during prior processing of the TProvisionConfig message,
    // leave here and cause a TError response to TDataClientStart.
    User::Leave(iSavedError);
    }

  iValidMacAddr = ReadMACSettings();
  
  iMMState = EStarting;
  User::LeaveIfError(iPktDrv->StartInterface());
  }


// -----------------------------------------------------------------------------
// CLANLinkCommon::CancelStartFlow
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::CancelStartFlow()
    {
    if (iMMState == EStarting)
        {
      iStopRequested = ETrue;
    
      iPktDrv->StopInterface();
      PostProgressMessage(KLinkLayerClosed, KErrCancel);
      PostFlowDownMessage(KErrCancel);
    
      MaybePostDataClientIdle();
      }
    }


// half a second should be enough for sending DHCPRELEASE
static const TInt KDhcpReleaseDelay = 500000;
  
// -----------------------------------------------------------------------------
// CLANLinkCommon::StopFlow
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::StopFlow(TInt aError)
  {
    DEBUG("CLANLinkCommon::StopFlow()");
    
  iStopRequested = ETrue;

  iMMState = EStopping;
  
  iError = aError;

  TCallBack callback(StopCb, this);
  TRAPD(err, iPeriodic = CPeriodic::NewL(CActive::EPriorityStandard));
  if(err == KErrNone)
      {
    // Call the callback after KDhcpReleaseDelay. This is done only once,
    // but we need to put something (KMaxTInt) to CPeriodic's interval.
    iPeriodic->Start(TTimeIntervalMicroSeconds32(KDhcpReleaseDelay), TTimeIntervalMicroSeconds32(KMaxTInt), callback);
      }
  else
      {
    iPeriodic = NULL;
      } 
  }


// -----------------------------------------------------------------------------
// CLANLinkCommon::SubConnectionGoingDown
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::SubConnectionGoingDown()
  {
  }
  
// -----------------------------------------------------------------------------
// CLANLinkCommon::SubConnectionError
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::SubConnectionError(TInt /*aError*/)
  {
  }


/*
Provisioning description for Ethernet CFProtocol Flow:

- on receipt of the TProvisionConfig message, the pointer contained within is stored
  in iAccessPointConfig and the provisioning information contained within the AccessPointConfig
  array is validated:
  - at least one of TLanIp4Provision or TLanIp6Provision must be present.  They are added by the EtherMCPr and
    populated from CommsDat.

- the packet driver is loaded.

- if any of the above steps fail, the resulting error is saved in iSaveError so that when TDataClientStart message is
  subsequently received, a TError message can be sent in response (there is no response to TProvisionConfig message).
*/

// -----------------------------------------------------------------------------
// CLANLinkCommon::ProvisionConfig
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::ProvisionConfig( const RMetaExtensionContainerC& aConfigData )
    {
    DEBUG("CLANLinkCommon::ProvisionConfig()");

    iSavedError = KErrNone;

    AccessPointConfig().Close();
    AccessPointConfig().Open(aConfigData);
    
    iLanIp4Provision = static_cast<const TLanIp4Provision*>(AccessPointConfig().FindExtension(STypeId::CreateSTypeId(TLanIp4Provision::EUid, TLanIp4Provision::ETypeId)));
    if (iLanIp4Provision)
        {
        DEBUG("CLANLinkCommon::ProvisionConfig() - IP4 config to be provisioned");
        }

    iLanIp6Provision = static_cast<const TLanIp6Provision*>(AccessPointConfig().FindExtension(STypeId::CreateSTypeId(TLanIp6Provision::EUid, TLanIp6Provision::ETypeId)));
    if (iLanIp6Provision)
        {
        DEBUG("CLANLinkCommon::ProvisionConfig() - IP6 config to be provisioned");
        }

    if (iLanIp4Provision == NULL && iLanIp6Provision == NULL)
        {
        // At least one set of IP4/6 provisioning information must be present
        iSavedError = KErrCorrupt;
        return;
        }

    TRAPD(err, ProvisionConfigL());
    if (err != KErrNone)
        {
        iSavedError = err;
        }
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::ProvisionConfigL
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::ProvisionConfigL()
    {
    DEBUG("CLANLinkCommon::ProvisionConfigL()");

    // Provision Bearers
    if (iBearers->Count())
        {
        ProvisionBearerConfigL(KDescIp());
        ProvisionBearerConfigL(KDescIp6());
        }

    iWLMServerCommon =  new (ELeave) CLANNifWLMServerCommon(this);
    iWLMServerCommon->ConstructL();

	TBool isAutomaticMgmt( ETrue );
	CRepository* repository = NULL;	
	TRAP_IGNORE( repository = CRepository::NewL( KCRUidWlanDeviceSettingsRegistryId ) );
	if( repository )
	    {
	    TInt temp( 0 );
	    TInt ret = repository->Get( KWlanAutomaticTrafficStreamMgmt, temp );
	    if( ret == KErrNone &&
	        !temp )
	        {
	        isAutomaticMgmt = EFalse;
	        }

	    delete repository;
	    repository = NULL;
	    }

	for( TUint idx( 0 ); idx < EWlmAccessClassMax; ++idx )
	    {
	    iAcArray[idx] = CLANNifWLMServerPerAC::NewL(
	        static_cast<TWlmAccessClass>( idx ),
	        KTsInactivityTime[idx],
	        isAutomaticMgmt );
	    }
    }

// -----------------------------------------------------------------------------
// CLANLinkCommon::Destroy
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::Destroy()
  {
    DEBUG("CLANLinkCommon::Destroy()");
    
  ASSERT(iMMState==EStopped);
  DeleteThisFlow();
  }

// Utility functions

void CLANLinkCommon::ProvisionBearerConfigL(const TDesC8& aName)
  {
    DEBUG("CLANLinkCommon::ProvisionBearerConfigL()");
  
  TInt index = -1;
  CLanxBearer* bearer = FindBearerByProtocolName(aName, index);
  if (bearer)
    {
        if (aName.CompareF(KDescIp) == 0)
        {
        if (iLanIp4Provision)
            {
            bearer->SetProvisionL(iLanIp4Provision); // CLanIp4Bearer
            }
        }
    else if (aName.CompareF(KDescIp6) == 0) 
        {
        if (iLanIp6Provision)
            {
            bearer->SetProvisionL(iLanIp6Provision); // CLanIp6Bearer
            }
        }
    }
  }


// -----------------------------------------------------------------------------
// CLANLinkCommon::PostProgressMessage
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::PostProgressMessage(TInt aStage, TInt aError)
  {
    DEBUG("CLANLinkCommon::PostProgressMessage()");
  
  //Be careful when sending TStateChange message as RNodeChannelInterface will override the activity id
  iSubConnectionProvider.RNodeInterface::PostMessage(Id(), TCFMessage::TStateChange( Elements::TStateChange( aStage, aError) ).CRef());
  }

// -----------------------------------------------------------------------------
// CLANLinkCommon::SetAllowedBearer
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::SetAllowedBearer(CLanxBearer* aBearer)
  {
    iOnlyThisBearer = aBearer;
  }
  
// -----------------------------------------------------------------------------
// CLANLinkCommon::BearereIsActive
// -----------------------------------------------------------------------------
//
TBool CLANLinkCommon::BearerIsActive(CLanxBearer* aBearer)
  {
    if (!iOnlyThisBearer || 
        aBearer == iOnlyThisBearer)
      {
      return ETrue;
      }
    return EFalse;      
  }

// -----------------------------------------------------------------------------
// CLANLinkCommon::PostFlowDownMessage
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::PostFlowDownMessage(TInt aError)
  {
    DEBUG("CLANLinkCommon::PostFlowDownMessage()");
  
  if (iMMState == EStopping)
      {
      iSubConnectionProvider.PostMessage(Id(), TCFDataClient::TStopped(aError).CRef());     
      }
    else if (iMMState == EStarting)
      {
      iSubConnectionProvider.PostMessage(Id(), TEBase::TError(TCFDataClient::TStart::Id(), aError).CRef());
      }
    else if (iMMState == EStarted)
      {
      iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TDataClientGoneDown(aError).CRef());
      }     
    iMMState = EStopped;  
  }


// -----------------------------------------------------------------------------
// CLANLinkCommon::PostFlowGoingDownMessage
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::PostFlowGoingDownMessage(TInt aError, MNifIfNotify::TAction /*aAction*/)
  {
    DEBUG("CLANLinkCommon::PostFlowGoingDownMessage()");
    
  // TDataClientGoneDown only makes sense if the flow was actually started
  ASSERT(iMMState == EStarted); 
  iMMState = EStopped;
  iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TDataClientGoneDown(aError).CRef());
  }

  
// -----------------------------------------------------------------------------
// CLANLinkCommon::PostDataClientStartedMessage
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::PostDataClientStartedMessage()
  {
    DEBUG("CLANLinkCommon::PostDataClientStartedMessage()");
  
  iMMState = EStarted;
  iSubConnectionProvider.PostMessage(Id(), TCFDataClient::TStarted().CRef());
  }


// -----------------------------------------------------------------------------
// CLANLinkCommon::MaybePostDataClientIdle
// -----------------------------------------------------------------------------
//
void CLANLinkCommon::MaybePostDataClientIdle()
  {
    DEBUG("CLANLinkCommon::MaybePostDataClientIdle()");
  
  if (iBearers->Count() == 0 && iMMState == EStopped)
    {
    iSubConnectionProvider.PostMessage(Id(), TCFControlProvider::TIdle().CRef());
    }
  }


void TEtherLLCFrame::SetDestAddr( TDesC8& aDest)
{
    DEBUG("TEtherLLCFrame::SetDestAddr()");

    Mem::Copy(iDestAddr, aDest.Ptr(), KMACByteLength);    
}

// -----------------------------------------------------------------------------
// CLANLinkCommon::SetSrcAddr
// -----------------------------------------------------------------------------
//
void TEtherLLCFrame::SetSrcAddr(TDesC8& aSrc)
{
    DEBUG("TEtherLLCFrame::SetSrcAddr()");

    Mem::Copy(iSrcAddr, aSrc.Ptr(), KMACByteLength);
}

// -----------------------------------------------------------------------------
// CLANLinkCommon::SetOUI
// -----------------------------------------------------------------------------
//
void TEtherLLCFrame::SetOUI(TUint32 aOUI)
{
    DEBUG("TEtherLLCFrame::SetOUI()");

  //aOUI is in native order, but the result is
  //always stored in network byte order.
  //Can't use the bigendian methods, because
  //they only work for 16 and 32 -bit items.
  OUI[0] = (TUint8)((aOUI>>16)&0xff);
  OUI[1] = (TUint8)((aOUI>>8)&0xff);
  OUI[2] = (TUint8)(aOUI&0xff);
}
