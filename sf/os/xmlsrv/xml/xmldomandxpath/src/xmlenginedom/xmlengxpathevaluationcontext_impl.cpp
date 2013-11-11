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
// XPath evaluation context implementation
//

#include "xmlengdomdefs.h"
#include "xmlengxpathevaluationcontext_impl.h"
#include <stdapis/libxml2/libxml2_globals.h>
#include <xml/dom/xmlengnode.h>
#include <xml/dom/xmlengnamespace.h>
#include <xml/dom/xmlengelement.h>

TUint TXmlEngXPathEvaluationContextImpl::ArgCount()
    {
    return iArgCount;
    }

const RXmlEngXPathResult TXmlEngXPathEvaluationContextImpl::Argument(
    TUint aIndex )
    {
    XE_ASSERT_DEBUG(aIndex < iArgCount);
    // "Peeking" a value from a XPath-expression evaluation stack
    return RXmlEngXPathResult(iContext->valueTab[iContext->valueNr - (iArgCount - aIndex)]);
    }

const RXmlEngXPathResult TXmlEngXPathEvaluationContextImpl::Result()
    {
    XE_ASSERT_DEBUG(iCurrentResult);
    if (iCurrentResult->type != XPATH_NODESET &&
        iCurrentResult->nodesetval)
        {
        xmlXPathFreeNodeSet(iCurrentResult->nodesetval);
        iCurrentResult->nodesetval = NULL;
        }
    if (iCurrentResult->type != XPATH_STRING && iCurrentResult->stringval)
        {
        xmlFree(iCurrentResult->stringval);
        iCurrentResult->stringval = NULL;
        }
    return RXmlEngXPathResult(iCurrentResult);
    }

void TXmlEngXPathEvaluationContextImpl::InitializeNodeSetResult()
    {
    XE_ASSERT_DEBUG(iCurrentResult);
    iCurrentResult->nodesetval = xmlXPathNodeSetCreate(NULL);
    if(iCurrentResult->nodesetval)
        {
        iCurrentResult->type = XPATH_NODESET;
        }
    // else OOM happened and will be handled later
    }

void TXmlEngXPathEvaluationContextImpl::AppendToResult(
    const TXmlEngNode aNode )
    {
    if(OOM_FLAG)
        return; // Do nothing in OOM
    if (aNode.NodeType() != TXmlEngNode::ENamespaceDeclaration)
        {
        xmlXPathNodeSetAdd(iCurrentResult->nodesetval, INTERNAL_NODEPTR(*const_cast<TXmlEngNode*>(&aNode)));
        }
    }
void TXmlEngXPathEvaluationContextImpl::AppendToResult(
    const TXmlEngNamespace aAppendedNsNode,
    const TXmlEngElement aNsParentNode)
    {
    XE_ASSERT_DEBUG(iCurrentResult);

    if(OOM_FLAG)
        return; // Do nothing in OOM
    // Add a copy of namespace node with special function
    xmlXPathNodeSetAddNs(iCurrentResult->nodesetval,
                         INTERNAL_NODEPTR(*const_cast<TXmlEngElement*>(&aNsParentNode)),
                         INTERNAL_NSPTR(*const_cast<TXmlEngNamespace*>(&aAppendedNsNode)));
    }

void TXmlEngXPathEvaluationContextImpl::AppendToResult(
    const RXmlEngNodeSet aNodeSet )
    {
    if(OOM_FLAG)
        return; // Do nothing in OOM
    xmlXPathNodeSetMerge(iCurrentResult->nodesetval, INTERNAL_NODESETPTR(aNodeSet));
    iCurrentResult->type = XPATH_NODESET;
    }

void TXmlEngXPathEvaluationContextImpl::SetResult(
    TReal aNumber )
    {
    XE_ASSERT_DEBUG(iCurrentResult);
    iCurrentResult->type = XPATH_NUMBER;
    iCurrentResult->floatval = aNumber;
    }

void TXmlEngXPathEvaluationContextImpl::SetResult(
    TBool aBoolean )
    {
    XE_ASSERT_DEBUG(iCurrentResult);
    iCurrentResult->type = XPATH_BOOLEAN;
    iCurrentResult->boolval = (aBoolean != 0);
    }

void TXmlEngXPathEvaluationContextImpl::SetResultL(
    const TDesC8& aString )
    {
    // Note: OOM flag is checked later (otherwise, there are two checks in the function)
    xmlChar* copy = xmlCharFromDesC8L(aString);
   
    iCurrentResult->type = XPATH_STRING;
    if(iCurrentResult->stringval)
        xmlFree(iCurrentResult->stringval);
    iCurrentResult->stringval = copy;
    }

// ---------------------------------------------           Impementation-specific methods
/**
Allocates memory for function result

@return TRUE is succeeded, FALSE if OOM has happened
*/
TBool TXmlEngXPathEvaluationContextImpl::Initialize()
    {
    iCurrentResult = (xmlXPathObjectPtr) xmlMalloc(sizeof(xmlXPathObject));
    if(iCurrentResult)
        memset(iCurrentResult, 0, (size_t)sizeof(xmlXPathObject));
    iCurrentResult->type = XPATH_STRING;
    return (TBool)iCurrentResult;
    }

void* TXmlEngXPathEvaluationContextImpl::ExtendedContext()
    {
    return iContext->comp->extendedContext;
    }
