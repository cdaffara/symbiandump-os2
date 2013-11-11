// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// XForms functions
//

#include "xmlengdomdefs.h" 
#include <xml/dom/xmlengdocument.h>
#include <xml/dom/xmlengxformsinstancemap.h>
#include <stdapis/libxml2/libxml2_hash.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>

const TInt KHashTableSize = 10;

// ---------------------------------------------------------------------------
// Initializes internal storage. Leaves created object on cleanup stack.
// @return New CXFormsInstanceMap object.
// @exception Leave on OOM.
// ---------------------------------------------------------------------------
//
EXPORT_C CXmlEngXFormsInstanceMap* CXmlEngXFormsInstanceMap::NewLC() 
    {
    CXmlEngXFormsInstanceMap* instance = new(ELeave) CXmlEngXFormsInstanceMap();
    CleanupStack::PushL(instance);
    instance->ConstructL();
    return instance;
    }

// ---------------------------------------------------------------------------
// Initializes internal storage.
// @return New CXFormsInstanceMap object.
// @exception Leave on OOM.
// ---------------------------------------------------------------------------
//
EXPORT_C CXmlEngXFormsInstanceMap* CXmlEngXFormsInstanceMap::NewL() 
    {
    CXmlEngXFormsInstanceMap* instance = NewLC();
    CleanupStack::Pop(instance);
    return instance;
    }

void CXmlEngXFormsInstanceMap::ConstructL()
    {
    iInternal = xmlHashCreate(KHashTableSize);
    OOM_IF_NULL(iInternal);
    }

// ---------------------------------------------------------------------------
// Destructor. Frees internal storage, but <b>does not</b> free the Document
// instances that were added. These are owned by the caller.
// ---------------------------------------------------------------------------
//
EXPORT_C CXmlEngXFormsInstanceMap::~CXmlEngXFormsInstanceMap() 
    {
    xmlHashFree(
        static_cast<xmlHashTablePtr>(iInternal), 
        (xmlHashDeallocator) NULL);
    }

// ---------------------------------------------------------------------------
// Adds entry to container. Document is associated with a name. If an entry
// with the same name already exists this method does nothing.
// @param[in] aInstance   The DOM document to be stored.
// @param[in] aName       The name that is associated with the document.
// ---------------------------------------------------------------------------
//
EXPORT_C void CXmlEngXFormsInstanceMap::AddEntryL(
    RXmlEngDocument& aInstance, 
    const TDesC8& aName ) 
    {
    xmlChar* name = xmlCharFromDesC8L(aName);
    if ( 0 > xmlHashAddEntry(
                static_cast<xmlHashTablePtr>(iInternal), 
                name, 
                INTERNAL_DOCPTR(aInstance))
        )
        {
        delete name;
        TEST_OOM_FLAG;
        // else: there is already instance with such name
        }
    else
        {
        delete name;
        }
    }
    
// ---------------------------------------------------------------------------
// Removes an entry from the container. If the entry does not exists
// method does nothing. Note that document is <b>not freed</b>.
// @param[in] aName   Name of entry to remove. 
// ---------------------------------------------------------------------------
//
EXPORT_C void CXmlEngXFormsInstanceMap::RemoveEntryL(
    const TDesC8& aName ) 
    {
    xmlChar* name = xmlCharFromDesC8L(aName);
    xmlHashRemoveEntry(
        static_cast<xmlHashTablePtr>(iInternal), 
        name, 
        (xmlHashDeallocator) NULL);
    delete name;
    }
