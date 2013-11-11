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
* Description:  Provisioning information for WLAN IP bearers
*
*/

/*
* %version: 5 %
*/

#ifndef __WLANPROVISION_H__
#define __WLANPROVISION_H__


#include <in_sock.h>
#include <comms-infras/metadata.h>


/**
Provisioning information for CLanIp4Bearer
*/
class TLanIp4Provision : public Meta::SMetaData
	{
public:
	enum 
		{
		EUid = 0x2001B2D3, ETypeId = 1
		};

public:
	inline TUint32 LocalAddr() const;
  	inline TUint32 NetMask() const;
  	inline TUint32 BroadcastAddr() const;
  	inline TUint32 DefGateway() const;
  	inline TUint32 PrimaryDns() const;
  	inline TUint32 SecondaryDns() const;
  	inline TUint32 Order() const;

	inline void SetLocalAddr(TUint32);
  	inline void SetNetMask(TUint32);
  	inline void SetBroadcastAddr(TUint32);
  	inline void SetDefGateway(TUint32);
  	inline void SetPrimaryDns(TUint32);
  	inline void SetSecondaryDns(TUint32);
  	inline void SetOrder(TUint32);
  	
private:
  	TUint32	iLocalAddr;
  	TUint32	iNetMask;
  	TUint32	iBroadcastAddr;
  	TUint32	iDefGateway;
  	TUint32	iPrimaryDns;
  	TUint32	iSecondaryDns;
  	TUint32 iOrder;

public:
	DATA_VTABLE
	};

/**
Provisioning information for CLanIp6Bearer
*/
class TLanIp6Provision : public Meta::SMetaData
	{
public:
	enum 
		{
		EUid = 0x2001B2D3, ETypeId = 2
		};

public:
	inline const TIp6Addr& PrimaryDns() const;
	inline const TIp6Addr& SecondaryDns() const;
	inline TUint32 Order() const;

	inline void SetPrimaryDns(const TIp6Addr&);
	inline void SetSecondaryDns(const TIp6Addr&);
  	inline void SetOrder(TUint32);
	
private:
	TIp6Addr iPrimaryDns;
	TIp6Addr iSecondaryDns;
	TUint32 iOrder;
	
public:
	DATA_VTABLE
	};

	
// TLanIp4Provision inline functions

inline TUint32 TLanIp4Provision::LocalAddr() const { return iLocalAddr; }
	
inline TUint32 TLanIp4Provision::NetMask() const { return iNetMask;	}
	
inline TUint32 TLanIp4Provision::BroadcastAddr() const { return iBroadcastAddr;	}
	
inline TUint32 TLanIp4Provision::DefGateway() const	{ return iDefGateway; }
	
inline TUint32 TLanIp4Provision::PrimaryDns() const	{ return iPrimaryDns; }
	
inline TUint32 TLanIp4Provision::SecondaryDns() const { return iSecondaryDns; }

inline TUint32 TLanIp4Provision::Order() const { return iOrder; }

inline void TLanIp4Provision::SetLocalAddr(TUint32 aLocalAddr) { iLocalAddr = aLocalAddr; }
	
inline void TLanIp4Provision::SetNetMask(TUint32 aNetMask) { iNetMask = aNetMask;	}
	
inline void TLanIp4Provision::SetBroadcastAddr(TUint32 aBroadcastAddr) { iBroadcastAddr = aBroadcastAddr;	}
	
inline void TLanIp4Provision::SetDefGateway(TUint32 aDefGateway) { iDefGateway = aDefGateway; }
	
inline void TLanIp4Provision::SetPrimaryDns(TUint32 aPrimaryDns) { iPrimaryDns = aPrimaryDns; }
	
inline void TLanIp4Provision::SetSecondaryDns(TUint32 aSecondaryDns) { iSecondaryDns = aSecondaryDns; }

inline void TLanIp4Provision::SetOrder(const TUint32 aOrder) {  iOrder = aOrder; }


// TLanIp6Provision inline functions

inline const TIp6Addr& TLanIp6Provision::PrimaryDns() const	{ return iPrimaryDns; }
	
inline const TIp6Addr& TLanIp6Provision::SecondaryDns() const {	return iSecondaryDns; }

inline TUint32 TLanIp6Provision::Order() const { return iOrder; }

inline void TLanIp6Provision::SetPrimaryDns(const TIp6Addr& aPrimaryDns) { iPrimaryDns = aPrimaryDns; }
	
inline void TLanIp6Provision::SetSecondaryDns(const TIp6Addr& aSecondaryDns) { iSecondaryDns = aSecondaryDns; }

inline void TLanIp6Provision::SetOrder(const TUint32 aOrder) { iOrder = aOrder; }

#endif  // __WLANPROVISION_H__
