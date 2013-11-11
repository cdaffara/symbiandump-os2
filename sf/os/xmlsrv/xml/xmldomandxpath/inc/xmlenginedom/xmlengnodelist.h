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
// Node list class
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGNODELIST_H
#define XMLENGNODELIST_H

#include <xml/dom/xmlengnode.h>
#include <xml/dom/xmlengnodelist_impl.h>

/**
Implements an iterator or filtered iterator for a tree.  

This classes behaves in two very different ways, depending on the
template parameter.

1)  RXmlEngNodeList<TXmlEngNode>

Through template specialization, this becomes an alias for RXmlEngNodeListImpl.
The RXmlEngNodeListImpl::OpenL() methods control which nodes are included in
the iterator.  It is possible to iterate across all node types or only a single
type.  It is possible to iterate only over nodes within a specific namespace.
It is also possible to iterate over nodes with the same name.

RXmlEngNodeListImpl should not be used directly.

@see RXmlEngNodeListImpl

2)  RXmlEngNodeList<X> where X is not TXmlEngNode

As the OpenL() methods of this class are private, this class cannot be opened
except by friend classes TXmlEngNode and TXmlEngElement.  As such, this form
should not be used.
*/
template<class T> 
class RXmlEngNodeList
{
    friend class TXmlEngNode;
    friend class TXmlEngElement;

public:
    /**
    Gets the list length.
    @return The number of nodes in the list
    */
	inline TInt Count() const;

    /**
    Checks whether a next node exists in the list
	@return ETrue of a next node exists, EFalse otherwise
    */
	inline TBool HasNext() const;

    /**
    Gets the next node in the list
	@return The next node casted to the templated type or NULL if it does not
	exist
    */
	inline T Next();

    /** 
	Closes a node list.  This does not affect any of the nodes.  Only
	internally used memory is freed.
	*/
	inline void Close();

private:

    inline void OpenL( 
			void* aHead,
			TXmlEngNode::TXmlEngDOMNodeType aType,
			const TDesC8&	aName = KNullDesC8,
			const TDesC8&	aNs = KNullDesC8);

    inline void Open( 
			void* aHead,
			TXmlEngNode::TXmlEngDOMNodeType aType);

private:
	RXmlEngNodeListImpl iList;
};

/**
This template specialization makes RXmlEngNodeList<TXmlEngNode> an alias for
RXmlEngNodeListImpl.  No part of the class specified above is used, rather the
interface (and implementation) of RXmlEngNodeListImpl is used instead.
*/
template<>
class RXmlEngNodeList<TXmlEngNode>: public RXmlEngNodeListImpl {};


#include <xml/dom/xmlengnodelist.inl>

#endif /* XMLENGNODELIST_H */

