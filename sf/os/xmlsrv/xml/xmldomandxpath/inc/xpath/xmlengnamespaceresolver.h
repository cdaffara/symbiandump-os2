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
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGNAMESPACERESOLVER_H
#define XMLENGNAMESPACERESOLVER_H

/** 
Provides an interface to query the namespace context of a DOM object.
@see TXmlEngXPathEvaluator
@see RXmlEngXPathExpression
*/
class MXmlEngNamespaceResolver
{
public:
    /** 
    Searches for the prefix that is bound to the given aNamespaceUri and
    applicable in the scope of this object.
    
    @param aNamespaceUri Namespace URI to search for
	@return The sought prefix or an empty string if not found or if aNamespaceUri is the
	default namespace
	@leave - Any system wide error code
    */
    virtual TPtrC8 LookupPrefixL(const TDesC8& aNamespaceUri) const = 0;

    /** 
    Searches for the namespace URI that is bound to the given prefix.

    @param aPrefix The namespace prefix to search for
    @return The sought URI or an empty string if the prefix is not bound
	@leave - Any system wide error code
    */
    virtual TPtrC8 LookupNamespaceUriL(const TDesC8& aPrefix) const = 0;
};    

#endif /* XMLENGNAMESPACERESOLVER_H */

