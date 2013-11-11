/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <cbcmode.h>

#include "cbcmodeshim.h"
#include "../common/inlines.h"

void CBlockChainingMode::Reset()
	{
	iRegister.Copy(iIV);
	iBT->Reset();
	}

TInt CBlockChainingMode::BlockSize() const
	{
	return (iBT->BlockSize());
	}

TInt CBlockChainingMode::KeySize() const
	{
	return (iBT->KeySize());
	}

void CBlockChainingMode::SetIV(const TDesC8& aIV)
	{
	//We are making the stipulation that anybody calling SetIV is not setting it
	//to a longer IV than they originally did.  Otherwise SetIV needs to leave.
	assert(aIV.Size() <= iIV.Size());
	iIV.Copy(aIV);
	Reset();
	}

EXPORT_C CBlockChainingMode::CBlockChainingMode() 
	: iBT(NULL), iRegister(0,0,0), iIV(0,0,0)
	{
	}

EXPORT_C CBlockChainingMode::~CBlockChainingMode()
	{
	delete iBT;
	delete iRegisterBuf;
	delete iIVBuf;
	}

EXPORT_C void CBlockChainingMode::ConstructL(CBlockTransformation* aBT, const TDesC8& aIV)
	{
	iRegisterBuf = aIV.AllocL();
	iRegister.Set(iRegisterBuf->Des());
	iIVBuf = aIV.AllocL();
	iIV.Set(iIVBuf->Des());

	// Take ownership last - doesn't take ownership if we leave
	iBT = aBT;
	}

/* CModeCBCEncryptor */
EXPORT_C CModeCBCEncryptor* CModeCBCEncryptor::NewL(CBlockTransformation* aBT, 
	const TDesC8& aIV)
	{	
	CModeCBCEncryptor* self = CModeCBCEncryptorShim::NewL(aBT, aIV);
	if (! self)
		{			
		// not able to use CryptoSpi, possibly due to an exterally 
		// derived legacy class so fallback to old implementation.			
		self = NewLC(aBT,aIV);
		CleanupStack::Pop(self);
		}	
	return self;	
	}

EXPORT_C CModeCBCEncryptor* CModeCBCEncryptor::NewLC(CBlockTransformation* aBT,
	const TDesC8& aIV)
	{
	CModeCBCEncryptor* self = new (ELeave)CModeCBCEncryptor();	
	CleanupStack::PushL(self);
	self->ConstructL(aBT, aIV);
	return self;
	}

CModeCBCEncryptor::CModeCBCEncryptor()
	{
	}

void CModeCBCEncryptor::Transform(TDes8& aBlock)
	{
	assert(aBlock.Size() == iBT->BlockSize());
	assert(iRegister.Size() == aBlock.Size());

	XorBuf(const_cast<TUint8*>(iRegister.Ptr()), aBlock.Ptr(), aBlock.Size());
	iBT->Transform(iRegister);
	aBlock.Copy(iRegister);
	}

/* CModeCBCDecryptor */
EXPORT_C CModeCBCDecryptor* CModeCBCDecryptor::NewL(CBlockTransformation* aBT, 
	const TDesC8& aIV)
	{
	CModeCBCDecryptor* self = CModeCBCDecryptorShim::NewL(aBT, aIV);
	if (! self)
		{			
		// not able to use CryptoSpi, possibly due to an exterally 
		// derived legacy class so fallback to old implementation.			
		self = NewLC(aBT,aIV);
		CleanupStack::Pop(self);
		}	
	return self;	
	}

EXPORT_C CModeCBCDecryptor* CModeCBCDecryptor::NewLC(CBlockTransformation* aBT, 
	const TDesC8& aIV)
	{
	CModeCBCDecryptor* self = new (ELeave)CModeCBCDecryptor();	
	CleanupStack::PushL(self);
	self->ConstructL(aBT, aIV);
	return self;	
	}

void CModeCBCDecryptor::ConstructL(CBlockTransformation* aBT, const TDesC8& aIV)
	{
	iIVBakBuf = aIV.AllocL();
	iIVBak.Set(iIVBakBuf->Des());
	CBlockChainingMode::ConstructL(aBT, aIV);
	}

CModeCBCDecryptor::~CModeCBCDecryptor(void)
	{
	delete iIVBakBuf;
	}

CModeCBCDecryptor::CModeCBCDecryptor()
	: iIVBak(0,0,0)
	{
	}

void CModeCBCDecryptor::Transform(TDes8& aBlock)
	{
	assert(aBlock.Size() == iBT->BlockSize());
	assert(iRegister.Size() == aBlock.Size());
	assert(iIVBak.Size() == aBlock.Size());
	
	// Take a copy of incoming block
	iIVBak.Copy(aBlock);

	// transform the block
	iBT->Transform(aBlock);

	// xor the output with the register
	XorBuf(const_cast<TUint8*>(aBlock.Ptr()), iRegister.Ptr(), 
		aBlock.Size());

	// Update the register to be the original block
	iRegister.Copy(iIVBak);
}

