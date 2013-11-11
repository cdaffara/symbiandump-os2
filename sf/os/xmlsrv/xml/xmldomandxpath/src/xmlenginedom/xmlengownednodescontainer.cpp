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
// Node container implementation
//

#include <xml/utils/xmlengmem.h>
#include "xmlengownednodescontainer.h"
#include "xmlengdomdefs.h"
#include <stdapis/libxml2/libxml2_tree.h>
#include <stdapis/libxml2/libxml2_globals.h>

// --------------------------------------------------------------------------------------
// Creates an instance of CXmlEngOwnedNodesContainer
// --------------------------------------------------------------------------------------
//
CXmlEngOwnedNodesContainer* CXmlEngOwnedNodesContainer::NewL()
    {
    CXmlEngOwnedNodesContainer* obj = new (ELeave) CXmlEngOwnedNodesContainer();
    obj->iNodes = NULL;
    obj->iLast = NULL;
    obj->iCount = 0;
    return obj;
    }

CXmlEngOwnedNodesContainer::~CXmlEngOwnedNodesContainer()
    {
    }
// --------------------------------------------------------------------------------------
// Searches for a node in the list
//
// @param   aNode Pointer to a libxml2 node
// @return Index in the list of nodes [0; iCount-1] or iCount if not found
// --------------------------------------------------------------------------------------
//
TUint CXmlEngOwnedNodesContainer::Lookup(xmlNodePtr aNode)
    {
    TUint i = 0;
    xmlNodePtr tmp = iNodes;
    for(; tmp; tmp = tmp->next, i++)
        if(tmp == aNode)
            return i;
    return iCount;
    }

// --------------------------------------------------------------------------------------
// Performs clean up of the list
// --------------------------------------------------------------------------------------
//
void CXmlEngOwnedNodesContainer::RemoveAll()
    {
    iNodes = NULL;
    iCount = 0;
    }

// --------------------------------------------------------------------------------------
// Performs clean up of all owned nodes:
// xmlFreeNode is called on every contained pointer
// --------------------------------------------------------------------------------------
//
void CXmlEngOwnedNodesContainer::FreeAll()
    {
    xmlNodePtr tmp = iNodes;
    xmlNodePtr ptr;
    for(;tmp;)
        {
        ptr = tmp;
        tmp = tmp->next;
        if(ptr)
            xmlFreeNode(ptr);
        }
    iNodes = NULL;
    iCount = 0;
    }


// --------------------------------------------------------------------------------------
// Adds an owned node.
//
// @param aNode  A pointer to some node to be added into the container
//
// @note This method does not check whether the pointer is already in the container
// @note In OOM situation the node is destroyed before leave occurs
// --------------------------------------------------------------------------------------
//
void CXmlEngOwnedNodesContainer::Add(
    xmlNodePtr aNode )
    {
    if(iLast)
        {
        iLast->next = aNode;
        aNode->prev = iLast;        
        }
    else
        {
        iNodes = iLast = aNode;
        }
    iLast = aNode;
    ++iCount;
    }

// --------------------------------------------------------------------------------------
// Excludes node pointer from the list of owned nodes.
// 
// does nothing if provided node pointer is not on the list.
// --------------------------------------------------------------------------------------
//
void CXmlEngOwnedNodesContainer::Remove(
    xmlNodePtr aNode )
    {
    if(aNode == iLast)
        {
        iLast = aNode->prev;
        }
    else
        {
        aNode->next->prev = aNode->prev;    
        }
    if(aNode == iNodes)
        {
        iNodes = aNode->next;
        }
    else
        {
        aNode->prev->next = aNode->next;
        }    
    
    aNode->prev = NULL;
    aNode->next = NULL;
    --iCount;
    }
