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
* Description: Class with methods needed in encryption and decryption process.        
*
*/

#include <e32std.h>

// XML Engine
#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_xmlmemory.h>
#include <stdapis/libxml2/libxml2_globals.h>

#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestd.h>
#include <xml/utils/xmlengutils.h>

#include <xml/dom/xmlengdom.h>

// XML Sec
#include "xmlsec_crypto.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_xmltree.h"
#include "xmlsec_xmlenc.h"
#include "xmlsec_templates.h"
#include "xmlsecwinternalutils.h"

#include "xmlsecwerrors.h"
#include "xmlsecwdefs.h"
#include "xmlsecwencrypt.h"
#include "xmlsecwkeymanager.h"
#include "xmlsecwtemplate.h"
#include "xmlsecwglobalstate.h"

const TInt K3DESKeySize = 24;
const TInt KAESKeySize256 = 32;

namespace Encrypt
    {

// ---------------------------------------------------------------------------
// Reset encryption ctxt
// ---------------------------------------------------------------------------
//  
void ResetCtx(xmlSecEncCtxPtr aCtx, CXmlSecKeyManager* aMngr)
    {
    xmlSecKeyPtr tmpKey = NULL;
    if(!aMngr)
        {
        tmpKey = aCtx->encKey;
        aCtx->encKey = NULL;
        }
    // reset ctx
    xmlSecEncCtxReset(aCtx);
    if(aMngr)
        {
        xmlSecEncCtxInitialize(aCtx,aMngr->GetKeyManagerPtr());    
        }    
    
    aCtx->encKey = tmpKey;
    }

// ---------------------------------------------------------------------------
// Read key from buffer
// ---------------------------------------------------------------------------
//  
xmlSecKeyPtr ReadKeyFromBufferL(const TDesC8& aKeyFile,
                                const TDesC8& aKeyName,
                                CXmlSecEncrypt::TXmlSecKeyType aAlgorithm)
    {
    if(aKeyFile.Length() == 0)
        {
        User::Leave(KErrWrongParameter);
        }
    char* name = XmlEngXmlCharFromDes8L(aKeyName); 
    CleanupStack::PushL(name);       
    char* tmpName = NULL;
    if(aKeyName.Length())
        {
        tmpName = name;
        }
    xmlSecKeyPtr keyPtr = NULL;
    // read key and check size
    if (aAlgorithm == CXmlSecEncrypt::EAES256)
        {
        if(aKeyFile.Size() != KAESKeySize256)
            {
            User::Leave(KErrKey);
            }
        keyPtr = xmlSecKeyReadMemory(xmlSecKeyDataAesId, aKeyFile.Ptr(), aKeyFile.Size());
        }	    
	else if (aAlgorithm == CXmlSecEncrypt::E3DES)
	    {
	    if(aKeyFile.Size() != K3DESKeySize)
            {
            User::Leave(KErrKey);
            }
        keyPtr = xmlSecKeyReadMemory(xmlSecKeyDataDesId, aKeyFile.Ptr(), aKeyFile.Size());
	    }        
    XmlEngOOMTestL();
    XmlSecErrorFlagTestL();

    if(!keyPtr) 
        {
        User::Leave(KErrKey);
        }
    // set key name
    if(xmlSecKeySetName(keyPtr, (const unsigned char*) tmpName) < 0) 
        {
        xmlSecKeyDestroy(keyPtr);
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrKey);
        }
    CleanupStack::PopAndDestroy(name);
    return keyPtr;
    }

// ---------------------------------------------------------------------------
// Read key from file
// ---------------------------------------------------------------------------
//  
xmlSecKeyPtr ReadKeyFromFileL(const TDesC8& aKeyFile,
                              const TDesC8& aKeyName,
                              CXmlSecEncrypt::TXmlSecKeyType aKeyFormat)
    {
    if(aKeyFile.Length() == 0)
        {
        User::Leave(KErrWrongParameter);
        }
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);    
    RFile keyFile;
    TInt size;
    RBuf keyName;
    // read key from file
    keyName.CreateL(KMaxFileName);
    CleanupClosePushL(keyName);
    keyName.Copy(aKeyFile);
    User::LeaveIfError(keyFile.Open(fs, keyName, EFileRead | EFileShareReadersOnly));
    CleanupStack::PopAndDestroy(&keyName);
    CleanupClosePushL(keyFile);
    User::LeaveIfError(keyFile.Size(size));
    HBufC8* key = HBufC8::NewLC(size);
    TPtr8 keyPtrD = key->Des();
    User::LeaveIfError(keyFile.Read(keyPtrD, size)); 
    xmlSecKeyPtr keyPtr = ReadKeyFromBufferL(*key,aKeyName,aKeyFormat);
    CleanupStack::PopAndDestroy(key);
    CleanupStack::PopAndDestroy(&keyFile);
    CleanupStack::PopAndDestroy(&fs);
    return keyPtr;
    }
  
    } // namespace Encrypt

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//  
EXPORT_C CXmlSecEncrypt* CXmlSecEncrypt::NewLC()
    {
    CXmlSecEncrypt* self = new (ELeave) CXmlSecEncrypt;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//   
EXPORT_C CXmlSecEncrypt* CXmlSecEncrypt::NewL()
    {
    CXmlSecEncrypt* self = CXmlSecEncrypt::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Encrypt data from buffer
// ---------------------------------------------------------------------------
// 
EXPORT_C RXmlEngDocument CXmlSecEncrypt::EncryptDataL(const TDesC8& aData)
    {
    if(!aData.Length())
        {
        User::Leave(KErrWrongParameter);
        }
    if(iTemplate.IsNull())
        {
        User::Leave(KErrTemplate);
        }
    xmlSecEncCtxPtr tmpCtx = ENC_CTX;
    
    Encrypt::ResetCtx(tmpCtx,NULL);    
           
    xmlNodePtr node = NULL;
    // Copy tmpl file
    xmlDocPtr result = xmlCopyDoc(INTERNAL_DOCPTR(iTemplate), 1);
    if(!result) 
        {
        User::Leave(KErrNoMemory);
        }
    // find start node
    node = xmlSecFindNode(xmlDocGetRootElement(result), xmlSecNodeEncryptedData, xmlSecEncNs);
    if(!node) 
        {
        xmlFreeDoc(result);
        User::Leave(KErrTemplate);
        }

    // encrypt the data
    if(xmlSecEncCtxBinaryEncrypt(tmpCtx, node, aData.Ptr(), aData.Length()) < 0) 
        {
        xmlFreeDoc(result);
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrEncrypt);
        }
    RXmlEngDocument doc;
    XmlSecGlobalState* gs = XmlSecGetTls();
    doc.OpenL(*gs->iDOMImpl,result);
    return doc;
	}

// ---------------------------------------------------------------------------
// Encrypt xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::EncryptXmlDocumentL(RXmlEngDocument& aDocument)
    {
    if(aDocument.IsNull())
        {
        User::Leave(KErrWrongParameter);
        }
    EncryptXmlNodeL(aDocument.DocumentElement());
    }
   
// ---------------------------------------------------------------------------
// Encrypt xml node
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::EncryptXmlNodeL(TXmlEngElement aNode)
    {
    if(aNode.IsNull())
        {
        User::Leave(KErrWrongParameter);
        }
    if(iTemplate.IsNull())
        {
        User::Leave(KErrTemplate);
        }
    xmlSecEncCtxPtr tmpCtx = ENC_CTX;
    
    Encrypt::ResetCtx(tmpCtx,NULL);    
    
    xmlNodePtr node = NULL;
    // find start node
    node = xmlSecFindNode(xmlDocGetRootElement(INTERNAL_DOCPTR(iTemplate)),
                                xmlSecNodeEncryptedData, xmlSecEncNs);
    if(!node) 
        {
        User::Leave(KErrTemplate);
        }
    
    xmlNodePtr result = xmlCopyNode(node, 1);
    if(!result) 
        {
        XmlEngOOMTestL();
        User::Leave(KErrNoMemory);
        }
    // encrypt the data
    if(xmlSecEncCtxXmlEncrypt(tmpCtx, result, INTERNAL_NODEPTR(aNode)) < 0) 
        {
        xmlFreeNode( result );
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrEncrypt);
        }
    }

// ---------------------------------------------------------------------------
// Encrypt xml node
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::EncryptXmlNodeKeyFromFileL(TXmlEngElement aNode,
                                                   RXmlEngDocument& aTemplate,
                                                   const TDesC8& aKeyFile,
                                                   const TDesC8& aKeyName,
                                                   TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromFileL(aKeyFile,aKeyName,aKeyFormat);
    SetTemplateL(aTemplate);  
    EncryptXmlNodeL(aNode);
    }
    
// ---------------------------------------------------------------------------
// Encrypt xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::EncryptXmlDocumentKeyFromFileL(RXmlEngDocument& aDoc,
                                                       RXmlEngDocument& aTemplate,
                                                       const TDesC8& aKeyFile,
                                                       const TDesC8& aKeyName,
                                                       TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromFileL(aKeyFile,aKeyName,aKeyFormat);
    SetTemplateL(aTemplate);  
    EncryptXmlDocumentL(aDoc);
    }

// ---------------------------------------------------------------------------
// Encrypt xml node
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::EncryptXmlNodeKeyFromBufferL(TXmlEngElement aNode,
                                                     RXmlEngDocument& aTemplate,
                                                     const TDesC8& aKey,
                                                     const TDesC8& aKeyName,
                                                     TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromBufferL(aKey,aKeyName,aKeyFormat);
    SetTemplateL(aTemplate);  
    EncryptXmlNodeL(aNode);
    }
    
// ---------------------------------------------------------------------------
// Encrypt xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::EncryptXmlDocumentKeyFromBufferL(RXmlEngDocument& aDoc,
                                                         RXmlEngDocument& aTemplate,
                                                         const TDesC8& aKey,
                                                         const TDesC8& aKeyName,
                                                         TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromBufferL(aKey,aKeyName,aKeyFormat);
    SetTemplateL(aTemplate);  
    EncryptXmlDocumentL(aDoc);
    }

// ---------------------------------------------------------------------------
// Decrypt xml node
// ---------------------------------------------------------------------------
// 
EXPORT_C HBufC8* CXmlSecEncrypt::DecryptXmlNodeKeyFromFileL(TXmlEngElement aNode,
                                                      const TDesC8& aKeyFile,
                                                      const TDesC8& aKeyName,
                                                      TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromFileL(aKeyFile,aKeyName,aKeyFormat);
    return DecryptXmlNodeL(aNode);
    }
    
// ---------------------------------------------------------------------------
// Decrypt xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C HBufC8* CXmlSecEncrypt::DecryptXmlDocumentKeyFromFileL(RXmlEngDocument& aDoc,
                                                          const TDesC8& aKeyFile,
                                                          const TDesC8& aKeyName,
                                                          TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromFileL(aKeyFile,aKeyName,aKeyFormat);
    return DecryptXmlDocumentL(aDoc);
    }

// ---------------------------------------------------------------------------
// Decrypt xml node
// ---------------------------------------------------------------------------
// 
EXPORT_C HBufC8* CXmlSecEncrypt::DecryptXmlNodeKeyFromBufferL(TXmlEngElement aNode,
                                                        const TDesC8& aKey,
                                                        const TDesC8& aKeyName,
                                                        TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromBufferL(aKey,aKeyName,aKeyFormat);
    return DecryptXmlNodeL(aNode);
    }
    
// ---------------------------------------------------------------------------
// Decrypt xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C HBufC8* CXmlSecEncrypt::DecryptXmlDocumentKeyFromBufferL(RXmlEngDocument& aDoc,
                                                            const TDesC8& aKey,
                                                            const TDesC8& aKeyName,
                                                            TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromBufferL(aKey,aKeyName,aKeyFormat);
    return DecryptXmlDocumentL(aDoc);
    }

// ---------------------------------------------------------------------------
// Decrypt xml node
// ---------------------------------------------------------------------------
//    
EXPORT_C HBufC8* CXmlSecEncrypt::DecryptXmlNodeL(TXmlEngElement aNode)
    {
    if(aNode.IsNull())
        {
        User::Leave(KErrWrongParameter);
        }
    
    xmlNodePtr node = NULL;
    xmlNodePtr root = INTERNAL_NODEPTR(aNode);
    // find start node
    node = xmlSecFindNode(root,xmlSecNodeEncryptedData, xmlSecEncNs);
    if(!node) 
        {
        User::Leave(KErrWrongParameter);    
        }
    
    xmlSecEncCtxPtr tmpCtx = ENC_CTX;
    
    Encrypt::ResetCtx(tmpCtx,NULL);    
    // decrypt data
    if((xmlSecEncCtxDecrypt(tmpCtx, node) < 0) 
            || (!tmpCtx->result))
        {
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrDecrypt);
        }
    // if result is data not xml part it is returned as hbufc8*
    if(!tmpCtx->resultReplaced)
        {
        xmlSecByte* result = xmlSecBufferGetData(tmpCtx->result);
        if(result)
            {
            TPtrC8 ptr(result,tmpCtx->result->size);
            return ptr.AllocL();
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// Decrypt xml document
// ---------------------------------------------------------------------------
//     
EXPORT_C HBufC8* CXmlSecEncrypt::DecryptXmlDocumentL(RXmlEngDocument& aDocument)
    {
    if(aDocument.IsNull())
        {
        User::Leave(KErrWrongParameter);
        }
    return DecryptXmlNodeL(aDocument.DocumentElement());
    }

// ---------------------------------------------------------------------------
// Set template from file
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::SetTemplateFromFileL(const TDesC8& aTemplate)
    {
    XmlSecTemplate::SetTemplateFromFileL(iTemplate,aTemplate);
    }
    
// ---------------------------------------------------------------------------
// Set template from file
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::SetTemplateFromFileL(RFs& aRFs, const TDesC8& aTemplate)
    {
    XmlSecTemplate::SetTemplateFromFileL(iTemplate,aTemplate,aRFs);
    }

// ---------------------------------------------------------------------------
// Set template from buffer
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::SetTemplateFromBufferL(const TDesC8& aTemplate)
    {
    XmlSecTemplate::SetTemplateFromBufferL(iTemplate,aTemplate);
    }
 
// ---------------------------------------------------------------------------
// Set template from DOM tree
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::SetTemplateL(const RXmlEngDocument& aTemplate)
    {
    XmlSecTemplate::SetTemplateL(iTemplate,aTemplate);
    }

// ---------------------------------------------------------------------------
// Set key info node in template
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::SetKeyInfoL(const TDesC8& aKeyName)
    {
    XmlSecTemplate::SetKeyInfoL(iTemplate,aKeyName);    
    }

// ---------------------------------------------------------------------------
// Set key info node in template
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::SetKeyInfoL(TXmlEngElement aKeyProp)
    {
    XmlSecTemplate::SetKeyInfoL(iTemplate,aKeyProp);
    }
    
// ---------------------------------------------------------------------------
// Create encryption template
// ---------------------------------------------------------------------------
// 
EXPORT_C const RXmlEngDocument& CXmlSecEncrypt::CreateTemplateL(TXmlSecKeyType aAlgorithm, 
                                                                TXmlSecDataType aDataType)
    {
    if(iTemplate.NotNull())
        {
        iTemplate.Close();
        }
    xmlNodePtr node = NULL;
    // create encryption template to encrypt XML file and replace 
    // its content with encryption result
    const xmlChar* mimeType = NULL;
    if(aDataType == CXmlSecEncrypt::ENode)
        {
        mimeType = xmlSecTypeEncElement;
        }
    else if (aDataType == CXmlSecEncrypt::ENodeContent)
        {
        mimeType = xmlSecTypeEncContent;
        }
    
    if (aAlgorithm == EAES256)
        {
        node = xmlSecTmplEncDataCreate(NULL, xmlSecTransformAes256CbcId,
				NULL, mimeType, NULL, NULL);
        }
	else if (aAlgorithm == E3DES)
	    {
	    node = xmlSecTmplEncDataCreate(NULL, xmlSecTransformDes3CbcId,
			    NULL, mimeType, NULL, NULL);
	    }		
    if(!node) 
        {
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
	    User::Leave(KErrTemplate);
	    }

    // we want to put encrypted data in the <enc:CipherValue/> node
    if(!xmlSecTmplEncDataEnsureCipherValue(node)) 
        {
        xmlFreeNode( node );
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrTemplate);
	    }

    // add <dsig:KeyInfo/> and <dsig:KeyName/> nodes to put key name in the signed document
    xmlNodePtr keyInfo = xmlSecTmplEncDataEnsureKeyInfo(node, NULL);
    if(!keyInfo) 
        {
        xmlFreeNode( node );
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrTemplate);
        }
    
    XmlSecGlobalState* gs = XmlSecGetTls();
    CleanupStack::PushL(TCleanupItem(LibxmlNodeCleanup,(TAny*)node)); 
    iTemplate.OpenL(*gs->iDOMImpl,TXmlEngElement(node));
    CleanupStack::Pop(node);
    return iTemplate;
    }

// ---------------------------------------------------------------------------
// Return current encryption template
// ---------------------------------------------------------------------------
// 
EXPORT_C const RXmlEngDocument& CXmlSecEncrypt::CurrentTemplate() const
    {
    return iTemplate;
    }

// ---------------------------------------------------------------------------
// Destroy current encryption template
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::DestroyCurrentTemplate()
    {
    XmlSecTemplate::DestroyTemplate(iTemplate);
    }

// ---------------------------------------------------------------------------
// Set key from file
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::SetKeyFromFileL(const TDesC8& aKeyFile,
                                        const TDesC8& aKeyName,
                                        TXmlSecKeyType aAlgorithm)
    {
    xmlSecEncCtxPtr tmp = ENC_CTX;
    if(tmp->encKey)
        {
        xmlSecKeyDestroy(tmp->encKey);
        tmp->encKey = NULL;
        }
    tmp->encKey = Encrypt::ReadKeyFromFileL(aKeyFile,aKeyName,aAlgorithm);
    }

// ---------------------------------------------------------------------------
// Set key from buffer
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecEncrypt::SetKeyFromBufferL(const TDesC8& aKeyFile,
                                          const TDesC8& aKeyName,
                                          TXmlSecKeyType aAlgorithm)
    {
    xmlSecEncCtxPtr tmp = ENC_CTX;
    if(tmp->encKey)
        {
        xmlSecKeyDestroy(tmp->encKey);
        tmp->encKey = NULL;
        }
    tmp->encKey = Encrypt::ReadKeyFromBufferL(aKeyFile,aKeyName,aAlgorithm);
    }
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
CXmlSecEncrypt::CXmlSecEncrypt()
    {
    iTemplate = RXmlEngDocument();
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
// 
void CXmlSecEncrypt::ConstructL()
    {
    // create encryption context
    iEncCtx = xmlSecEncCtxCreate(NULL);
    if(!iEncCtx)
        {
        User::Leave(KErrNoMemory);
	    }
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// 
CXmlSecEncrypt::~CXmlSecEncrypt()
    {
    if(iTemplate.NotNull())
        {
        iTemplate.Close();
        }
    if(iEncCtx)
        {
        xmlSecEncCtxDestroy(ENC_CTX);
        }
    }
