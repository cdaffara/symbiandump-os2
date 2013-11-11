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
// XPath evaluator functions
//

#include "xmlengdomdefs.h" 
#include <xml/dom/xmlengxpathevaluator.h>
#include <xml/dom/xmlengxpathextensionfunction.h>
#include <stdapis/libxml2/libxml2_xpath.h>
#include <stdapis/libxml2/libxml2_globals.h>
#include <xml/dom/xmlengxpatherrors.h>
#include <xml/dom/xmlengnode.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>
// Default constructor
// --------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngXPathEvaluator::TXmlEngXPathEvaluator() : iInstanceTrees(NULL) 
	{
	}

// --------------------------------------------------------------------------------------------
// Precompiles and stores an XPath expression, which is returned to caller.
//
// @param[in] aExpression The expression as a string.
// @param[in] aResolver An object that can resolve of namespace prefixes into namespace URIs
// @return Compiled XPath expression.
// 
// May leave with KErrXPath code
// --------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngXPathExpression TXmlEngXPathEvaluator::CreateExpressionL(
    const TDesC8& aExpression, 
    const MXmlEngNamespaceResolver* /* aResolver */)
    {
    xmlChar* exp = xmlCharFromDesC8L(aExpression);
    xmlXPathCompExprPtr cExpr = xmlXPathCtxtCompile(NULL, exp);
    delete exp;
    if(xmlOOMFlag())
        {
        xmlXPathFreeCompExpr(cExpr);
        OOM_HAPPENED;
        }
    if(!cExpr)
        {
        XmlEngLeaveL(KXmlEngErrXPathSyntax);
        }

    return RXmlEngXPathExpression(cExpr, iInstanceTrees);
    }
    
// --------------------------------------------------------------------------------------------
// Evaluates the XPath expression given as parameter. The result is returned.
//
// @param[in] aExpression The expression as a string.
// @param[in] aContextNode The node relative to which the expression is evaluated. 
// @param[in] aResolver (for future use) 
// @return Result of evaluation as an RXmlEngXPathResult object.
// May leave with KXmlEngErrXPathSyntax code for invalid XPathExpression and KXmlEngErrXPathResult 
// for undefined result of evaluation.
// --------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngXPathResult TXmlEngXPathEvaluator::EvaluateL(
    const TDesC8& aExpression, 
    const TXmlEngNode aContextNode, 
    const MXmlEngNamespaceResolver* /* aResolver */ )
    {
    RXmlEngXPathExpression expr = CreateExpressionL(aExpression, NULL);
    CleanupClosePushL(expr);
    
    RXmlEngXPathResult result = expr.EvaluateL(aContextNode);
    CleanupStack::PopAndDestroy(&expr);
    return result;
    }

// --------------------------------------------------------------------------------------------
// Does the same as #evaluate, but as a side-effect calculates the dependency list of the 
// expression. This is meant to be used by the XForms engine.
//
// @param[in] aExpression The expression as a string.
// @param[in] aContextNode The node relative to which the expression is evaluated. 
// @param[in] aResolver (for future use) 
// @param[in,out] aDependents After the method has returned, contains set of nodes that 
// the expression is dependent on.
// @return Result of evaluation as an RXmlEngXPathResult object.
// 
// May leave with KXmlEngErrXPathResult or KXmlEngErrXPathSyntax codes.
// --------------------------------------------------------------------------------------------
//
EXPORT_C RXmlEngXPathResult TXmlEngXPathEvaluator::EvaluateWithDependenciesL(
    const TDesC8& aExpression, 
    const TXmlEngNode aContextNode, 
    const MXmlEngNamespaceResolver* /* aResolver */,
    RXmlEngNodeSet& aDependents )
    {
    RXmlEngXPathExpression expr = CreateExpressionL(aExpression, NULL);
    CleanupClosePushL(expr);
    RXmlEngXPathResult result = expr.EvaluateWithDependenciesL(aContextNode, aDependents);
    CleanupStack::PopAndDestroy(&expr);
    return result;
    }

// --------------------------------------------------------------------------------------------
// Registers the instance map to be used when evaluating XForms expressions. The map 
// contains DOM Documents and their names. For example, when evaluating the expression 
// "instance('a')" the evaluator looks up a Document that has the name "a" from the map. 
// Method does not take owership, caller has to free the map.
//
// @param[in] aInstanceTrees The instance map.
// --------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngXPathEvaluator::SetInstanceMap(
    CXmlEngXFormsInstanceMap* aInstanceTrees )
    {
    iInstanceTrees = aInstanceTrees;
    }

// -----------------------------------------------------------------------------
// Returns the registered instance map.
// @return pointer instance map or NULL if not registered.
// -----------------------------------------------------------------------------
//
EXPORT_C CXmlEngXFormsInstanceMap* TXmlEngXPathEvaluator::InstanceMap()
    {
    return iInstanceTrees;
    }
