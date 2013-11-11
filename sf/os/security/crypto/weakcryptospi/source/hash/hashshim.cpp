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
* hash shim implementation
* hash shim implementation
*
*/


/**
 @file
*/

#include "hashshim.h"
#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/keys.h>


using namespace CryptoSpi;

//
// MD2 shim implementation
//
CMD2Shim* CMD2Shim::NewL()
	{
	CMD2Shim* self=CMD2Shim::NewLC();
	CleanupStack::Pop();
	return self;	
	}

CMD2Shim* CMD2Shim::NewLC()
	{
	CMD2Shim* self=new(ELeave) CMD2Shim();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

TInt CMD2Shim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrNotSupported);
	
	if (KHashInterfaceUid.iUid==aExtensionId && iHashImpl)
		{
		a0=iHashImpl;
		ret=KErrNone;	
		}
		
	return ret;
	}

CMD2Shim::CMD2Shim()
	{		
	}

CMD2Shim::~CMD2Shim()
	{
	delete iHashImpl;				
	}


void CMD2Shim::ConstructL()
	{	
	CHashFactory::CreateHashL(iHashImpl,
							KMd2Uid,
							KHashModeUid,
							NULL, NULL);
	}

CMessageDigest* CMD2Shim::CopyL()
	{
	CMD2Shim* copy=new(ELeave) CMD2Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->CopyL();
	CleanupStack::Pop();
	return copy;
	}
	
CMessageDigest* CMD2Shim::ReplicateL()
	{
	CMD2Shim* copy=new(ELeave) CMD2Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->ReplicateL();
	CleanupStack::Pop();
	return copy;
	}


TInt CMD2Shim::BlockSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iBlockSize/8;
	}

TInt CMD2Shim::HashSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}	
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iOutputSize/8;
	}

void CMD2Shim::Update(const TDesC8& aMessage)
	{
	iHashImpl->Update(aMessage);	
	}
	
	
TPtrC8 CMD2Shim::Final(const TDesC8& aMessage)
	{
	return iHashImpl->Final(aMessage);
	}

TPtrC8 CMD2Shim::Final()
	{
	TPtrC8 ptr(KNullDesC8());
	return iHashImpl->Final(ptr);
	}
		
void CMD2Shim::Reset()
	{
	iHashImpl->Reset();
	}
	

TPtrC8 CMD2Shim::Hash(const TDesC8& aMessage)
	{
	return iHashImpl->Hash(aMessage);
	}

//
// Implementation of MD5 shim
//
CMD5Shim* CMD5Shim::NewL()
	{
	CMD5Shim* self=CMD5Shim::NewLC();
	CleanupStack::Pop();
	return self;	
	}

CMD5Shim* CMD5Shim::NewLC()
	{
	
	CMD5Shim* self=new(ELeave) CMD5Shim();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	
	}

CMD5Shim::CMD5Shim()
	{		
	}
	
TInt CMD5Shim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrNotSupported);
	
	if (KHashInterfaceUid.iUid==aExtensionId && iHashImpl)
		{
		a0=iHashImpl;
		ret=KErrNone;	
		}
		
	return ret;
	}

CMD5Shim::~CMD5Shim()
	{
	delete iHashImpl;				
	}


void CMD5Shim::ConstructL()
	{	
	CHashFactory::CreateHashL(iHashImpl,
							KMd5Uid,
							KHashModeUid,
							NULL,
							NULL);
	}

CMessageDigest* CMD5Shim::CopyL()
	{
	CMD5Shim* copy=new(ELeave) CMD5Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->CopyL();
	CleanupStack::Pop();
	return copy;
	}

CMessageDigest* CMD5Shim::ReplicateL()
	{
	CMD5Shim* copy=new(ELeave) CMD5Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->ReplicateL();
	CleanupStack::Pop();
	return copy;
	}

TInt CMD5Shim::BlockSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}		
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iBlockSize/8;
	}

TInt CMD5Shim::HashSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}		
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iOutputSize/8;
	}

void CMD5Shim::Update(const TDesC8& aMessage)
	{
	iHashImpl->Update(aMessage);	
	}
	
TPtrC8 CMD5Shim::Final(const TDesC8& aMessage)
	{
	return iHashImpl->Final(aMessage);
	}

TPtrC8 CMD5Shim::Final()
	{
	TPtrC8 ptr(KNullDesC8());
	return iHashImpl->Final(ptr);
	}
		
void CMD5Shim::Reset()
	{
	iHashImpl->Reset();
	}
	

TPtrC8 CMD5Shim::Hash(const TDesC8& aMessage)
	{
	return iHashImpl->Hash(aMessage);
	}
	
//
// Implementation of SHA1 shim
//	
	
CSHA1Shim* CSHA1Shim::NewL()
	{
	CSHA1Shim* self=CSHA1Shim::NewLC();
	CleanupStack::Pop();
	return self;	
	}

CSHA1Shim* CSHA1Shim::NewLC()
	{
	
	CSHA1Shim* self=new(ELeave) CSHA1Shim();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	
	}

CSHA1Shim::CSHA1Shim()
	{		
	}
	
	
TInt CSHA1Shim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrNotSupported);
	
	if (KHashInterfaceUid.iUid==aExtensionId && iHashImpl)
		{
		a0=iHashImpl;
		ret=KErrNone;	
		}
		
	return ret;
	}

CSHA1Shim::~CSHA1Shim()
	{
	delete iHashImpl;			
	}


void CSHA1Shim::ConstructL()
	{
	CHashFactory::CreateHashL(iHashImpl,
							KSha1Uid,
							KHashModeUid,
							NULL,
							NULL);
	}

CMessageDigest* CSHA1Shim::CopyL()
	{
	CSHA1Shim* copy=new(ELeave) CSHA1Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->CopyL();
	CleanupStack::Pop();
	return copy;
	}
	
CMessageDigest* CSHA1Shim::ReplicateL()
	{
	CSHA1Shim* copy=new(ELeave) CSHA1Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->ReplicateL();
	CleanupStack::Pop();
	return copy;
	}

TInt CSHA1Shim::BlockSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}	
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iBlockSize/8;
	}

TInt CSHA1Shim::HashSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iOutputSize/8;
	}

void CSHA1Shim::Update(const TDesC8& aMessage)
	{
	iHashImpl->Update(aMessage);	
	}
	
TPtrC8 CSHA1Shim::Final(const TDesC8& aMessage)
	{
	return iHashImpl->Final(aMessage);
	}

TPtrC8 CSHA1Shim::Final()
	{
	TPtrC8 ptr(KNullDesC8());
	return iHashImpl->Final(ptr);
	}
		
void CSHA1Shim::Reset()
	{
	iHashImpl->Reset();
	}

TPtrC8 CSHA1Shim::Hash(const TDesC8& aMessage)
	{
	return iHashImpl->Hash(aMessage);
	}
	

//
// Implementation of SHA2 shim
//	
	
CSHA2Shim* CSHA2Shim::NewL(TSH2Algo aAlgorithmId)
	{
	CSHA2Shim* self=CSHA2Shim::NewLC(aAlgorithmId);
	CleanupStack::Pop();
	return self;	
	}

CSHA2Shim* CSHA2Shim::NewLC(TSH2Algo aAlgorithmId)
	{
	
	CSHA2Shim* self=new(ELeave) CSHA2Shim();
	CleanupStack::PushL(self);
	self->ConstructL(aAlgorithmId);
	return self;
	}

CSHA2Shim::CSHA2Shim()
	{		
	}
	
TInt CSHA2Shim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrNotSupported);
	
	if (KHashInterfaceUid.iUid==aExtensionId && iHashImpl)
		{
		a0=iHashImpl;
		ret=KErrNone;	
		}
	return ret;
	}

CSHA2Shim::~CSHA2Shim()
	{
	delete iHashImpl;			
	}

void CSHA2Shim::ConstructL(TSH2Algo aAlgorithmId)
	{
	TUid algoId = {0};
	switch(aAlgorithmId)
		{
		case E224Bit:
			{
			algoId = KSha224Uid;
			break;
			}
		case E256Bit:
			{
			algoId = KSha256Uid;
			break;
			}
		case E384Bit:
			{
			algoId = KSha384Uid;
			break;
			}
		case E512Bit:
			{
			algoId = KSha512Uid;
			break;
			}
		default:
			User::Leave(KErrNotSupported);
		}
	
	CHashFactory::CreateHashL(iHashImpl,
							  algoId,
							  KHashModeUid,
							  NULL,
							  NULL);				
	}

CMessageDigest* CSHA2Shim::CopyL()
	{
	CSHA2Shim* copy=new(ELeave) CSHA2Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->CopyL();
	CleanupStack::Pop();
	return copy;
	}
	
CMessageDigest* CSHA2Shim::ReplicateL()
	{
	CSHA2Shim* copy=new(ELeave) CSHA2Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->ReplicateL();
	CleanupStack::Pop();
	return copy;
	}

TInt CSHA2Shim::BlockSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}	
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iBlockSize/8;
	}

TInt CSHA2Shim::HashSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iOutputSize/8;
	}

void CSHA2Shim::Update(const TDesC8& aMessage)
	{
	iHashImpl->Update(aMessage);	
	}
	
TPtrC8 CSHA2Shim::Final(const TDesC8& aMessage)
	{
	return iHashImpl->Final(aMessage);
	}

TPtrC8 CSHA2Shim::Final()
	{
	TPtrC8 ptr(KNullDesC8());
	return iHashImpl->Final(ptr);
	}
		
void CSHA2Shim::Reset()
	{
	iHashImpl->Reset();
	}

TPtrC8 CSHA2Shim::Hash(const TDesC8& aMessage)
	{
	return iHashImpl->Hash(aMessage);
	}

//
// Implementation of HMAC shim
//	

CHMACShim* CHMACShim::NewL(const TDesC8& aKey, CMessageDigest* aDigest)
	{
	CHMACShim* self(0);
	
	// Check whether the hash contains an SPI plug-in
	TAny* implPtr(0);
	TInt err = aDigest->GetExtension(CryptoSpi::KHashInterface, implPtr, NULL);	
	if (err == KErrNone && implPtr)
		{
		CryptoSpi::CHash* impl(static_cast<CryptoSpi::CHash*>(implPtr));
		const CryptoSpi::TCharacteristics* c(0);
		impl->GetCharacteristicsL(c);
		const CryptoSpi::THashCharacteristics* hashCharacteristics(static_cast<const CryptoSpi::THashCharacteristics*>(c));
			
		
		// Verify that the plug-in supports Hmac mode
		if (hashCharacteristics->IsOperationModeSupported(CryptoSpi::KHmacModeUid))
			{
			
	#ifndef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT				
			// Make an own copy
			CHash* myHash=impl->ReplicateL();
			CleanupStack::PushL(myHash);	
	#endif	
			// Set the key
			TKeyProperty keyProperty = {KHmacModeUid, KNullUid, KSymmetricKey, KNonEmbeddedKeyUid};
		 	CCryptoParams* keyParam =CCryptoParams::NewLC();
		 	keyParam->AddL(aKey, KHmacKeyParameterUid);
		 	CKey* uniKey= CKey::NewLC(keyProperty, *keyParam);
	#ifndef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
		 	myHash->SetKeyL(*uniKey);		 			 	
		 	CleanupStack::PopAndDestroy(2, keyParam);
			// Set hash to Hmac mode			
			myHash->SetOperationModeL(CryptoSpi::KHmacModeUid);
			self = new(ELeave) CHMACShim(myHash, aDigest);
			CleanupStack::Pop(myHash);
	#else 	
		 	TUid algorithmUID = algorithmUID.Uid(hashCharacteristics->cmn.iAlgorithmUID);
			//Create a pointer for the Mac Implementation Object
			CMac* macImpl = NULL;
			CMacFactory::CreateMacL(macImpl,algorithmUID,*uniKey, NULL);
			CleanupStack::PushL(macImpl);
		 	self = new(ELeave) CHMACShim(macImpl, aDigest,uniKey, algorithmUID);
		 	CleanupStack::Pop(macImpl);
		 	CleanupStack::Pop(uniKey);			
		 	CleanupStack::PopAndDestroy(keyParam);
	#endif		 	
			}
		}				
	return self;
	}
	
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	

CHMACShim::CHMACShim(CryptoSpi::CMac* aImpl, CMessageDigest* aDigest, CryptoSpi::CKey* aKey, TUid aAlgorithmUid)
	: CHMAC(aDigest), iMacImpl(aImpl), iKey(aKey), iAlgorithmUid(aAlgorithmUid)
	{		
	}

#else

CHMACShim::CHMACShim(CryptoSpi::CHash* aImpl, CMessageDigest* aDigest)
	: CHMAC(aDigest), iMacImpl(aImpl)
	{		
	}

#endif

CHMACShim::CHMACShim()
	{		
	}

CHMACShim::~CHMACShim()
	{
	delete iMacImpl;
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
	delete iKey;
#endif
	}

TInt CHMACShim::BlockSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iMacImpl->GetCharacteristicsL(ptr));

	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}

#ifndef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT			
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iBlockSize/8;
#else		
	const TMacCharacteristics* macPtr=static_cast<const TMacCharacteristics*>(ptr);
	return macPtr->iHashAlgorithmChar->iBlockSize/8;
#endif	
	}

TInt CHMACShim::HashSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iMacImpl->GetCharacteristicsL(ptr));
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}

#ifndef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT			
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iOutputSize/8;
#else			
	const TMacCharacteristics* macPtr=static_cast<const TMacCharacteristics*>(ptr);
	return macPtr->iHashAlgorithmChar->iOutputSize/8;
#endif	
	}

void CHMACShim::Update(const TDesC8& aMessage)
	{
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	// The error is ignored as the legacy code methods are non-leaving and they call 
	// the new MAC interfaces which uses leaving methods for processing MAC value. 	
	// This call always call the non-leaving legacy method.	
	TRAP_IGNORE(iMacImpl->UpdateL(aMessage));
#else
	iMacImpl->Update(aMessage);
#endif	
	}
	
TPtrC8 CHMACShim::Final(const TDesC8& aMessage)
	{
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	TPtrC8 macPtr(KNullDesC8());
	// The error is ignored as the legacy code methods are non-leaving and they call 
	// the new MAC interfaces which uses leaving methods for processing MAC value. 	
	// This call always call the non-leaving legacy method.	
	TRAP_IGNORE(macPtr.Set(iMacImpl->FinalL(aMessage)));	
	return macPtr;
#else
	return iMacImpl->Final(aMessage);
#endif	
	}

TPtrC8 CHMACShim::Final()
	{
	TPtrC8 ptr(KNullDesC8());
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
	TPtrC8 macPtr(KNullDesC8());
	// The error is ignored as the legacy code methods are non-leaving and they call 
	// the new MAC interfaces which uses leaving methods for processing MAC value. 	
	// This call always call the non-leaving legacy method.	
	TRAP_IGNORE(macPtr.Set(iMacImpl->FinalL(ptr)));	
	return macPtr;
#else
	return iMacImpl->Final(ptr);
#endif	
	}
		
void CHMACShim::Reset()
	{
	iMacImpl->Reset();
	}
	
TPtrC8 CHMACShim::Hash(const TDesC8& aMessage)
	{
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	TPtrC8 macPtr(KNullDesC8());
	// The error is ignored as the legacy code methods are non-leaving and they call 
	// the new MAC interfaces which uses leaving methods for processing MAC value. 	
	// This call always call the non-leaving legacy method.	
	TRAP_IGNORE(macPtr.Set(iMacImpl->MacL(aMessage)));	
	return macPtr;
#else
	return iMacImpl->Hash(aMessage);
#endif	
	}
	
CMessageDigest* CHMACShim::ReplicateL()
	{
	CHMACShim* copy=new(ELeave) CHMACShim();
	CleanupStack::PushL(copy);
	copy->iMacImpl=iMacImpl->ReplicateL();
	CleanupStack::Pop(copy);
	return copy;	
	}
	
CMessageDigest* CHMACShim::CopyL()
	{
	CHMACShim* copy=new(ELeave) CHMACShim();
	CleanupStack::PushL(copy);
	copy->iMacImpl=iMacImpl->CopyL();
	CleanupStack::Pop(copy);
	return copy;	
	}

//
// Implemetation of MD4 shim
//
CMD4Shim* CMD4Shim::NewL()
	{
	CMD4Shim* self=CMD4Shim::NewLC();
	CleanupStack::Pop();
	return self;	
	}

CMD4Shim* CMD4Shim::NewLC()
	{
	CMD4Shim* self=new(ELeave) CMD4Shim();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

CMD4Shim::CMD4Shim()
	{		
	}
	
TInt CMD4Shim::Extension_(TUint aExtensionId, TAny*& a0, TAny* /*a1*/)
	{
	TInt ret(KErrNotSupported);
	
	if (KHashInterfaceUid.iUid==aExtensionId && iHashImpl)
		{
		a0=iHashImpl;
		ret=KErrNone;	
		}
		
	return ret;
	}

CMD4Shim::~CMD4Shim()
	{
	delete iHashImpl;				
	}


void CMD4Shim::ConstructL()
	{	
	CHashFactory::CreateHashL(iHashImpl,KMd4Uid,KHashModeUid,NULL,NULL);
	}

CMessageDigest* CMD4Shim::CopyL()
	{
	CMD4Shim* copy=new(ELeave) CMD4Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->CopyL();
	CleanupStack::Pop();
	return copy;
	}

CMessageDigest* CMD4Shim::ReplicateL()
	{
	CMD4Shim* copy=new(ELeave) CMD4Shim();
	CleanupStack::PushL(copy);
	copy->iHashImpl=iHashImpl->ReplicateL();
	CleanupStack::Pop();
	return copy;
	}

TInt CMD4Shim::BlockSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}		
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iBlockSize/8;
	}

TInt CMD4Shim::HashSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return ((err>0)? KErrGeneral : err);
		}		
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iOutputSize/8;
	}

void CMD4Shim::Update(const TDesC8& aMessage)
	{
	iHashImpl->Update(aMessage);	
	}
	
TPtrC8 CMD4Shim::Final(const TDesC8& aMessage)
	{
	return iHashImpl->Final(aMessage);
	}

TPtrC8 CMD4Shim::Final()
	{
	TPtrC8 ptr(KNullDesC8());
	return iHashImpl->Final(ptr);
	}
		
void CMD4Shim::Reset()
	{
	iHashImpl->Reset();
	}
	

TPtrC8 CMD4Shim::Hash(const TDesC8& aMessage)
	{
	return iHashImpl->Hash(aMessage);
	}
	
