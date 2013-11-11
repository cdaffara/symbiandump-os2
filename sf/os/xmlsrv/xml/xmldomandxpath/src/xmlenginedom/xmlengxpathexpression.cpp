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
// XPath expression functions
//

#include <stdapis/libxml2/libxml2_globals.h>
#include <stdapis/libxml2/libxml2_xpath.h>
#include "libxml2_xpathinternals_private.h"
#include <xml/dom/xmlengxpathexpression.h>
#include <xml/dom/xmlengxformsinstancemap.h>
#include <xml/dom/xmlengnode.h>
#include "xmlengdomdefs.h"
#include <xml/dom/xmlengxpatherrors.h>

#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>

#define COMPILED_EXPR (static_cast<xmlXPathCompExprPtr>(iInternal))
#define INSTANCE_MAP (iInternal2 ? \
                      static_cast<xmlHashTablePtr> \
                      (static_cast<CXmlEngXFormsInstanceMap*>(iInternal2)->iInternal) : NULL)

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
//
EXPORT_C RXmlEngXPathExpression::RXmlEngXPathExpression():iInternal(NULL), iInternal2(NULL), reserved1(0), reserved2(NULL)
    {
    }

// --------------------------------------------------------------------------------------------
// Resets the compiled expression and frees the allocated memory.
// --------------------------------------------------------------------------------------------
//
EXPORT_C void RXmlEngXPathExpression::Destroy()
    {
    xmlXPathFreeCompExpr(COMPILED_EXPR);
    iInternal = NULL;
    iInternal2 = NULL;
    }

// --------------------------------------------------------------------------------------------
// Evaluates this expression.
// @param[in] aContextNode The node relative to which the expression is evaluated.
// @param[in]     aResolver     Namespace-resolver object
// 
// @return Result of evaluation as an RXmlEngXPathResult object.
// May leave with KXmlEngErrXPathResult code if result is undefuned
// --------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngXPathResult RXmlEngXPathExpression::EvaluateL(
    TXmlEngNode aContextNode,
    const MXmlEngNamespaceResolver* /* aResolver */) const
    {
    xmlXPathContextPtr ctxt = xmlXPathNewContext(INTERNAL_NODEPTR(aContextNode)->doc);
    OOM_IF_NULL(ctxt);
    ctxt->node = INTERNAL_NODEPTR(aContextNode);
    ctxt->instanceDocs = INSTANCE_MAP;
    xmlXPathObjectPtr resultObj = xmlXPathCompiledEval(COMPILED_EXPR, ctxt);
    xmlXPathFreeContext(ctxt);
    if(OOM_FLAG)
        {
        xmlXPathFreeObject(resultObj);
        XmlEngLeaveL(KErrNoMemory);
        }
    if(!resultObj || resultObj->type == XPATH_UNDEFINED)
        {
        xmlXPathFreeObject(resultObj);
        XmlEngLeaveL(KXmlEngErrXPathResult);
        }
    return RXmlEngXPathResult(resultObj);
    }

// --------------------------------------------------------------------------------------------
// Does the same as #EvaluateL, but as a side-effect calculates the dependency list of the
// expression. This is meant to be used by the XForms engine.
//
// @param[in]     aContextNode  The node relative to which the expression is evaluated.
// @param[in,out] aDependents   After the method has returned, contains set of nodes that
// the expression is dependent on.
// @param[in]     aResolver     Namespace-resolver object
// 
// @return Result of evaluation as an RXmlEngXPathResult object.
// May leave with KXmlEngErrXPathResult code if result is undefuned,
// argument node set is not destroyed then.
// --------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngXPathResult RXmlEngXPathExpression::EvaluateWithDependenciesL(
    TXmlEngNode aContextNode,
    RXmlEngNodeSet& aDependents,
    const MXmlEngNamespaceResolver* /* aResolver */) const
    {
    xmlXPathContextPtr ctxt = xmlXPathNewContext(INTERNAL_NODEPTR(aContextNode)->doc);
    OOM_IF_NULL(ctxt);
    ctxt->node = INTERNAL_NODEPTR(aContextNode);
    ctxt->instanceDocs = INSTANCE_MAP;
    xmlXPathObjectPtr resultObj = xmlXPathCompiledEvalWithDependencies(
                                        COMPILED_EXPR,
                                        ctxt,
                                        static_cast<xmlNodeSetPtr>(aDependents.iInternal));
    xmlXPathFreeContext(ctxt);
    if(resultObj->type == XPATH_UNDEFINED)
        {
        xmlXPathFreeObject(resultObj);
        XmlEngLeaveL(KXmlEngErrXPathResult);
        }

    return RXmlEngXPathResult(resultObj);
    }

EXPORT_C void RXmlEngXPathExpression::SetExtendedContext(
    void* aContext )
    {
    COMPILED_EXPR->extendedContext = aContext;
    }

EXPORT_C void* RXmlEngXPathExpression::ExtendedContext() const
    {
    return COMPILED_EXPR->extendedContext;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
RXmlEngXPathExpression::RXmlEngXPathExpression(void* aData, void* aData2):iInternal(aData), iInternal2(aData2)
    {
    }
