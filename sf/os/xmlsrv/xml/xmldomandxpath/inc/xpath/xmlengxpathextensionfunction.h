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
// Extension function interface
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGXPATHFUNCTION_H
#define XMLENGXPATHFUNCTION_H

#include <xml/dom/xmlengxpathresult.h>

class MXmlEngXPathEvaluationContext;

/**
Provides an interface for an object to participate in the evaluation of XPath
expression via a XPath function call.

Objects that implement the MXmlEngXPathExtensionFunction interface can be
registered in a XmlEngXPathConfiguration object for global action (across
all evaluations globally). 

@see XmlEngXPathConfiguration::AddExtensionFunctionL()

When the extension function is no longer required, it must be deregistered
by calling XmlEngXPathConfiguration::RemoveExtensionFunction().
*/ 
class MXmlEngXPathExtensionFunction
    {
public:
    /** Evaluation status */
    enum TXmlEngEvaluationStatus
        {
		/** Evaluation was accomplished successfully */
        ESucceeded,
		/** An exceptional condition occurs */
        EError,
		/** Type of argument does not match function specification */
        EInvalidArgumentType,
		/** Number of arguments do not match function specification */
        EInvalidArgumentNumber
        };
        
public:
    /** 
    Gets the minimum number of arguments the function can take.
    @return Minimum number of arguments
    */
    virtual TInt MinArity() {return 0;}

    /** 
    Gets the maximum number of arguments the function can take.
    
	@return The maximum number of arguments or -1 if unbounded
    */
    virtual TInt MaxArity() {return MinArity();}

    /**
	Called to evaluate the extension function.

	This function is called only if simple arity constraints (min/max number of
	arguments) are satisfied. The implementation of this function may need to
	check further that the types of arguments and their positions in the
	function call match function-specific requirements.
    
	@param aEvalCtxt Evaluation context, which contains data about arguments
	and provides a number of methods for setting the result of the extension
	function.

    @return ESucceeded if evaluation success, otherwise one of the errors 
	defined in TXmlEngEvaluationStatus.
    */
    virtual TXmlEngEvaluationStatus Evaluate(MXmlEngXPathEvaluationContext* aEvalCtxt) = 0; 
    };

#endif /* XMLENGXPATHFUNCTION_H */

