// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Processing instruction node functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGPROCESSINGINSTRUCTION_H
#define XMLENGPROCESSINGINSTRUCTION_H

#include <xml/dom/xmlengnode.h>



/**
This class represents an XML processing instruction in the DOM tree.  This class
is usually instantiated through RXmlEngDocument::CreateProcessingInstruction().
*/
class TXmlEngProcessingInstruction : public TXmlEngNode
{
  public:
    /** Default constructor */
	inline TXmlEngProcessingInstruction();
    
    /**
	Gets a reference to the the target part of the processing instruction.

    @code
        <?target data?>
    @endcode

    @see http://www.w3.org/TR/2004/REC-xml-20040204/#sec-pi
    @return A reference to the target part
    */
    IMPORT_C TPtrC8 Target() const;

    /**
    Gets a reference to the data part of the processing instruction.

    @code
        <?target data?>
    @endcode

    @see http://www.w3.org/TR/2004/REC-xml-20040204/#sec-pi
    @return A reference to the data part
    */
    IMPORT_C TPtrC8 Data() const;

    /**
	Copies the parameter and sets the data part of the processing instruction.
	Note: The processing instruction contents should not contain the "?>"
	sequence
    @param aData The data part to set
	@leave - One of the system-wide error codes
    */
    IMPORT_C void SetDataL(const TDesC8& aData);

protected:
    /**
    Constructor
    @param aInternal Processing instruction pointer
    */
	inline TXmlEngProcessingInstruction(void* aInternal);
};


#include <xml/dom/xmlengprocessinginstruction.inl>
#endif /* XMLENGPROCESSINGINSTRUCTION_H */

