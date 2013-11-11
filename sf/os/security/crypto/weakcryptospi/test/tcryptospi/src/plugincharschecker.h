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
#if (!defined __PLUGINCHARSCHECKER_H__)
#define __PLUGINCHARSCHECKER_H__

#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/extendedcharacteristics.h>
#include "te_cryptospiconversion.h"

class CPluginCharsChecker : public CBase
	{
public:

	static CPluginCharsChecker* NewL();
	static CPluginCharsChecker* NewLC();
	
	~CPluginCharsChecker();
	
	TBool checkCommonCharacteristics(const CryptoSpi::TCommonCharacteristics* aCharacteristics,
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
										TPtrC& aErrorMessage);
									
	TBool checkSymmetricCharacteristics(const CryptoSpi::TSymmetricCipherCharacteristics* aCharacteristics,
										TInt aSymmetricMaxKeyLength,
										TInt aSymmetricBlockSize,
										TVariantPtrC aSymmetricPaddingModes,
										TInt aSymmetricPaddingModeNum,
										TVariantPtrC aSymmetricOperationModes,
										TInt aSymmetricOperationModeNum,
										TInt aSymmetricKeySupportMode,
										TPtrC& aErrorMessage);
									
	TBool checkAsymmetricCharacteristics(const CryptoSpi::TAsymmetricCipherCharacteristics* aCharacteristics,
										TInt aAsymmetricMaxKeyLength,
										TVariantPtrC aAsymmetricPaddingModes,
										TInt aAsymmetricPaddingModeNum,
										TInt aAsymmetricKeySupportMode,
										TPtrC& aErrorMessage);
									
	TBool checkHashCharacteristics(const CryptoSpi::THashCharacteristics* aCharacteristics,
										TInt aBlockSize,
										TInt aOutputSize,
										TVariantPtrC aHashOperationModes,
										TInt aHashOperationModeNum,
										TPtrC& aErrorMessage);
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	TBool checkMacCharacteristics(const CryptoSpi::TMacCharacteristics* aCharacteristics,
									TVariantPtrC aMacMode, 
									TPtrC& aErrorMessage);
#endif
									
	TBool checkRandomCharacteristics(const CryptoSpi::TRandomCharacteristics* aCharacteristics,
										TInt aRandomBlockingMode,
										TPtrC& aErrorMessage);
										
	TBool checkAsymSignatureCharacteristics(const CryptoSpi::TAsymmetricSignatureCharacteristics* aCharacteristics,
										TInt aAsymSignMaxKeyLength,
										TVariantPtrC aAsymSignPaddingModes,
										TInt aAsymSignPaddingModeNum,
										TInt aAsymSignKeySupportMode,
										TPtrC& aErrorMessage);
										
	TBool checkAsymKeypairGenCharacteristics(const CryptoSpi::TAsymmetricKeypairGeneratorCharacteristics* aCharacteristics,
										TInt aAsymKeypairGenMaxKeyLength,
										TPtrC& aErrorMessage);
										
	TBool checkSelectedPlugin(const CryptoSpi::TCharacteristics* aCharacteristics,
												TVariantPtrC aAlgorithmUid,
												TVariantPtrC aImplementationUid,
												TPtrC& aErrorMessage);

private:

	CPluginCharsChecker();
	};


#endif
