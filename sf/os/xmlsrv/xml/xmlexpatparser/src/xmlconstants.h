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
//

#ifndef __XMLCONSTANTS_H__
#define __XMLCONSTANTS_H__

#include <e32base.h>

/**
This file describes useful XML constants.

The UTF-8 character representation protocol is described here.

num of bytes| Bits used in encoding	| Bit representation

          1 |                     7 | 0vvvvvvv

          2 |                    11 | 110vvvvv 10vvvvvv

          3 |                    16 | 1110vvvv 10vvvvvv 10vvvvvv

          4 |                    21 | 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv

          5 |                    26 | 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv

          6 |                    31 | 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv

Because each XML entity not accompanied by external encoding information and not in
UTF-8 or UTF-16, encoding must begin with an XML encoding declaration, in which the
first characters must be '<?xml', any conforming processor can detect, after two to
four octets of input, which of the following cases apply. In reading this list, it
may help to know that in UCS-4, '<' is "0x0000003C" and '?' is "0x0000003F", and the
Byte Order Mark (BOM) required of UTF-16 data streams is "#xFEFF". 

The notation ## is used to denote any byte value except that two consecutive ##s 
cannot be both 00.

ASCII characters are represented as ASCII values
@file
*/

/**
Bit mask to capture the utf-8 single byte header character encoding.
@publishedAll
*/
const TUint KUTF8SingleHeaderMask		= 0x80; // 10000000


/**
Bit mask to capture the utf-8 double byte header character encoding.
@publishedAll
*/
const TUint KUTF8DoubleHeaderMask		= 0xE0; // 11100000

/**
Bit mask to capture the utf-8 triple byte header character encoding.
@publishedAll
*/
const TUint KUTF8TripleHeaderMask		= 0xF0; // 11110000

/**
Bit mask to capture the utf-8 quadruple byte header character encoding.
@publishedAll
*/
const TUint KUTF8QuadrupleHeaderMask	= 0xF8; // 11111000

/**
Bit mask to capture the utf-8 quinary byte header character encoding.
@publishedAll
*/
const TUint KUTF8QuinaryHeaderMask		= 0xFC; // 11111100

/**
Bit mask to capture the utf-8 senary byte header character encoding.
@publishedAll
*/
const TUint KUTF8SenaryHeaderMask		= 0xFE; // 11111110

/**
The utf-8 bit pattern describing a single byte header character encoding.
@publishedAll
*/
const TUint KUTF8SingleByteHeader		= 0x00; // 00000000

/**
The utf-8 bit pattern describing a double byte header character encoding.
@publishedAll
*/
const TUint KUTF8DoubleByteHeader		= 0xC0; // 11000000

/**
The utf-8 bit pattern describing a triple byte header character encoding.
@publishedAll
*/
const TUint KUTF8TripleByteHeader		= 0xE0; // 11100000

/**
The utf-8 bit pattern describing a quadruple byte header character encoding.
@publishedAll
*/
const TUint KUTF8QuadrupleByteHeader	= 0xF0; // 11110000

/**
The utf-8 bit pattern describing a quinary byte header character encoding.
@publishedAll
*/
const TUint KUTF8QuinaryByteHeader		= 0xF8; // 11111000

/**
The utf-8 bit pattern describing a senary byte header character encoding.
@publishedAll
*/
const TUint KUTF8SenaryByteHeader		= 0xFC; // 11111100

/**
The byte count of a utf-8 single byte character encoding.
@publishedAll
*/
const TInt KUTF8SingleByteCount			= 1; 

/**
The byte count of a utf-8 double byte character encoding.
@publishedAll
*/
const TInt KUTF8DoubleByteCount			= 2; 

/**
The byte count of a utf-8 triple byte character encoding.
@publishedAll
*/
const TInt KUTF8TripleByteCount			= 3; 

/**
The byte count of a utf-8 quadruple byte character encoding.
@publishedAll
*/
const TInt KUTF8QuadrupleByteCount		= 4; 

/**
The byte count of a utf-8 quinary byte character encoding.
@publishedAll
*/
const TInt KUTF8QuinaryByteCount		= 5; 

/**
The byte count of a utf-8 senary byte character encoding.
@publishedAll
*/
const TInt KUTF8SenaryByteCount			= 6;

/**
The byte count required to encode '<?xml'.
@publishedAll
*/
const TInt KEncodingByteCount			= 4;


/**
The encoding text to search for that describes the encoding of an xml document.
@publishedAll
*/
_LIT8(KEncodingTxt, "encoding=\"");

/**
The end tag (>) symbol used in xml to close the scope of and element.
@publishedAll
*/
const TUint8 KXMLEndTag					= '>';

/**
The quotation (") symbol used in xml.
@publishedAll
*/
const TUint8 KQuotation					= '\"';


enum TParseMode 
/**
Lists enumerations used to describe one or more Parse modes. Users can set this information
via the SetParseMode method on the RXmlParser object.
@see RXmlParser
@publishedAll
*/
	{

/**
This enumeration when set specifies the convertion of elements and attributes to lowercase. 
This can be used for case-insensitive HTML so that a tag can be matched to a static 
string in the string pool.
@see RStringPool
*/
	EParseModeConvertTagsToLowerCase	= 0x0001,

/**
This enumeration when set reports an error when unrecognised tags are found.
*/
	EParseModeErrorOnUnrecognisedTags	= 0x0002,

/**
This enumeration when set reports unrecognised tags.
*/
	EParseModeReportUnrecognisedTags	= 0x0004,

/**
This enumeration when set reports the namespace.
*/
	EParseModeReportNamespaces			= 0x0008,

/**
This enumeration when set reports the namespace prefix.
*/
	EParseModeReportNamespacePrefixes	= 0x0010,

/**
This enumeration when set sends all content data for an element in one chunk.
*/
	EParseModeSendFullContentInOneChunk	= 0x0020,

/**
This enumeration when set reports namespace mappings via the OnStartPrefixMapping & 
OnEndPrefixMapping methods.
@see MMarkupCallback
*/
	EParseModeReportNamespaceMapping	= 0x0040,

/**
This enumeration when set describes the data in the specified encoding, otherwise
it is specified in utf-8.
*/
	EParseModeRawContent                = 0x0080,

/**
This enumeration when set states that all string comparisons be non-folded.
Fold is defined as: The removal of differences between characters that are deemed 
unimportant for the purposes of inexact or case-insensitive matching. 
As well as ignoring differences of case, folding ignores any accent on a character.
*/
	EParseModeStrict					= 0x0100,

/**
This enumeration is a mask that covers the total enumerations thus far, and as
such should be updated to reflect any new enumerations added.
*/
	EParseModeAllMask                   = 0x01FF,

	};


enum TEncoding
/**
Lists enumerations used to describe the encoding of an xml document. 
The first line of an xml document generally has the encoding described,
however, the data upto this description is specified in the encoding. The actual 
description is described in ASCII.
@publishedAll
*/
	{

// With BOM (Byte Order Mark):

/**
This enumeration represents a BOM subset with the following values 00 00 FE FF.
Posible encodings include: UCS-4, big-endian machine (1234 order).
*/
	EEncodingUCS_4BEBOM = 0,

/**
This enumeration represents a BOM subset with the following values FF FE 00 00.
Posible encodings include: UCS-4, little-endian machine (4321 order).
*/
	EEncodingUCS_4LEBOM,
	
/**
This enumeration represents a BOM subset with the following values 00 00 FF FE.
Posible encodings include: UCS-4, unusual octet order (2143).
*/
	EEncodingUCS_4UO1BOM,

/**
This enumeration represents a BOM subset with the following values FE FF 00 00.
Posible encodings include: UCS-4, unusual octet order (3412).
*/
	EEncodingUCS_4UO2BOM,

/**
This enumeration represents a BOM subset with the following values FE FF ## ##.
Posible encodings include: UTF-16, big-endian.
*/
	EEncodingUTF_16BEBOM,

/**
This enumeration represents a BOM subset with the following values FF FE ## ##.
Posible encodings include: UTF-16, little-endian.
*/
	EEncodingUTF_16LEBOM,

/**
This enumeration represents a BOM subset with the following values EF BB BF ##.
Posible encodings include: UTF-8.
*/
	EEncodingUTF_8BOM,


// Without a Byte Order Mark:

/**
This enumeration represents a non BOM subset with the following values 00 00 00 3C.
Posible encodings include: UCS-4 or other encoding with a 32-bit code unit
and ASCII characters encoded as ASCII values, in respectively big-endian (1234), 
little-endian (4321) and two unusual byte orders (2143 and 3412). The encoding 
declaration must be read to determine which of UCS-4 or other supported 32-bit encodings applies.
*/
	EEncodingUCS_4BE,

/**
This enumeration represents a non BOM subset with the following values 3C 00 00 00.
Posible encodings include: UCS-4 or other encoding with a 32-bit code unit
and ASCII characters encoded as ASCII values, in respectively big-endian (1234), 
little-endian (4321) and two unusual byte orders (2143 and 3412). The encoding 
declaration must be read to determine which of UCS-4 or other supported 32-bit encodings applies.
*/
	EEncodingUCS_4LE,

/**
This enumeration represents a non BOM subset with the following values 00 00 3C 00.
Posible encodings include: UCS-4 or other encoding with a 32-bit code unit
and ASCII characters encoded as ASCII values, in respectively big-endian (1234), 
little-endian (4321) and two unusual byte orders (2143 and 3412). The encoding 
declaration must be read to determine which of UCS-4 or other supported 32-bit encodings applies.
*/
	EEncodingUCS_4BO1,

/**
This enumeration represents a non BOM subset with the following values 00 3C 00 00 
Posible encodings include: UCS-4 or other encoding with a 32-bit code unit
and ASCII characters encoded as ASCII values, in respectively big-endian (1234), 
little-endian (4321) and two unusual byte orders (2143 and 3412). The encoding 
declaration must be read to determine which of UCS-4 or other supported 32-bit encodings applies. 
*/
	EEncodingUCS_4BO2,

/**
This enumeration represents a non BOM subset with the following values 00 3C 00 3F.
Posible encodings include: UTF-16BE or big-endian ISO-10646-UCS-2 or other encoding
with a 16-bit code unit in big-endian order and ASCII characters encoded as ASCII 
values (the encoding declaration must be read to determine which).
*/
	EEncodingUTF_16BE,
	
/**
This enumeration represents a non BOM subset with the following values 3C 00 3F 00.
Posible encodings include: UTF-16LE or little-endian ISO-10646-UCS-2 or other encoding
with a 16-bit code unit in little-endian order and ASCII characters encoded as ASCII 
values (the encoding declaration must be read to determine which).
*/
	EEncodingUTF_16LE,

/**
This enumeration represents a non BOM subset with the following values 3C 3F 78 6D.
Posible encodings include: UTF-8, ISO 646, ASCII, some part of ISO 8859, Shift-JIS, 
EUC, or any other 7-bit, 8-bit, or mixed-width encoding which ensures that the 
characters of ASCII have their normal positions, width, and values; the actual encoding
declaration must be read to detect which of these applies, but since all of these 
encodings use the same bit patterns for the relevant ASCII characters, the encoding 
declaration itself may be read reliably.
*/
	EEncodingUTF_8,

/**
This enumeration represents a non BOM subset with the following values 4C 6F A7 94.
Posible encodings include: EBCDIC (in some flavor; the full encoding declaration must 
be read to tell which code page is in use).
*/

	EEncodingEBCDIC,

/**
This enumeration represents a non BOM subset with a combination of other values.
Posible encodings include: Other UTF-8 without an encoding declaration, or else the data 
stream is mislabeled (lacking a required encoding declaration), corrupt, fragmentary, 
or enclosed in a wrapper of some kind.
*/
	EEncodingOTHER,

	};

#endif // __XMLCONSTANTS_H__
