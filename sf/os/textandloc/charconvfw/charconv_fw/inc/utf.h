/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__UTF_H__)
#define __UTF_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif


class CnvUtfConverter
/** 
Converts text between Unicode (UCS-2) and the two Unicode transformation 
formats UTF-7 and UTF-8. There are no functions to convert directly between 
UTF-7 and UTF-8.

Objects of this class do not need to be created because all the member functions 
are static. The four functions are passed text in the second argument and 
output the resulting text in the first argument. Sixteen-bit descriptors are 
used to hold text encoded in UCS-2 (i.e. normal 16 bit Unicode), and eight-bit 
descriptors are used to hold text encoded in either of the transformation 
formats.

The conversion functions return the number of characters which were not converted 
because the output descriptor was not long enough to hold all of the converted 
text. This allows users of this class to perform partial conversions on an 
input descriptor, handling the case when the input descriptor is truncated 
mid way through a multi-byte character. The caller does not have to guess 
how big to make the output descriptor for a given input descriptor- they 
can simply do the conversion in a loop using a small output descriptor. The 
ability to handle truncated descriptors is particularly useful if the caller 
is receiving information in chunks from an external source. 
@publishedAll
@released
*/
	{
public:
	/** Conversion error flags. At this stage there is only one error flag 
	- others may be added in the future. */
	enum TError
		{
 		/** The input descriptor contains a single corrupt character. This 
 		might occur when the input descriptor only contains some of the bytes 
 		of a single multi-byte character. */
		EErrorIllFormedInput=KErrCorrupt
		};
	 
	 /** Initial value for the state argument in a set of related calls to
	ConvertToUnicode(). */
	enum {KStateDefault=0}; 
public:
	// the conversion functions return either one of the TError values above, or the number of unconverted elements left at the end of the input descriptor
	IMPORT_C static TInt ConvertFromUnicodeToUtf7(TDes8& aUtf7, const TDesC16& aUnicode, TBool aEncodeOptionalDirectCharactersInBase64);
	static TInt ConvertFromUnicodeToUtf7(TDes8& aUtf7, const TDesC16& aUnicode, TBool aIsImapUtf7, TBool aEncodeOptionalDirectCharactersInBase64);
	IMPORT_C static TInt ConvertFromUnicodeToUtf8(TDes8& aUtf8, const TDesC16& aUnicode);
	static TInt ConvertFromUnicodeToUtf8(TDes8& aUtf8, const TDesC16& aUnicode, TBool aGenerateJavaConformantUtf8);
	IMPORT_C static TInt ConvertToUnicodeFromUtf7(TDes16& aUnicode, const TDesC8& aUtf7, TInt& aState);
	static TInt ConvertToUnicodeFromUtf7(TDes16& aUnicode, const TDesC8& aUtf7, TBool aIsImapUtf7, TInt& aState);
	IMPORT_C static TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8);
	static TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8, TBool aGenerateJavaConformantUtf8);
	static TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8, TBool aGenerateJavaConformantUtf8,
			TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);

	IMPORT_C static HBufC8* ConvertFromUnicodeToUtf7L(const TDesC16& aUnicode,TBool aEncodeOptionalDirectCharactersInBase64);
	IMPORT_C static HBufC8* ConvertFromUnicodeToUtf8L(const TDesC16& aUnicode);
	IMPORT_C static HBufC16* ConvertToUnicodeFromUtf7L(const TDesC8& aUtf7); 
	IMPORT_C static HBufC16* ConvertToUnicodeFromUtf8L(const TDesC8& aUtf8);


	};

#endif

