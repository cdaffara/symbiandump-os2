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

#include <xml/dom/xmlengcharacterdata.h>
#include <stdapis/libxml2/libxml2_tree.h>
#include "xmlengdomdefs.h"
#include <stdapis/libxml2/libxml2_globals.h>
#include <xml/utils/xmlengxestrings.h>		//pjj18 new

#define CHARACTER_DATA (static_cast<xmlNodePtr>(iInternal))

// -------------------------------------------------------------------------------
// @return Character contents of the node
//
// This method applies to TXmlEngCDATASection, TXmlEngComment and TXmlEngTextNode nodes.
// -------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngCharacterData::Contents() const
    {
    return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_NODE ? LIBXML_NODE->content : NULL)).PtrC8();
    }
    
// -------------------------------------------------------------------------------
// Sets contents of basic character nodes: TXmlEngTextNode, TXmlEngComment, TXmlEngCDATASection
//
// @param aNewContents  The actual value to store
//
// The input is taken as non-escaped: for example, 
// aNewContents = "123 > 34 && P" will be serialized as "123 &gt; 34 &amp;&amp; P"
//
// Escaped contents may be set only for TXmlEngElement and TXmlEngAttr nodes.
// @see TXmlEngAttr::SetRawValueL(), TXmlEngElement::SetRawTextL()
// -------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngCharacterData::SetContentsL(
    const TDesC8& aNewContents )
    {
    char* newContent = (char*)new(ELeave) TUint8[aNewContents.Length() + 1];
    *(Mem::Copy((TAny*)newContent, aNewContents.Ptr(), aNewContents.Length())) = 0;
    if (LIBXML_NODE->content
            &&
         !(LIBXML_NODE->doc       &&
           LIBXML_NODE->doc->dict &&
           xmlDictOwns(LIBXML_NODE->doc->dict, LIBXML_NODE->content)))
        {
        xmlFree(LIBXML_NODE->content);
        }
    
    LIBXML_NODE->content = (unsigned char *)newContent;
    }

// -----------------------------------------------------------------------------
// Extends the contents of the node by appending aString
// -----------------------------------------------------------------------------
//
EXPORT_C void TXmlEngCharacterData::AppendContentsL(
    const TDesC8& aString)
    {
    if (!LIBXML_NODE) 
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    
    TPtrC8 prevContent = Contents();
    TInt len = prevContent.Length() + aString.Length() + 1;
    char* newContent = (char*)new(ELeave) TUint8[len];
    
    TAny* last = Mem::Copy((TAny*)newContent, prevContent.Ptr(), prevContent.Length());
    *(Mem::Copy(last, aString.Ptr(), aString.Length())) = 0;
    
    if (LIBXML_NODE->content
            &&
         !(LIBXML_NODE->doc       &&
           LIBXML_NODE->doc->dict &&
           xmlDictOwns(LIBXML_NODE->doc->dict, LIBXML_NODE->content)))
        {
        xmlFree(LIBXML_NODE->content);
        }
    
    LIBXML_NODE->content = (unsigned char *)newContent;
    }

// -------------------------------------------------------------------------------
// @return Number of characters in the contents
// -------------------------------------------------------------------------------
//
EXPORT_C TUint TXmlEngCharacterData::Length() const
    {
    return CHARACTER_DATA && CHARACTER_DATA->content ?
        xmlUTF8Strlen(CHARACTER_DATA->content) : 0;
    }
