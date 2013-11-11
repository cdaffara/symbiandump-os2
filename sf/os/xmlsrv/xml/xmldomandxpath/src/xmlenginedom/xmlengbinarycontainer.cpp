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
// Methods for all kind of text nodes
//

#include <xml/dom/xmlengbinarycontainer.h>
#include <stdapis/libxml2/libxml2_tree.h>
#include "xmlengdomdefs.h"
#include <stdapis/libxml2/libxml2_globals.h>

// -------------------------------------------------------------------------------
// @return Binary contents of the container
//
// -------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngBinaryContainer::Contents() const
    {
    return TPtrC8((const TUint8*)LIBXML_NODE->content, (TInt)LIBXML_NODE->properties);
    }

// -------------------------------------------------------------------------------
// Sets contents of binary container
//
// @param aNewContents  The actual value to store
// -------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngBinaryContainer::SetContentsL( const TDesC8& aNewContents )
    {
    if (!LIBXML_NODE) 
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    // Note: TXmlEngBinaryContainer is treated internally by Libxml2 as text nodes.
    // Hence, appending 0 at the end to ensure that text is not parsed beyond actual data in any case.
    
    TUint len = aNewContents.Size();
    unsigned char* str = new(ELeave) unsigned char[len + 1];
    memcpy(str, aNewContents.Ptr(), len);
    str[len] = 0;
               
    if (LIBXML_NODE->content
            &&
         !(LIBXML_NODE->doc       &&
           LIBXML_NODE->doc->dict &&
           xmlDictOwns(LIBXML_NODE->doc->dict, LIBXML_NODE->content)))
        {
        xmlFree(LIBXML_NODE->content);
        }       
    LIBXML_NODE->properties = (xmlAttr*) len;
    LIBXML_NODE->content = str;
    }

// -------------------------------------------------------------------------------
// Extends the contents of binary container by appending aContents
//
// @param aContents  Content to be appended to current content
// -------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngBinaryContainer::AppendContentsL( const TDesC8& aData )
    {
        if (!LIBXML_NODE) 
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    
    // Note: TXmlEngBinaryContainer is treated internally by Libxml2 as text nodes.
    // Hence, appending 0 at the end to ensure that text is not parsed beyond actual data in any case.
    
    TPtrC8 prevContent = Contents();
    TInt len = prevContent.Length() + aData.Length();
    unsigned char* newContent = new(ELeave) unsigned char[len + 1];
    
    TAny* last = Mem::Copy((TAny*)newContent, prevContent.Ptr(), prevContent.Length());
    memcpy(last, aData.Ptr(), aData.Length());
    newContent[len] = 0;
    
    if (LIBXML_NODE->content
            &&
         !(LIBXML_NODE->doc       &&
           LIBXML_NODE->doc->dict &&
           xmlDictOwns(LIBXML_NODE->doc->dict, LIBXML_NODE->content)))
        {
        xmlFree(LIBXML_NODE->content);
        }
    LIBXML_NODE->properties = (xmlAttr*) len;    
    LIBXML_NODE->content = (unsigned char *)newContent;    
    }
