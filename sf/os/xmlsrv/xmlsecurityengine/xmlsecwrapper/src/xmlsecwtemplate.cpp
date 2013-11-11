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
* Description: Class with methods used in encryption and decryption process.       
*
*/

#include <e32std.h>
#include <utf.h>

#include "xmlsecwerrors.h"
#include "xmlsecwdefs.h"
#include "xmlsecwtemplate.h"

#include "xmlsec_templates.h"
#include "xmlsec_xmlsec.h"
#include "xmlsec_xmltree.h"

#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengdomparser.h>
#include <xml/utils/xmlengutils.h>

// ---------------------------------------------------------------------------
// Set Template 
// ---------------------------------------------------------------------------
//        
void XmlSecTemplate::SetTemplateL(RXmlEngDocument& aTemplate, const RXmlEngDocument& aInput)
    {
    if(aInput.IsNull())
        {
        User::Leave(KErrTemplate);
        };
    if(aTemplate.NotNull())
        {
        aTemplate.Close();
        }
    aTemplate = aInput.CloneDocumentL();
    }

// ---------------------------------------------------------------------------
// Set Template from file
// ---------------------------------------------------------------------------
//          
void XmlSecTemplate::SetTemplateFromFileL(RXmlEngDocument& aTemplate, 
                                          const TDesC8& aFile, 
                                          RFs& aRFs)
    {
    if(aTemplate.NotNull())
        {
        aTemplate.Close();
        }
    // load template
    HBufC16* buf = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aFile);
    CleanupStack::PushL(buf);
    RXmlEngDOMImplementation dom;
    dom.OpenL();
    CleanupClosePushL(dom);
    RXmlEngDOMParser parser;
    User::LeaveIfError(parser.Open(dom));
    CleanupClosePushL(parser);
    aTemplate = parser.ParseFileL(aRFs,buf->Des());
    CleanupStack::PopAndDestroy(&parser);
    CleanupStack::PopAndDestroy(&dom);
    CleanupStack::PopAndDestroy(buf);
    }
   
// ---------------------------------------------------------------------------
// Set Template from file
// ---------------------------------------------------------------------------
//          
void XmlSecTemplate::SetTemplateFromFileL(RXmlEngDocument& aTemplate, const TDesC8& aFile)
    {
    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    XmlSecTemplate::SetTemplateFromFileL(aTemplate, aFile, rfs);
    CleanupStack::PopAndDestroy(&rfs);
    }
    
// ---------------------------------------------------------------------------
// Set Template from buffer 
// ---------------------------------------------------------------------------
//          
void XmlSecTemplate::SetTemplateFromBufferL(RXmlEngDocument& aTemplate,const TDesC8& aBuffer)
    {
    if(aTemplate.NotNull())
        {
        aTemplate.Close();
        }
    // load template
    RXmlEngDOMImplementation dom;
    dom.OpenL();
    CleanupClosePushL(dom);
    RXmlEngDOMParser parser;
    User::LeaveIfError(parser.Open(dom));
    CleanupClosePushL(parser);
    aTemplate = parser.ParseL(aBuffer);
    CleanupStack::PopAndDestroy(&parser);
    CleanupStack::PopAndDestroy(&dom);
    }
    
// ---------------------------------------------------------------------------
// Destroy Template
// ---------------------------------------------------------------------------
//         
void XmlSecTemplate::DestroyTemplate(RXmlEngDocument& aTemplate)
    {
    if(aTemplate.NotNull())
        {
        aTemplate.Close();
        }
    }    
    
// ---------------------------------------------------------------------------
// Add <KeyName> element to <KeyInfo> node and set the value of it.
// ---------------------------------------------------------------------------
//           
void XmlSecTemplate::SetKeyInfoL(RXmlEngDocument& aTemplate, const TDesC8& aKeyName)
    {
    if(!aKeyName.Compare(KNullDesC8))
        {
        User::Leave(KErrWrongParameter);
        }
    if(aTemplate.IsNull())
        {
        User::Leave(KErrTemplate);
        }
    xmlNodePtr keyInfo = NULL;
    TXmlEngNode tmpNode = aTemplate.DocumentElement();
    xmlNodePtr root = INTERNAL_NODEPTR(tmpNode);
    keyInfo = xmlSecFindNode(root,xmlSecNodeKeyInfo, xmlSecDSigNs);
    if(!keyInfo)
        {
        User::Leave(KErrTemplate);
        }
    TXmlEngElement(keyInfo).RemoveChildren();
    char* name = XmlEngXmlCharFromDes8L(aKeyName); 
    root = xmlSecTmplKeyInfoAddKeyName(keyInfo,(unsigned char*)name);
    delete name;
    if(!root)
        {
        User::Leave(KErrNoMemory);
        }    
    }
    
// ---------------------------------------------------------------------------
// Add element to <KeyInfo> node. 
// ---------------------------------------------------------------------------
//          
void XmlSecTemplate::SetKeyInfoL(RXmlEngDocument& aTemplate, TXmlEngElement aKeyProp)
    {
    if(aKeyProp.IsNull())
        {
        User::Leave(KErrWrongParameter);
        }
    if(aTemplate.IsNull())
        {
        User::Leave(KErrTemplate);
        }        
    xmlNodePtr keyInfo = NULL;
    TXmlEngNode tmpNode = aTemplate.DocumentElement();
    xmlNodePtr root = INTERNAL_NODEPTR(tmpNode);
    keyInfo = xmlSecFindNode(root,xmlSecNodeKeyInfo, xmlSecDSigNs);
    if(!keyInfo)
        {
        User::Leave(KErrTemplate);
        }
    TXmlEngElement keyIn(keyInfo);
    keyIn.RemoveChildren();
    keyIn.AppendChildL(aKeyProp);
    }
