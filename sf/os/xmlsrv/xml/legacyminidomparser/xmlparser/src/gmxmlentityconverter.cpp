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
// MDXMLEntityConverter.cpp
// @file
// This class represents a generic entity converter for an XML parser or composer.
// 
//

#include <f32file.h>
#include <stringpool.h>

#include <gmxmlconstants.h>
#include <gmxmlcomposer.h>
#include <gmxmlentityconverter.h>
#include "xmlentityreferences.h"

const TInt KMaxBuiltinEntityLength = 20;

//
// CMDXMLEntityConverter			//
//

EXPORT_C CMDXMLEntityConverter::CMDXMLEntityConverter()
//
// Constructor
//
	{
	}

EXPORT_C CMDXMLEntityConverter::~CMDXMLEntityConverter()
	{
	}

//==================================================================================

TInt CMDXMLEntityConverter::EntityToTextL(TDes& aTextToConvert)
//
// Takes a block of text and converts any entity references found to the 
// appropriate text.  Because built-in and character entity references are
// longer than the replacement text, this takes place in-situ.
// @param aTextToConvert Text to be converted - replacement text goes
// out in the same
//
	{
	TInt error = KErrNone;
	TInt beginEntityOffset = 0;
	TInt beginSearchOffset = 0;
	TPtrC searchPtr = TPtrC(aTextToConvert);

	// Need to convert all entity refs in this bit of data.  Don't want to convert
	// things more than once so we keep a marker and move it on so each time round
	// the loop we start looking for entity references after the end of the previous
	// one.
	while((beginEntityOffset = searchPtr.Locate('&')) != KErrNotFound)
		{
		searchPtr.Set(aTextToConvert.Mid(beginSearchOffset + beginEntityOffset));
		TInt endEntityOffset = searchPtr.Locate(';');
		if(endEntityOffset == KErrNotFound)
			{
			// No point continuing past here - even if we find more & they can't be
			// terminated either. 
			error = KErrXMLBadEntity;
			break;
			}
		endEntityOffset += beginEntityOffset;
		// Can't construct a modifiable descriptor that's just part of another one.  Looks
		// like we need to copy the section containing the reference and twiddle that, then
		// copy the result back if it's valid.
		HBufC* entityRef = searchPtr.Mid(0, (endEntityOffset - beginEntityOffset)+1).AllocLC();
		TInt convertErr = KErrNone;
		TPtr entityRefDes = entityRef->Des();
		if((convertErr = ConvertEntityRefL(entityRefDes)) == KErrNotSupported)
			{
			convertErr = DTDEntityToText(entityRefDes);
			}

		if(!convertErr)
			{
			// We need to replace this section of aTextToConvert with entityRefDes
			// we don't know whether DTDEntityToText has actually changed anything -
			// because of constraints on the published behaviour of DTDEntityToText we
			// can't compel DTDEntityToText to return KErrUnsupported - it thinks it 
			// could be dealing with multiple entity refs and just cos it doesn't support
			// one of them doesn't mean it doesn't support the rest.  Probably true that
			// the entity ref will be a builtin, so won't need to call DTDEntityToText 
			// anyway.  ConvertEntityRefL will return KErrNone only if it converted so
			// we probably won't be copying things gratuitously in the common case. 
			aTextToConvert.Replace(beginEntityOffset + beginSearchOffset, (endEntityOffset - beginEntityOffset)+1, entityRefDes);
			}
		else
			{
			error = KErrXMLBadEntity;
			}

		beginSearchOffset += beginEntityOffset + entityRefDes.Length();
		searchPtr.Set(aTextToConvert.Mid(beginSearchOffset));
		CleanupStack::PopAndDestroy(entityRef);
		}

	return error;
	}

//==================================================================================

EXPORT_C TInt CMDXMLEntityConverter::OutputComposedTextL( CMDXMLComposer* aComposer, const TDesC& aTextToConvert)
//
// Outputs a block of text to a composer with offending characters replaced by
// entity references.
// @param aComposer the composer to be used for output
// @param aTextToConvert The text to be converted.
// @return Returns KErrNone if successful or a file write error
// @leave can Leave due to OOM
//
	{
	TBuf<1> oneChar;
	TInt offset = 0;
	TInt error = KErrNone;
	TInt textLen = aTextToConvert.Length();

	while(error == KErrNone && (offset < textLen))
		{
		oneChar = aTextToConvert.Mid(offset, 1);
		offset++;

	// Built in entities
		RStringPool	pool;
		CleanupClosePushL(pool);
		pool.OpenL(XMLEntityReferences::Table);


		TBool found = EFalse;
		for (TUint i=0; i<XMLEntityReferences::Table.iCount; i++)
			{
			RStringF entity = pool.StringF(i, XMLEntityReferences::Table);
			TBuf<KMaxBuiltinEntityLength> buf;
			buf.Copy(entity.DesC());
			TLex string(buf);

			TPtrC entityRef = string.NextToken(); 
			if(oneChar.Compare(string.NextToken()) == 0)
				{
				error = aComposer->OutputDataL(entityRef);
				found = ETrue;
				break;
				}

			}

		CleanupStack::PopAndDestroy(); // close pool

		if(!found)
			error = aComposer->OutputDataL(oneChar);
		else
			found = EFalse;
		
		}

	return error;
	}

//==================================================================================

TInt CMDXMLEntityConverter::DTDEntityToText(TDes& /*aTextToConvert*/)
//
// DTD Specific entity to text converter
// Takes a block of text and converts any entity references found to the 
// appropriate text.  We hope that this can happen in-situ.
// @param aTextToConvert Text to be converted - replacement text goes
// out in the same
//
	{ // Nothing in the generic case.
	return KErrNone;
	}

//==================================================================================

TInt CMDXMLEntityConverter::ConvertEntityRefL(TDes& aTextToConvert)
	{
#ifdef _DEBUG
	_LIT(KEntityConverter, "GMXML Entity Converter");
	__ASSERT_ALWAYS(aTextToConvert.Length() > 1, User::Panic(KEntityConverter, KErrArgument));
#endif

	TInt err = KErrNotSupported;
	TBool charRef = EFalse;
	TInt idLen = 0;
	TRadix elementRadix = EDecimal;

	if(aTextToConvert.Find(KXMLHexidecimalId) != KErrNotFound)
		{
		charRef = ETrue;
		idLen = TPtrC(KXMLHexidecimalId).Length();
		elementRadix = EHex;
		}
	else if(aTextToConvert.Find(KXMLDecimalId) != KErrNotFound)
		{
		charRef = ETrue;
		idLen = TPtrC(KXMLDecimalId).Length();
		elementRadix = EDecimal;
		}

	if(charRef)
		{
		// Character entities
		/*
			algorithm explanation:
			The entity reference in the descriptor holding the string is known by it's positions.
				offset = offset to beginning of '&#x' from start of supplied descriptor. (0 indicates start of descriptor)
				entitylen = offset to ';' character from the start of the entity reference.

				the length of the entity preamble (&#x in this case) is known.

			The algorithm assigns the required portion of the descriptor to a TLex16 object so that the 
			value of the code can be read out.

			The math for the assign is as follows:
				start of number = offset + length of preamble
				num of characters to take = entityLen - length of preamble

			The entity is then deleted from the string, and the new code is inserted in the
			correct location.
				The deleted characters are 'entityLen' charcters from 'offset'.
			
			
			e.g. for the string <a href="http://&#x77;ww.symbian.com">

			offset = 16
			entitylen = 5
			string assigned to lexer = '77'
			code = 0x77 (extracted from lexer)

			descriptor after delete operation:

				<a href="http://&ww.symbian.com">

			array dereference fills in the descriptor such that the ampersand is replaced.
				<a href="http://www.symbian.com">
		*/
		TInt entityLen = aTextToConvert.Length();

		TLex16 element;
		element.Assign(aTextToConvert.Mid(idLen, (entityLen - idLen)));

		TUint16 code;
		err = element.Val(code, elementRadix);

		if(!err)
			{
			aTextToConvert.Delete(1, entityLen);
			aTextToConvert[0] = code;
			}
		}
	else
		{
		// Built in entities
		RStringPool	pool;
		CleanupClosePushL(pool);
		pool.OpenL(XMLEntityReferences::Table);


		for (TUint i=0; (i<XMLEntityReferences::Table.iCount) && (err != KErrNone); i++)
			{
			RStringF entity = pool.StringF(i, XMLEntityReferences::Table);
			TBuf<KMaxBuiltinEntityLength> buf;
			buf.Copy(entity.DesC());
			TLex string(buf);

			TPtrC token = string.NextToken(); 
			TInt offset;
			if((offset = aTextToConvert.Find(token)) != KErrNotFound)
				{
				aTextToConvert.Replace(offset, token.Length(), string.NextToken());
				err = KErrNone;
				}
			}
		CleanupStack::PopAndDestroy(); //close pool
		}

	return err;
	}

// End Of File
