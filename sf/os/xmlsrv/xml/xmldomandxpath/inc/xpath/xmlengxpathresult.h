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
// Result of XPath expression
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGXPATHRESULT_H
#define XMLENGXPATHRESULT_H

#include <xml/dom/xmlengnodeset.h>

/**
Represents the result of a XPath expression.

The result may be one of the standard types: string, boolean, number (float), 
node set or be undefined in the case of error.

The returned result should be freed after use.
*/
class RXmlEngXPathResult
{
	friend class RXmlEngXPathExpression;
    friend class TXmlEngXPathEvaluationContextImpl;

public:
    /**
	Result types of a XPath expression.
	See http://w3.org/TR/xpath#section-Introduction
    */
    enum TXmlEngXPathResultType
        {
        EUndefined = 0,
        ENodeset = 1,
        EBoolean = 2,
        ENumber = 3,
        EString = 4
        //Other node types defined in XPath are not supported.
	    } ;

public:
	/** Default constructor */
	IMPORT_C RXmlEngXPathResult();

	/** Frees any allocated memory */
	IMPORT_C void Free();

	/** Frees any allocated memory */
	inline void Close();

	/**
	Retrieves the type of the result (e.g. boolean, number, string, etc.).  
	The type of a result should always be checked before using the result.
	
	If there was an error and no value was set, undefined is returned.
	Otherwise nodeset is returned.	
	
	@return Type of result.
	
	@see TXmlEngXPathResultType
	*/
	IMPORT_C TXmlEngXPathResultType Type() const;
	
	/**
	Retrieves a node-set result.  A Non-RXmlEngNodeSet result will return NULL.
	No conversion is done from other types.
	
	For more information no NULL nodes:
	@see TXmlEngNode
	
	@return Node-set result
	*/
	IMPORT_C RXmlEngNodeSet AsNodeSet() const;
	
	/**
	Retrieves a string copy of result.
	@param A buffer allocated by the caller that holds the result string
	@leave KErrNoMemory if out of memory situation occurs.
	@leave - Otherwise any of the system wide errors.
	*/	
	IMPORT_C void AsStringL(RBuf8& aOutput) const;

	/**
	Retrieves a numeric value result.  Conversion can occur from other types.
	 
	Node sets, strings, and booleans will be cast to numbers.  Anything else
	will return NAN (i.e. not a number).
	
	@return Numeric value of the result.
	
	@see Math
	*/	
	IMPORT_C TReal AsNumber() const;
	
	/**
	Retrieves a boolean result.  Conversion can occur from other types.
	
	Numbers will be returned as true unless it is not a number or 0.0.
	A string of 0 length will return false, otherwise true.
	If the node exists it will be returned as true, otherwise false.
	
	@return Boolean value of the result
	*/	
	IMPORT_C TBool AsBoolean() const;

private:
	/**
	Constructor
	@param aData Internal representation of XPath result
	*/
	RXmlEngXPathResult(void* aData);
	
private: 
	void* iInternal;
    };	

#include <xml/dom/xmlengxpathresult.inl>

#endif /* XMLENGXPATHRESULT_H */

