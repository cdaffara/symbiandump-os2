/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: An interface to the Symbian Unified Key Store       
*
*/

#ifndef __XMLSECCERTMAN_SYMBIANKEYSTORE_H_
#define __XMLSECCERTMAN_SYMBIANKEYSTORE_H__

#include <e32base.h>
#include <unifiedkeystore.h>
#include <e32std.h>
#include <signed.h>

class CSymbianKeyStore : public CActive
    {
public:        
    
    /**
     * Create new object.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */
    IMPORT_C static CSymbianKeyStore* NewL();
    
    /**
     * Create new object and put it on clean stack.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */
    IMPORT_C static CSymbianKeyStore* NewLC();
        
    /**
     * Destructor.
     */
    virtual ~CSymbianKeyStore();
        
    /**
     * Create Unified Key Store
     *
     * @since  S60 v3.2
     */
    IMPORT_C void CreateUnifiedKeyStoreL();

    /**
     * Find a key in the Unified Key Store
     *
     * @since S60 v3.2 
     * @param aLabel The name of the key to be found
     * @param aAlgo The algorithm of the key to be found
     */
    IMPORT_C void FindKey(const TDesC8 &aLabel, CKeyInfoBase::EKeyAlgorithm aAlgo);
    
    /**
     * Check if a key is found in the Unified Key Store
     *
     * @since  S60 v3.2
     * @return KErrNone if the key is found
     * @return KErrNotFound if the key is not found
     */    
    IMPORT_C TInt hasKey();
        
    /**
     * Get the size of the key stored
     *
     * @since  S60 v3.2
     * @return TUint Size of the key
     */            
    IMPORT_C TUint GetKeySize();
    
    /**
     * Get the algorithm of the key stored
     *
     * @since  S60 v3.2
     * @return CCTKeyInfo::EKeyAlgorithm
     */                
    IMPORT_C CCTKeyInfo::EKeyAlgorithm GetKeyAlgorithm();

    /**
     * Create RSA Key
     *
     * @since  S60 v3.2
     * @param aSize Bit size of the key
     * @param aKeyName Name of the key 
     */
    IMPORT_C void CreateRSAKey(TUint aSize, const TDesC8 &aKeyName);
                
    /**
     * Import key
     *
     * @since  S60 v3.2
     * @param aKeyData The key data to import, ASN.1 DER encoded PKCS#8
     * @aKeyName Name of the key 
     */
    IMPORT_C void ImportKey(const TDesC8 &aKeyData, const TDesC8 &aKeyName);

    /**
     * Opens RSA key for signing the data 
     *
     * @since  S60 v3.2
     * @param aDataToSign The data to be signed
     * @param aLen Length of the data
     */       
    IMPORT_C void RSASignL(const TUint8* aDataToSign, TUint aLen);
        
    /**
     * Verify a RSA signed data 
     *
     * @since  S60 v3.2
     * @param aDataToVerify The signed data to be verified
     * @param aDataLen Length of the signed data
     * @param aSig A reference to the signature that signed the data
     * @param aSigLen Length of the signature
     */       
    IMPORT_C void RSAVerifyL(const TUint8* aDataToVerify, 
                                                            TUint aDataLen, 
                                                            const TUint8* aSig, 
                                                            TUint aSigLen);
                
    /**
     * Verify an RSA signed data with a public key passed from a certificate
     *
     * @since  S60 v3.2
     * @param aDataToVerify The signed data to be verified
     * @param aDataLen Length of the signed data
     * @param aSig A reference to the signature that signed the data
     * @param aSigLen Length of the signature
     * @param aSubPubKeyInfo    A handle to the public key passed from a certificate      
     * @return ETrue The verification is succeeded
     * @return EFalse The verification is failed
     */        
    IMPORT_C TBool RSAVerifyWithPublicKeyL(const TUint8* aDataToVerify, 
                                                        TUint aDataLen, 
                                                        const TUint8* aSig, 
                                                        TUint aSigLen, 
                                                        CSubjectPublicKeyInfo *aSubPubKeyInfo);

    /**
     * Gets Signed data
     *
     * @since  S60 v3.2
     * @param aLen sets length of signed data
     */         
    IMPORT_C const TUint8* GetSignedData(TUint* aLen);
    
    /**
     * Gets verification result
     *
     * @since  S60 v3.2
     * @return ETrue The verification was succeeded
     * @return EFalse The cerification was failed
     */         
    IMPORT_C TBool GetVerifyResult();
    
    /**
     * Get the error flag
     *
     * @since S60 v3.2
     * @return error code
     */         
    IMPORT_C TInt CSymbianKeyStore::GetError();
    
    /**
     * Perform RSA sign operation
     *
     * @since S60 v3.2
     */         
    void PerformRSASignOperation();
    
    /**
     * Perform RSA verify operation
     *
     * @since S60 v3.2
     */          
    void PerformRSAVerifyOperationL();
    
    /**
     * Export public key if none present
     *
     * @since S60 v3.2
     */   
    void ExportRSAPublicKeyL();
    
protected:
    /**
     * From CActive Callback function, invoked to handle responses from the server
     */
    void RunL();    
    
    /**
     * This function is called as part of the active object's Cancel().
     */  
    void DoCancel();
    
    /**
     * Handles Leaves from RunL function.
     */
    TInt RunError(TInt aError);

private:
    /**
     * C++ default constructor.
     */ 
    CSymbianKeyStore();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * To find matched key in the phase EFindingKeys
     */        
    void FindMatchedKey();
    
    /**
     * Sets iSignature buffer
     */          
    void GetRSASignatureL();
        
    /**
     * Release all resources kept in iKeys array (exept for iKey
     * which is released separetly) and empty iKeys array
     */
    void ResetAndDestroyKeysArray();

private:
    /** 
     * State of active object. 
     * EUnitialized standard state
     * EInitializingKeystore during initialize keystore
     * EFindingKeys during finding keys
     * ECreateKey during creating key
     * EImportKey during importing key
     * EOpenRsaKeyForSigning during preparing key for signing
     * EPerformRSASignOperation during signing  
     * EExportPublic during exporting public key
     */ 	
    enum TState
        {
        EUnitialized,
        EInitializingKeystore,    
        EFindingKeys,
        ECreateKey,
        EImportKey,
        EOpenRSAKeyForSigning,
        EPerformRSASignOperation,
        EExportPublic
        };

private:
    /**
     * A reference to the CUnifiedKeyStore class
     */
    CUnifiedKeyStore *iKeyStore;

    /** 
     * Store the generated key
     */
    CCTKeyInfo *iKey;

    /**
     * Store the RSA Signer
     */
    MRSASigner *iRSASigner;
    
    /** 
     * Store the data to be signed
     */
    HBufC8* iDataToSign;
    
    /** 
     * Store the data to be verified
     */
    HBufC8* iDataToVerify;    

    /** 
     * Contain the result of the RSA Signer
     */
    CRSASignature* iRSASignature; 
    
    /** 
     * Store the signed signature
     */
    HBufC8* iSignature;

    /**
     * Contain the exported public key
     */
    HBufC8* iPublicKeyData;

    /**
     * An internal state
     */
    TState iState;

    /**
     * To store the key label to find
     */ 
    HBufC *iKeyLabelToFind;
    
    /**
     * Store the result of signature verification
     */
    TBool iVerifyResult;
    
    /**
     * To check if it was out of memory during previous verify operation
     */
    TBool iOutOfMemoryFlag;
             
    /**
     * Store the keys found
     */
    RMPointerArray< CCTKeyInfo > iKeys;         
    
    /**
     * A reference to the File Server Client
     */
    RFs iFs;
    
    /**
     * Error flag
     */
    TInt iError;
    };

#endif      // __XMLSECCERTMAN_SYMBIANKEYSTORE_H__  
