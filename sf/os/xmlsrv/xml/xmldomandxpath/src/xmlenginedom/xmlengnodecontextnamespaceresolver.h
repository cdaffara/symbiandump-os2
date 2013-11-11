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
 @internalComponent
 @released
*/

#ifndef XMLENGNODECTXTNAMESPACERESOLVER_H
#define XMLENGNODECTXTNAMESPACERESOLVER_H

#include <xml/dom/xmlengnamespaceresolver.h>
#include <xml/dom/xmlengelement.h>

/**
 * Implementation of namespace resolver for XPath
 *
 */
class CXmlEngNodeContextNamespaceResolver: public CBase, public MXmlEngNamespaceResolver
{
public:
	/**
	 * Constructor
	 *
	 * @param aContextNode Node in which looking for namespace starts
	 */
    IMPORT_C CXmlEngNodeContextNamespaceResolver(const TXmlEngNode& aContextNode);

	/**
	 * Destructor
	 *
	 */
    IMPORT_C ~CXmlEngNodeContextNamespaceResolver();
	
	/**
     * Searches the prefix that is bound to the given aNamespaceUri and
     * applicable in the scope of this TXmlEngNode.
     *
     * @param aNamespaceUri Namespace Uri that should be found
     * @return A sought prefix or NULL if not found or aNamespaceUri is the default namespace
     */
    IMPORT_C TPtrC8 LookupPrefixL(const TDesC8& aNamespaceUri) const;

	/**
     * Searches the namespace URI that is bound to the given prefix.
     *
     * @param aPrefix Namespace prefix that should be found
     * @return A sought URI or NULL if the prefix is not bound
     */
    IMPORT_C TPtrC8 LookupNamespaceUriL(const TDesC8& aPrefix) const;
    /** @} **/
private:
	/** Namespace resolver context */
    TXmlEngElement iContextElement;
};   

#endif /* XMLENGNODECTXTNAMESPACERESOLVER_H */
