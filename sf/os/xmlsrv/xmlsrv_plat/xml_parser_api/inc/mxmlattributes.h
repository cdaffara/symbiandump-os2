/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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






/**
 * DO NOT USE THIS API. DEPRECATED AND REMOVED IN S60 5.0. USE SYMBIAN XML FRAMEWORK INSTEAD.
 */









#ifndef __MXMLATTRIBUTES_H
#define __MXMLATTRIBUTES_H

#warning The XML API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information


//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Interface for a list of XML elemnet attributes.
*  Contains a list of attributes belonging to a certain element and is returned in the 
*  MXMLContentHandler::StartElement() callback.
*
*  @lib XMLInterface.dll
*  @since ?Series60_version
*
*/

class MXMLAttributes
{
public:
   
/**
* Look up the index of an attribute by name.
* @since ?Series60_version
* @param aName: Name of attribute to look up
* @return Index of aName
*/
	virtual TInt GetIndex(TDesC& aName) = 0;

/**
* Get the number of attributes in the list.
* @since ?Series60_version
* @param none
* @return The number of attributes in the list.
*/
	virtual  TInt GetLength() = 0;
	
/**
* Look up the name of an attribute by index.
* @since ?Series60_version
* @param aIndex: Index of attribute to look up.
* @return Name of attribute at index aIndex.
*/
	virtual TPtrC GetName(TInt aIndex) = 0;

/**
* Look up an attribute's value by name.
* @since ?Series60_version
* @param aName: Name of attribute to look up
* @return Value of attribute aName.
*/
	virtual TPtrC GetValue(TDesC& aName) = 0;

/**
* Look up an attribute's value by index.
* @since ?Series60_version
* @param aIndex: Index of attribute to look up.
* @return Value of attribute at aIndex.
*/
	virtual TPtrC GetValue(TInt& aIndex) = 0;

/**
* Look up an attribute's type by index.
* @since ?Series60_version
* @param aIndex: Index of attribute to look up.
* @return Type of attribute at aIndex.
*/
// R&D: Feature: GetType missing from Attributes
	virtual TPtrC GetType(TInt aIndex) = 0;

/**
* Look up an attribute's type by name.
* @since ?Series60_version
* @param aIndex: Index of attribute to look up.
* @return ?description
*/
// R&D: Feature: GetType missing from Attributes
	virtual TPtrC GetType(TDesC& aName) = 0;
  
};

#endif      // __MXMLATTRIBUTES_H  
            
// End of File
