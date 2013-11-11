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
// XPath expression declaration
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGXPATHEXPRESSION_H
#define XMLENGXPATHEXPRESSION_H

#include <xml/dom/xmlengxpathresult.h>
#include <xml/dom/xmlengnamespaceresolver.h>

class TXmlEngNode;

/**
A compiled XPath expression is a textual expression transformed after
syntactical analysis into some internal representation of the expression.  The
only missing element for evaluation of the expression is evaluation context:
the context node (where to start) and an optional set of prefix-to-namespace bindings.

XPath Expressions are created and instantiated by the Evaluator:
@see TXmlEngXPathEvaluator
 
See the XPath spec here:
http://www.w3.org/TR/2004/NOTE-DOM-Level-3-XPath-20040226/xpath.html
*/
class RXmlEngXPathExpression
{
    friend class TXmlEngXPathEvaluator;

public:
	/** XPath expression parts */
    enum TXmlEngXPathExpressionPartType
        {
	    EFunction,
        EPrefix
        };

public:
    /** Default constructor */
    IMPORT_C RXmlEngXPathExpression();
	
	/** Resets the compiled expression and frees the allocated memory */
    IMPORT_C void Destroy();
    
    /** Resets the compiled expression and frees the allocated memory */
    inline void Close();

	// XPath 1.0 methods

    /**
    Evaluates this expression.
    
    @param aContextNode The node relative to which the expression is evaluated.
    @param aResolver Not supported in current API.  Reserved for future use.
    @return Result of evaluation
    @leave KXmlEngErrXPathResult Error evaluating expression
	@leave KXmlEngErrXPathSyntax Error compiling expression
	@leave KErrNoMemory Memory allocation error
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C RXmlEngXPathResult EvaluateL(TXmlEngNode aContextNode, 
                                    const MXmlEngNamespaceResolver* aResolver = NULL) const;


	// Extensions to XPath 1.0

	/**
	Does the same thing as EvaluateL(), but also calculates the dependency list
	of the expression.
    
    @param aContextNode The node relative to which the expression is evaluated.
    @param aDependents After the method has returned, contains set of nodes that
    the expression is dependent on.
    @param aResolver Not supported in current API.  Reserved for future use.
    @return Result of evaluation 
    @leave KXmlEngErrXPathResult Error evaluating expression
	@leave KXmlEngErrXPathSyntax Error compiling expression
	@leave KErrNoMemory Memory allocation error
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C RXmlEngXPathResult EvaluateWithDependenciesL(TXmlEngNode aContextNode, 
                                                    RXmlEngNodeSet& aDependents, 
                                                    const MXmlEngNamespaceResolver* aResolver = NULL) const;

	/**
	Set context data for the expression currently being evaluated.
	 
	Extended Context is data set by API users and available for XPath extension
	functions.  For more information:
	@see RXmlEngXPathExpression
	 
	@param aContext Extended context pointer
    */ 
    IMPORT_C void SetExtendedContext(void* aContext);

	/**
	Gets extended context data for the expression currently being evaluated.	 
	 
	@return Extended context pointer or NULL if not sete
	 
	@see SetExtendedContext
    */ 
    IMPORT_C void* ExtendedContext() const;
    
private:
    RXmlEngXPathExpression(void* aData, void* aData2);

private:
    void* iInternal;
    void* iInternal2;
    TInt32 reserved1; 
    TAny* reserved2; 
};  

#include <xml/dom/xmlengxpathexpression.inl>

#endif /* XMLENGXPATHEXPRESSION_H */

