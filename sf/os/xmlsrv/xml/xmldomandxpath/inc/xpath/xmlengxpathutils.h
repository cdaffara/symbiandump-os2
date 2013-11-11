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
// XPath Utils
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGXPATHUTILS_H
#define XMLENGXPATHUTILS_H

#include <xml/dom/xmlengnodeset.h>

/**
Implements utility functions such as conversion methods for XPath objects.

The XPath specification considers a node result as a set of only one node,
however, in this API it is possible to perform conversion routines 
directly on nodes.
*/
class XmlEngXPathUtils
    {
public:
	/**
	Converts node to number
	@param aNode Node to be converted
	@return The number represented by the node or NaN if cannot convert.
	@leave KErrNoMemory Memory allocation failure
	*/    
    IMPORT_C static TReal ToNumberL(const TXmlEngNode& aNode);

	/**
	Converts a node-set to a number by taking the node in the node set that
	appears first in the document and converting that to a number.

	@param aNodeSet Node-set to be converted	 	 
	@return The number represented by the node set or NaN if cannot convert.
	@leave KErrNoMemory Memory allocation failure
	*/    
    IMPORT_C static TReal ToNumberL(const RXmlEngNodeSet& aNodeSet);

	/**
	Converts TBool value to TReal
	@param aValue TBool value to be converted
	@return 1.0 if ETrue and 0.0 otherwise
	@leave - Does not leave
	*/     
    IMPORT_C static TReal ToNumberL(TBool aValue);
    
	/**
	Converts a string to a number
	@param aString String to be converted
	@return The number respresented by the string or NaN if cannot convert.
	@leave KErrNoMemory Memory allocation failure
	*/     
    IMPORT_C static TReal ToNumberL(const TDesC8& aString);

	/**
	Converts a node to a string.  Any existing text in aOutput is destroyed.
	This method allocates memory for the buffer.

	@param aNode The node to be converted
	@param aOutput A buffer owned by the caller that holds the returned string
	@leave KErrNoMemory Memory allocation failure
	*/ 
    IMPORT_C static void ToStringL(const TXmlEngNode& aNode, RBuf8& aOutput );

	/**
	Converts a node-set to a string by taking the node in the node set that
	appears first in the document and converting that to a string.  Any
	existing text in aOutput is destroyed.  This method allocates memory for
	the buffer.

	@param aNodeSet The node-set to be converted
	@param aOutput A buffer owned by the caller that holds the returned string
	@leave KErrNoMemory Memory allocation failure
	*/     
	IMPORT_C static void ToStringL(const RXmlEngNodeSet& aNodeSet, RBuf8&
			aOutput );

	/**
	Converts a TBool value to a string.  Any existing text in aOutput is
	destroyed.  This method allocates memory for the buffer.

	@param aValue The TBool value to be converted
	@param aOutput A buffer owned by the caller that holds the returned string.
	"true" if ETrue and "false" otherwise.
	@leave KErrNoMemory Memory allocation failure
	*/     
    IMPORT_C static void ToStringL(TBool aValue, RBuf8& aOutput );

	/**
	Converts a TReal value to a string.  Any existing text in aOutput is
	destroyed.  This method allocates memory for the buffer.

	@param aValue The TReal value to be converted
	@param aOutput A buffer owned by the caller that holds the returned string.
	@leave KErrNoMemory Memory allocation failure
	*/      
    IMPORT_C static void ToStringL(TReal aValue, RBuf8& aOutput );

	/**
	Converts a node to a TBool
	@param aNode Node to be converted
	@return ETrue if the node is not NULL, EFalse otherwise
	*/
    IMPORT_C static TBool ToBoolean(const TXmlEngNode& aNode);

	/**
 	Converts a node-set to a TBool
	@param aNodeSet Node-set to be converted
	@return ETrue if the nodeset is not NULL, EFalse otherwise
	*/    
    IMPORT_C static TBool ToBoolean(const RXmlEngNodeSet& aNodeSet);

	/**
	Converts a string to a TBool
	@param aValue String that should be converted
	@return ETrue if the string is not an empty string, EFalse otherwise
	*/    
    IMPORT_C static TBool ToBoolean(const TDesC8& aValue);

	/**
	Converts a string to a TBool
	@deprecated This function is deprecated and will be removed in future
	releases.  ToBoolean() should be used instead.
	@param aValue String that should be converted
	@return ETrue if the string is not an empty string, EFalse otherwise
	@leave - Does not leave
	*/    
    IMPORT_C static TBool ToBooleanL(const TDesC8& aValue);

	/**
	Converts a TReal value to a TBool
	@param aValue TReal value to be converted
	@return EFalse if aValue is NaN or 0.0, ETrue otherwise
	*/    
    IMPORT_C static TBool ToBoolean(TReal aValue);

private:
	/** Default constructor */ 
    inline XmlEngXPathUtils();
    };	

#include <xml/dom/xmlengxpathutils.inl>

#endif /* XMLENGXPATHUTILS_H */

