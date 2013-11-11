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
* crypto plugins loader implementation
* crypto spi state implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptocharacteristics.h>
#include <cryptospi/cryptospidef.h>
#include "cryptospiutil.h"
#include <cryptospi/extendedcharacteristics.h>


using namespace CryptoSpi;


//
// Implementation of CCharacteristics
//
CCharacteristics::CCharacteristics()
	{
	}
	
CCharacteristics::~CCharacteristics()
	{
	iCreatorName.Close();
	iAlgorithmName.Close();
	}
	
void CCharacteristics::InternalizeL(RReadStream& aStream)
	{
	iInterfaceUid=aStream.ReadInt32L();
	iAlgorithmUid=aStream.ReadInt32L();
	iImplementationUid=aStream.ReadInt32L();
	iCreatorName.CreateL(aStream, KMaxFileName);
	iIsFIPSApproved=aStream.ReadInt8L();
	iIsHardwareSupported=aStream.ReadInt8L();
	iMaxConcurrencySupported=aStream.ReadUint32L();
	iAlgorithmName.CreateL(aStream, KMaxFileName);
	iLatency=aStream.ReadInt32L();
	iThroughput=aStream.ReadInt32L();
	}

//
// Implementation of CHashCharacteristics
//
CHashCharacteristics* CHashCharacteristics::NewL()
	{
	CHashCharacteristics* self=CHashCharacteristics::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CHashCharacteristics* CHashCharacteristics::NewLC()
	{
	CHashCharacteristics* self=new(ELeave) CHashCharacteristics();
	CleanupStack::PushL(self);
	return self;
	}

CHashCharacteristics::~CHashCharacteristics()
	{
	iSupportedOperationModes.Close();
	}

CHashCharacteristics::CHashCharacteristics()
	{
	}

void CHashCharacteristics::InternalizeL(RReadStream& aStream)
	{
	CCharacteristics::InternalizeL(aStream);
	iBlockSize=aStream.ReadUint32L();
	iOutputSize=aStream.ReadUint32L();
	TUint32 count=aStream.ReadUint32L();
	for (TUint i=0;i<count;i++)
		{
		TInt32 mode=aStream.ReadInt32L();
		iSupportedOperationModes.AppendL(mode);
		}
	}

TBool CHashCharacteristics::IsOperationModeSupported(TUid aOperationMode) const	
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(iInterfaceUid == KHashInterface);
	
	TInt count(iSupportedOperationModes.Count());
	TBool supported(EFalse);
	
	for (TInt i = 0; i < count; ++i)
		{
		if (iSupportedOperationModes[i] == aOperationMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
		
	return supported;
	}

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
//
// Implementation of CMacCharacteristics
//
CMacCharacteristics* CMacCharacteristics::NewL()
	{
	CMacCharacteristics* self=CMacCharacteristics::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CMacCharacteristics* CMacCharacteristics::NewLC()
	{
	CMacCharacteristics* self=new(ELeave) CMacCharacteristics();
	CleanupStack::PushL(self);
	return self;
	}

CMacCharacteristics::~CMacCharacteristics()
	{
	delete iAlgorithmChar;
	}

CMacCharacteristics::CMacCharacteristics()
	{
	}

void CMacCharacteristics::InternalizeL(RReadStream& aStream)
	{
	CCharacteristics::InternalizeL(aStream);
	iMacMode = aStream.ReadUint32L();
	if(iMacMode == KHmacMode)
		{
		iAlgorithmChar = CHashCharacteristics::NewL();
		}
	else if (iMacMode == KSymmetricCipherMode)
		{
		iAlgorithmChar = CSymmetricCipherCharacteristics::NewL();
		}
	User::LeaveIfNull(iAlgorithmChar);
	iAlgorithmChar->InternalizeL(aStream);
	}
#endif
//
// Implementation of CRandomCharacteristics
//
CRandomCharacteristics* CRandomCharacteristics::NewL()
	{
	CRandomCharacteristics* self=CRandomCharacteristics::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CRandomCharacteristics* CRandomCharacteristics::NewLC()
	{
	CRandomCharacteristics* self=new(ELeave) CRandomCharacteristics();
	CleanupStack::PushL(self);
	return self;
	}

CRandomCharacteristics::~CRandomCharacteristics()
	{
	}

CRandomCharacteristics::CRandomCharacteristics()
	{
	}

void CRandomCharacteristics::InternalizeL(RReadStream& aStream)
	{
	CCharacteristics::InternalizeL(aStream);
	iBlockingMode=aStream.ReadUint32L();
	}
	
//
// Implementation of CSymmetricCipherCharacteristics
//
CSymmetricCipherCharacteristics* CSymmetricCipherCharacteristics::NewL()
	{
	CSymmetricCipherCharacteristics* self=CSymmetricCipherCharacteristics::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSymmetricCipherCharacteristics* CSymmetricCipherCharacteristics::NewLC()
	{
	CSymmetricCipherCharacteristics* self=new(ELeave) CSymmetricCipherCharacteristics();
	CleanupStack::PushL(self);
	return self;
	}

CSymmetricCipherCharacteristics::~CSymmetricCipherCharacteristics()
	{
	iSupportedPaddingModes.Close();
	iSupportedOperationModes.Close();
	}

CSymmetricCipherCharacteristics::CSymmetricCipherCharacteristics()
	{
	}

void CSymmetricCipherCharacteristics::InternalizeL(RReadStream& aStream)
	{
	CCharacteristics::InternalizeL(aStream);
	iMaximumKeyLength=aStream.ReadUint32L();
	iBlockSize=aStream.ReadUint32L();
	TUint32 count=aStream.ReadUint32L();
	for (TUint i=0;i<count;i++)
		{
		TInt32 mode=aStream.ReadInt32L();
		iSupportedPaddingModes.AppendL(mode);
		}
	
	count=aStream.ReadUint32L();
	for (TUint j=0;j<count;j++)
		{
		TInt32 mode=aStream.ReadInt32L();
		iSupportedOperationModes.AppendL(mode);
		}
	iKeySupportMode=aStream.ReadInt32L();
	}

TBool CSymmetricCipherCharacteristics::IsOperationModeSupported(TUid aOperationMode) const
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(iInterfaceUid == KSymmetricCipherInterface);
	
	TBool supported(EFalse);
	TInt count(iSupportedOperationModes.Count());

	for (TInt i = 0; i < count; ++i)
		{
		if (iSupportedOperationModes[i] == aOperationMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
	return supported;
	}

TBool CSymmetricCipherCharacteristics::IsPaddingModeSupported(TUid aPaddingMode) const	
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(iInterfaceUid == KSymmetricCipherInterface);
	
	TBool supported(EFalse);
	TInt count(iSupportedPaddingModes.Count());
	
	for (TInt i = 0; i < count; ++i)
		{
		if (iSupportedPaddingModes[i] == aPaddingMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
	return supported;		
	}
	
//
// Implementation of CAsymmetricCipherCharacteristics
//		
CAsymmetricCipherCharacteristics* CAsymmetricCipherCharacteristics::NewL()
	{
	CAsymmetricCipherCharacteristics* self=CAsymmetricCipherCharacteristics::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CAsymmetricCipherCharacteristics* CAsymmetricCipherCharacteristics::NewLC()
	{
	CAsymmetricCipherCharacteristics* self=new(ELeave) CAsymmetricCipherCharacteristics();
	CleanupStack::PushL(self);
	return self;
	}

CAsymmetricCipherCharacteristics::~CAsymmetricCipherCharacteristics()
	{
	iSupportedPaddingModes.Close();
	}

CAsymmetricCipherCharacteristics::CAsymmetricCipherCharacteristics()
	{
	}

void CAsymmetricCipherCharacteristics::InternalizeL(RReadStream& aStream)
	{
	CCharacteristics::InternalizeL(aStream);
	iMaximumKeyLength=aStream.ReadUint32L();
	TUint32 count=aStream.ReadUint32L();
	
	for (TUint i=0;i<count;i++)
		{
		TInt32 mode=aStream.ReadInt32L();
		iSupportedPaddingModes.AppendL(mode);
		}
	iKeySupportMode=aStream.ReadInt32L();
	}
		
TBool CAsymmetricCipherCharacteristics::IsPaddingModeSupported(TUid aPaddingMode) const	
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(iInterfaceUid == KAsymmetricCipherInterface);
	
	TBool supported(EFalse);
	TInt count(iSupportedPaddingModes.Count());
	
	for (TInt i = 0; i < count; ++i)
		{
		if (iSupportedPaddingModes[i] == aPaddingMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
	return supported;
	}
		
//
// Implementation of CAsymmetricSignatureCharacteristics
//
CAsymmetricSignatureCharacteristics* CAsymmetricSignatureCharacteristics::NewL()
	{
	CAsymmetricSignatureCharacteristics* self=CAsymmetricSignatureCharacteristics::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CAsymmetricSignatureCharacteristics* CAsymmetricSignatureCharacteristics::NewLC()
	{
	CAsymmetricSignatureCharacteristics* self=new(ELeave) CAsymmetricSignatureCharacteristics();
	CleanupStack::PushL(self);
	return self;
	}

CAsymmetricSignatureCharacteristics::~CAsymmetricSignatureCharacteristics()
	{
	iSupportedPaddingModes.Close();
	}

CAsymmetricSignatureCharacteristics::CAsymmetricSignatureCharacteristics()
	{
	}

void CAsymmetricSignatureCharacteristics::InternalizeL(RReadStream& aStream)
	{
	CCharacteristics::InternalizeL(aStream);
	iMaximumKeyLength=aStream.ReadUint32L();
	TUint32 count=aStream.ReadUint32L();
	for (TUint i=0;i<count;i++)
		{
		TInt32 mode=aStream.ReadInt32L();
		iSupportedPaddingModes.AppendL(mode);
		}
	iKeySupportMode=aStream.ReadInt32L();
	}

TBool CAsymmetricSignatureCharacteristics::IsPaddingModeSupported(TUid aPaddingMode) const	
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(iInterfaceUid == KSignerInterface || iInterfaceUid == KVerifierInterface);
	
	TBool supported(EFalse);
	TInt count(iSupportedPaddingModes.Count());

	for (TInt i = 0; i < count; ++i)
		{
		if (iSupportedPaddingModes[i] == aPaddingMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
	return supported;		
	}
	
//
// Implementation of CKeyAgreementCharacteristics
//
CKeyAgreementCharacteristics* CKeyAgreementCharacteristics::NewL()
	{
	CKeyAgreementCharacteristics* self=CKeyAgreementCharacteristics::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CKeyAgreementCharacteristics* CKeyAgreementCharacteristics::NewLC()
	{
	CKeyAgreementCharacteristics* self=new(ELeave) CKeyAgreementCharacteristics();
	CleanupStack::PushL(self);
	return self;
	}

CKeyAgreementCharacteristics::~CKeyAgreementCharacteristics()
	{
	}

CKeyAgreementCharacteristics::CKeyAgreementCharacteristics()
	{
	}

void CKeyAgreementCharacteristics::InternalizeL(RReadStream& aStream)
	{
	CCharacteristics::InternalizeL(aStream);
	}
	
//
// Implementation of CKeypairGeneratorCharacteristics
//
CKeypairGeneratorCharacteristics* CKeypairGeneratorCharacteristics::NewL()
	{
	CKeypairGeneratorCharacteristics* self=CKeypairGeneratorCharacteristics::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CKeypairGeneratorCharacteristics* CKeypairGeneratorCharacteristics::NewLC()
	{
	CKeypairGeneratorCharacteristics* self=new(ELeave) CKeypairGeneratorCharacteristics();
	CleanupStack::PushL(self);
	return self;
	}

CKeypairGeneratorCharacteristics::~CKeypairGeneratorCharacteristics()
	{
	}

CKeypairGeneratorCharacteristics::CKeypairGeneratorCharacteristics()
	{
	}

void CKeypairGeneratorCharacteristics::InternalizeL(RReadStream& aStream)
	{
	CCharacteristics::InternalizeL(aStream);
	iMaximumKeyLength=aStream.ReadUint32L();
	}

//
// Implementation of CCharacteristicsAndPluginName
//
CCharacteristicsAndPluginName* CCharacteristicsAndPluginName::NewL(TInt32 aInterface)
	{
	CCharacteristicsAndPluginName* self=CCharacteristicsAndPluginName::NewLC(aInterface);
	CleanupStack::Pop(self);
	return self;		
	}

CCharacteristicsAndPluginName* CCharacteristicsAndPluginName::NewLC(TInt32 aInterface)
	{
	CCharacteristicsAndPluginName* self=new(ELeave) CCharacteristicsAndPluginName();
	CleanupStack::PushL(self);
	self->ConstructL(aInterface);
	return self;		
	}
	
CCharacteristicsAndPluginName::CCharacteristicsAndPluginName()
	{
	}

CCharacteristicsAndPluginName::~CCharacteristicsAndPluginName()
	{
	delete iCharacteristic;
	if (iExtendedCharacteristic)
		{
		delete iExtendedCharacteristic;	
		}
	}

void CCharacteristicsAndPluginName::ConstructL(TInt32 aInterface)
	{
	iCharacteristic=CryptoSpiUtil::CreateCharacteristicsL(aInterface);
	}

void CCharacteristicsAndPluginName::InternalizeL(RReadStream& aStream)
	{
	iCharacteristic->InternalizeL(aStream);
	TInt index=aStream.ReadInt8L();
	CryptoSpiUtil::DllIndexToName(index, iDllName);
	}

