/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* crypto plugin characteristic implementation
*
*/



#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/romlit.h>

using namespace CryptoSpi;

EXPORT_C TBool TSymmetricCipherCharacteristics::IsOperationModeSupported(TUid aOperationMode) const
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(cmn.iInterfaceUID == KSymmetricCipherInterface);
	
	TBool supported(EFalse);
	for (TUint i = 0; i < iOperationModeNum; ++i)
		{
		if (iSupportedOperationModes[i] == aOperationMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
	return supported;
	}

EXPORT_C TBool TSymmetricCipherCharacteristics::IsPaddingModeSupported(TUid aPaddingMode) const	
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(cmn.iInterfaceUID == KSymmetricCipherInterface);
	
	TBool supported(EFalse);
	for (TUint i = 0; i < iPaddingModeNum; ++i)
		{
		if (iSupportedPaddingModes[i] == aPaddingMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
	return supported;		
	}

EXPORT_C TBool TAsymmetricCipherCharacteristics::IsPaddingModeSupported(TUid aPaddingMode) const	
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(cmn.iInterfaceUID == KAsymmetricCipherInterface);
	
	TBool supported(EFalse);
	for (TInt i = 0; i < iPaddingModeNum; ++i)
		{
		if (iSupportedPaddingModes[i] == aPaddingMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
	return supported;
	}

EXPORT_C TBool TAsymmetricSignatureCharacteristics::IsPaddingModeSupported(TUid aPaddingMode) const	
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(cmn.iInterfaceUID == KSignerInterface || cmn.iInterfaceUID == KVerifierInterface);
	
	TBool supported(EFalse);
	for (TInt i = 0; i < iPaddingModeNum; ++i)
		{
		if (iSupportedPaddingModes[i] == aPaddingMode.iUid)
			{
			supported = ETrue;
			break;
			}
		}
	return supported;		
	}
	
EXPORT_C TBool THashCharacteristics::IsOperationModeSupported(TUid aOperationMode) const	
	{
	// Sanity check to catch incorrect casts of characteristics classes
	ASSERT(cmn.iInterfaceUID == KHashInterface);
	
	TBool supported(EFalse);
	for (TUint i = 0; i < iOperationModeNum; ++i)
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

void TMacCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	iMacChar.ExternalizeL(aStream);
	aStream.WriteInt32L(iMacMode);
	if(iMacMode == KHmacMode)
		{
		//externalize THashCharacteristics object
		iHashAlgorithmChar->ExternalizeL(aStream);
		}
	else if(iMacMode == KSymmetricCipherMode)
		{
		//externalize TSymmetricCipherCharacteristics object
		iCipherAlgorithmChar->ExternalizeL(aStream);
		}
	}

#endif

void TCommonCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iInterfaceUID);
	aStream.WriteInt32L(iAlgorithmUID);
	aStream.WriteInt32L(iImplementationUID);	
	aStream<<(*iCreatorName)();
	aStream.WriteInt8L(iIsFIPSApproved);
	aStream.WriteInt8L(iIsHardwareSupported);
	aStream.WriteUint32L(iMaxConcurrencySupported);
	aStream<<(*iAlgorithmName)();
	aStream.WriteInt32L(iLatency);
	aStream.WriteInt32L(iThroughput);
	}

void THashCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	cmn.ExternalizeL(aStream);
	aStream.WriteUint32L(iBlockSize);
	aStream.WriteUint32L(iOutputSize);
	aStream.WriteUint32L(iOperationModeNum);
	for (TUint i=0;i<iOperationModeNum;i++)
		{
		aStream.WriteInt32L(iSupportedOperationModes[i]);
		}
	}
	
void TRandomCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	cmn.ExternalizeL(aStream);
	aStream.WriteUint32L(iBlockingMode);
	}

void TSymmetricCipherCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	cmn.ExternalizeL(aStream);
	aStream.WriteUint32L(iMaximumKeyLength);
	aStream.WriteUint32L(iBlockSize);
	aStream.WriteUint32L(iPaddingModeNum);
	for (TUint i=0;i<iPaddingModeNum;i++)
		{
		aStream.WriteInt32L(iSupportedPaddingModes[i]);
		}
	aStream.WriteUint32L(iOperationModeNum);
	for (TUint j=0;j<iOperationModeNum;j++)
		{
		aStream.WriteInt32L(iSupportedOperationModes[j]);
		}
	aStream.WriteInt32L(iKeySupportMode);
	}

void TAsymmetricCipherCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	cmn.ExternalizeL(aStream);
	aStream.WriteUint32L(iMaximumKeyLength);
	aStream.WriteUint32L(iPaddingModeNum);
	for (TInt i=0;i<iPaddingModeNum;i++)
		{
		aStream.WriteInt32L(iSupportedPaddingModes[i]);
		}
	aStream.WriteInt32L(iKeySupportMode);
	}

void TAsymmetricSignatureCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	cmn.ExternalizeL(aStream);
	aStream.WriteUint32L(iMaximumKeyLength);
	aStream.WriteUint32L(iPaddingModeNum);
	for (TInt i=0;i<iPaddingModeNum;i++)
		{
		aStream.WriteInt32L(iSupportedPaddingModes[i]);
		}
	aStream.WriteInt32L(iKeySupportMode);
	}

void TKeyAgreementCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	cmn.ExternalizeL(aStream);
	}
	
void TAsymmetricKeypairGeneratorCharacteristics::ExternalizeL(RWriteStream& aStream) const
	{
	cmn.ExternalizeL(aStream);
	aStream.WriteUint32L(iMaximumKeyLength);	
	}

