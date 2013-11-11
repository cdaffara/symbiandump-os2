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
* Description:  ?Description
*
*/



#ifndef __CXMLATTRIBUTES_H
#define __CXMLATTRIBUTES_H

//  INCLUDES
#include <e32std.h>
#include <badesca.h>  
#include <xml/mxmlattributes.h>


// CLASS DECLARATION

/**
*  Implementation of MXMLAttributes interface.
*  @lib XMLInterface.dll
*  @since ?Series60_version
*/
class CXMLAttributes : public CBase, public MXMLAttributes 
{
public:  // Constructors and destructor
        
 /**
 * Two-phased constructor.
 */
	static CXMLAttributes* NewL();

	static CXMLAttributes* NewLC();
        
/**
* Destructor.
*/
	virtual ~CXMLAttributes();

public: // Implementation of MXMLAttributes Functions 
        
/**
* Implementation of MXMLAttributes::GetIndexL(TDesC& aName)
*/
	TInt GetIndex(TDesC& aName);

/**
* Implementation of MXMLAttributes::GetLenght()
*/
	TInt GetLength();
	
/**
* Implementation of MXMLAttributes::GetNameL(TInt aIndex).
*/
	TPtrC GetName(TInt  aIndex);

/**
* Implementation of MXMLAttributes::GetValueL(TDesC& aName, TDesC& aValue).
*/
	TPtrC GetValue(TDesC& aName);

/**
* Implementation of MXMLAttributes::GetValueL(TInt& aIndex, TDesC& aValue).
*/
	TPtrC GetValue(TInt& aIndex);

/**
* Implementation of MXMLAttributes::GetTypeL(Tint aIndex, TDesC& aType).
*/
// R&D: Feature: GetType missing from Attributes
	TPtrC GetType(TInt aIndex);

/**
* Implementation of MXMLAttributes::GetTypeL(TDesC& aName, TDesC& aType).
*/
// R&D: Feature: GetType missing from Attributes
	TPtrC GetType(TDesC& aName);

public: // New Functions		
		
/**
* Appends new attribute to attrinbuteslist.
* @since ?Series60_version
* @param aName: Attribute name.
* @param aValue: Attribute value.
* @return none.
*/
	void AppendL(const TDesC& aName, const TDesC& aValue);

/**
* Resets the attribute list.
* @since ?Series60_version
* @param aName: Attribute name.
* @param aValue: Attribute value.
* @return none.
*/
	void Reset();

protected:
/**
 * C++ default constructor.
 */
	CXMLAttributes();

private:

	void ConstructL();

private:    // Data
		
	// Store Attribute Names
	CDesCArraySeg* iDescAttName;
	// Store Attribute Values
	CDesCArraySeg* iDescAttValue;
};

#endif     // __CXMLATTRIBUTES_H  
            
// End of File
