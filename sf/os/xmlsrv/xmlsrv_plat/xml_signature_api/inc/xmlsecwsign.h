/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       Class with methods used during signing and verification process.
*
*/







#ifndef C_SIGN_H
#define C_SIGN_H

#include <e32base.h>
#include <f32file.h>

#include <xml/dom/xmlengdocument.h>
    
class CXmlSecKeyManager;
class TXmlEngElement;

/**
 *  Class used to sign data.
 *
 *  How to use:
 *  
 *  Signing with key from file:
 *  <pre>
 *  1. Create new object:
 *          CXmlSecSign* sign = CXmlSecSign::NewLC();
 *  2. Set template for signing: 
 *          sign->SetTemplateFromFileL(doc);
 *  3. Set the key that should be used during signing: 
 *          sign->SetKeyFromFileL(keyfilename,keyname,CXmlSecSign::ERSA_PRIVATE); 
 *  4. Sign xml node:
 *          element = sign->SignXmlDocumentL(doc); 
 *  </pre>
 *
 *  Signing with key from SymbianKeyStore:
 *  <pre>
 *  1. Create new object:
 *          CXmlSecSign* sign = CXmlSecSign::NewLC();
 *  2. Set template for signing: 
 *          sign->SetTemplateFromFileL(doc);
 *  3. Set the name of the key that should be used during signing: 
 *          sign->SetKeyInfoL(keyname);
 *  4. Sign xml node:
 *          element = sign->SignXmlDocumentL(doc,FALSE);
 *	   second parameter sets to FALSE determine using keys from SymbianKeyStore.
 *  </pre>
 *  
 *  Verification with key:
 *  <pre>
 *  1. Create new object:
 *          CXmlSecSign* sign = CXmlSecSign::NewLC();
 *  3. Set the key that should be used during verification (it should be set before verification): 
 *          sign->SetKeyFromFileL(keyfilename, keyname, CXmlSecSign::ERSA_PUBLIC); 
 *  3. Verify the signature 
 *          ver=sign->VerifyXmlNodeL(element);
 *     function will return true if sign is correct
 *  </pre>
 *  
 *  Verification with certificate from root's cert chain:
 *  <pre>
 *  1. Create new object:
 *          CXmlSecSign* sign = CXmlSecSign::NewLC();
 *  2. Add trusted root certificate from file:
 *          sign->AddTrustedCertFromFileL(trustedCert);
 *  3. Verify the signature:
 *          ver=sign->VerifyXmlNodeL(element, CXmlSecSign::ERootCertChain);
 *     Function will return true if sign is correct.
 *  </pre>
 *  
 *  Verification with certificate from SymbianCertStore:
 *  <pre>
 *  1. Create new object:
 *          CXmlSecSign* sign = CXmlSecSign::NewLC();
 *  2. Verify the signature:
 *          ver=sign->VerifyXmlNodeL(element, CXmlSecSign::ECertStore);
 *     Function will return true if sign is correct
 *  </pre>
 *
 *  @lib XmlSecWrapper.dll
 *  @since S60 v3.2
 */
class CXmlSecSign : public CBase
    {
public:
    /** 
     * Key types. 
     * Currently simmetric key (used by HMAC algorithm) and
     * asimmetric key (used by RSA algorithm) is supported.
     * RSAPrivate is for private key (PKCS#8 syntax, DER encoding)
     * RSAPrivate is for public key (DER encoding)
     */ 
     enum TXmlSecKeyType 
         { 
         ERSAPrivate = 0,
         ERSAPublic, 
         EHMAC 
         };

    /** 
     * Verification key repository.
     * A place where is key to use in verification.
     *      EThisObject      -   Key can set by SetKey method
     *      ECertStore       -   Symbian certificate store (@see CUnifiedCertStore)
     *      ERootCertChain   -   Trusted root cert set can be add by AddTrustedCertFrom(File/Buffer)L method
     */ 
     enum TXmlSecVerificationKeyRepository 
         { 
         EThisObject = 0,
         ECertStore, 
         ERootCertChain
         };

    /** Use Enveloped Signature Transform */
    static const TUint KEnvelopedSignature   = 0x01;
    /** Use c14n Transform */
	static const TUint KC14N                 = 0x02;
	/** Use Exclusive c14n Transform */
	static const TUint KExclusiveC14N        = 0x04;

public:
    /**
     * Creates new object.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */
    IMPORT_C static CXmlSecSign* NewL();

    /**
     * Creates new object and puts it on cleanup stack.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */
    IMPORT_C static CXmlSecSign* NewLC();
    
    /**
     * Destructor
     */
    virtual ~CXmlSecSign();

    /**
     * Signs xml document.
     *
     * @since S60 v3.2
     * @param aDocument DOM tree that should be signed.
     * @param aUseCurrentKey Current key (sets by SetKey function) will be used.
     *            If sets to FALSE then key manager will be used.
     * @return TElement with signature node (It is part of the document).
     *
     * @note If signature template was used, result should be added by user to 
     *       correct node in the document. If signature context was in
     *       document previously, no action is needed (signature is in place
     *       where it was before signing)
     */
    IMPORT_C TXmlEngElement SignXmlDocumentL(RXmlEngDocument& aDocument,
                                             TBool aUseCurrentKey = ETrue);
    
    /**
     * Signs xml document.
     *
     * @since S60 v3.2
     * @param aDocument DOM tree that should be signed.
     * @param aTemplate Template document
     * @param aKeyFile File with key (only binary format. 
     *                 In case of RSA key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key
     * @param aKeyType key type (i.e. HMAC)
     * @return TElement with signature node (It is part of the document).
     *
     * @note If signature template was used, result should be added by user to 
     *       correct node in the document. If signature context was in
     *       document previously, no action is needed (signature is in place
     *       where it was before signing)
     */
    IMPORT_C TXmlEngElement SignXmlDocumentKeyFromFileL(RXmlEngDocument& aDocument,
                                                        RXmlEngDocument& aTemplate,
                                                        const TDesC8& aKeyFile,
                                                        const TDesC8& aKeyName,
                                                        TXmlSecKeyType aKeyType);
                                   
    /**
     * Signs xml document.
     *
     * @since S60 v3.2
     * @param aDocument DOM tree that should be signed.
     * @param aKeyFile File with key (only binary format. 
     *                 In case of RSA key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key
     * @param aKeyType key type (i.e. HMAC)
     * @return TElement with signature node (It is part of the document).
     *
     * @note Signature context should be in the document that should be sign
     * @note If signature template was used, result should be added by user to 
     *       correct node in the document. If signature context was in
     *       document previously, no action is needed (signature is in place
     *       where it was before signing)
     */
    IMPORT_C TXmlEngElement SignXmlDocumentKeyFromFileL(RXmlEngDocument& aDocument,
                                                  const TDesC8& aKeyFile,
                                                  const TDesC8& aKeyName,
                                                  TXmlSecKeyType aKeyType);
    
    /**
     * Signs xml document.
     *
     * @since S60 v3.2
     * @param aDocument DOM tree that should be signed.
     * @param aTemplate Template document
     * @param aKey Buffer with key (only binary format. 
     *             In case of RSA key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key
     * @param aKeyType key type (i.e. HMAC)
     * @return TElement with signature node (It is part of the document).
     *
     * @note If signature template was used, result should be added by user to 
     *       correct node in the document. If signature context was in
     *       document previously, no action is needed (signature is in place
     *       where it was before signing)
     */
    IMPORT_C TXmlEngElement SignXmlDocumentKeyFromBufferL(RXmlEngDocument& aDocument,
                                                    RXmlEngDocument& aTemplate,
                                                    const TDesC8& aKey,
                                                    const TDesC8& aKeyName,
                                                    TXmlSecKeyType aKeyType);
                                   
    /**
     * Signs xml document.
     *
     * @since S60 v3.2
     * @param aDocument DOM tree that should be signed.
     * @param aKey Buffer with key (only binary format. 
     *             In case of RSA key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key
     * @param aKeyType key type (i.e. HMAC)
     * @return TElement with signature node (It is part of the document).
     *
     * @note Signature context should be in the document that should be sign
     * @note If signature template was used, result should be added by user to 
     *       correct node in the document. If signature context was in
     *       document previously, no action is needed (signature is in place
     *       where it was before signing)
     */
    IMPORT_C TXmlEngElement SignXmlDocumentKeyFromBufferL(RXmlEngDocument& aDocument,
                                                    const TDesC8& aKey,
                                                    const TDesC8& aKeyName,
                                                    TXmlSecKeyType aKeyType);
                                   
    /**
     * Signs xml nodes.
     *
     * @since S60 v3.2
     * @param aNodes Nodes that should be signed.
     * @param aUseCurrentKey Current key (sets by SetKey method) will be used.
     *            If sets to FALSE then key manager will be used.
     * @return TElement with signature node (It is part of the document).
     *
     * @note If signature template was used, result should be added by user to 
     *       correct node in the document. If signature context was in
     *       document previously, no action is needed (signature is in place
     *       where it was before signing)
     */
    IMPORT_C TXmlEngElement SignXmlNodesL(RArray<TXmlEngElement>& aNodes,
                                    TBool aUseCurrentKey = ETrue);
    
    /**
     * Verifies xml node.
     *
     * @since S60 v3.2
     * @param aNode Node that should be verified (<Signature> element).
     * @param aKeySource place from key to verification will be taken. @see TXmlSecVerificationKeyRepository
     * @return ETrue if correctly verificated.
     */
    IMPORT_C TBool VerifyXmlNodeL(TXmlEngElement aNode,
                                  TXmlSecVerificationKeyRepository aKeyRepository = EThisObject);
    
    /**
     * Verifies xml node.
     *
     * @since S60 v3.2
     * @param aNode Node that should be verified (<Signature> element).
     * @param aKeyFile File with key (only binary format. 
     *                 In case of RSA key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key
     * @param aKeyType key type (i.e. HMAC)
     * @return TRUE if node was correctly verificated.
     */
    IMPORT_C TBool VerifyXmlNodeKeyFromFileL(TXmlEngElement aNode,
                                             const TDesC8& aKeyFile,
                                             const TDesC8& aKeyName,
                                             TXmlSecKeyType aKeyType);
                                      
    /**
     * Verifies xml node.
     *
     * @since S60 v3.2
     * @param aNode Node that should be verified (<Signature> element).
     * @param aKey Buffer with key (only binary format. 
     *             In case of RSA key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key
     * @param aKeyType key type (i.e. HMAC)
     * @return TRUE if node was correctly verificated.
     */
    IMPORT_C TBool VerifyXmlNodeKeyFromBufferL(TXmlEngElement aNode,
                                               const TDesC8& aKey,
                                               const TDesC8& aKeyName,
                                               TXmlSecKeyType aKeyType);
    
    /**
     * Verifies xml document.
     *
     * @since S60 v3.2
     * @param aDocument Document that should be verified.
     * @param aKeySource place from key to verification will be taken. @see TXmlSecVerificationKeyRepository
     * @return TRUE if node was correctly verificated.
     */
    IMPORT_C TBool VerifyXmlDocumentL(const RXmlEngDocument& aDocument, 
                                TXmlSecVerificationKeyRepository aKeyRepository = EThisObject);
    
    /**
     * Verifies xml document.
     *
     * @since S60 v3.2
     * @param aDocument DOM tree that should be verified.
     * @param aKeyFile File with key (only binary format. 
     *                 In case of RSA key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key
     * @param aKeyType key type (i.e. HMAC)
     * @return TRUE if document was correctly verificated.
     */
    IMPORT_C TBool VerifyXmlDocumentKeyFromFileL(const RXmlEngDocument& aDocument,
                                                 const TDesC8& aKeyFile,
                                                 const TDesC8& aKeyName,
                                                 TXmlSecKeyType aKeyType);
                                      
    /**
     * Verifies xml document.
     *
     * @since S60 v3.2
     * @param aDocument DOM tree that should be verified.
     * @param aKey Buffer with key (only binary format. 
     *             In case of RSA key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key
     * @param aKeyType key type (i.e. HMAC)
     * @return TRUE if document was correctly verificated.
     */
    IMPORT_C TBool VerifyXmlDocumentKeyFromBufferL(const RXmlEngDocument& aDocument,
                                                   const TDesC8& aKey,
                                                   const TDesC8& aKeyName,
                                                   TXmlSecKeyType aKeyType);
                             
    /**
     * Sets signing template (parses template file).
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate template file
     */
    IMPORT_C void SetTemplateFromFileL(const TDesC8& aTemplate);
    
    /**
     * Sets signing template (parses template file).
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aRFs handle to a file server session.
     * @param aTemplate template file
     */
    IMPORT_C void SetTemplateFromFileL(RFs& aRFs, const TDesC8& aTemplate);
    
    /**
     * Sets signing template (parses buffer that contains template xml).
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate template xml in buffer
     */
    IMPORT_C void SetTemplateFromBufferL(const TDesC8& aTemplate);
    
    /**
     * Set copy of the argument as a sign template.
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate DOM tree with template document
     */
    IMPORT_C void SetTemplateL(const RXmlEngDocument& aTemplate);
    
    /**
     * Adds <KeyName> element to <KeyInfo> node and sets the value of it.
     * The element will be added to current template.
     *
     * @since S60 v3.2
     * @param aKeyName Key name that should be set.
     */
    IMPORT_C void SetKeyInfoL(const TDesC8& aKeyName);
    
    /**
     * Adds element to <KeyInfo> node. This method allows to add specific data
     * to <KeyInfo> element.
     * The element will be added to current template.
     * 
     * @since S60 v3.2
     * @param aKeyProp Element that should be set.
     */
    IMPORT_C void SetKeyInfoL(TXmlEngElement aKeyProp);
            
    /**
     * Creates template for singning. Template is created for specific key algorithm.
     * This templete will sign whole xml document.
     *
     * This template will be used until new template will be set.
     * Only SHA1 digest method is supported.
     *
     * @since S60 v3.2
     * @param aKeyType Key type determine signatureMethod.
     * @param aCert If sets to TRUE then element <X509Certificate> will be added to template.
     * @param aTransform Decide what transform should be used in reference elements
     *          Use one or more const (e.g. KEnvelopeSignature | KExclusiveC14N)
     * @param aPref prefix that will be used for signature namespace
     * @param aNewLine should new line be used in template
     * @return template document
     */
    IMPORT_C const RXmlEngDocument& CreateTemplateL(TXmlSecKeyType aKeyType,                                        
                                        TBool aCert = EFalse,
                                        TUint aTransforms = KEnvelopedSignature,
                                        const TDesC8& aPref = KNullDesC8(),
                                        TBool aNewLine = TRUE);
    
    /**
     * Creates template for signing. Template is created for specific key algorithm.
     * Templete allows to sign element(s) identified by id.
     *
     * This template will be used until new template will be set.
     * Only SHA1 digest method is supported.
     *
     * @since S60 v3.2
     * @param aKeyType Key type determine signatureMethod.
     * @param aNodes Array with nodes that should be signed
     * @param aId Id name that should be used in Reference (i.e."ns:Id")
     * @param aCert If sets to TRUE then element <X509Certificate> will be added to template.
     * @param aTransform Decide what transform should be used in reference elements
     *          Use one or more const (e.g. KEnvelopeSignature | KExclusiveC14N)
     * @param aPref prefix that will be used for signature namespace
     * @param aNewLine should new line be used in template
     * @return template document
     */
    IMPORT_C const RXmlEngDocument& CreateTemplateL(TXmlSecKeyType aKeyType,
                                        RArray<TXmlEngElement>& aNodes,
                                        const TDesC8& aId,
                                        TBool aCert = EFalse,
                                        TUint aTransforms = KEnvelopedSignature,
                                        const TDesC8& aPref = KNullDesC8(),
                                        TBool aNewLine = TRUE);
        
    /**
     * Gets current template.
     *
     * @since S60 v3.2
     * @return template document
     */
    IMPORT_C const RXmlEngDocument& CurrentTemplate() const;
    
    /**
     * Destroys current template.
     *
     * @since S60 v3.2
     */
    IMPORT_C void DestroyCurrentTemplate();
    
    /**
     * Reads key from file and puts it to the SymbianKeyStore. 
     * Key will be used during signing/verification action until new key will be set.
     *
     * @since S60 v3.2
     * @param aKeyFile File with key (only binary format. 
     *                 In case of RSA private key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key (it can be null descriptor when HMAC key is used)
     * @param aKeyFormat key type (i.e. HMAC)
     */
    IMPORT_C void SetKeyFromFileL(const TDesC8& aKeyFile,
                                  const TDesC8& aKeyName,
                                  TXmlSecKeyType aKeyType);
    
    /**
     * Reads key from buffer and puts it to the SymbianKeyStore. 
     * Key will be used during signing/verification action until new key will be set.
     *
     * @since S60 v3.2
     * @param aKey Buffer with key (only binary format. 
     *             In case of RSA private key type - PKCS#8 syntax, DER encoding)
     * @param aKeyName name of the key (it can be null descriptor when HMAC key is used)
     * @param aKeyFormat key type (i.e. HMAC). 
     */
    IMPORT_C void SetKeyFromBufferL(const TDesC8& aKey,
                                    const TDesC8& aKeyName,
                                    TXmlSecKeyType aKeyType);
    /**
     * Reads certificate from file. 
     * This certificate will be added to current key.
     *
     * @since S60 v3.2
     * @param aCertFile Name of the file with certificate inside 
     */
    IMPORT_C void SetCertFromFileL(const TDesC8& aCertFile);
    
    /**
     * Reads certificate from buffer. 
     * This certificate will be added to current key.
     *
     * @since S60 v3.2
     * @param aCert Buffer with certificate inside 
     */
    IMPORT_C void SetCertFromBufferL(const TDesC8& aCert);

    /**
     * Reads X509 certificate from file and adds it to root's cert chain handled in memory (destroyed after session). 
     * This certificate IS NOT added to SymbianCertStore. 
     *
     * @since S60 v3.2
     * @param aCertFile Name of the file with certificate.
     */
    IMPORT_C void AddTrustedCertFromFileL(const TDesC8& aCertFile);

    /**
     * Reads X509 certificate from buffer and adds it to root's cert chain handled in memory (destroyed after session). 
     * This certificate IS NOT added to SymbianCertStore. 
     *
     * @since S60 v3.2
     * @param aCert Buffer with certificate.
     */
    IMPORT_C void AddTrustedCertFromBufferL(const TDesC8& aCert);

private:
    /**
     * Default constructor.
     *
     * @since S60 v3.2
     */
    CXmlSecSign();
    
    /**
     * Second phase constructor.
     *
     * @since S60 v3.2
     */
    void ConstructL();
    
    /**
     * Signs the xml node.
     *
     * @since S60 v3.2
     * @param aNode TElement Node that should be sign.
     * @param aUseCurrentKey Current key (set by SetKey method) will be used.
     *            If sets to FALSE then key manager will be used.
     * @return TElement with signed node.
     */
    TXmlEngElement CXmlSecSign::SignXmlNodeL(TXmlEngElement aNode, TBool aUseCurrentKey);

private:   
    /** 
     * DOM tree with template 
     */
    RXmlEngDocument iTemplate;
    
    /** 
     * Signing context 
     */
    void* iSigCtx;
    
    /**
     * Key manager 
     */
    CXmlSecKeyManager* iMngr;

    /**
     * Flag to skip searching for Template in document 
     */
    TBool iSkipTmplLookUp;

    };
    
#endif // C_SIGN_H
