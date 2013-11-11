/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/
#include "rijndaelimpl.h"
#include "keys.h"
#include "pluginconfig.h"

using namespace HwCrypto;

const TUint KAESKeyBytes128 = 16;
const TUint KAESKeyBytes192 = 24;
const TUint KAESKeyBytes256 = 32;
const TUint KAESBlockBytes = 16;

_LIT(KLddFileName,"cryptoldd.ldd");
_LIT(KPddFileName,"crypto.h4.pdd");


/* CRijndaelmpl*/
CH4RijndaelImpl::CH4RijndaelImpl(
	TUid aCryptoMode,
	TUid aOperationMode,
	TUid aPadding) :
	CH4CipherImpl(KAESBlockBytes, aCryptoMode, aOperationMode, aPadding)
	{
	}

CH4RijndaelImpl* CH4RijndaelImpl::NewL(const CKey& aKey, TUid aCryptoMode,	TUid aOperationMode, TUid aPadding)
	{
	CH4RijndaelImpl* self = CH4RijndaelImpl::NewLC(aKey, aCryptoMode, aOperationMode, aPadding);
	CleanupStack::Pop(self);
	return self;
	}
	
CH4RijndaelImpl* CH4RijndaelImpl::NewLC(const CKey& aKey, TUid aCryptoMode, TUid aOperationMode, TUid aPadding)
	{
	CH4RijndaelImpl* self = new(ELeave) CH4RijndaelImpl(aCryptoMode, aOperationMode, aPadding);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}
		
CH4RijndaelImpl::~CH4RijndaelImpl()
	{
	iDriver.Close();
	}
	
void CH4RijndaelImpl::ConstructL(const CKey& aKey)
	{
	// Load PDD
    TInt r = User::LoadPhysicalDevice(KPddFileName);
	if((r != KErrNone) && (r != KErrAlreadyExists)) User::Leave(r);

	// Load LDD
    r = User::LoadLogicalDevice(KLddFileName);
	if((r != KErrNone) && (r != KErrAlreadyExists)) User::Leave(r);


	CH4CipherImpl::ConstructL(aKey);			
	}

TUid CH4RijndaelImpl::ImplementationUid() const
	{
	return KCryptoPluginAesUid;
	}

const CExtendedCharacteristics* CH4RijndaelImpl::GetExtendedCharacteristicsL()
	{
	return StaticGetExtendedCharacteristicsL();
	}

CExtendedCharacteristics* CH4RijndaelImpl::StaticGetExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

	
TBool CH4RijndaelImpl::IsValidKeyLength(TInt aKeyBytes) const
	{
	switch(aKeyBytes)
		{
		case KAESKeyBytes128:
		case KAESKeyBytes192:
		case KAESKeyBytes256:
			return ETrue;
		default:
			return EFalse;
		}			
	}

void CH4RijndaelImpl::DoSetupL()
	{
	iDriver.Close();
	User::LeaveIfError(iDriver.Open());

	TBool encrypt = (iCryptoMode.iUid == KCryptoModeEncrypt);
	RCryptoDriver::TChainingMode chainingMode = (iOperationMode.iUid == KOperationModeCBC) ? (RCryptoDriver::ECbcMode) : (RCryptoDriver::EEcbMode);
	User::LeaveIfError(iDriver.SetAesConfig(encrypt, chainingMode, iKey->Des(), iIv));
	}

void CH4RijndaelImpl::DoWriteL(const TUint8* aBuffer, TUint aNumBytes)
	{
	TPtrC8 ptr(aBuffer, aNumBytes);

    TRequestStatus status;
	iDriver.AesWrite(status, ptr);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}

void CH4RijndaelImpl::DoReadL(TDes8 &aBuffer, TUint32 aLength)
	{
    TRequestStatus status;
	iDriver.AesRead(status, aBuffer, aLength);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	}


	
// End of file
