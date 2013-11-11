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
* Description:  Protocol definitions
*
*/

/*
* %version: 8 %
*/

#ifndef __WLANPROTO_H__
#define __WLANPROTO_H__


#include "wlmclientserver.h"
#include "wlancontrolinterface.h"

#include <comms-infras/nifif.h>
#include <in_sock.h>

#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_flowbinders.h>

class TLanIp4Provision;
class TLanIp6Provision;

const TUint KWlannifMajorVersionNumber = 0;
const TUint KWlannifMinorVersionNumber = 0;
const TUint KWlannifBuildVersionNumber = 0;

const TInt KEtherMaxDataLen = 1500;
// This should actually be 1492,
// but we allow 8 bytes more because at least in WINS we may receive such packets...
const TInt KEtherLLCMaxDataLen = 1500; 
// Allow 8 bytes more. Some sources say we should allow even more
// => change this if needed. (e.g. http://www.cisco.com/warp/public/105/encheat.html:
// max length field value 0x5EE (1518))
const TInt KEtherLLCMaxLengthFieldValue = 1508; 
// I guess this value is ok also for WLAN. 
// (Ideally we should ask this from the drivers but there isn't any API for that at the moment.)
const TInt KEtherSpeedInKbps = 10000; 
const TInt KMinEtherType = 0x0600; // 1536

/*******************************************************************************
* A quick intro to the Ethernet packet format:
*
* A plain frame looks like this:
* 
* struct EtherFrame {
*   TUint64 iDestAddr, iSrcAddr;
*   TUint16 iType; // ether type, should be >= 1536 (0x0600) (must not conflict with IEEE 802.3 length field)
*   TUint8  iData[1500]; // Payload.
* };
*
* iType denotes the type of the payload, 0x0800 denotes IP.
*                                        0x0806 denotes ARP.
*                                        <= 1500 denotes IEEE 802.3 encapsulation and is interpreted as length
* A frame may also contain an extra 'LLC' field which encapsulates the original
* payload in a 5 byte header. The LLC field is somewhat complex, but here we
* confine ourselves to the LLC-SNAP variant.
*
* struct EtherLLCFrame {
*   TUint64 iDestAddr, iSrcAddr;
*   TUint16 iLen; // length, must be <=1500
*   TUint8 iDSAP=0xAA; // Which denotes an LLC-SNAP frame.
*   TUint8 iSSAP=0xAA; // Which denotes an LLC-SNAP frame.
*   TUint8 iCTRL=3;    // Which denotes an LLC-SNAP frame encapsulated by
*                      // Ethernet.
*   TUint24 OUI=iDstAddr&0xffffff; // 0 is also a valid value.
*   TUint16 iType; // The actual iType as described earlier.
*   TUint8  iData[1492]; // Payload.
* };
*
*******************************************************************************/

_LIT8(KWlanLog, "Wlannwif");

const TUint32 KMACByteLength = 6;
// Ethernet types (ref. e.g. http://www.iana.org/assignments/ethernet-numbers)
const TUint16 KIPFrameType  = 0x0800; // IPv4 ether type
const TUint16 KIP6FrameType = 0x86DD; // IPv6 ether type
const TUint16 KArpFrameType = 0x0806; // ARP ether type
// 802.2 LLC constants
const TUint8 KLLC_SNAP_DSAP = 0xAA;
const TUint8 KLLC_SNAP_SSAP = 0xAA;
const TUint8 KLLC_SNAP_CTRL = 3;

// Ethernet header types
enum TEtherHeaderType
    {
    EStandardEthernet, 
    ELLCEthernet
    };

const TUint KEtherHeaderSize = 14;
const TUint KEtherLLCHeaderSize = KEtherHeaderSize + 8;

struct TEtherFrame
    {
   TUint16 GetType() { return BigEndian::Get16((TUint8*)&iType); }
   TUint16 iDestAddr[3];
   TUint16 iSrcAddr[3];
   TUint16 iType;
   TUint8  iData[KEtherMaxDataLen];
    };

struct TEtherLLCFrame
    {
    TUint16 GetType() {return(TUint16)((iTypeHi<<8)|iTypeLo);}
    void SetType(TUint16 aType)
        {
        iTypeHi=(TUint8)(aType>>8);
        iTypeLo=(TUint8)(aType&0xff);
        }
    void SetDestAddr( TDesC8& aDest);
    void SetSrcAddr( TDesC8& aSrc);
    void SetOUI( TUint32 aOUI);
    TUint16 iDestAddr[3];
    TUint16 iSrcAddr[3];
    union
        {
        TUint16 iLen;
        TUint16 iType;
        };
    TUint8 iDSAP;
    TUint8 iSSAP;
    TUint8 iCtrl;
    TUint8 OUI[3];
    TUint8 iTypeHi;
    TUint8 iTypeLo;
    TUint8 iData[KEtherLLCMaxDataLen];
    };

/**
 * User priority values as defined 802.1D specification.
 */
enum T8021DPriority
    {
    E8021DUserPriorityBE    = 0,    // Best Effort
    E8021DUserPriorityBK1   = 1,    // Background
    E8021DUserPriorityBK2   = 2,    // Background
    E8021DUserPriorityEE    = 3,    // Excellent Effort
    E8021DUserPriorityCL    = 4,    // Controlled Load
    E8021DUserPriorityVI    = 5,    // Video
    E8021DUserPriorityVO    = 6,    // Voice
    E8021DUserPriorityNC    = 7,    // Network Control
    E8021DUserPriorityMax   = 8     // Defined as the upper bound
    };

/**
 * Mapping of access classes to the corresponding user priority values.
 */
const T8021DPriority KACtoUP[EWlmAccessClassMax] =
    {
    E8021DUserPriorityBE,   // EWlmAccessClassBackground
    E8021DUserPriorityBK2,  // EWlmAccessClassBestEffort
    E8021DUserPriorityCL,   // EWlmAccessClassVideo
    E8021DUserPriorityVO,   // EWlmAccessClassVoice
    };

//forward declaration
class CLANNifWLMServerCommon;
class CLANNifWLMServerPerAC;
class CPcCardPktDrv;
class CLanxBearer;

typedef CArrayPtrFlat<CLanxBearer> CLanxBearerPtrArray;

/**
*  CLANLinkCommon
*/
class CLANLinkCommon : public ESock::CSubConnectionFlowBase, public ESock::MFlowBinderControl
    {   
    friend class CPktDrvBase;

public:
    enum TAction 
        {
        EReconnect,
        EDisconnect
        };

    IMPORT_C static CLANLinkCommon* NewL(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
    IMPORT_C CLANLinkCommon(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
    IMPORT_C virtual ~CLANLinkCommon();
    IMPORT_C void ConstructL();
    
    // from MFlowBinderControl
    IMPORT_C virtual ESock::MLowerControl* GetControlL(const TDesC8& aProtocol);
    IMPORT_C virtual ESock::MLowerDataSender* BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl);
    IMPORT_C virtual void Unbind(ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl);
    virtual ESock::CSubConnectionFlowBase* Flow()
        {
        return this;
        }
    
    // from MCFNode
    IMPORT_C virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    
    // Dispatch functions for messages received from SCPR.
    
    void StartFlowL();
    void CancelStartFlow(); 
    void StopFlow(TInt aError);
    void SubConnectionGoingDown();
    void SubConnectionError(TInt aError);
    void BindToL(Messages::TNodeId& aCommsBinder);

    void Destroy();

    // Utility functions for sending messages to SCPR.
    void PostProgressMessage(TInt aStage, TInt aError);
    void PostDataClientStartedMessage();
    void PostFlowDownMessage(TInt aError);
    void PostFlowGoingDownMessage(TInt aError, MNifIfNotify::TAction aAction);
    void MaybePostDataClientIdle();

    // CSubConnectionFlowBase
    MFlowBinderControl* DoGetBinderControlL()
        {
        return this;
        }

    void Process( RMBufChain& aPdu, TUint8* aEtherHeader, TUint8 aUPValue );
    IMPORT_C void ResumeSending(); // Flow Control unblocked
    IMPORT_C void LinkLayerUp(); // Notify the Protocol that the line is up
    IMPORT_C void LinkLayerDown(TInt aError, TAction aAction); // Notify the Protocol that the line is down
    
    IMPORT_C TInt ReadInt(const TDesC& aField, TUint32& aValue);
    IMPORT_C TInt WriteInt(const TDesC& aField, TUint32 aValue);
    IMPORT_C TInt ReadDes(const TDesC& aField, TDes8& aValue);
    IMPORT_C TInt ReadDes(const TDesC& aField, TDes16& aValue);
    IMPORT_C TInt WriteDes(const TDesC& aField, const TDesC8& aValue);
    IMPORT_C TInt WriteDes(const TDesC& aField, const TDesC16& aValue);
    IMPORT_C TInt ReadBool(const TDesC& aField, TBool& aValue);
    IMPORT_C TInt WriteBool(const TDesC& aField, TBool aValue);    
    IMPORT_C void IfProgress(TInt aStage, TInt aError);
    IMPORT_C void IfProgress(TSubConnectionUniqueId aSubConnectionUniqueId, TInt aStage, TInt aError);
    IMPORT_C void NifEvent(TNetworkAdaptorEventType aEventType, TUint aEvent, const TDesC8& aEventData, TAny* aSource=0);
    
    
    TUint Mtu() const;
    TUint SpeedMetric() const {return KEtherSpeedInKbps;}

    const TBuf8<KMACByteLength>& MacAddress() const;

    TInt FrameSend(RMBufChain& aPdu, TDesC8& aDestAddr, TUint16 aType );
    void SetAllowedBearer(CLanxBearer* aBearer);
    TBool BearerIsActive(CLanxBearer* aBearer);

public: // Admission control related functions

    /**
     * The traffic mode of an access class has changed.
     *
     * @param aAccessClass Access class.
     * @param aMode Traffic mode of the access class.
     */
    void AcTrafficModeChanged(
        TWlmAccessClass aAccessClass,
        TWlmAcTrafficMode aMode );

    /**
     * The traffic status of an access class has changed.
     *
     * @param aAccessClass Access class.
     * @param aStatus Traffic status of the access class.
     */
    void AcTrafficStatusChanged(
        TWlmAccessClass aAccessClass,
        TWlmAcTrafficStatus aStatus );

private:
    
    // Configuration methods
    TBool ReadMACSettings();
    void ReadEthintSettingsL();

    TInt EtherFrame(
        RMBufChain &aChain,
        TDesC8& aDestAddr,
        TUint16 aType,
        TUint8& aDscp );

    // checks that the packet driver has returned a valid MAC address,
    TBool CheckMac( TDes8& aMacAddr ); 

   
    TInt GetProtocolType( 
        TUint8* aEtherHeader, 
        TUint16& aEtherType );

    static TInt StopCb(TAny* aThisPtr);
    void StopInterface();

    /**
     * DownGradePacket
     * @param aUP: downgraded UP value
     * @param aDrop: sets to ETrue if packet to be dropped 
     * Downgrades the UP value of the packet if stream is not created. Also decides if packet to be
     * dropped. This can happen if ACM is set for all traffic streams and TS is not yet created
     */
    void DownGradePacket(T8021DPriority& aUP, TBool& aDrop);
    
    
    // Driver management
    void LoadPacketDriverL();

    
    // Search CLanxBearer array by various keys (used in bind/unbind/provisioning)
    CLanxBearer* FindBearerByProtocolName(const TDesC8& aProtocol, TInt& aIndex) const;
    CLanxBearer* FindBearerByUpperControl(const ESock::MUpperControl* aUpperControl, TInt& aIndex) const;
    
    // Provisioning
	void ProvisionConfig(const ESock::RMetaExtensionContainerC& aConfigData);
    void ProvisionConfigL();
    void ProvisionBearerConfigL(const TDesC8& aName);
 
protected:
 
    CPcCardPktDrv* iPktDrv;

    /** */
    TBuf8<KMACByteLength> iMacAddr;

    /** Handle to the NIF-WLAN engine interface class. */
	CLANNifWLMServerCommon* iWLMServerCommon;

    /** Array containing handles to per-AC instances. */
	CLANNifWLMServerPerAC* iAcArray[EWlmAccessClassMax];

    /** indicates if iMacAddr contains a valid Mac address */
    TBool iValidMacAddr;

    enum TlanIfStatus {EIfPending, EIfUp, EIfBusy, EIfDown};
    /** */
    TlanIfStatus iState;
    
    /** */
    TEtherHeaderType iEtherType;

    /** Bearer List */
    CLanxBearerPtrArray  *iBearers;
    CLanxBearer          *iOnlyThisBearer;
    
    /** iPeriodic and iError needed for sending DHCPRELEASE when the 
     * interface is being closed
     */
    CPeriodic* iPeriodic;
    
    // Provisioning related
    const TLanIp4Provision* iLanIp4Provision;       // ip4 provisioning information from SCPR
    const TLanIp6Provision* iLanIp6Provision;       // ip6 provisioning information from SCPR
    TBool iStopRequested;

    TInt iError;                                    // Saved from StopFlow() argument
    TInt iSavedError;                               // saved errors from processing TProvisionConfig message

    enum TMeshMashineFlowState
        {
        EStopped,
        EStarting,
        EStarted,
        EStopping,
        };
    TMeshMashineFlowState iMMState; 
};
    
inline const TBuf8<KMACByteLength>& CLANLinkCommon::MacAddress() const
{
    return iMacAddr;
}

#endif // __WLANPROTO_H__
