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

#ifndef XMLENGSERIALIZATIONOPTIONS_H
#define XMLENGSERIALIZATIONOPTIONS_H

#include <e32base.h>

class MXmlEngOutputStream;
class MXmlEngDataSerializer;
class MXmlEngNodeFilter;

/**
This class stores serialization options to be used when a document is
serialized.
@see RXmlEngDocument::SaveL()
*/
class TXmlEngSerializationOptions 
	{
public:
    /** Use indent in output */
    static const TUint KOptionIndent                 = 0x01;
    /** Skip xml declaration */
	static const TUint KOptionOmitXMLDeclaration     = 0x02;
	/** Add standalone in xml declaration. KOptionOmitXMLDeclaration must not be set */
	static const TUint KOptionStandalone             = 0x04;
	/** Add encoding in xml declaration. KOptionOmitXMLDeclaration must not be set */
	static const TUint KOptionEncoding               = 0x08;
	/** Not supported */
	static const TUint KOptionIncludeNsPrefixes      = 0x10;
	/** Not supported */
	static const TUint KOptionCDATASectionElements   = 0x20;
	/** Decode base64 encoded binary containers content upon XOP serialization */
	static const TUint KOptionDecodeBinaryContainers		 = 0x40;	
    
	/**
    Constructor
	@param aOptionFlags Serialization options
	@param aEncoding Serialization encoding.  If KNullDesC8, the document
	encoding is used.
    */
    IMPORT_C TXmlEngSerializationOptions( TUint aOptionFlags = 
    								   (TXmlEngSerializationOptions::KOptionIndent | 
    									TXmlEngSerializationOptions::KOptionStandalone |
    									TXmlEngSerializationOptions::KOptionEncoding), 
                                    const TDesC8& aEncoding = KNullDesC8);

	/**
	Sets a node filter.  The node filter is used to choose which nodes from the
	tree are serialized.  Ownership of the node filter is not transferred and
	the caller is responsible for freeing aFilter.

	@param aFilter The node filter to set
    */
    IMPORT_C void SetNodeFilter(MXmlEngNodeFilter* aFilter);  

	/**
	Sets a data serializer.  Ownership of the data serializer is not
	transferred and the caller is responsbile for freeing aSerializer.

	@param aSerializer The data serializer to set
    */
    IMPORT_C void SetDataSerializer(MXmlEngDataSerializer* aSerializer);  

public:
	/** Options flag */
    TUint iOptions;
	/** Encoding */
    TPtrC8 iEncoding;
	/** Node filter */
    MXmlEngNodeFilter* iNodeFilter;
    /** Data serializer */
    MXmlEngDataSerializer* iDataSerializer;
	};


#endif /* XMLENGSERIALIZATIONOPTIONS_H */

