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
// Implementation of nodes container
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGOWNED_NODES_CONTAINER_H
#define XMLENGOWNED_NODES_CONTAINER_H

#include <e32base.h>
#include <libxml2_tree.h>



/**
 * Container for nodes owned by document
 * 
 */
class CXmlEngOwnedNodesContainer: public CBase
    {
 public:
    /**
     * Creates an instance of CXmlEngOwnedNodesContainer.
     *
     */
    static CXmlEngOwnedNodesContainer* NewL();

    /**
     * Destructor.
     *
     */
    ~CXmlEngOwnedNodesContainer();
 
    /**
     * Adds an owned node.
     *
     * @param aNodePtr A pointer to some node to be added into the container
     * 
     * @note This method does not check whether the pointer is already in the container
     * @note In OOM situation the node is destroyed before leave occurs
     */
    void Add(xmlNodePtr aNodePtr);

    /**
     * Excludes node pointer from the list of owned nodes.
     * Does nothing if provided node pointer is not on the list.
     *
     * @param aNodePtr A pointer to node that should be removed.
     */
    void Remove(xmlNodePtr aNodePtr);

    /**
     * Performs clean up of the list
     *
     */
    void RemoveAll();

    /**
     * Performs clean up of all owned nodes:
     * xmlFreeNode is called on every contained pointer
     *
     */
    void FreeAll();

private:
    /**
     * Default constructor.
     *
     */
    CXmlEngOwnedNodesContainer() {}

    /**
     * Searches for a node in the list
     *
     * @param aPtr Pointer to a libxml2 node
     * @return Index in the list of nodes [0; iLastIndex-1] or iLastIndex if not found
     */
    TUint Lookup(xmlNodePtr aPtr);

public:
	/** Node pointer */
    xmlNodePtr iNodes;
    /** Last node pointer */
    xmlNodePtr iLast;     
	/** Element count */
    TUint iCount;     
    };



#endif /* XMLENGOWNED_NODES_CONTAINER_H */
