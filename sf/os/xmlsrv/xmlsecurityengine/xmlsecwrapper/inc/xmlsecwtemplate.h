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

#ifndef C_TEMPLATE_H
#define C_TEMPLATE_H

#include <e32std.h>
#include <f32file.h>
 
#include <xml/dom/xmlengdom.h>
#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_xmlmemory.h>
#include <stdapis/libxml2/libxml2_parser.h>
#include <stdapis/libxml2/libxml2_globals.h>

class XmlSecTemplate
    {
public:
    /**
     * Sets template.
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate template document
     * @param aInput DOM tree with template document
     */
    static void SetTemplateL(RXmlEngDocument& aTemplate, const RXmlEngDocument& aInput);
    
    
    /**
     * Sets template from file.
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate template document
     * @param aFile name of the file with template inside 
     * @param aRFs handle to a file server session
     */
    static void SetTemplateFromFileL(RXmlEngDocument& aTemplate, const TDesC8& aFile, RFs& aRFs);
    
    /**
     * Sets template from file.
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate  template document
     * @param aFile name of the file with template inside 
     */
    static void SetTemplateFromFileL(RXmlEngDocument& aTemplate, const TDesC8& aFile);
    
    /**
     * Sets template from buffer.
     * This template will be used until new template will be set.
     *
     * @since S60 v3.2
     * @param aTemplate template document
     * @param aBuffer template in buffer 
     */  
    static void SetTemplateFromBufferL(RXmlEngDocument& aTemplate,const TDesC8& aBuffer);
    
    /**
     * Destroys current template.
     *
     * @since S60 v3.2
     * @param aTemplate template to destroy
     */
    static void DestroyTemplate(RXmlEngDocument& aTemplate);
    
    /**
     * Adds <KeyName> element to <KeyInfo> node and sets the value of it.
     * The element will be added to aTemplate template.
     *
     * @since S60 v3.2
     * @param aTemplate template document.
     * @param aKeyName Key name that should be set.
     */
    static void SetKeyInfoL(RXmlEngDocument& aTemplate, const TDesC8& aKeyName);
    
    /**
     * Adds element to <KeyInfo> node. 
     * This method allow to add specific data to <KeyInfo> element.
     * The element will be added to aTemplate template.
     * 
     * @since S60 v3.2
     * @param aTemplate template document
     * @param aKeyProp TElement that should be set in template document.
     */  
    static void SetKeyInfoL(RXmlEngDocument& aTemplate, TXmlEngElement aKeyProp);
    };
    
#endif // C_TEMPLATE_H
