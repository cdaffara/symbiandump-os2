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
// Node set functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGNODESET_H
#define XMLENGNODESET_H

#include <xml/dom/xmlengnode.h>

/**
This class implements the node set container, which is one of the result type
in XPath.
*/
class RXmlEngNodeSet
{
    friend class TXmlEngXPathEvaluator;
    friend class RXmlEngXPathResult;
    friend class RXmlEngXPathExpression;

public:
    /** Default constructor */
    IMPORT_C RXmlEngNodeSet();

    /**
    Frees any allocated resources.
    
	As a node set only refers to existing nodes in the DOM tree, no nodes are
	freed.  However, namespace declarations are copied into the node set and
	these copies are indeed freed.
    */
    IMPORT_C void Free(); 

    /** Closes the node set.  This simply calls Free().  */
    inline void Close();

	/**
    Initializes the node set to an empty state.

	@see Free()
    
	This method is used when preparing node sets that will be used with
	RXmlEngXPathExpression::EvaluateWithDependenciesL(TXmlEngNode,RXmlEngNodeSet&)
	and
	TXmlEngXPathEvaluator::EvaluateWithDependenciesL(aExpression,aContextNode,aResolver,aDependents).
	
	@leave KErrNoMemory Memory allocation failure
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C void InitializeL();

    /**
    Gets the size of the node set
    @return The number of nodes
    */
    IMPORT_C TInt Length() const;

    /**
    Checks whether a node is in the node set.
    
    @param aNode The node to check
    @return ETrue if the node is in the node set, EFalse otherwise
    */
    IMPORT_C TBool Contains(TXmlEngNode aNode) const;

    /**
    Retrieves a node from the node set by index
    
    @param aIndex Node index ( 0 <= aIndex < Length() )
    @return The node
	@leave KXmlEngErrWrongUseOfAPI aIndex is less than 0 or greater than
	Length(), in debug builds only.
	@leave - Otherwise, any of the system wide errors.
    */
    IMPORT_C TXmlEngNode operator[](TInt aIndex) const;
    
private:
	/** 
	Constructor 
	@param aData Internal data pointer
	*/	 
    RXmlEngNodeSet(void* aData);
    
private:
    void* iInternal;

};    

#include <xml/dom/xmlengnodeset.inl>

#endif /* XMLENGNODESET_H */

