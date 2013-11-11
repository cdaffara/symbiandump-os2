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
// XPath evaluation context interface
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGXPATHEVALCTXT_H
#define XMLENGXPATHEVALCTXT_H

#include <xml/dom/xmlengxpathresult.h>
#include <xml/dom/xmlengelement.h>

/** 
Provides the XPath evaluation context interface.  An object of this type
provides the argument values to an extension function to be evalutated 
and stores the result of evalutation.

@see MXmlEngXPathExtensionFunction
@see MXmlEngXPathExtensionFunction::Evaluate()

Objects of this type are not registered with the XPath library and must
be destroyed by their owner.

Any memory allocated by this object must be freed when the object is
destroyed.
*/
class MXmlEngXPathEvaluationContext
    {
public:
    /** 
    Gets the number of arguments provided.
    @return The number of arguments provided
    */
    virtual TUint ArgCount() = 0;

	/** 
	Gets an argument by index. Does not transfer ownership. Index starts at 0.
	@pre 0 <= aIndex < ArgCount().  Function panics if precondition not met.
    @return The n-th argument of the function
    */
    virtual const RXmlEngXPathResult Argument(TUint aIndex) = 0;

    /** 
    Gets the result written so far.  The result is undefined before SetResult(),
	InitializeNodeSetResult() or AppendToResult() have been called.
    
	@pre SetResult() or InitializeNodeSetResult() and AppendToResult() have
	been previously called.
    @return The result evaluated by the function so far.
    */
    virtual const RXmlEngXPathResult Result() = 0;
    
	/** 
	Sets the type of result to node set and initializes a new node set.  Any
	existing result node set is replaced.  

	@see AppendToResult(const TXmlEngNode)
	@see AppendToResult(const RXmlEngNodeSet)
	@see AppendToResult(const TXmlEngNamespace, const TXmlEngElement)
    */ 
	virtual void InitializeNodeSetResult() = 0;  

	/** 
	Appends a node to the node set result.  InitializeNodeSetResult() should be
	called prior to the first call to an AppendToResult() method.
	
	This method will return and do nothing if a previous OOM condition in the
    underlying libxml2 library has not been cleared. See libxml2_modules.h.
     
	@pre OOM_FLAG==0, otherwise method returns and does nothing.
    @param aNode Node to be added
    */ 
    virtual void AppendToResult(const TXmlEngNode aNode) = 0;

	/**
	Appends a namespace node to the node set result.  InitializeNodeSetResult()
	should be called prior to the first call to an AppendToResult() method.

	A namespace node is the representation of an existing namespace declaration
	within an element node.  This function cannot be used to add a new namespace
	declaration, rather it adds an existing namespace declaration attached to
	aNsParentNode.

	@see TXmlEngNamespace

	This method will return and do nothing if a previous OOM condition in the
    underlying libxml2 library has not been cleared. See libxml2_modules.h.
     
	@pre OOM_FLAG==0, otherwise method returns and does nothing.
    @param aAppendedNsNode A namspace node to add to the node set result.
	@param aNsParentNode An element node that holds the namespace declaration
	represented by aAppendedNsNode.
    */
    virtual void AppendToResult(const TXmlEngNamespace aAppendedNsNode, 
                                const TXmlEngElement aNsParentNode) = 0;

	/** 
	Appends a node set to the node set result.  Nodes are merged into a
	resulting node set.  InitializeNodeSetResult() should be called prior to
	the first call to an AppendToResult() method.
    
    This method will return and do nothing if a previous OOM condition in the
    underlying libxml2 library has not been cleared. See libxml2_modules.h.
     
	@pre OOM_FLAG==0, otherwise method returns and does nothing.
    @param aNodeSet The node set to be appended
    */
    virtual void AppendToResult(const RXmlEngNodeSet aNodeSet) = 0;
    
	/** 
	Sets the type of result to floating point number and stores the number as
	the result.
    @param aNumber The number to store as the result
    */ 
    virtual void SetResult(TReal aNumber) = 0;
    
    /** 
	Sets the type of result to boolean and stores the value as the result.
    @param aBoolean The value to store as the result
    */
    virtual void SetResult(TBool aBoolean) = 0;

    /** 
	Sets the type of result to string and stores the string as the result.
    @param aString The string to store as the result
    */
    virtual void SetResultL(const TDesC8& aString) = 0;
    
    /** 
    Gets the data specified in RXmlEngXPathExpression::SetExtendedContext() 
    for the expression currently being evaluated.
    */    
    virtual void* ExtendedContext() = 0;
    };

#endif /* XMLENGXPATHEVALCTXT_H */

