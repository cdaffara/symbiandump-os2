// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __GMXMLENTITYCONVERTER_H__
#define __GMXMLENTITYCONVERTER_H__

#include <e32std.h>
#include <txtetext.h>


class CMDXMLEntityConverter: public CBase
/**
 * This class represents a generic entity converter for an XML parser or composer.
 * It is responsible for converting entity references to text and vice-versa.
 * @publishedPartner
 * @released
 */
	{
public:
	/**
	 * Constructor
	 */
	IMPORT_C CMDXMLEntityConverter();

	/** Destructor. */
	IMPORT_C virtual ~CMDXMLEntityConverter();

	/**
	 * Takes a block of text and converts any entity references found to the
	 * appropriate text.  Because built-in and character entity references are
	 * longer than the replacement text, this takes place in-situ.
	 * @param aTextToConvert Text to be converted - replacement text goes
	 * out in the same
	 * @return Returns KErrNone if successful or no entity found
	 * @return Returns KErrXMLBadEntity if malformed entity found
	 */
	TInt EntityToTextL(TDes& aTextToConvert);

	/**
	 * Outputs a block of text to a composer with offending characters replaced by
	 * entity references.
	 * @param aComposer the composer to be used for output
	 * @param aTextToConvert The text to be converted.
	 * @return Returns KErrNone if succcessful or a file write error
	 * @leave can Leave due to OOM
	 */
	IMPORT_C virtual TInt OutputComposedTextL( CMDXMLComposer* aComposer, const TDesC& aTextToConvert );

protected:
	/**
	 * DTD Specific entity to text converter
	 * Takes a block of text and converts any entity references found to the
	 * appropriate text.  We hope that this can happen in-situ.
	 * @param aTextToConvert Text to be converted - replacement text goes
	 * out in the same
	 * @return Returns KErrNone if successful or no entity found
	 * @return Returns KErrXMLBadEntity if malformed entity found
	 */
	virtual TInt DTDEntityToText(TDes& aTextToConvert);

private:
	/**
	 * Replaces the built in entity reference with its replacement text.
	 * @param aTextToConvert An entity reference to convert.  This should begin with &
	 * and end with ;.
	 * @return Returns KErrNone if successful
	 * @return Returns KErrUnsupported if aTextToConvert contains no recognised entity references
	 * @return Returns KErrOverflow or KErrGeneral if the character reference couldn't be parsed
	 */
	TInt ConvertEntityRefL(TDes& aTextToConvert);

	};


#endif
