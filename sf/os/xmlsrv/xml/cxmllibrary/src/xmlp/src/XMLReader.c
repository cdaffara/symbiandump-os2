/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


/*
This module provides a text (character) stream, pointers into the stream
and operations on segments of the stream as though they were strings.
The goal is to isolate the client from stream buffers, cross buffer
issues and some character set encoding concerns.

This particular version is for input from a single buffer.
*/

#include "cxml_internal.h"
#include <xml/cxml/nw_string_char.h>
#include <xml/cxml/nw_xmlp_xmlreader.h>

static
NW_Status_t
NW_XML_Reader_ReadAsciiChar(NW_Uint32 c, NW_Uint32* pReturnChar)
{
    /* This looks a bit weird but the idea is to force the conversion
    of the ASCII character through the same function that is used
    to read a character from the text.  This will impose the same conversion
    limitations and the same result encoding. */
    NW_Int32 byteCount;
    NW_Uint8 buf[2];
    NW_Ucs2 c_ucs2;
    buf[0] = (NW_Uint8)(c & 0xff);
    buf[1] = 0;
    /* should only use this function for ASCII */
    if (c > 127) {
        return NW_STAT_FAILURE;
    }
    /* call it UTF-8 because ASCII doesn't work with NW_String_readChar()
    at the moment */
    byteCount = NW_String_readChar((NW_Byte*)buf, &c_ucs2, HTTP_utf_8);
    if (byteCount != 1) {
        return NW_STAT_FAILURE;
    }
    *pReturnChar = c_ucs2;
    return NW_STAT_SUCCESS;
}

/* assumes this is just a handoff of the buffer (i.e., won't make a copy) */
EXPORT_C NW_Status_t
NW_XML_Reader_InitFromBuffer(NW_XML_Reader_t* pT, NW_Uint32 length, unsigned char* pBuf)
{
    pT->encoding = 0;
    pT->endianness = NW_NATIVE_ENDIAN;
    pT->index = 0;
    pT->charIndex = 0;
    pT->lineColumn.crCount = 0;
    pT->lineColumn.lfCount = 0;
    pT->lineColumn.charsSinceLastCR = 0;
    pT->lineColumn.charsSinceLastLF = 0;
    pT->end = 0;
    pT->length = length;
    pT->pBuf = pBuf;
    return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_XML_Reader_DataAddressFromBuffer(NW_XML_Reader_t* pT,
                                 NW_Uint32 start, NW_Uint32* length,
                                 unsigned char** ppData)
{
    NW_ASSERT(start < pT->length);
    NW_ASSERT(*length <= pT->length);
    NW_ASSERT((start + *length) <= pT->length);
    *ppData = NULL;
    if (start < pT->length) {
        *ppData = pT->pBuf + start;
        *length = (((start + *length) <= pT->length) ?
                   *length : (pT->length - start));
        return NW_STAT_SUCCESS;
    }
    return NW_STAT_FAILURE;
}

/* peekOrAdvance: first arg "advance": peek = 0, advance = 1 */
static
NW_Status_t
NW_XML_Reader_PeekOrAdvanceOffset(NW_Bool advance, NW_XML_Reader_t* pT,
                               NW_Uint32 offsetCharCount, NW_Uint32* pC)
{
    NW_Ucs2 c_ucs2;
    NW_Uint32 i;
    NW_Uint32 charCount = 0;
    NW_Int32 byteCount = 0;
    NW_Uint32 crCount = 0;
    NW_Uint32 lfCount = 0;
    NW_Uint32 charsPastCR = 0;
    NW_Uint32 charsPastLF = 0;
    NW_Bool resetPastCR = 0;
    NW_Bool resetPastLF = 0;

    NW_ASSERT(!(advance && !offsetCharCount)); 

    if (pT->end) {
        return NW_STAT_FAILURE;
    }
    for (i = pT->index; i < pT->length; i += (NW_Uint32)byteCount) {
        NW_ASSERT(charCount <= offsetCharCount);

        /* It is assumed that this func returns UNICODE code points. */
        byteCount = NW_String_readChar((NW_Byte*)&(pT->pBuf[i]),
                                       &c_ucs2, pT->encoding);
        *pC = c_ucs2;
        if (byteCount == -1) {
            return NW_STAT_FAILURE;
        }
        if (charCount == offsetCharCount) {
            /* This catches NW_String_readChar() reading past buffer end
            and can be removed when the readChar function does proper
            error checking. */
            if ((i + (NW_Uint32)byteCount) > pT->length) {
                return NW_STAT_FAILURE;
            }
            break;
        }
        charCount++;
        charsPastCR++;
        charsPastLF++;
        if (c_ucs2 == 0xd /* CR */) {
            crCount++;
            resetPastCR = 1;
            charsPastCR = 0;
        } else if (c_ucs2 == 0xa /* LF */) {
            lfCount++;
            resetPastLF = 1;
            charsPastLF = 0;
        }
    }
    if (i >= pT->length) {
        pT->end = 1;
    }
    /* This catches NW_String_readChar() reading past buffer end and can be
    removed when the readChar function does proper error checking. */
    if (i > pT->length) {
        return NW_STAT_FAILURE;
    }
    if (advance) {
        pT->index = i;
        pT->charIndex += charCount;
        pT->lineColumn.crCount += crCount;
        pT->lineColumn.lfCount += lfCount;
        if (resetPastCR) {
            pT->lineColumn.charsSinceLastCR = charsPastCR;
        } else {
            pT->lineColumn.charsSinceLastCR += charsPastCR;
        }
        if (resetPastLF) {
            pT->lineColumn.charsSinceLastLF = charsPastLF;
        } else {
            pT->lineColumn.charsSinceLastLF += charsPastLF;
        }
    }
    return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_XML_Reader_PeekOffset(NW_XML_Reader_t* pT, NW_Uint32 nChars, NW_Uint32* pC)
{
    return NW_XML_Reader_PeekOrAdvanceOffset(0, pT, nChars, pC);
}

EXPORT_C NW_Status_t
NW_XML_Reader_AdvanceOffset(NW_XML_Reader_t* pT, NW_Uint32 nChars)
{
    NW_Uint32 c;
    return NW_XML_Reader_PeekOrAdvanceOffset(1, pT, nChars, &c);
}

EXPORT_C 
void NW_XML_Reader_GetPosition(NW_XML_Reader_t* pT, NW_Uint32* pByteIndex,
                            NW_Uint32* pCharIndex,
                            NW_XML_Reader_LineColumn_t* pLineColumn)
{
    *pByteIndex = pT->index;
    *pCharIndex = pT->charIndex;
    pLineColumn->crCount = pT->lineColumn.crCount;
    pLineColumn->lfCount = pT->lineColumn.lfCount;
    pLineColumn->charsSinceLastCR = pT->lineColumn.charsSinceLastCR;
    pLineColumn->charsSinceLastLF = pT->lineColumn.charsSinceLastLF;
}

/* Note: Setting the position (similar to seeking in a file) is in general
not possible without reading the characters (usually reading forward) because
character encoding may use a variable numbers of bytes per character. This is
here so that if you have defined a valid interval, then you can reposition to
the beginning of the interval. Setting to the position to a bad value will
not always be caught immediately. Don't forget to also save and set line
and column with position. */
EXPORT_C void
NW_XML_Reader_SetPosition(NW_XML_Reader_t* pT, NW_Uint32 byteIndex,
                       NW_Uint32 charIndex,
                       const NW_XML_Reader_LineColumn_t* pLineColumn)
{
    pT->index = byteIndex;
    pT->charIndex = charIndex;
    pT->lineColumn.crCount = pLineColumn->crCount;
    pT->lineColumn.lfCount = pLineColumn->lfCount;
    pT->lineColumn.charsSinceLastCR = pLineColumn->charsSinceLastCR;
    pT->lineColumn.charsSinceLastLF = pLineColumn->charsSinceLastLF;
}

/*
Reader Interval Functions
*/

EXPORT_C void
NW_XML_Reader_Interval_Start(NW_XML_Reader_Interval_t* pI, NW_XML_Reader_t* pT)
{
    /* set both start and stop for safety in later use */
    pI->start = pI->stop = pT->index;
    pI->charStart = pI->charStop = pT->charIndex;
}

EXPORT_C void
NW_XML_Reader_Interval_Stop(NW_XML_Reader_Interval_t* pI, NW_XML_Reader_t* pT)
{
    pI->stop = pT->index;
    pI->charStop = pT->charIndex;
}

/* BEGIN GENERIC Reader CHARACTER AND STRING FUNCTIONS */

/* pMatch is 1 if ASCII character c matches Reader char in its encoding */
EXPORT_C NW_Status_t
NW_XML_Reader_AsciiCharMatch(NW_XML_Reader_t* pT, NW_Uint32 asciiC, NW_Uint32* pMatch)
{
    NW_Uint32 c_text, c_ascii;
    NW_Status_t s = NW_XML_Reader_Peek(pT, &c_text);
    *pMatch = 0;
    if (NW_STAT_IS_SUCCESS(s)) {
        s = NW_XML_Reader_ReadAsciiChar(asciiC, &c_ascii);
        if (NW_STAT_IS_SUCCESS(s)) {
            *pMatch = (c_text == c_ascii);
        }
    }
    return s;
}

/* pMatch is 1 if ASCII string matches Reader sequence in its encoding */
EXPORT_C NW_Status_t
NW_XML_Reader_AsciiStringMatch(NW_XML_Reader_t* pT, NW_Uint32 length, const NW_Uint8* pString,
                            NW_Uint32* pMatch)
{
    NW_Uint32 c_text, c_ascii;
    NW_Uint32 i;
    NW_Status_t s = NW_STAT_SUCCESS;
    *pMatch = 0;
    NW_ASSERT(length);
    for (i = 0; i < length; i++) {
        s = NW_XML_Reader_PeekOffset(pT, i, &c_text);
        if (NW_STAT_IS_FAILURE(s)) {
            break;
        }
        s = NW_XML_Reader_ReadAsciiChar(pString[i], &c_ascii);
        if (NW_STAT_IS_FAILURE(s)) {
            break;
        }
        if (c_text != c_ascii) {
            break;
        }
    }
    if (i == length) {
        *pMatch = 1;
    }
    return s;
}

/* Note: For XML, whitespace is only ASCII 0x20 (space),
0x09 (tab), 0x0d (CR), 0x0a (LF).  The base test used here,
CXML_Str_Isspace(), includes two other forms of whitespace. */
EXPORT_C NW_Status_t
NW_XML_Reader_SkipSpace(NW_XML_Reader_t* pT)
{
    NW_Uint32 c;
    NW_Status_t s = NW_STAT_SUCCESS;
    for (;;) {
        s = NW_XML_Reader_Peek(pT, &c);
        if (NW_STAT_IS_FAILURE(s)) {
            break;
        }
        if (c > 0xffff) { /* validate casting */
            break;
        }
        if (!CXML_Str_Isspace((NW_Ucs2)(c & 0xffff))) {
            break;
        }
        s = NW_XML_Reader_Advance(pT);
        if (NW_STAT_IS_FAILURE(s)) {
            break;
        }
        if (pT->end){
         /* At the end so break */
            break;
          }
    }
    return s;
}

/* Note: For XML, whitespace is only ASCII 0x20 (space),
0x09 (tab), 0x0d (CR), 0x0a (LF).  The base test used here,
CXML_Str_Isspace(), includes two other forms of whitespace. */
EXPORT_C NW_Status_t
NW_XML_Reader_IsSpace(NW_XML_Reader_t* pT, NW_Uint32* pMatch)
{
    NW_Uint32 c;
    NW_Status_t s;

    *pMatch = 0;
    s  = NW_XML_Reader_Peek(pT, &c);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (c > 0xffff) { /* validate casting */
        return NW_STAT_FAILURE;
    }
    if (CXML_Str_Isspace((NW_Ucs2)(c & 0xffff))) {
        *pMatch = 1;
    }
    return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_XML_Reader_IsLetter(NW_XML_Reader_t* pT, NW_Uint32* pMatch)
{
    NW_Uint32 c;
    NW_Status_t s;

    *pMatch = 0;
    s = NW_XML_Reader_Peek(pT, &c);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    /* This is an approximation to what XML charaters are "letter".
    Everything above the 8-bit range is considered to be a "letter".*/
    if (c >= 0x41 && c <= 0x5a) {
        *pMatch = 1;
    }
    else if (c >= 0x61 && c <= 0x7a) {
        *pMatch = 1;
    }
    else if (c >= 0xc0 && c <= 0xd6) {
        *pMatch = 1;
    }
    else if (c >= 0xd8 && c <= 0xf6) {
        *pMatch = 1;
    }
    else if (c >= 0xf8) {/* letters become anything above 0xf8 */
        *pMatch = 1;
    }
    return NW_STAT_SUCCESS;
}

/* Note: For XML, digits include not only the ASCII digits but
other language forms of digits.  The base test used here,
CXML_Str_Isdigit() only tests for ASCII digits. */
EXPORT_C NW_Status_t
NW_XML_Reader_IsDigit(NW_XML_Reader_t* pT, NW_Uint32* pMatch)
{
    NW_Uint32 c;
    NW_Status_t s;

    *pMatch = 0;
    s = NW_XML_Reader_Peek(pT, &c);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (c > 0xffff) {/* validate casting */
        return NW_STAT_SUCCESS;
    }
    if (CXML_Str_Isdigit((NW_Ucs2)(c & 0xffff))) {
        *pMatch = 1;
    }
    return NW_STAT_SUCCESS;
}

/* Returns an estimate of the current line and column position in the text.
It is an estimate because it has to guess at what the intended line ending
sequence is using a count of CR and LF characters.  Line and Column indices
are 1-based not 0-based. */
EXPORT_C void
NW_XML_Reader_GetLineColumn(NW_XML_Reader_t* pT, NW_Uint32* pLine,
                         NW_Uint32* pColumn)
{
    NW_Uint32 crCount, lfCount, charsSinceCR, charsSinceLF;
    crCount = pT->lineColumn.crCount;
    lfCount = pT->lineColumn.lfCount;
    charsSinceCR = pT->lineColumn.charsSinceLastCR;
    charsSinceLF = pT->lineColumn.charsSinceLastLF;
    if (crCount == lfCount) {
        /* assume CR, LF, DOS style */
        /* use a bias in favor of CR followed by LF
        which will give the correct column for DOS */
        *pLine = lfCount + 1;
        *pColumn = charsSinceLF + 1;
    } else if (lfCount == 0) {
        /* assume CR only, Unix style */
        *pLine = crCount + 1;
        *pColumn = charsSinceCR + 1;
    } else if (crCount == 0) {
        /* assume LF only, Mac style */
        *pLine = lfCount + 1;
        *pColumn = charsSinceLF + 1;
    } else {
        /* an unclear situation so use
        thresholds on the ratio to guess */
        NW_Uint32 ratio;
        ratio = ((crCount * 100) / lfCount);
        if (ratio > 300) {/* more than 3 to 1 crCount to lfCount */
            /* assume CR only, Unix style */
            *pLine = crCount + 1;
            *pColumn = charsSinceCR + 1;
        } else if (ratio < 33) {/* less than 1 to 3 crCount to lfCount */
            /* assume LF only, Mac style */
            *pLine = lfCount + 1;
            *pColumn = charsSinceLF + 1;
        } else {
            /* assume CR, LF, DOS style */
            /* use a bias in favor of CR, LF sequence (DOS style)
            which will give the correct column */
            *pLine = lfCount + 1;
            *pColumn = charsSinceLF + 1;
        }
    }
}

