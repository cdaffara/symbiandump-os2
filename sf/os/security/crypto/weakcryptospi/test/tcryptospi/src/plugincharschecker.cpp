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
 @internalTechnology
*/
 
#include "plugincharschecker.h"

using namespace CryptoSpi;

CPluginCharsChecker::~CPluginCharsChecker()
	{
	}

CPluginCharsChecker::CPluginCharsChecker()
	{
	}
	
CPluginCharsChecker* CPluginCharsChecker::NewL()
	{
	CPluginCharsChecker* me = CPluginCharsChecker::NewLC();
	CleanupStack::Pop(me);
	return me;
	}
	
CPluginCharsChecker* CPluginCharsChecker::NewLC()
	{
	CPluginCharsChecker* me = new(ELeave) CPluginCharsChecker();
	CleanupStack::PushL(me);
	return me;
	}
	
	
TBool CPluginCharsChecker::checkCommonCharacteristics(const TCommonCharacteristics* aCharacteristics,
												TVariantPtrC aInterfaceUid,
												TVariantPtrC aAlgorithmUid,
												TVariantPtrC aImplementationUid,
												TVariantPtrC aCreatorName,
												TBool aFIPSApproved,
												TBool aHardwareSupported,
												TInt aMaxConcurrencySupported,
												TVariantPtrC aAlgorithmName,
												TInt aLatency,
												TInt aThroughput,
												TPtrC& aErrorMessage)
	{
	TBool result = ETrue;
	
	TRomLitC16 creatorName = *(aCharacteristics->iCreatorName);
	TRomLitC16 algorithmName = *(aCharacteristics->iAlgorithmName);
	
	if(!(aCharacteristics->iInterfaceUID == aInterfaceUid))
		{
		aErrorMessage.Set(_L("COMMON_INTERFACE_ID"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iAlgorithmUID == aAlgorithmUid))
		{
		aErrorMessage.Set(_L("COMMON_ALGORITHM_ID"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iImplementationUID == aImplementationUid))
		{
		aErrorMessage.Set(_L("COMMON_IMPLEMENTATION_ID"));
		result = EFalse;
		}
		
	if(!(*creatorName == *(static_cast<TRomLitC16>(aCreatorName))))
		{
		aErrorMessage.Set(_L("COMMON_CREATOR_NAME"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iIsFIPSApproved == aFIPSApproved)) 
		{
		aErrorMessage.Set(_L("COMMON_FIPS_APPROVED"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iIsHardwareSupported == aHardwareSupported))
		{
		aErrorMessage.Set(_L("COMMON_HARDWARE_SUPPORTED"));
		result = EFalse;
		}
	
	if(!(aCharacteristics->iMaxConcurrencySupported == aMaxConcurrencySupported))
		{
		aErrorMessage.Set(_L("COMMON_MAXIMUM_CONCURRENCY"));
		result = EFalse;	
		}
	
	if(!(*algorithmName == *(static_cast<TRomLitC16>(aAlgorithmName))))
		{
		aErrorMessage.Set(_L("COMMON_ALGORITHM_NAME"));
		result = EFalse;
		}
	
	if(!(aCharacteristics->iLatency == aLatency))
		{
		aErrorMessage.Set(_L("COMMON_LATENCY"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iThroughput == aThroughput))
		{	
		aErrorMessage.Set(_L("COMMON_THROUGHPUT"));
		result = EFalse;
		}
										
	return result;	
	}
	
	
TBool CPluginCharsChecker::checkSymmetricCharacteristics(const TSymmetricCipherCharacteristics* aCharacteristics,
												TInt aSymmetricMaxKeyLength,
												TInt aSymmetricBlockSize,
												TVariantPtrC aSymmetricPaddingModes,
												TInt aSymmetricPaddingModeNum,
												TVariantPtrC aSymmetricOperationModes,
												TInt aSymmetricOperationModeNum,
												TInt aSymmetricKeySupportMode,
												TPtrC& aErrorMessage)
	{
	TBool result = ETrue;
	
	TInt32 supportedPaddingModes = *(aCharacteristics->iSupportedPaddingModes);
	TInt32 supportedOperationModes = *(aCharacteristics->iSupportedOperationModes);
	
	if(!(aCharacteristics->iMaximumKeyLength == aSymmetricMaxKeyLength)) 
		{
		aErrorMessage.Set(_L("SYMMETRIC_MAXIMUM_KEY_LENGTH"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iBlockSize == aSymmetricBlockSize))
		{
		aErrorMessage.Set(_L("SYMMETRIC_BLOCK_SIZE"));
		result = EFalse;
		}
	
	if(!(supportedPaddingModes == *(static_cast<const TInt32*>(aSymmetricPaddingModes))))
		{
		aErrorMessage.Set(_L("SYMMETRIC_SUPPORTED_PADDING_MODES"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iPaddingModeNum == aSymmetricPaddingModeNum))
		{
		aErrorMessage.Set(_L("SYMMETRIC_PADDING_MODE_NUM"));
		result = EFalse;
		}
		
	if(!(supportedOperationModes == *(static_cast<const TInt32*>(aSymmetricOperationModes)))) 
		{
		aErrorMessage.Set(_L("SYMMETRIC_OPERATION_MODES"));
		result = EFalse;
		}
						
	if(!(aCharacteristics->iOperationModeNum == aSymmetricOperationModeNum))
		{
		aErrorMessage.Set(_L("SYMMETRIC_OPERATION_MODE_NUM"));
		result = EFalse;
		}
	
	if(!(aCharacteristics->iKeySupportMode == aSymmetricKeySupportMode))
		{
		aErrorMessage.Set(_L("SYMMETRIC_KEY_SUPPORT_MODE"));
		result = EFalse;
		}
		
	return result;	
	}

	
TBool CPluginCharsChecker::checkAsymmetricCharacteristics(const TAsymmetricCipherCharacteristics* aCharacteristics,
													TInt aAsymmetricMaxKeyLength,
													TVariantPtrC aAsymmetricPaddingModes,
													TInt aAsymmetricPaddingModeNum,
													TInt aAsymmetricKeySupportMode,
													TPtrC& aErrorMessage)
	{
	TBool result = ETrue;
	
	TInt32 supportedPaddingModes = *(aCharacteristics->iSupportedPaddingModes);
	
	if(!(aCharacteristics->iMaximumKeyLength == aAsymmetricMaxKeyLength)) 
		{
		aErrorMessage.Set(_L("ASYMMETRIC_MAXIMUM_KEY_LENGTH"));
		result = EFalse;
		}
		
	if(!(supportedPaddingModes == *(static_cast<const TInt32*>(aAsymmetricPaddingModes))))
		{
		aErrorMessage.Set(_L("ASYMMETRIC_SUPPORTED_PADDING_MODES"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iPaddingModeNum == aAsymmetricPaddingModeNum))
		{
		aErrorMessage.Set(_L("ASYMMETRIC_PADDING_MODE_NUM"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iKeySupportMode == aAsymmetricKeySupportMode))
		{	
		aErrorMessage.Set(_L("ASYMMETRIC_KEY_SUPPORT_MODE"));
		result = EFalse;
		}
	
	return result;	
	}
	
	
TBool CPluginCharsChecker::checkHashCharacteristics(const THashCharacteristics* aCharacteristics,
													TInt aBlockSize,
													TInt aOutputSize,
													TVariantPtrC aHashOperationModes,
													TInt aHashOperationModeNum,
													TPtrC& aErrorMessage)
	{
	TBool result = ETrue;
	
	TInt32 supportedOperationModes = *(aCharacteristics->iSupportedOperationModes);

	if(!(aCharacteristics->iBlockSize == aBlockSize)) 
		{
		aErrorMessage.Set(_L("HASH_BLOCK_SIZE"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iOutputSize == aOutputSize))
		{
		aErrorMessage.Set(_L("HASH_OUTPUT_SIZE"));
		result = EFalse;
		}
		
	if(!(supportedOperationModes == *(static_cast<const TInt32*>(aHashOperationModes)))) 
		{
		aErrorMessage.Set(_L("HASH_SUPPORTED_OPERATION_MODES"));
		result = EFalse;	
		}
		
	if(!(aCharacteristics->iOperationModeNum == aHashOperationModeNum))
		{
		aErrorMessage.Set(_L("HASH_OPERATION_MODE_NUM"));
		result = EFalse;	
		}
	
	return result;
	}
	
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
TBool CPluginCharsChecker::checkMacCharacteristics(const TMacCharacteristics* aCharacteristics,
													TVariantPtrC aMacMode,
													TPtrC& aErrorMessage)
	{
	TBool result = ETrue;

	if(!(aCharacteristics->iMacMode == aMacMode)) 
		{
		aErrorMessage.Set(_L("MAC_MODE"));
		result = EFalse;
		}
		
	return result;
	}
#endif

TBool CPluginCharsChecker::checkRandomCharacteristics(const TRandomCharacteristics* aCharacteristics,
														TInt aRandomBlockingMode,
														TPtrC& aErrorMessage)
	{
	TBool result = ETrue;
	
	if(!(aCharacteristics->iBlockingMode == aRandomBlockingMode)) 
		{
		aErrorMessage.Set(_L("RANDOM_BLOCKING_MODE"));
		result = EFalse;
		}
		
	return result;	
	}
	

TBool CPluginCharsChecker::checkAsymSignatureCharacteristics(const TAsymmetricSignatureCharacteristics* aCharacteristics,
															TInt aAsymSignMaxKeyLength,
															TVariantPtrC aAsymSignPaddingModes,
															TInt aAsymSignPaddingModeNum,
															TInt aAsymSignKeySupportMode,
															TPtrC& aErrorMessage)
	{
	TBool result = ETrue;
	
	TInt32 supportedPaddingModes = *(aCharacteristics->iSupportedPaddingModes);
	
	if(!(aCharacteristics->iMaximumKeyLength == aAsymSignMaxKeyLength)) 
		{
		aErrorMessage.Set(_L("ASYMSIGN_MAXIMUM_KEY_LENGTH"));
		result = EFalse;
		}
		
	if(!(supportedPaddingModes == *(static_cast<const TInt32*>(aAsymSignPaddingModes))))
		{
		aErrorMessage.Set(_L("ASYMSIGN_SUPPORTED_PADDING_MODES"));
		result = EFalse;
		}
		
	if(!(aCharacteristics->iPaddingModeNum == aAsymSignPaddingModeNum))
		{
		aErrorMessage.Set(_L("ASYMSIGN_PADDING_MODE_NUM"));
		result = EFalse;
		}

	if(!(aCharacteristics->iKeySupportMode == aAsymSignKeySupportMode))
		{
		aErrorMessage.Set(_L("ASYMSIGN_KEY_SUPPORT_MODE"));
		result = EFalse;
		}
		
	return result;	
	}
	
TBool CPluginCharsChecker::checkAsymKeypairGenCharacteristics(const CryptoSpi::TAsymmetricKeypairGeneratorCharacteristics* aCharacteristics,
																TInt aAsymKeypairGenMaxKeyLength,
																TPtrC& aErrorMessage)
	{
	TBool result = ETrue;
	
	if(!(aCharacteristics->iMaximumKeyLength == aAsymKeypairGenMaxKeyLength)) 
		{
		aErrorMessage.Set(_L("ASYMKEYPAIRGEN_MAX_KEY_LENGTH"));
		result = EFalse;
		}
		
	return result;
	}	

TBool CPluginCharsChecker::checkSelectedPlugin(const TCharacteristics* aCharacteristics,
												TVariantPtrC aAlgorithmUid,
												TVariantPtrC aImplementationUid,
												TPtrC& aErrorMessage)
	{
	TBool result = ETrue;
	
	const TCommonCharacteristics* commonChars = static_cast<const TCommonCharacteristics*>(aCharacteristics);
	
	if(!(commonChars->iAlgorithmUID == aAlgorithmUid))   
		{
		aErrorMessage.Set(_L("COMMON_ALGORITHM_ID"));
		result = EFalse;
		}
		
	if(!(commonChars->iImplementationUID == aImplementationUid))
		{
		aErrorMessage.Set(_L("COMMON_IMPLEMENTATION_ID"));
		result = EFalse;	
		}	
	
	return result;	
	}
