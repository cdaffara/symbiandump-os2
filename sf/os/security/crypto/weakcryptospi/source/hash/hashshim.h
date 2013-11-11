/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* hash shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __HASHSHIM_H__
#define __HASHSHIM_H__

#include <e32def.h>
#include <hash.h>
#include <cryptospi/cryptohashapi.h>
#include <cryptospi/keys.h>

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	#include <cryptospi/cryptomacapi.h>
#endif

//
// MD2 shim class
//
NONSHARABLE_CLASS(CMD2Shim) : public CMD2
	{
public:

	static CMD2Shim* NewL();
	static CMD2Shim* NewLC();
	~CMD2Shim();
	CMessageDigest* CopyL();
	TInt BlockSize();
	TInt HashSize()	;
	void Update(const TDesC8& aMessage);
	TPtrC8 Final(const TDesC8& aMessage);
	TPtrC8 Final();
	void Reset();
	TPtrC8 Hash(const TDesC8& aMessage);	
	CMessageDigest* ReplicateL();
	
protected:
	//virtual from CBase
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:	
	CMD2Shim();
	void ConstructL();
	
private:
	CryptoSpi::CHash* iHashImpl;
	};


//
// MD5 shim class
//

NONSHARABLE_CLASS(CMD5Shim) : public CMD5
	{
public:
	static CMD5Shim* NewL();
	static CMD5Shim* NewLC();
	~CMD5Shim();
	CMessageDigest* CopyL();
	TInt BlockSize();
	TInt HashSize()	;
	void Update(const TDesC8& aMessage);
	TPtrC8 Final(const TDesC8& aMessage);
	TPtrC8 Final();
	void Reset();
	TPtrC8 Hash(const TDesC8& aMessage);
	CMessageDigest* ReplicateL();
	
protected:
	//virtual from CBase
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:	
	CMD5Shim();
	void ConstructL();
	
private:
	CryptoSpi::CHash* iHashImpl;
	};



//
// SHA1 shim class
//

NONSHARABLE_CLASS(CSHA1Shim) : public CSHA1
	{
public:
	static CSHA1Shim* NewL();
	static CSHA1Shim* NewLC();
	~CSHA1Shim();
	CMessageDigest* CopyL();
	TInt BlockSize();
	TInt HashSize()	;
	void Update(const TDesC8& aMessage);
	TPtrC8 Final(const TDesC8& aMessage);
	TPtrC8 Final();
	void Reset();
	TPtrC8 Hash(const TDesC8& aMessage);
	CMessageDigest* ReplicateL();
	
protected:
	//virtual from CBase
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:	
	CSHA1Shim();
	void ConstructL();
	
private:
	CryptoSpi::CHash* iHashImpl;
	};

//
// SHA2 shim class
//

NONSHARABLE_CLASS(CSHA2Shim) : public CSHA2
	{
public:
	static CSHA2Shim* NewL(TSH2Algo aAlgorithmId);
	static CSHA2Shim* NewLC(TSH2Algo aAlgorithmId);
	~CSHA2Shim();
	CMessageDigest* CopyL();
	TInt BlockSize();
	TInt HashSize()	;
	void Update(const TDesC8& aMessage);
	TPtrC8 Final(const TDesC8& aMessage);
	TPtrC8 Final();
	void Reset();
	TPtrC8 Hash(const TDesC8& aMessage);
	CMessageDigest* ReplicateL();
	
protected:
	//virtual from CBase
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:	
	CSHA2Shim();
	void ConstructL(TSH2Algo aAlgorithmId);
	
private:
	CryptoSpi::CHash* iHashImpl;
	};



//
// HMac shim class
//

NONSHARABLE_CLASS(CHMACShim) : public CHMAC
	{
public:	
	static CHMACShim* NewL(const TDesC8& aKey, CMessageDigest* aDigest);
	static CHMACShim* NewLC(const TDesC8& aKey, CMessageDigest* aDigest);
	~CHMACShim();
	CMessageDigest* CopyL();
	TInt BlockSize();
	TInt HashSize()	;
	void Update(const TDesC8& aMessage);
	TPtrC8 Final(const TDesC8& aMessage);
	TPtrC8 Final();
	void Reset();
	TPtrC8 Hash(const TDesC8& aMessage);
	CMessageDigest* ReplicateL();

private:

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	CHMACShim(CryptoSpi::CMac* aImpl, CMessageDigest* aDigest, CryptoSpi::CKey* aKey, TUid aAlgorithmUid);
#else
	CHMACShim(CryptoSpi::CHash* aImpl, CMessageDigest* aDigest);
#endif	
	CHMACShim();
	void ConstructL(const TDesC8& aKey, TUid aAlgorithmUid, CMessageDigest* aDigest);
	
private:

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	CryptoSpi::CMac* iMacImpl;
	CryptoSpi::CKey* iKey;
	TUid iAlgorithmUid;
#else
	CryptoSpi::CHash* iMacImpl;
#endif
	};

//
// MD4 shim class
//

NONSHARABLE_CLASS(CMD4Shim) : public CMD4
	{
public:
	static CMD4Shim* NewL();
	static CMD4Shim* NewLC();
	~CMD4Shim();
	CMessageDigest* CopyL();
	TInt BlockSize();
	TInt HashSize();
	void Update(const TDesC8& aMessage);
	TPtrC8 Final(const TDesC8& aMessage);
	TPtrC8 Final();
	void Reset();
	TPtrC8 Hash(const TDesC8& aMessage);
	CMessageDigest* ReplicateL();
	
protected:
	//virtual from CBase
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
private:	
	CMD4Shim();
	void ConstructL();
	
private:
	CryptoSpi::CHash* iHashImpl;
	};

#endif // __HASHSHIM_H__
