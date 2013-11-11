// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains generic XML constants
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __GMXMLDOMCONSTANTS_H__
#define __GMXMLDOMCONSTANTS_H__

#include <e32base.h>


// Error codes
/**
* Starting value for all the component's error codes.
* @publishedPartner
* @released
*/
const TInt KErrXMLBase = -9000;
/**
* XML attribute value error code. 
* @publishedPartner
* @released
*/
const TInt KErrXMLBadAttributeValue =			KErrXMLBase - 1;
/**
* XML attribute name error code. 
* @publishedPartner
* @released
*/
const TInt KErrXMLBadAttributeName = 			KErrXMLBase - 2;
/**
* Invalid XML child element error code. 
* @publishedPartner
* @released
*/
const TInt KErrXMLInvalidChild = 				KErrXMLBase - 3;



_LIT(KXMLDocumentElementNodeName, "documentelement");


#endif
