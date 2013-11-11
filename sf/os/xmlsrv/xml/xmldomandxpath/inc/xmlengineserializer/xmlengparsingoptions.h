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
// Serialization options functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGPARSINGOPTIONS_H
#define XMLENGPARSINGOPTIONS_H

#include <e32std.h>

/**
Container for parsing options used for deserialization.
*/
class TXmlEngParsingOptions 
	{
public:
    /** Encode data containers to base 64 */
    static const TUint KOptionEncodeToBase64         = 0x01;
    
    /** Provide raw binary octets */
    static const TUint KOptionRawData		         = 0x02;    

	/**
    Constructor
	@param aOptionFlags Parsing options
    */
    IMPORT_C TXmlEngParsingOptions( TUint aOptionFlags = 
			TXmlEngParsingOptions::KOptionRawData);

public:
	/** Options flag */
    TUint iOptions;
	};

#endif /* XMLENGPARSINGOPTIONS_H */

