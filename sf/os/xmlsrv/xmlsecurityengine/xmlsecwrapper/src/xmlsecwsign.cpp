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
* Description: Class with methods used in sign and verification process.       
*
*/

// XML Engine
#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_xmlmemory.h>
#include <stdapis/libxml2/libxml2_parser.h>
#include <stdapis/libxml2/libxml2_globals.h>

#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestd.h>
#include <xml/utils/xmlengutils.h>

#include <xml/utils/xmlengxestrings.h>

#include <xml/dom/xmlengdom.h>

// XML Sec
#include "xmlsec_crypto.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_xmltree.h"
#include "xmlsec_xmldsig.h"
#include "xmlsec_templates.h"
#include "xmlsecc_x509.h"

#include "xmlsecwsign.h"
#include "xmlsecwkeymanager.h"
#include "xmlsecwerrors.h"
#include "xmlsecwdefs.h"
#include "xmlsecwtemplate.h"
#include "xmlsecwinternalutils.h"
#include "xmlsecwglobalstate.h"

namespace Sign
    {
// ---------------------------------------------------------------------------
// Reset sign ctx
// ---------------------------------------------------------------------------
// 
xmlSecDSigCtxPtr ResetCtxL(xmlSecDSigCtxPtr aCtx, CXmlSecKeyManager* aMngr)
    {
    xmlSecKeyPtr tmpKey = NULL;
    if(!aMngr)
        {
        tmpKey = aCtx->signKey;
        aCtx->signKey = NULL;
        }
    // destroy old ctx
    xmlSecDSigCtxDestroy(aCtx);
    // create new ctx
    if(aMngr)
        {
        aCtx = xmlSecDSigCtxCreate(aMngr->GetKeyManagerPtr());
        }
    else
        {
        aCtx = xmlSecDSigCtxCreate(NULL);
        }
    // add key if needed
    if ( !aCtx  ) 
        {
        if ( tmpKey ) 
            {
	        xmlSecKeyDestroy( tmpKey );
            }
        User::Leave( KErrNoMemory );
        }
    
    aCtx->signKey = tmpKey;
    tmpKey = NULL;
    return aCtx;
    }
        
// ---------------------------------------------------------------------------
// Read key from file
// ---------------------------------------------------------------------------
//  
xmlSecKeyPtr ReadKeyFromFileL(const TDesC8& aKeyFile,
                              const TDesC8& aKeyName,
                              CXmlSecSign::TXmlSecKeyType aKeyFormat)
    {
    if(aKeyFile.Length() == 0)
        {
        User::Leave(KErrWrongParameter);
        }
    
    xmlSecKeyPtr keyPtr = NULL;
    char* file = XmlEngXmlCharFromDes8L(aKeyFile);
    CleanupStack::PushL(file);
    char* name = XmlEngXmlCharFromDes8L(aKeyName); 
    CleanupStack::PushL(name);
    // load key - used method depend key type
    if (aKeyFormat == CXmlSecSign::ERSAPrivate)
        {
        if(aKeyName.Length() == 0)
            {
            User::Leave(KErrWrongParameter);
            }
        keyPtr = xmlSecCryptoAppKeyLoadWithName(file, xmlSecKeyDataFormatPkcs8Der,
                                    name, NULL, NULL, NULL);
        }
    else if (aKeyFormat == CXmlSecSign::ERSAPublic)
        {
        if(aKeyName.Length() == 0)
            {
            User::Leave(KErrWrongParameter);
            }
        keyPtr = xmlSecCryptoAppKeyLoadWithName(file, xmlSecKeyDataFormatDer,
                                    name, NULL, NULL, NULL);
        }
    else if (aKeyFormat == CXmlSecSign::EHMAC)
        keyPtr = xmlSecKeyReadBinaryFile(xmlSecKeyDataHmacId,file);
	
	if(!keyPtr) 
        {
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrKey);
        }
    const xmlChar* KTmpName = NULL;
    if(aKeyName.Length())
        {
        KTmpName = (xmlChar*) name;
        }
    // add key name to key
    if((aKeyFormat == CXmlSecSign::EHMAC) && (xmlSecKeySetName(keyPtr,KTmpName) < 0))
        {
        xmlSecKeyDestroy(keyPtr);
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrKey);
        }
        
    CleanupStack::PopAndDestroy(name);
    CleanupStack::PopAndDestroy(file);    
    return keyPtr;
    }

// ---------------------------------------------------------------------------
// Read key from buffer
// ---------------------------------------------------------------------------
// 
xmlSecKeyPtr ReadKeyFromBufferL(const TDesC8& aBuffer,
                                const TDesC8& aKeyName,
                                CXmlSecSign::TXmlSecKeyType aKeyFormat)
    {
    if(aBuffer.Length() == 0)
        {
        User::Leave(KErrWrongParameter);
        }
    char* name = XmlEngXmlCharFromDes8L(aKeyName); 
    CleanupStack::PushL(name);
    xmlSecKeyPtr keyPtr = NULL;
    // load key - used method depend key type
    if (aKeyFormat == CXmlSecSign::ERSAPrivate)
        {
        if(aKeyName.Length() == 0)
            {
            User::Leave(KErrWrongParameter);
            }
        keyPtr = xmlSecCryptoAppKeyLoadMemoryWithName(aBuffer.Ptr(), aBuffer.Size(),
                                    xmlSecKeyDataFormatPkcs8Der, name, NULL, NULL, NULL);
        }
    else if (aKeyFormat == CXmlSecSign::ERSAPublic)
        {
        if(aKeyName.Length() == 0)
            {
            User::Leave(KErrWrongParameter);
            }
        keyPtr = xmlSecCryptoAppKeyLoadMemoryWithName(aBuffer.Ptr(), aBuffer.Size(),
                                    xmlSecKeyDataFormatDer, name, NULL, NULL, NULL);
        }
    else if (aKeyFormat == CXmlSecSign::EHMAC)
        keyPtr = xmlSecKeyReadMemory(xmlSecKeyDataHmacId,aBuffer.Ptr(), aBuffer.Size());
    
    XmlEngOOMTestL();
    XmlSecErrorFlagTestL();
    if(!keyPtr) 
        {
        User::Leave(KErrKey);
        }
    
    const xmlChar* KTmpName = NULL;
    if(aKeyName.Length())
        {
        KTmpName = (xmlChar*) name;
        }
    // add key name to key
    if((aKeyFormat == CXmlSecSign::EHMAC) && (xmlSecKeySetName(keyPtr,KTmpName) < 0))
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
// Reset template settings
// ---------------------------------------------------------------------------
//   
void TemplateCleanupL(TAny* aPref)
    {
   
    if(aPref)
        {
        CleanupStack::PushL(aPref);
        CleanupStack::PopAndDestroy(aPref);
        xmlSetPrefix(NULL);
        }
    xmlSetNewLineFlag(1);
    }
    
    }// namespace Sign

// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
//   
EXPORT_C CXmlSecSign* CXmlSecSign::NewLC()
    {
    CXmlSecSign* self = new (ELeave) CXmlSecSign;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// Two phase constructor
// ---------------------------------------------------------------------------
// 
EXPORT_C CXmlSecSign* CXmlSecSign::NewL()
    {
    CXmlSecSign* self = CXmlSecSign::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Sign the xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C TXmlEngElement CXmlSecSign::SignXmlDocumentL(RXmlEngDocument& aDocument, 
                                                      TBool aUseCurrentKey)
    {
    if(aDocument.IsNull())
        {
        User::Leave(KErrWrongParameter);
        }
    return SignXmlNodeL(aDocument.DocumentElement(),aUseCurrentKey);
    }

// ---------------------------------------------------------------------------
// Sign the xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C TXmlEngElement CXmlSecSign::SignXmlDocumentKeyFromFileL(RXmlEngDocument& aDocument,
                                                     RXmlEngDocument& aTemplate,
                                                     const TDesC8& aKeyFile,
                                                     const TDesC8& aKeyName,
                                                     TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromFileL(aKeyFile,aKeyName,aKeyFormat);
		if(EFalse == iTemplate.IsSameNode(aTemplate))   
		{
			SetTemplateL(aTemplate);  
		}
		iSkipTmplLookUp = ETrue; //Set to TRUE indicating to skip searching for Template in document
    return SignXmlDocumentL(aDocument);
    }

// ---------------------------------------------------------------------------
// Sign the xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C TXmlEngElement CXmlSecSign::SignXmlDocumentKeyFromFileL(RXmlEngDocument& aDocument,
                                                     const TDesC8& aKeyFile,
                                                     const TDesC8& aKeyName,
                                                     TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromFileL(aKeyFile,aKeyName,aKeyFormat);
    return SignXmlDocumentL(aDocument);
    }

// ---------------------------------------------------------------------------
// Sign the xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C TXmlEngElement CXmlSecSign::SignXmlDocumentKeyFromBufferL(RXmlEngDocument& aDocument,
                                                       RXmlEngDocument& aTemplate,
                                                       const TDesC8& aKey,
                                                       const TDesC8& aKeyName,
                                                       TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromBufferL(aKey,aKeyName,aKeyFormat);
   	if(EFalse == iTemplate.IsSameNode(aTemplate))   
		{
			SetTemplateL(aTemplate);  
		}  
    iSkipTmplLookUp = ETrue; //Set to TRUE indicating to skip searching for Template in document
    return SignXmlDocumentL(aDocument);
    }

// ---------------------------------------------------------------------------
// Sign the xml document
// ---------------------------------------------------------------------------
// 
EXPORT_C TXmlEngElement CXmlSecSign::SignXmlDocumentKeyFromBufferL(RXmlEngDocument& aDocument,
                                                       const TDesC8& aKey,
                                                       const TDesC8& aKeyName,
                                                       TXmlSecKeyType aKeyFormat)
    {
    SetKeyFromBufferL(aKey,aKeyName,aKeyFormat);
    return SignXmlDocumentL(aDocument);
    }


// ---------------------------------------------------------------------------
// Sign the xml node
// ---------------------------------------------------------------------------
//     
TXmlEngElement CXmlSecSign::SignXmlNodeL(TXmlEngElement aNode, TBool aUseCurrentKey)
    {
    TXmlEngElement ele;
    TBool add = FALSE;
    xmlSecDSigCtxPtr tmpCtx = SIG_CTX;
    // Reset ctxt
    if(aUseCurrentKey)
        {
        iSigCtx = NULL;
        tmpCtx = Sign::ResetCtxL(tmpCtx,NULL);
        iSigCtx = tmpCtx;
        }
    else
        {
        if(!iMngr)
            {
            iMngr = CXmlSecKeyManager::GetInstanceL();
            }
        iSigCtx = NULL;
        tmpCtx = Sign::ResetCtxL(tmpCtx,iMngr);
        iSigCtx = tmpCtx;
        }        
        
    xmlNodePtr node = NULL;
    
    // skip if there is a template already available which may or may not contain Signature(from document)
    if(EFalse == iSkipTmplLookUp)
    {
    // find <Signature> in aNode
    node = xmlSecFindNode(xmlDocGetRootElement(INTERNAL_NODEPTR(aNode)->doc),
                                xmlSecNodeSignature, xmlSecDSigNs);
    }   
 		TXmlEngElement sign(node);
    if(!node)
        {
        if(iTemplate.NotNull())
            {
            // process template
            node = xmlSecFindNode(xmlDocGetRootElement(INTERNAL_DOCPTR(iTemplate)),
                                xmlSecNodeSignature, xmlSecDSigNs);
            if(!node)
                {
                User::Leave(KErrTemplate);
                }
            node = xmlCopyNode(node, 1);
            if(!node)
                {
                User::Leave(KErrNoMemory);
                }
            // add template to document that should be signed
            sign = TXmlEngElement(node);
            aNode.AppendChildL(sign);
            add = TRUE;
            }
        }
        
    // sign the data
    if(xmlSecDSigCtxSign(tmpCtx, node) < 0) 
        {
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrSign);
        }
    if(add)
        {
        sign.Unlink();    
        }
    iSkipTmplLookUp = EFalse; // Reset the Flag
    return sign;
    }

// ---------------------------------------------------------------------------
// Sign the xml nodes
// ---------------------------------------------------------------------------
//     
EXPORT_C TXmlEngElement CXmlSecSign::SignXmlNodesL(RArray<TXmlEngElement>& aNodes,
                                       TBool aUseCurrentKey)
    {
    if(!aNodes.Count())
        {
        User::Leave(KErrWrongParameter);
        }
    return SignXmlNodeL(aNodes[0].OwnerDocument().DocumentElement(),aUseCurrentKey);
    }

// ---------------------------------------------------------------------------
// Verify xml node
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CXmlSecSign::VerifyXmlNodeL(TXmlEngElement aNode, 
                                     TXmlSecVerificationKeyRepository aKeyRepository)
    {
    xmlSecDSigCtxPtr tmpCtx = SIG_CTX;
    
    // Reset ctxt
    if( aKeyRepository == EThisObject )
        {
        iSigCtx = NULL;
        tmpCtx = Sign::ResetCtxL(tmpCtx,iMngr);
        iSigCtx = tmpCtx;   
        }
    else
        {
        if(!iMngr)
            {
            iMngr = CXmlSecKeyManager::GetInstanceL();
            }
        iSigCtx = NULL;
        tmpCtx = Sign::ResetCtxL(tmpCtx,iMngr);
        iSigCtx = tmpCtx;
        }    
    // should cert store be used        
    if ( aKeyRepository == ECertStore )
        {
        xmlSecSetCertStoreFlag();
        }
    // verify data
    if (xmlSecDSigCtxVerify(tmpCtx, INTERNAL_NODEPTR(aNode)) < 0)
        {
        if ( aKeyRepository == ECertStore )
            {
            xmlSecResetCertStoreFlag();   
            }
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrVerify);
        }
    // check result
    if (tmpCtx->status == xmlSecDSigStatusSucceeded)
        {
         if ( aKeyRepository == ECertStore )
            {
            xmlSecResetCertStoreFlag();   
            }
        return ETrue;
        }
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// Verify xml node
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CXmlSecSign::VerifyXmlNodeKeyFromFileL(TXmlEngElement aNode,
                                                const TDesC8& aKeyFile,
                                                const TDesC8& aKeyName,
                                                TXmlSecKeyType aKeyType)
    {
    SetKeyFromFileL(aKeyFile,aKeyName,aKeyType);
    return VerifyXmlNodeL(aNode);
    }

// ---------------------------------------------------------------------------
// Verify xml node
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CXmlSecSign::VerifyXmlNodeKeyFromBufferL(TXmlEngElement aNode,
                                                  const TDesC8& aKey,
                                                  const TDesC8& aKeyName,
                                                  TXmlSecKeyType aKeyType)
    {
    SetKeyFromBufferL(aKey,aKeyName,aKeyType);
    return VerifyXmlNodeL(aNode);
    }
    
// ---------------------------------------------------------------------------
// Verify xml document
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CXmlSecSign::VerifyXmlDocumentL(const RXmlEngDocument& aDocument, 
                                         TXmlSecVerificationKeyRepository aKeyRepository)
    {
    xmlNodePtr node = NULL;
    TXmlEngNode tmpNode = aDocument.DocumentElement();
    xmlNodePtr root = INTERNAL_NODEPTR(tmpNode);
    node = xmlSecFindNode(root,xmlSecNodeSignature, xmlSecDSigNs);
    if(!node) 
        {
        User::Leave(KErrVerify);    
        }   
    return VerifyXmlNodeL(TXmlEngElement(node),aKeyRepository);
    }
    
// ---------------------------------------------------------------------------
// Verify xml document
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CXmlSecSign::VerifyXmlDocumentKeyFromFileL(const RXmlEngDocument& aDocument,
                                                    const TDesC8& aKeyFile,
                                                    const TDesC8& aKeyName,
                                                    TXmlSecKeyType aKeyType)
    {
    SetKeyFromFileL(aKeyFile,aKeyName,aKeyType);
    return VerifyXmlDocumentL(aDocument);
    }

// ---------------------------------------------------------------------------
// Verify xml document
// ---------------------------------------------------------------------------
//     
EXPORT_C TBool CXmlSecSign::VerifyXmlDocumentKeyFromBufferL(const RXmlEngDocument& aDocument,
                                                      const TDesC8& aKey,
                                                      const TDesC8& aKeyName,
                                                      TXmlSecKeyType aKeyType)
    {
    SetKeyFromBufferL(aKey,aKeyName,aKeyType);
    return VerifyXmlDocumentL(aDocument);
    }
    
// ---------------------------------------------------------------------------
// Set template from file
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::SetTemplateFromFileL(const TDesC8& aTemplate)
    {
    XmlSecTemplate::SetTemplateFromFileL(iTemplate,aTemplate);
    }
    
// ---------------------------------------------------------------------------
// Set template from file
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::SetTemplateFromFileL(RFs& aRFs, const TDesC8& aTemplate)
    {
    XmlSecTemplate::SetTemplateFromFileL(iTemplate,aTemplate,aRFs);
    }

// ---------------------------------------------------------------------------
// Set template from buffer
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::SetTemplateFromBufferL(const TDesC8& aTemplate)
    {
    XmlSecTemplate::SetTemplateFromBufferL(iTemplate,aTemplate);
    }
 
// ---------------------------------------------------------------------------
// Set template from DOM tree
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::SetTemplateL(const RXmlEngDocument& aTemplate)
    {
    		XmlSecTemplate::SetTemplateL(iTemplate,aTemplate);
    }

// ---------------------------------------------------------------------------
// Set key info node in template
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::SetKeyInfoL(const TDesC8& aKeyName)
    {
    XmlSecTemplate::SetKeyInfoL(iTemplate,aKeyName);
    }

// ---------------------------------------------------------------------------
// Set key info node in template
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::SetKeyInfoL(TXmlEngElement aKeyProp)
    {
    XmlSecTemplate::SetKeyInfoL(iTemplate,aKeyProp);
    }
    
// ---------------------------------------------------------------------------
// Create template
// ---------------------------------------------------------------------------
// 
EXPORT_C const RXmlEngDocument& CXmlSecSign::CreateTemplateL(TXmlSecKeyType aKeyFormat,
                                            RArray<TXmlEngElement>& aNodes,
                                            const TDesC8& aId,
                                            TBool aX509Cert,
                                            TUint aTransform,
                                            const TDesC8& aPref,
                                            TBool aNewLine)
    {
    _LIT8(KSep,":");
    _LIT8(KPoint,"#");
    if(iTemplate.NotNull())
        {
        iTemplate.Close();
        }
    
    if(!aNewLine)
        {
        xmlSetNewLineFlag(0);
        }
    
    unsigned char* pref = NULL;
    if(aPref.Length())
        {
        pref = (unsigned char*) XmlEngXmlCharFromDes8L(aPref);
        xmlSetPrefix(pref);
        }
    CleanupStack::PushL(TCleanupItem(Sign::TemplateCleanupL,(TAny*)pref));
        
    xmlNodePtr signNode = NULL;
    xmlNodePtr refNode = NULL;
    xmlNodePtr keyInfoNode = NULL;
    
    if((aKeyFormat == ERSAPrivate) || (aKeyFormat == ERSAPublic))
        {
        // create signature template for RSA-SHA1 enveloped signature
        signNode = xmlSecTmplSignatureCreate(NULL, xmlSecTransformExclC14NId,
    				         xmlSecTransformRsaSha1Id, NULL);
        }
    else if(aKeyFormat == CXmlSecSign::EHMAC)
        {
        // create signature template for HMAC-SHA1 enveloped signature
        signNode = xmlSecTmplSignatureCreate(NULL, xmlSecTransformExclC14NId,
            		         xmlSecTransformHmacSha1Id, NULL);
    
        }
    if(!signNode) 
        {
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrTemplate);
        }
        
    CleanupStack::PushL(TCleanupItem(LibxmlNodeCleanup,(TAny*)signNode)); 
        
    TInt nodeCount = aNodes.Count();
    if(!nodeCount)
        {
        // add reference
        refNode = xmlSecTmplSignatureAddReference(signNode, xmlSecTransformSha1Id,
    					NULL, NULL, NULL);
        if(!refNode) 
            {
            //xmlFreeNode( signNode );
            XmlEngOOMTestL();
            XmlSecErrorFlagTestL();
            User::Leave(KErrTemplate);
            }

        if(KEnvelopedSignature & aTransform)
            {
            // add enveloped transform
            if(!xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformEnvelopedId)) 
                {
                //xmlFreeNode( signNode );
                XmlEngOOMTestL();
                XmlSecErrorFlagTestL();
                User::Leave(KErrTemplate);
                }    
            }
         if(KC14N & aTransform)
            {
            // add enveloped transform
            if(!xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformInclC14NId)) 
                {
                //xmlFreeNode( signNode );
                XmlEngOOMTestL();
                XmlSecErrorFlagTestL();
                User::Leave(KErrTemplate);
                }    
            }
         else if(KExclusiveC14N & aTransform)
            {
            // add enveloped transform
            if(!xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformExclC14NId)) 
                {
                //xmlFreeNode( signNode );
                XmlEngOOMTestL();
                XmlSecErrorFlagTestL();
                User::Leave(KErrTemplate);
                }             
            }
        }
    else
        {
        TPtrC8 nameSpace = KNullDesC8();
        TPtrC8 uriVal = KNullDesC8();
        TXmlEngString str;
        TInt pos = aId.Find(KSep);
        // check id of element    
        if(pos > 0)
            {
            nameSpace.Set(aNodes[0].LookupNamespaceUriL(aId.Left(pos)));
            if(!nameSpace.Length())
                {
                //xmlFreeNode( signNode );
                if (OOM_FLAG)
                    {
                    XmlEngLeaveOOML();
                    }
                XmlSecErrorFlagTestL();
                User::Leave(KErrIdUndefineNS);
                }
            }
        // for all elements with id add reference 
        TXmlEngElement elem;
        for(TInt i = 0; i < nodeCount; i++)
            {
            elem = aNodes[i];
            uriVal.Set(elem.AttributeValueL(aId.Right(aId.Length() - (pos + 1)),nameSpace));
            if(uriVal.Length())
                {
                str.SetL(KPoint);
                CleanupClosePushL(str);
                TXmlEngString str2;
                str2.SetL(uriVal);
                CleanupClosePushL(str2);
                str.AppendL(str2);
                CleanupStack::PopAndDestroy(&str2);
                // add reference
                refNode = xmlSecTmplSignatureAddReference(signNode, xmlSecTransformSha1Id,
        		    			NULL, (unsigned char*)str.Cstring(), NULL);
        		CleanupStack::PopAndDestroy(&str);
                if(!refNode) 
                    {
                    //xmlFreeNode( signNode );
                    XmlEngOOMTestL();
                    XmlSecErrorFlagTestL();
                    User::Leave(KErrTemplate);
                    }
        
                if(KEnvelopedSignature & aTransform)
                    {
                    // add enveloped transform
                    if(!xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformEnvelopedId)) 
                        {
                        //xmlFreeNode( signNode );
                        XmlEngOOMTestL();
                        XmlSecErrorFlagTestL();
                        User::Leave(KErrTemplate);
                        }    
                    }
                 if(KC14N & aTransform)
                    {
                    // add enveloped transform
                    if(!xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformInclC14NId)) 
                        {
                        //xmlFreeNode( signNode );
                        XmlEngOOMTestL();
                        XmlSecErrorFlagTestL();
                        User::Leave(KErrTemplate);
                        }    
                    }
                 else if(KExclusiveC14N & aTransform)
                    {
                    // add enveloped transform
                    if(!xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformExclC14NId)) 
                        {
                        //xmlFreeNode( signNode );
                        XmlEngOOMTestL();
                        XmlSecErrorFlagTestL();
                        User::Leave(KErrTemplate);
                        }             
                    }
                }
            }
        }
    
    // add <dsig:KeyInfo/> and <dsig:X509Data/>
    keyInfoNode = xmlSecTmplSignatureEnsureKeyInfo(signNode, NULL);
    if(!keyInfoNode) 
        {
        //xmlFreeNode( signNode );
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrTemplate);
        }
    if(aKeyFormat != CXmlSecSign::EHMAC && aX509Cert)
        {
        if(!xmlSecTmplKeyInfoAddX509Data(keyInfoNode))
            {
            //xmlFreeNode( signNode );
            XmlEngOOMTestL();
            XmlSecErrorFlagTestL();
            User::Leave(KErrTemplate);
            }
        }
            
    xmlDocPtr doc = xmlNewDoc(NULL);
    xmlNodePtr tmpNode = xmlAddChild(INTERNAL_NODEPTR(doc), signNode);
    if(!tmpNode)
        {
        xmlFreeDoc(doc);
        //xmlFreeNode(signNode);
        User::Leave(KErrNoMemory);
        }
    CleanupStack::Pop(signNode);
    // return created template
    RXmlEngDocument retDoc;
    XmlSecGlobalState* gs = XmlSecGetTls();
    retDoc.OpenL(*gs->iDOMImpl,doc);
    iTemplate = retDoc;
    CleanupStack::PopAndDestroy();
    return iTemplate;
    }
    
// ---------------------------------------------------------------------------
// Create template
// ---------------------------------------------------------------------------
// 
EXPORT_C const RXmlEngDocument& CXmlSecSign::CreateTemplateL(TXmlSecKeyType aKeyFormat,
                                                             TBool aX509Cert,
                                                             TUint aTransform,
                                                             const TDesC8& aPref,
                                                             TBool aNewLine)
    {
    RArray<TXmlEngElement> array;
    CleanupClosePushL(array);
    CreateTemplateL(aKeyFormat,array,KNullDesC8(),aX509Cert,aTransform,aPref,aNewLine);
    CleanupStack::PopAndDestroy(&array);
    return iTemplate;
    }
        
// ---------------------------------------------------------------------------
// Return current encryption template
// ---------------------------------------------------------------------------
// 
EXPORT_C const RXmlEngDocument& CXmlSecSign::CurrentTemplate() const
    {
    return iTemplate;
    }

// ---------------------------------------------------------------------------
// Destroy current encryption template
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::DestroyCurrentTemplate()
    {
    XmlSecTemplate::DestroyTemplate(iTemplate);
    }

// ---------------------------------------------------------------------------
// Read key from buffer and puts it in SymbianKeyStore
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::SetKeyFromBufferL(const TDesC8& aBuffer,
                                       const TDesC8& aKeyName,
                                       TXmlSecKeyType aKeyFormat)
    {
    xmlSecDSigCtxPtr tmpCtx = SIG_CTX;
    if(tmpCtx->signKey)
        {
        xmlSecKeyDestroy(tmpCtx->signKey);
        tmpCtx->signKey = NULL;
        }
    tmpCtx->signKey = Sign::ReadKeyFromBufferL(aBuffer,aKeyName,aKeyFormat);
    }
    
// ---------------------------------------------------------------------------
// Read key from file and puts it in SymbianKeyStore
// ---------------------------------------------------------------------------
// 
EXPORT_C void CXmlSecSign::SetKeyFromFileL(const TDesC8& aKeyFile,
                                     const TDesC8& aKeyName,
                                     TXmlSecKeyType aKeyFormat)
    {
    xmlSecDSigCtxPtr tmpCtx = SIG_CTX;
    if(tmpCtx->signKey)
        {
        xmlSecKeyDestroy(tmpCtx->signKey);
        tmpCtx->signKey = NULL;
        }
    tmpCtx->signKey = Sign::ReadKeyFromFileL(aKeyFile,aKeyName,aKeyFormat);
    }
    
// ---------------------------------------------------------------------------
// Read certificate from file
// ---------------------------------------------------------------------------
//     
EXPORT_C void CXmlSecSign::SetCertFromFileL(const TDesC8& aCertFile)
    {
    xmlSecDSigCtxPtr tmpCtx = SIG_CTX;
    
    if(!tmpCtx->signKey) 
        {
        User::Leave(KErrKey);
        }
    const TInt length = aCertFile.Length();
    char* cStringFileName = new char[ length+1 ];
    if (!cStringFileName)
        {
        User::Leave( KErrNoMemory );
        }
    // copy cert data
    memcpy(cStringFileName, (char*) aCertFile.Ptr(), length);
    cStringFileName[ length ] = NULL;
    // read cert data, add cert to key
    if(xmlSecCryptoAppKeyCertLoad(tmpCtx->signKey, cStringFileName, xmlSecKeyDataFormatDer) < 0) 
        {
        delete[] cStringFileName;
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrCert);
        }
    delete[] cStringFileName;
    }

// ---------------------------------------------------------------------------
// Read certificate from buffer
// ---------------------------------------------------------------------------
//     
EXPORT_C void CXmlSecSign::SetCertFromBufferL(const TDesC8& aCert)
    {
    xmlSecDSigCtxPtr tmpCtx = SIG_CTX;
    
    if(!tmpCtx->signKey) 
        {
        User::Leave(KErrKey);
        }
    // add cert data to key
    if(xmlSecSymbianCryptoAppKeyCertLoadMemory(tmpCtx->signKey,aCert.Ptr(),aCert.Size(),
                                               xmlSecKeyDataFormatDer) < 0) 
        {
        User::Leave(KErrCert);
        }
    }

// ---------------------------------------------------------------------------
// Add cert from file to root's cert chain
// ---------------------------------------------------------------------------
//     
EXPORT_C void CXmlSecSign::AddTrustedCertFromFileL(const TDesC8& aCertFile)
    {
    if(!iMngr)
        {
        iMngr = CXmlSecKeyManager::GetInstanceL();
        }
    const TInt length = aCertFile.Length();
    char* cStringFileName = new char[ length+1 ];
    if (!cStringFileName)
        {
        User::Leave( KErrNoMemory );
        }
    memcpy(cStringFileName, (char*) aCertFile.Ptr(), length);
    cStringFileName[ length ] = NULL;
    // load cert data
    if(xmlSecCryptoAppKeysMngrCertLoad(iMngr->GetKeyManagerPtr(), cStringFileName,
                        xmlSecKeyDataFormatDer, xmlSecKeyDataTypeTrusted) < 0)
        {
        delete[] cStringFileName;
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrCert);
        }
    delete[] cStringFileName;
    }
    
// ---------------------------------------------------------------------------
// Add cert from buffer to root's cert chain
// ---------------------------------------------------------------------------
//     
EXPORT_C void CXmlSecSign::AddTrustedCertFromBufferL(const TDesC8& aCert)
    {
    if(!iMngr)
        {
        iMngr = CXmlSecKeyManager::GetInstanceL();
        }
    if(xmlSecCryptoAppKeysMngrCertLoadMemory(iMngr->GetKeyManagerPtr(), aCert.Ptr(),
                        aCert.Size(), xmlSecKeyDataFormatDer, xmlSecKeyDataTypeTrusted) < 0)
        {
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrCert);
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
CXmlSecSign::CXmlSecSign()
    {
    iTemplate = RXmlEngDocument();
    iSigCtx = NULL;
    iSkipTmplLookUp = EFalse; //Initialize the flag to FALSE
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
// 
void CXmlSecSign::ConstructL()
    {
    // create sign context
    iSigCtx = xmlSecDSigCtxCreate(NULL);
    if(!iSigCtx)
        {
        User::Leave(KErrNoMemory);
	    }
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// 
CXmlSecSign::~CXmlSecSign()
    {
    if(iTemplate.NotNull())
        {
        iTemplate.Close();
        }
    if(iSigCtx)
        {
        xmlSecDSigCtxDestroy(SIG_CTX);
        }
    }    

