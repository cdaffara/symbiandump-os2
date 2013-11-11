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
// XPath evaluation context declaration
//



/**
 @file
 @internalComponent
 @released
*/
#ifndef XMLENGXPATHEVALCTXT_IMPL_H
#define XMLENGXPATHEVALCTXT_IMPL_H

#include <xml/dom/xmlengxpathevaluationcontext.h>
#include <libxml2_xpathinternals.h>

/**
 * XPath evaluation context
 *
 */
class TXmlEngXPathEvaluationContextImpl: public MXmlEngXPathEvaluationContext
    {
public:
	/**
     * Constructor
	 * 
	 * @param aContext Pointer to XPath parser context
	 * @param aArgCount Number of arguments
	 */
    TXmlEngXPathEvaluationContextImpl(xmlXPathParserContextPtr aContext, TUint aArgCount)
        : iContext(aContext), 
          iArgCount(aArgCount), 
          iCurrentResult(NULL),
          iOOM(EFalse) {}

	/**
     * Get number of arguments provided
     *
     * @return Number of arguments provided
     *
     * @note 
     *    The implementation of extension function should not check
     *    that ArgCount() returns value in the range [ MinArity() ; MaxArity() ]
     *    - the function is called iif the statement holds:<br>
     *      <b>MinArity()</b>  =<  <i>arg.count</i>  =< <b>MaxArity()</b> 
     */
    TUint ArgCount();

	/**
     * Retrieves arguments by index (zero-based)
     *
     * @return N-th argument of the function
     *
     * @note
     *    - Argument index starts at 0
     *    - Returned result should not be freed
     */
    const RXmlEngXPathResult Argument(TUint aIndex);

	/**
     * Get the result written so far.
     * 
     * @return Result evaluated by the function so far.
     *
     * Initially, function result is undefined. It can be changed
     * by SetResult() functions for a boolean, string or number values 
     * or by combined use of InitializeNodeSetResult() and AppendToResult() methods
     */
    const RXmlEngXPathResult Result();

	/**
     * Sets a NODESET type of the result (floating-point number): an empty
     * instance of NodeSet is created.
     *
     * The node set is to be filled up by methods AppendToResult(TXmlEngNode) and AppendToResult(RXmlEngNodeSet)
     * This method replaces any existing prepared result so far 
     * (even an initialized already node set)
     * @see AppendToResult(TXmlEngNode), AppendToResult(RXmlEngNodeSet)
     *
     */ 
    void  InitializeNodeSetResult();     

	/**
     * Appends a node to the node set result.
     * @note InitializeNodeSetResult() should be called prior any call to 
     *       AppendToResult(TXmlEngNode) or AppendToResult(RXmlEngNodeSet)
     *
     * @param aNode Node that should be add
     */ 
    void  AppendToResult(const TXmlEngNode aNode);
    void  AppendToResult(const TXmlEngNamespace aAppendedNsNode, const TXmlEngElement aNsParentNode);
	/**
    * Appends a node set to the node set result. Nodes are merged into resulting node set.
    * @note InitializeNodeSetResult() should be called prior any call to 
    *       AppendToResult(TXmlEngNode) or AppendToResult(RXmlEngNodeSet)
    *
    * @param aNodeSet Nodes that should be add
    */
    void  AppendToResult(const RXmlEngNodeSet aNodeSet);

	/**
     * Sets a NUMBER type of the result (a floating-point number)
     *
     * @param aNumber New number
     */ 
    void  SetResult(TReal aNumber);

	/**
     * Sets a BOOLEAN type of the result 
     *
     * @param aBoolean New value
     */
    void  SetResult(TBool aBoolean);

	/**
     * Sets a STRING type of the result
     *
     * @param aString New string value
     */
    void  SetResultL(const TDesC8& aString);

	/**
     * Gets data specified in RXmlEngXPathExpression::SetExtendedContext(void*) method
     * for the expression currently being evaluated.
     *
     */ 
    void* ExtendedContext();

    /**
     * Allocates memory for function result
     * 
     * @return TRUE is succeeded, FALSE if OOM has happened
     */
    TBool Initialize();

private:
	/** XPath parser */
    xmlXPathParserContextPtr iContext;
	/** Arguments count */
    TUint iArgCount;
	/** XPath object */
    xmlXPathObjectPtr iCurrentResult;    
	/** OOM flag */
    TBool iOOM;
    };

#endif /* XMLENGXPATHEVALCTXT_IMPL_H */
