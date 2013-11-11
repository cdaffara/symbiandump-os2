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
* Description:  Defines LanIp6Bearer class
*
*/

/*
* %version: 6 %
*/

#ifndef CLANIP6BEARER_H
#define CLANIP6BEARER_H

#include "CLanxBearer.h"
#include <eui_addr.h>
#include "WlanProvision.h"

/**
@internalComponent
*/
const TUint8 KMulticastPrefix[2] = {0x33, 0x33};

struct TIpv6Header
{
	TUint8 GetVersion() { return (TUint8)((iVersionClassHi>>4)&0xff);}
	TUint8 GetClass() { return (TUint8)( ((iVersionClassHi<<4)| (iClassLoFlowHi>>4)) &0xff); }
	TUint8 iVersionClassHi; // The Upper nybble provides version.
	TUint8 iClassLoFlowHi;
	TUint16 iFlowLo; // Two bytes, but it doesn't start on a word boundary.
	TUint16 iPayloadLength;
	TUint8 iNextHeader;
	TUint8 iHopLimit;
	union
		{
		TUint8 iSourceAddrB[16];
		TUint16 iSourceAddrW[8];
		TUint32 iSourceAddrL[4];
		};
	union
		{
		TUint8 iDestAddrB[16];
		TUint16 iDestAddrW[8];
		TUint32 iDestAddrL[4];
		};
};


NONSHARABLE_CLASS(CLanIp6Bearer) : public CLanxBearer
{
public:
	CLanIp6Bearer(CLANLinkCommon* aLink);
	virtual void ConstructL();

	// MLowerControl
	virtual TInt Control(TUint aLevel,TUint aName,TDes8& aOption);
	virtual TInt GetConfig(TBinderConfig& aConfig);

	// MLowerDataSender
	virtual MLowerDataSender::TSendResult Send(RMBufChain& aPdu);

	//Additional methods.
	// Perhaps need different version of this
	virtual TBool WantsProtocol(TUint16 aProtocolCode,const TUint8* aPayload);
	virtual void Process(RMBufChain& aPdu);
	virtual void UpdateMACAddr();

	virtual const TDesC8& ProtocolName() const;
	
	// Support for provisioning
	virtual void SetProvisionL(const Meta::SMetaData* aProvision);

private:
	void ResolveMulticastIp6(TDes8& aDstAddr,RMBufChain& aPdu);
	void ReadCommDbLanSettingsL();

	enum {KIPProtocol=0x800, KIP6Protocol=6};
	
	TEui64Addr iEuiMac;
	TIp6Addr iPrimaryDns;
	TIp6Addr iSecondaryDns;

	const TLanIp6Provision* iProvision;		// Provisioning information from SCPR
};

inline CLanIp6Bearer::CLanIp6Bearer(CLANLinkCommon* aLink) : 
   CLanxBearer(aLink)
{
}


#endif // CLANIP6BEARER_H
