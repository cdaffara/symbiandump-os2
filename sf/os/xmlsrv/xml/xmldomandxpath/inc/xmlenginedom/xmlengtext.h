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
// Text node functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGTEXT_H
#define XMLENGTEXT_H

#include <xml/dom/xmlengcharacterdata.h>


/**
The TXmlEngTextNode class inherits from TXmlEngCharacterData and represents the
textual content (termed "character data" in XML) of an Element or TXmlEngAttr. 

If there is no markup inside an element's content, the text is contained in a
single TXmlEngTextNode that is the only child of the element. If there is
markup, it is parsed into the information items (elements, comments, etc.) and
the TXmlEngTextNode nodes that form the list of children of the element.

When a document is first made available via the DOM, there is only one
TXmlEngTextNode node for each block of text. Users may create adjacent
TXmlEngTextNode nodes that represent the contents of a given element without
any intervening markup, but should be aware that there is no way to represent
the separations between these nodes in XML or HTML, so they will not (in
general) persist between DOM editing sessions. 

No lexical check is done on the content of a TXmlEngTextNode node and depending
on its position in the document, some characters must be escaped during
serialization using character references.  For example, the characters "<&" if
the textual content is part of an element or an attribute, the character
sequence "]]>" when part of an element, the quotation mark character " or the
apostrophe character ' when part of an attribute. 

@see http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#ID-1312295772
*/
class TXmlEngTextNode : public TXmlEngCharacterData
{
public:
    /** Default constructor */
	inline TXmlEngTextNode();

    /**
    Check if the element content is whitespace.  Whitespace may be an empty element,
	or the characters space, tab, LF, or CR.
    @return ETrue if the element content only contains whitespace, EFalse otherwise
    */
    IMPORT_C TBool IsElementContentWhitespace() const;

protected:
    /**
    Constructor
    @param aInternal Text node pointer
    */
    inline TXmlEngTextNode(void* aInternal);
};


#include <xml/dom/xmlengtext.inl>

#endif /* XMLENGTEXT_H */

