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
//

#include "xmlengdnodecontextnamespaceresolver.h"
#include "xmlengddomdefs.h"


EXPORT_C CXmlEngNodeContextNamespaceResolver::~CXmlEngNodeContextNamespaceResolver()
    {
    }

EXPORT_C CXmlEngNodeContextNamespaceResolver::CXmlEngNodeContextNamespaceResolver(
    const TXmlEngNode& aContextNode )
    {
    TXmlEngNode node = aContextNode;
    while (node.NodeType() != TXmlEngNode::EElement && 
           node.ParentNode().NotNull())
        {
        node = node.ParentNode();
        }
    iContextElement = (node.NodeType() == TXmlEngNode::EElement) ? node.AsElement() : TXmlEngElement();
    }

EXPORT_C TDOMString CXmlEngNodeContextNamespaceResolver::LookupPrefixL(
    const TDesC8& aNamespaceUri ) const
    {
    if(iContextElement.NotNull())
        return iContextElement.LookupPrefixL(aNamespaceUri);
    else
        return NULL;
    }

EXPORT_C TDOMString CXmlEngNodeContextNamespaceResolver::LookupNamespaceUriL(
    const TDesC8& aPrefix ) const
    {
    if(iContextElement.NotNull())    
        return iContextElement.LookupNamespaceUriL(aPrefix);
    else
        return NULL;
    }


