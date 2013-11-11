// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// XForms instance map declaration
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGCXFORMSINSTANCEMAP_H
#define XMLENGCXFORMSINSTANCEMAP_H

#include <e32base.h>

class RXmlEngDocument;

/**
This class associates DOM Documents with a string name.
Entries can be added and removed.
*/
class CXmlEngXFormsInstanceMap : public CBase
{
    friend class RXmlEngXPathExpression;

public:
    /**
    Allocates and constructs a new CXmlENgXFormsInstanceMap object.
    @return A new CXmlEngXFormsInstanceMap object.
    @leave KErrNoMemory Memory allocation failure
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C static CXmlEngXFormsInstanceMap* NewL();

	/**
	Allocates and constructs a new CXmlENgXFormsInstanceMap object, and leaves
	it on the cleanup stack.
    @return A new CXmlEngXFormsInstanceMap object.
    @leave KErrNoMemory Memory allocation failure
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C static CXmlEngXFormsInstanceMap* NewLC();

public:
    /**
	Destructor. Frees internal storage, but does not free the Document
	instances that were added. These are owned by the caller.
    */
    IMPORT_C ~CXmlEngXFormsInstanceMap();

	/**
	Adds an entry to the map. The document is associated with a name. If an
	entry with the same name already exists this method does nothing.

	Ownership of the document is not transferred.
    
    @param aInstance   The DOM document to be stored.
    @param aName       The name that to be associated with the document.
	@leave KErrNoMemory Memory allocation failure
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C void AddEntryL(RXmlEngDocument& aInstance, const TDesC8& aName);

    /**
    Removes an entry from the map. If the entry does not exist, the
    method does nothing. Note that the document is not freed.
     
    @param aName Name of entry to remove. 
	@leave KErrNoMemory Memory allocation failure
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C void RemoveEntryL(const TDesC8& aName);

private:
    /** Second phase constructor */
    void ConstructL();    

    /** Default constructor */
    CXmlEngXFormsInstanceMap() {}

private:
    void *iInternal;             
};

#endif // XMLENGCXFORMSINSTANCEMAP_H

