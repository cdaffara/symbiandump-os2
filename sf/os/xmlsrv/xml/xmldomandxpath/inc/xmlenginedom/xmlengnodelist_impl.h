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
// Node list implementation functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGNODELIST_IMPL_H
#define XMLENGNODELIST_IMPL_H

#include <e32def.h>
#include <xml/dom/xmlengnode.h>

/** 
This class should not be used directly.  However, it defines the interface for
RXmlEngNodeList<TXmlEngNode> through template specialization.

@see RXmlEngNodeList

This class implements an iterator or filtered iterator for a tree.  The OpenL()
methods control which nodes are included in the iterator.  It is possible to
iterate across all node types or only a single type.  It is possible to iterate
only over nodes within a specific namespace.  It is also possible to iterate
over nodes with the same name.
*/
class RXmlEngNodeListImpl
{
public:

    /** Default constructor */
    IMPORT_C RXmlEngNodeListImpl();

    /** 
	Closes a node list.  This does not affect any of the nodes.  Only
	internally used memory is freed.
	*/
    IMPORT_C void Close();

    /** 
    Gets the list length.
    @return Number of nodes in the list
    */
    IMPORT_C TInt  Count() const;

    /** 
    Checks whether the next node exists in the list
    @return ETrue if the next node exists, EFalse otherwise
    */
    IMPORT_C TBool HasNext() const;

    /** 
    Gets the next node in the list
    @return The next node or NULL if it does not exist
    */
    IMPORT_C TXmlEngNode Next();

    /** 
	Initializes the list.  The parameters to this method control what nodes are
	included in the list.  If aType is NULL, all nodes are included in the
	list.  Both aName and aNs, if specified, are copied internally.

	@param aHead The first node in the list
    @param aType Only include nodes of this type
    @param aName Only include nodes that match this name
    @param aNs Only include nodes that match this namespace
	@leave - One of the system-wide error codes
    */
    void OpenL( 
			void* aHead,
			TXmlEngNode::TXmlEngDOMNodeType aType,
			const TDesC8&	aName = KNullDesC8,
			const TDesC8&	aNs = KNullDesC8);

    /** 
	Initializes the list.  The parameters to this method control what nodes are
	included in the list.  If aType is NULL, all nodes are included in the
	list.

	@param aHead The first node in the list
    @param aType Only include nodes of this type
    */
    void Open( 
			void* aHead,
			TXmlEngNode::TXmlEngDOMNodeType aType);

private:

	/**
	Finds the next node in the list according to the criteria specified in OpenL().
	@param aCurrentNode The current node pointer
	@return The next node or NULL if no next node
	*/
    void* FindNextNode(void* aCurrentNode) const;

    void*	        iCurrentNode;
	TInt		    iType;  // NodeType:4 bits (0-3) & MatchName flag (bit 4)
	unsigned char*	iName;
	unsigned char*	iNsUri;
};

/**
Compares two strings.
@param aStr1 String 1 to compare
@param aStr2 String 2 to compare
@return ETrue of aStr1 and aStr2 are equal or if they are both NULL.  EFalse otherwise.
*/
TBool StrEqualOrNull(const void* aStr1, const void* aStr2);
			

#endif /* XMLENGNODELIST_IMPL_H */

