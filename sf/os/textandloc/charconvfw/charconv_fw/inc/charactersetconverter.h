/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__CHARACTERSETCONVERTER_H__)
#define __CHARACTERSETCONVERTER_H__

#include <e32base.h>
#include <ecom/ecom.h>

/**
This constant holds the UID of the CharConv Character Set Conversion 
ECOM Interface.
@publishedAll
@released
*/
const TUid KCharacterSetConvertorInterfaceUid = {0x101F7F1D};

/**
This is the base class defining the methods in the CharConv Character Set
Conversion plugin interface. CharConv plugin suppliers would define 
implementation classes inheriting from this interface class to add additional
character set support to CharConv.
@publishedAll
@released
*/
NONSHARABLE_CLASS(CCharacterSetConverterPluginInterface) : public CBase
	{

public:

/**
 *
 * A function prototype which must be implemented by a character
 * conversion plug-in DLL to return the character (represented by one or
 * more byte values) which is used by default as the replacement for
 * unconvertible Unicode characters.
 *
 * This character is used when converting from Unicode into a foreign
 * character set, if there is no equivalent for the Unicode character in
 * the foreign character set. The default replacement character can be
 * overriden by a call to <code>CCnvCharacterSetConverter::SetReplacemen-
 * tForUnconvertibleUnicodeCharactersL()</code>.This function is exported
 * at ordinal position 1 in the plug-in DLL. It is called by <code>CCnvC-
 * haracterSetConverter::PrepareToConvertToOrFromL()</code>.To implement
 * this function, you should #include convgeneratedcpp.h in the cpp file.
 * This gives access to the
 * <code>ReplacementForUnconvertibleUnicodeCharacters_internal()</code>
 * function. This function can then be implemented by calling
 * <code>ReplacementForUnconvertibleUnicodeCharacters_internal()</code>.
 *
 * @since     6.0
 * @return   " const TDesC8& "
 *            The single character which is to be used to replace
 *            unconvertible characters.
 */
	virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters() = 0;

/**
 *
 * A function prototype which must be implemented by a character
 * conversion plug-in DLL to convert from Unicode into a foreign
 * character set.
 *
 * This function is exported at ordinal position 2 in the plug-in DLL. It
 * is called by
 * <code>CCnvCharacterSetConverter::ConvertFromUnicode()</code>.For many
 * non-complex character sets, the implementation of this function is
 * trivial. Include convgeneratedcpp.h in the .cpp file to get access to
 * the <code>SCnvConversionData</code> object called
 * <code>conversionData</code>. Then call
 * <code>CCnvCharacterSetConverter::DoConvertFromUnicode()</code>
 * specifying <code>conversionData</code> as the first argument and
 * passing in all parameters unchanged.For complex character sets, you
 * need to create an array of character conversion data objects
 * (<code>CnvUtilities::SCharacterSet</code>s), then call
 * <code>CnvUtilities::ConvertFromUnicode()</code>.
 *
 * @since     6.0
 * @param     " CCnvCharacterSetConverter::TEndianness
		  aDefaultEndiannessOfForeignCharacters "
 *            The default endian-ness to use when writing the
 *            characters in the foreign character set.
 * @param     " const TDesC8& aReplacementForUnconvertibleUnicodeCharacters
		  "
 *            The single character which is used to replace
 *            unconvertible characters.
 * @param     " TDes8& aForeign "
 *            On return, contains the converted text in the target
 *            non-Unicode character set.
 * @param     " const TDesC16& aUnicode "
 *            The source Unicode text to be converted.
 * @param     " CCnvCharacterSetConverter::TArrayOfAscendingIndices&
		  aIndicesOfUnconvertibleCharacters "
 *            A buffer which on return holds the indices of each
 *            Unicode character in the source text which could not
 *            be converted (because the target character set does
 *            not have an equivalent character).
 * @return   " TInt "
 *            The number of unconverted characters left at the end
 *            of the input descriptor (e.g. because
 *            <code>aForeign</code> was not long enough to hold all
 *            the text), or a negative error value, as defined in
 *            <code>TError</code>.
 */
	virtual TInt ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters) = 0;

/**
 *
 * A function prototype which must be implemented by a character
 * conversion plug-in DLL to convert from a foreign character set into
 * Unicode.
 *
 * This function is exported at ordinal position 3 in the plug-in DLL. It
 * is called by
 * <code>CCnvCharacterSetConverter::ConvertToUnicode()</code>.For many
 * non-complex character sets, the implementation of this function is
 * trivial. Include convgeneratedcpp.h in the .cpp file to get access to
 * the <code>SCnvConversionData</code> object called
 * <code>conversionData</code>. Then call
 * <code>CCnvCharacterSetConverter::DoConvertToUnicode()</code>
 * specifying <code>conversionData</code> as the first argument and
 * passing in all other parameters unchanged.For complex character sets,
 * you need to create an array of character conversion methods
 * (<code>CnvUtilities::SMethod</code>s or
 * <code>CnvUtilities::SState</code>s, depending on whether the complex
 * character set encoding is modal or not), then call
 * <code>CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign()</code>
 * or <code>CnvUtilities::ConvertToUnicodeFromModalForeign()</code>.
 *
 * @since     6.0
 * @param     " CCnvCharacterSetConverter::TEndianness
		  aDefaultEndiannessOfForeignCharacters "
 *            The default endian-ness to use when reading characters
 *            in the foreign character set.
 * @param     " TDes16& aUnicode "
 *            On return, contains the text converted into Unicode.
 * @param     " const TDesC8& aForeign "
 *            The non-Unicode source text to be converted.
 * @param     " TInt& aState "
 *            Used to save state information across multiple calls
 *            to <code>ConvertToUnicode()</code>.
 * @param     " TInt& aNumberOfUnconvertibleCharacters "
 *            On return, contains the number of bytes which were not
 *            converted.
 * @param     " TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter "
 *            On return, contains the index of the first bytein the
 *            input text that could not be converted. A negative
 *            value indicates that all the characters were
 *            converted.
 * @return   " TInt "
 *            The number of unconverted bytes left at the end of the
 *            input descriptor (e.g. because the output descriptor
 *            is not long enough to hold all the text), or one of
 *            the error values defined in <code>TError</code>.
 */
	virtual TInt ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter) = 0;

/**
 *
 * A function which must be implemented by a character conversion plug-in
 * DLL to calculate how probable it is that a sample piece of text is
 * encoded in this character set.
 *
 * This function was added in 6.1 as the 4th-ordinal exported function in
 * the plug-in DLL interface. It is called by
 * <code>CCnvCharacterSetConverter::AutoDetectCharacterSetL()</code> for
 * each character conversion plug-in DLL.
 *
 * @since     6.1
 * @param     "  TBool& aSetToTrue"
 *            This value should be set to <code>ETrue</code>. It is
 *            used to indicate to <code>CCnvCharacterSetConverter::-
 *            AutoDetectCharacterSetL()</code> that the plug-in DLL
 *            is implementing a function of this signature and is
 *            therefore not the empty, reserved function that was
 *            previously exported at the 4th ordinal position in the
 *            plug-in DLL in v6.0.
 * @param     "TInt& aConfidenceLevel"
 *            On return, indicates how confident the function is
 *            about its return value. Set to a value between 0 and
 *            100. Zero indicates no confidence, (and the return
 *            value should be disregarded), 100 indicates total
 *            confidence.
 * @param     "const TDesC8& aSample"
 *            The sample text string, as passed to <code>CCnvCharac-
 *            terSetConverter::AutoDetectCharacterSetL()</code>.
 * @return   "TBool"
 *            <code>ETrue</code> if it is more probable that the
 *            sample text is encoded in this character set than not.
 *            <code>EFalse</code> if it is more probable that the
 *            sample text is not encoded in this character set. The
 *            confidence level applies to this value.
 */
	virtual TBool IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample) = 0;

	static CCharacterSetConverterPluginInterface* NewL(TUid aInterfaceImplUid)
		{
		return reinterpret_cast <CCharacterSetConverterPluginInterface*> (
			REComSession::CreateImplementationL(
			aInterfaceImplUid,
			_FOFF(CCharacterSetConverterPluginInterface, iDtor_ID_Key))); 
		};
	
/**
Destroys the Charconv Plug-in interface implementation specified by iDtor_ID_Key
*/
	virtual ~CCharacterSetConverterPluginInterface()
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		};
	

private:

	TUid iDtor_ID_Key;

	};


#endif //__CHARACTERSETCONVERTER_H__

