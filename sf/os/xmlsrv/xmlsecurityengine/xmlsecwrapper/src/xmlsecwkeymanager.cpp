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
* Description: Key manager functionality       
*
*/


#include <e32base.h>

#include <stdapis/libxml2/libxml2_globals.h>

#include <xml/utils/xmlengmem.h>

#include "xmlsec_crypto.h"
#include "xmlsec_xmlsec.h"

#include "xmlsecwkeymanager.h"
#include "xmlsecwerrors.h"
#include "xmlsecwglobalstate.h"
#include "xmlsecwinternalutils.h"
    
// ---------------------------------------------------------------------------
// Get instance of key manager.
// ---------------------------------------------------------------------------
// 
CXmlSecKeyManager* CXmlSecKeyManager::GetInstanceL()
    {
    XmlSecGlobalState* gs = XmlSecGetTls();
    CXmlSecKeyManager* self = gs->iKeyManager;
    if(!self)
        {
        self = new (ELeave) CXmlSecKeyManager;
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(self);
        gs->iKeyManager = self;
        }
    return self;
    }

// ---------------------------------------------------------------------------
// Return pointer to key manager (XMLSEC)
// ---------------------------------------------------------------------------
//  
xmlSecKeysMngr* CXmlSecKeyManager::GetKeyManagerPtr()
    {
    return iKeyManager;
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// 
CXmlSecKeyManager::~CXmlSecKeyManager()
    {
    if(iKeyManager)
        {
        xmlSecKeysMngrDestroy(iKeyManager);
        }
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
// 
void CXmlSecKeyManager::ConstructL()
    {
    iKeyManager = xmlSecKeysMngrCreate();
    if(!iKeyManager) 
        {
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
        User::Leave(KErrKeyManagerInit);
        }

    if(xmlSecCryptoAppDefaultKeysMngrInit(iKeyManager) < 0)
        {
    	xmlSecKeysMngrDestroy(iKeyManager);
    	iKeyManager = NULL;
        XmlEngOOMTestL();
        XmlSecErrorFlagTestL();
    	User::Leave(KErrKeyManagerInit);
        }    
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
// 
CXmlSecKeyManager::CXmlSecKeyManager()
    {
    }    
