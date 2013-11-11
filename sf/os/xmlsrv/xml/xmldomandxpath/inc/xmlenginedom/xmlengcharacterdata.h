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
// All text nodes functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGCHARACTERDATA_H
#define XMLENGCHARACTERDATA_H

#include <xml/dom/xmlengnode.h>

/**
This class represents XML text nodes in the DOM tree.  Inherited classes
further specialize (e.g. TXmlEngTextNode, TXmlEngComment).

The CharacterData Interface is described by the DOM Spec here:
http://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/core.html#ID-FF21A306
*/
class TXmlEngCharacterData : public TXmlEngNode
{
public:

    /** Get contents of the node.
    @return The content of the node
    */
    IMPORT_C TPtrC8 Contents() const;
    
    /**
    Sets contents of the node.

    The input is taken as non-escaped: for example, 
    aNewContents = "123 > 34 && P" will be serialized as "123 &gt; 34 &amp;&amp; P"
    
    @param aNewContents  The actual value to store
	@leave - One of the system-wide error codes
    */
    IMPORT_C void SetContentsL(const TDesC8& aNewContents);

    /** Append to the contents of the node
    @param aString Content to be appended
	@leave KXmlEngErrNullNode Node is NULL
	@leave - One of the system-wide error codes
    */
    IMPORT_C void AppendContentsL(const TDesC8& aString);

    /** Get length of the content.
    @return Number of characters in the contents
    */
    IMPORT_C TUint Length() const;

protected:
    /** Default constructor */
	inline TXmlEngCharacterData(); 

    /**
    Constructor
    @param aInternal node pointer
    */
	inline TXmlEngCharacterData(void* aInternal);
};


#include <xml/dom/xmlengcharacterdata.inl>

#endif /* XMLENGCHARACTERDATA_H */

