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
// XPath evaluator declaration
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGXPATHEVALUATOR_H
#define XMLENGXPATHEVALUATOR_H

#include <xml/dom/xmlengxpathexpression.h>
#include <xml/dom/xmlengxformsinstancemap.h>
#include <xml/dom/xmlengnamespaceresolver.h>

class MXmlEngXPathExtensionFunction;

class MXmlEngNamespaceResolver;
/**
Evaluates XPath expressions on a DOM tree.

See the XPath specfication here:
http://www.w3.org/TR/2004/NOTE-DOM-Level-3-XPath-20040226/xpath.html
*/
class TXmlEngXPathEvaluator
{
public:
    /** Default constructor */
    IMPORT_C TXmlEngXPathEvaluator();

	// XPath 1.0 methods
	
    /**
    Precompiles and stores an XPath expression, which is returned to caller.

	Information on how to write XPath expressions can be found here:
	@see http://en.wikipedia.org/wiki/XPath
		 
    @param aExpression The expression to create
    @param aResolver Not supported in current API.  Reserved for future use.
    @return The compiled XPath expression.
	@leave KXmlEngErrXPathSyntax Error compiling expression
	@leave KErrNoMemory Memory allocation error
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C RXmlEngXPathExpression CreateExpressionL(const TDesC8& aExpression,
                                   const MXmlEngNamespaceResolver* aResolver = NULL);

    /**
    Evaluates the XPath expression given as a parameter and returns the result.
    
    @param aExpression The expression to evaluate
    @param aContextNode The node relative to which the expression is evaluated. 
    @param aResolver Not supported in current API. Reserved for future use.
    @return The result of the evaluation.
    @leave KXmlEngErrXPathResult Error evaluating expression
	@leave KXmlEngErrXPathSyntax Error compiling expression
	@leave KErrNoMemory Memory allocation error
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C RXmlEngXPathResult EvaluateL(const TDesC8& aExpression,
                                   const TXmlEngNode aContextNode,
                                   const MXmlEngNamespaceResolver* aResolver = NULL);


	// Extensions to XPath 1.0

	/**
	Does the same thing as EvaluateL(), but also calculates the dependency list
	of the expression.
    
    @param aExpression The expression to evaluate
    @param aContextNode The node relative to which the expression is evaluated. 
    @param aResolver Not supported in current API. Reserved for future use.
    @param aDependents After the method has returned, contains set of nodes that 
    the expression is dependent on.
    @return The result of evaluation

	@leave KXmlEngErrXPathResult Error evaluating expression
	@leave KXmlEngErrXPathSyntax Error compiling expression
	@leave KErrNoMemory Memory allocation error
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C RXmlEngXPathResult EvaluateWithDependenciesL(const TDesC8& aExpression,
                                   const TXmlEngNode aContextNode,
                                   const MXmlEngNamespaceResolver* aResolver,
                                   RXmlEngNodeSet& aDependents);

    /**
    Registers the instance map to be used when evaluating form expressions. The map 
    contains DOM Documents and their names. For example, when evaluating the expression 
    "instance('a')" the evaluator looks up a document that has the name "a" from the map. 

    @param aInstanceTrees The instance map, ownership is not transferred.
    */
    IMPORT_C void SetInstanceMap(CXmlEngXFormsInstanceMap* aInstanceTrees);

    /**
    Returns the registered instance map.  Ownership is not transferred.
    @return A pointer to the instance map or NULL if not registered.
    */
    IMPORT_C CXmlEngXFormsInstanceMap* InstanceMap();

private:
    CXmlEngXFormsInstanceMap* iInstanceTrees;
};



#endif /* XMLENGXPATHEVALUATOR_H */

