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
* Description:       Class with methods used in encryption and decryption process.
*
*/







#ifndef C_ENCRYPT_H
#define C_ENCRYPT_H

#include <e32base.h>
#include <f32file.h>

#include <xml/dom/xmlengdocument.h>

class CXmlSecKeyManager;

/**
 *  Class used to encrypt/decrypt data.
 *
 *  How to use:
 *  
 *  Encryption:
 *  <pre>
 *  1. Create new object:
 *          CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();      
 *  2. Set the key that should be used during encryption: 
 *          enc->SetKeyFromFileL(filename, keyname, CXmlSecEncrypt::E3DES);
 *  3. Set template for encryption: 
 *          enc->SetTemplateFromFileL(templatefile);
 *  4. Encrypt data from buffer:
 *          doc = enc->EncryptDataL(data);
 *     data will be added to template or encrypt xml node: 
 *	        enc->EncryptXmlNodeL(doc.DocumentElement()); 
 *     node will be replaced by template with encrypted data
 *  </pre>
 *  
 *  Decryption:
 *  <pre>
 *  1. Create new object:
 *          CXmlSecEncrypt* enc = CXmlSecEncrypt::NewLC();
 *  2. Set the key that should be used during decryption:  
 *          enc->SetKeyFromFileL(keyfile,keyname,CXmlSecEncrypt::EAES256);
 *  3. Decrypt xml node:
 *          enc->DecryptXmlNodeL(doc.DocumentElement()); 
 *   Function will replace encryption template with encrypted data. If result of encryption will
 *   not be a document then result will be returned as HBufC8*.
 *  </pre>
 *   
 *  @note Key should be set before encryption
 *
 *  @lib XmlSecWrapper.dll
 *  @since S60 v3.2
 */
class CXmlSecEncrypt : public CBase
    {
public:
    /**
     * Key types.
     * Currently symmetric key (AES (256 bit) or 3DES)
     */
    enum TXmlSecKeyType
        {
        EAES256 = 0,
        E3DES
        };

    /**
     * Encryption data types
     */
    enum TXmlSecDataType
        {
        ENode = 0,
        ENodeContent,
        EData
        };

public:
    /**
     * Creates new object.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */
    IMPORT_C static CXmlSecEncrypt* NewL();
    
    /**
     * Creates new object and puts it on cleanup stack.
     *
     * @since S60 v3.2
     * @return pointer to new object
     */
    IMPORT_C static CXmlSecEncrypt* NewLC();
    
    /**
     * Destructor.
     *
     * @since S60 v3.2
     */
    virtual ~CXmlSecEncrypt();

    /**
     * Encrypts data from buffer.
     *
     * @since S60 v3.2
     * @param aData data that should be encrypted
     * @return xml document with encrypted data
     */
    IMPORT_C RXmlEngDocument EncryptDataL(const TDesC8& aData);
    
    /**
     * Encrypts xml document.
     *
     * @since S60 v3.2
     * @param aDocument xml document that should be encrypted.
     *
     * @note Encrypted content will replace aDocument content.
     *       aDocument contains the result.
     */
    IMPORT_C void EncryptXmlDocumentL(RXmlEngDocument& aDocument);
                                      
    /**
     * Encrypts xml node.
     *
     * @since S60 v3.2
     * @param aNode xml node that should be encrypted.
     *
     * @note Encrypted content will replace aNode.
     *       aNode contains the result.
     */
    IMPORT_C void EncryptXmlNodeL(TXmlEngElement aNode);
                                  
    /**
     * Encrypts xml node.
     *
     * @since S60 v3.2
     * @param aNode xml node that should be encrypted.
     * @param aTemplate DOM tree with template document
     * @param aKeyFile name of the file with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     *
     * @note Encrypted content will replace aNode.
     *       aNode contains the result.
     */
    IMPORT_C void EncryptXmlNodeKeyFromFileL(TXmlEngElement aNode,
                                             RXmlEngDocument& aTemplate,
                                             const TDesC8& aKeyFile,
                                             const TDesC8& aKeyName,
                                             TXmlSecKeyType aKeyType);
    /**
     * Encrypts xml document.
     *
     * @since S60 v3.2
     * @param aDocument xml document that should be encrypted.
     * @param aTemplate DOM tree with template document
     * @param aKeyFile name of the file with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     *
     * @note Encrypted content will replace aNode.
     *       aNode contains the result.
     */
    IMPORT_C void EncryptXmlDocumentKeyFromFileL(RXmlEngDocument& aDoc,
                                                 RXmlEngDocument& aTemplate,
                                                 const TDesC8& aKeyFile,
                                                 const TDesC8& aKeyName,
                                                 TXmlSecKeyType aKeyType);
                                                 
    /**
     * Encrypts xml node.
     *
     * @since S60 v3.2
     * @param aNode xml node that should be encrypted.
     * @param aTemplate DOM tree with template document
     * @param aKey buffer with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     *
     * @note Encrypted content will replace aNode.
     *       aNode contains the result.
     */
    IMPORT_C void EncryptXmlNodeKeyFromBufferL(TXmlEngElement aNode,
                                               RXmlEngDocument& aTemplate,
                                               const TDesC8& aKeyFile,
                                               const TDesC8& aKeyName,
                                               TXmlSecKeyType aKeyType);
    /**
     * Encrypts xml document.
     *
     * @since S60 v3.2
     * @param aDocument xml document that should be encrypted.
     * @param aTemplate DOM tree with template document
     * @param aKey buffer with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     *
     * @note Encrypted content will replace aNode.
     *       aNode contains the result.
     */
    IMPORT_C void EncryptXmlDocumentKeyFromBufferL(RXmlEngDocument& aDoc,
                                                   RXmlEngDocument& aTemplate,
                                                   const TDesC8& aKey,
                                                   const TDesC8& aKeyName,
                                                   TXmlSecKeyType aKeyType);

    /**
     * Decrypts data contained in a node.
     *
     * @since S60 v3.2
     * @param aNode <EncryptedData> element (see XML Encryption recommendation)
     * @return Decryption result. If xml data will be a result then it will replace aNode content
     *         and null will be returned. If result type will be different then result will be returned
     *         in HBufC8 pointer.
     */
    IMPORT_C HBufC8* DecryptXmlNodeL(TXmlEngElement aNode);
    
    /**
     * Decrypts data contained in xml DOM tree.
     *
     * @since S60 v3.2
     * @param aDocument DOM tree with encrypted data
     * @return Decryption result. If xml data will be a result then it will replace aDocument content
     *         and null will be returned. If result type will be different then result will be returned
     *         in HBufC8 pointer.
     */
    IMPORT_C HBufC8* DecryptXmlDocumentL(RXmlEngDocument& aDocument);
    
    /**
     * Decrypts xml node.
     *
     * @since S60 v3.2
     * @param aNode xml node that should be decrypted.
     * @param aKeyFile name of the file with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     * @return Decryption result. If xml data will be a result then it will replace aDocument content
     *         and null will be returned. If result type will be different then result will be returned
     *         in HBufC8 pointer.
     */
    IMPORT_C HBufC8* DecryptXmlNodeKeyFromFileL(TXmlEngElement aNode,
                                                const TDesC8& aKeyFile,
                                                const TDesC8& aKeyName,
                                                TXmlSecKeyType aKeyType);
    /**
     * Decrypts xml document.
     *
     * @since S60 v3.2
     * @param aDocument xml document that should be decrypted.
     * @param aKeyFile name of the file with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     * @return Decryption result. If xml data will be a result then it will replace aDocument content
     *         and null will be returned. If result type will be different then result will be returned
     *         in HBufC8 pointer.
     */
    IMPORT_C HBufC8* DecryptXmlDocumentKeyFromFileL(RXmlEngDocument& aDocument,
                                                    const TDesC8& aKeyFile,
                                                    const TDesC8& aKeyName,
                                                    TXmlSecKeyType aKeyType);
                                         
    /**
     * Decrypts xml node.
     *
     * @since S60 v3.2
     * @param aNode xml node that should be decrypted.
     * @param aKey buffer with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     * @return Decryption result. If xml data will be a result then it will replace aDocument content
     *         and null will be returned. If result type will be different then result will be returned
     *         in HBufC8 pointer.
     */
    IMPORT_C HBufC8* DecryptXmlNodeKeyFromBufferL(TXmlEngElement aNode,
                                                  const TDesC8& aKey,
                                                  const TDesC8& aKeyName,
                                                  TXmlSecKeyType aKeyType);
    /**
     * Decrypts xml document.
     *
     * @since S60 v3.2
     * @param aDocument xml document that should be decrypted.
     * @param aKey buffer with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     * @return Decryption result. If xml data will be a result then it will repleace aDocument content
     *         and null will be returned. If result type will be different then result will be returned
     *         in HBufC8 pointer.
     */
    IMPORT_C HBufC8* DecryptXmlDocumentKeyFromBufferL(RXmlEngDocument& aDocument,
                                                      const TDesC8& aKey,
                                                      const TDesC8& aKeyName,
                                                      TXmlSecKeyType aKeyType);
                             
    /**
     * Sets encryption template (parses template file).
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate template file
     */
    IMPORT_C void SetTemplateFromFileL(const TDesC8& aTemplate);
    
    /**
     * Sets encryption template (parses template file).
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aRFs handle to a file server session.
     * @param aTemplate template file
     */
    IMPORT_C void SetTemplateFromFileL(RFs& aRFs,
                                       const TDesC8& aTemplate);
    
    /**
     * Sets encryption template (parses buffer that contains template xml).
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate template xml in buffer
     */
    IMPORT_C void SetTemplateFromBufferL(const TDesC8& aTemplate);
    
    /**
     * Set copy of the argument as a encryption template.
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
     * Creates encryption template. Template will be created for specific key algorithm.
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aKeyType Key algorithm
     * @param aDataType Type of encrypted data. It can be xml node or data.
     * @return template document
     */
    IMPORT_C const RXmlEngDocument& CreateTemplateL(TXmlSecKeyType aKeyType,
                                              TXmlSecDataType aDataType = CXmlSecEncrypt::ENode);
    
    /**
     * Return current encryption template.
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
     * Reads key from file. Key will be used during encrypt/decrypt action until 
     * new key will be set.
     *
     * @since S60 v3.2
     * @param aKeyFile name of the file with key inside 
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyFormat key algorithm (i.e. AES)
     */
    IMPORT_C void SetKeyFromFileL(const TDesC8& aKeyFile,
                                  const TDesC8& aKeyName,
                                  TXmlSecKeyType aKeyType);
    
    /**
     * Reads key from buffer. Key will be used during encrypt/decrypt action until 
     * new key will be set.
     *
     * @since S60 v3.2
     * @param aKey buffer with key inside
     * @param aKeyName name of the key. In case of AES and 3DES it can be null.
     * @param aKeyType key algorithm (i.e. AES)
     */
    IMPORT_C void SetKeyFromBufferL(const TDesC8& aKey,
                                    const TDesC8& aKeyName,
                                    TXmlSecKeyType aKeyType);
    
private:
    /**
     * Default constructor.
     *
     * @since S60 v3.2
     */
    CXmlSecEncrypt();
    
    /**
     * Second phase constructor.
     *
     * @since S60 v3.2
     */
    void ConstructL();

private:   
    /**
     * DOM tree with template 
     */
    RXmlEngDocument iTemplate;
    
    /** 
     * Encryption context 
     */
    void* iEncCtx;
    
    /** 
     * Key manager 
     */
    CXmlSecKeyManager* iMngr;
    };
    
#endif // C_ENCRYPT_H
