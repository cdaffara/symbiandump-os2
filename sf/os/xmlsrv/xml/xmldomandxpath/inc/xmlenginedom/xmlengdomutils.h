/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/** @file
@publishedAll
@deprecated
*/
#ifndef XMLENGDOMUTILS_H
#define XMLENGDOMUTILS_H

#include <xml/dom/xmlengelement.h>

/**
@deprecated This class will be removed in future releases.  See
TXmlEngElement::RenameL() for a replacement.
*/
class TDomUtils
{
  public:
  
	/**
	Renames a TXmlEngElement node.
	@param aElement Element node
	@param aLocalName New name
	@param aNamespaceUri New namespace uri
	@param aPrefix New namespace prefix
	@leave - One of the system-wide error codes
	*/
	IMPORT_C void  XmlEngRenameElementL(TXmlEngElement aElement, const TDesC8& aLocalName, 
                               const TDesC8& aNamespaceUri, 
                               const TDesC8& aPrefix);  
                                   
    
};


#endif /* XMLENGDOMUTILS_H */
