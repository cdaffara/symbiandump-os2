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
* Description: Structure with global states for component.       
*
*/

#ifndef C_GLOABALSTATE_H
#define C_GLOABALSTATE_H

#include <xml/dom/xmlengdomimplementation.h>

class CXmlSecKeyManager;    
/**
 * Strucure with global states for component.
 * Holds key manager pointer and user counter.
 *
 * @lib XmlSecWrapper.dll
 * @since S60 v3.2 
 */
struct XmlSecGlobalState
    {
public:
    /**
     * Key manager pointer.
     */
    CXmlSecKeyManager* iKeyManager;
    
    /**
     * User counter.
     */
    TInt iUserCount;
    
    /**
     * DOM implementation.
     */
    RXmlEngDOMImplementation* iDOMImpl;
    };
    
    /**
     * Gets global state for dll
     */
    XmlSecGlobalState* XmlSecGetTls();
    
    /**
     * Sets global state for dll
     */
    XmlSecGlobalState* XmlSecSetTlsL();
	
    
#endif // C_GLOABALSTATE_H
