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
* Description: Key manager class.       
*
*/

#ifndef C_KEYMANAGER_H
#define C_KEYMANAGER_H
    
/**
 * Class that contains key manager.
 *
 * Class allows application to have only one key menager.
 * Class should be used internal.
 *
 * @lib XmlSecWrapper.dll
 * @since S60 v3.2
 */
class CXmlSecKeyManager : public CBase
    {

public:
    /**
     * Gets instance of key manager.
     *
     * @since S60 v3.2
     * @return Key manager instance.
     */
    LOCAL_C CXmlSecKeyManager* GetInstanceL();
    
    /**
     * Gets pointer to XmlSec key manager.
     *
     * @since S60 v3.2
     * @return Key manager pointer.
     */
    xmlSecKeysMngr* GetKeyManagerPtr();
    
    /**
     * Destructor.
     *
     * @since S60 v3.2
     */
    virtual ~CXmlSecKeyManager();

private:
    /**
     * Constructor.
     *
     * @since S60 v3.2
     */
    CXmlSecKeyManager();
    
    /**
     * Second phase constructor.
     *
     * @since S60 v3.2
     */
    void ConstructL();

private:
    /** 
     * XmlSec key manager pointer 
     */
    xmlSecKeysMngr* iKeyManager;
    };
    
#endif // C_KEYMANAGER_H
