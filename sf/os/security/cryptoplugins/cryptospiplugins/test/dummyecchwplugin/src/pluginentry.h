/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file
 * @internalComponent
 * @released
 */
#ifndef CPLUGINENTRY_H
#define CPLUGINENTRY_H

#include <cryptospi/cryptoparams.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>
#include "keys.h"
#include <cryptospi/hashplugin.h>
#include <cryptospi/randomplugin.h>
#include "symmetriccipherplugin.h"
#include "asymmetriccipherplugin.h"
#include "signerplugin.h"
#include "verifierplugin.h"
#include "keypairgeneratorplugin.h"
#include "keyagreementplugin.h"

using namespace CryptoSpi;

class CCryptoPluginEntry
    {
public:
    /**
     * Enumerates the set of plug-ins supported by the module for a given interface
     * e.g. all of the hash plug-ins.
     *
     * @param aInterface The UID of the plug-in interface type. If the UID is not recognised
     * then the NULL pointer must be returned.
     * @param aNumPlugins The number of plug-in characteristics objects in the result.
     */
    IMPORT_C static const TCharacteristics** Enumerate(TUid aInterface,
            TInt& aNumPlugins);

    IMPORT_C static void GetExtendedCharacteristicsL(TUid aImplementationUid,
            CExtendedCharacteristics*&);
    
    /**
     * Creates a new instance of an asymmetric cipher
     *
     * @param aPlugin A reference to a pointer that should be set to point to 
     * the new asymmetric cipher object.
     * @param aImplementationId The UID of the asymmetric cipher plug-in to instantiate.
     * @param aKey The encryption/decryption key.
     * @param aPaddingMode The padding mode.
     * @param aAlgorithmParams The parameters that are specific to a particular 
     * algorithm. This is for extendibility and will normally be null.
     * 
     * @leave Function can leave with any system wide error codes in 
     * case of failure.
     */
    IMPORT_C static void CreateAsymmetricCipherL(MAsymmetricCipher*& aPlugin,
            TUid aImplementationId, const CKey& aKey, TUid aCryptoMode,
            TUid aPaddingMode, const CCryptoParams* aAlgorithmParams);

    /**
     * Creates a new instance of an asymmetric signer.
     *
     * @param aPlugin A reference to a pointer that should be set to point to 
     * the new asymmetric signer object.
     * @param aImplementationId The UID of the signer plug-in to instantiate.
     * @param aKey The signing key.
     * @param aPaddingMode The padding mode.
     * @param aAlgorithmParams The parameters that are specific to a particular 
     * algorithm. This is for extendibility and will normally be null.
     * 
     * @leave Function can leave with any system wide error codes in 
     * case of failure.
     */
    IMPORT_C static void CreateAsymmetricSignerL(MSigner*& aPlugin,
            TUid aImplementationId, const CKey& aKey, TUid aPaddingMode,
            const CCryptoParams* aAlgorithmParams);

    /**
     * Creates a new instance of an asymmetric verifier.
     *
     * @param aPlugin A reference to a pointer that should be set to point to 
     * the new asymmetric verifier object.
     * @param aImplementationId The UID of the verifier plug-in to instantiate.
     * @param aKey The key to verify the signature with.
     * @param aAlgorithmParams The parameters that are specific to a particular 
     * algorithm. This is for extendibility and will normally be null.
     *
     * @leave Function can leave with any system wide error codes in 
     * case of failure.
     */
    IMPORT_C static void CreateAsymmetricVerifierL(MVerifier*& aPlugin,
                                                   TUid aImplementationId,
                                                   const CKey& aKey,
                                                   TUid aPaddingMode,
                                                   const CCryptoParams* aAlgorithmParams);

    /**
     * Creates a new instance of a Hash object.
     *
     * @param aPlugin A reference to a pointer that should be set to point to 
     * the new hash object.
     * @param aImplementationId The UID of the hash plug-in to instantiate.
     * @param aAlgorithmParams The parameters that are specific to a particular 
     * algorithm. This is for extendibility and will normally be null.
     *
     * @leave Function can leave with any system wide error codes in 
     * case of failure.
     */
    IMPORT_C static void CreateHashL(MHash*& aPlugin,
                                     TUid aImplementationId,
                                     TUid aOperationMode,
                                     const CKey* aKey,
                                     const CCryptoParams* aAlgorithmParams);

    /**
     * Creates a new instance of a Random object.
     *
     * @param aPlugin A reference to a pointer that should be set to point to the new random object.
     * @param aImplementationId The UID of the random plug-in to instantiate.
     * @param aAlgorithmParams The parameters that are specific to a particular 
     * algorithm. This is for extendibility and will normally be null.
     *
     * @leave Function can leave with any system wide error codes in 
     * case of failure.
     */
    IMPORT_C static void CreateRandomL(MRandom*& aPlugin,
                                       TUid aImplementationId,
                                       const CCryptoParams* aAlgorithmParams);


    /**
     * Creates a new instance of a key agreement system.
     *
     * @param aPlugin A reference to a pointer that should be set to point to 
     * the new asymmetric key pair generator object.
     * @param aImplementationId The UID of the key agreement plug-in to instantiate.
     * @param aPrivateKey The private key to combine with the other parties public key
     * during the agreement.
     * @param aAlgorithmParams The parameters that are specific to a particular 
     * algorithm. This is for extendibility and will normally be null.
     *
     * @leave Function can leave with any system wide error codes in 
     * case of failure.
     */
    IMPORT_C static void CreateKeyAgreementL(MKeyAgreement*& aPlugin,
                                             TUid aImplementationId,
                                             const CKey& aPrivateKey,
                                             const CCryptoParams* aAlgorithmParams);

    /**
     * Creates a new instance of an asymmetric key pair generator.
     *
     * @param aPlugin A reference to a pointer that should be set to point to 
     * the new asymmetric key pair generator object.
     * @param aImplementationId The UID of the verifier plug-in to instantiate.
     * @param aAlgorithmParams The parameters that are specific to a particular 
     * algorithm. This is for extendibility and will normally be null.
     *
     * @leave Function can leave with any system wide error codes in 
     * case of failure.
     */
    IMPORT_C static void CreateKeyPairGeneratorL(MKeyPairGenerator*& aPlugin,
                                                 TUid aImplementationId,
                                                 const CCryptoParams* aAlgorithmParams);
                                        
    /**
     * Creates a new instance of a symmetric cipher
     *
     * @param aPlugin A reference to a pointer that should be set to point to 
     * the new asymmetric object.
     * @param aImplementationId The UID of the symmetric cipher plug-in to instantiate.
     * @param aKey The encryption/decryption key.
     * @param aCryptoMode Encrypt or Decrypt.
     * @param aOperationMode the block cipher mode to use ECB, CBC, CTR etc
     * @param aPadding the padding scheme to use.
     * @param aAlgorithmParams The parameters that are specific to a particular 
     * algorithm. This is for extendibility and will normally be null.
     *
     * @leave Function can leave with any system wide error codes in 
     * case of failure.
     */
    IMPORT_C static void CreateSymmetricCipherL(MSymmetricCipher*& aPlugin,
                                                TUid aImplementationId,
                                                const CKey& aKey,
                                                TUid aCryptoMode,
                                                TUid aOperationMode,
                                                TUid aPadding,
                                                const CCryptoParams* aAlgorithmParams);


    };

#endif // CPLUGINENTRY_H
