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
// Node set functions implementation
//

#include <stdapis/libxml2/libxml2_globals.h>
#include <stdapis/libxml2/libxml2_xpath.h>
#include <stdapis/libxml2/libxml2_xpathinternals.h>

#include <xml/dom/xmlengnodeset.h>
#include <xml/dom/xmlengnode.h>
#include "xmlengdomdefs.h"
#include <xml/utils/xmlengmem.h>

#define THIS_NODESET (static_cast<xmlNodeSetPtr>(iInternal))
// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C RXmlEngNodeSet::RXmlEngNodeSet():iInternal(NULL)
    {
    }

// -----------------------------------------------------------------------------
// Frees any allocated resources for the RXmlEngNodeSet instance.
//
// @note All nodes referred to in a node set are "live",
//       meaning that they are not copies of actual nodes in
//       a document tree. <br>
//       <b> The only exception for this are namespace declarations, 
//       which ARE COPIES AND ARE DESTROYED with a node set</b>
// -----------------------------------------------------------------------------
//
EXPORT_C void RXmlEngNodeSet::Free()
    {
    // NOTE: namespace nodes are new objects (->next points to parent element)
    //       and should be destroyed; all other nodes are "live" and belong 
    //       to the document tree (so, should not be deleted)
    //       xmlXPathNodeSetFreeNs(xmlNsPtr ns) is used for namespaces
    if (!THIS_NODESET)
        return;  // nothing to free..
    xmlXPathFreeNodeSet(THIS_NODESET);
    iInternal = NULL;
    }

// -----------------------------------------------------------------------------
// @return Number of nodes in a node set
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RXmlEngNodeSet::Length() const
    {
    return THIS_NODESET ? THIS_NODESET->nodeNr : 0;
    }

// -----------------------------------------------------------------------------
// Retrieves a node from node set by index
//
// @param aIndex  Node index  { 0 <= aIndex < Length() } 
// -----------------------------------------------------------------------------
//
EXPORT_C TXmlEngNode RXmlEngNodeSet::operator[](
    TInt aIndex) const
    {
    XE_ASSERT_DEBUG(THIS_NODESET && aIndex >= 0 && aIndex < THIS_NODESET->nodeNr);
    return TXmlEngNode(THIS_NODESET->nodeTab[aIndex]);
    }

// -----------------------------------------------------------------------------
// Checks whether aNode is in a resulting node set
//
// @return TRUE is node in the node set, FALSE otherwise
// -----------------------------------------------------------------------------
//
EXPORT_C TBool RXmlEngNodeSet::Contains(
    TXmlEngNode aNode) const
    {
    return xmlXPathNodeSetContains(THIS_NODESET, INTERNAL_NODEPTR(aNode));
    }

// -----------------------------------------------------------------------------
// Initializes a RXmlEngNodeSet instance as being "empty"
//
// This method is used when preparing node sets that will
// be used with RXmlEngXPathExpression::EvaluateWithDependenciesL(TXmlEngNode,RXmlEngNodeSet&)
// and TXmlEngXPathEvaluator::EvaluateWithDependenciesL(aExpression,aContextNode,aResolver,aDependents)
// -----------------------------------------------------------------------------
//
EXPORT_C void RXmlEngNodeSet::InitializeL()
    {
    Free();
    // create the libxml nodeset "object"
    iInternal = xmlXPathNodeSetCreate(NULL);
    OOM_IF_NULL(iInternal);
    }
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
RXmlEngNodeSet::RXmlEngNodeSet(void* aData):iInternal(aData)
    {
    }
