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


/** ----------------------------------------------------------------------- **
    @package:     NW_XML

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_XML_READER_H
#define NW_XML_READER_H

#include <xml/cxml/cxml_proj.h>
/** ----------------------------------------------------------------------- **
    @enum:        NW_XML_Endianness

    @synopsis:    Endian indication.

    @scope:       public
    @names:
       [NW_NATIVE_ENDIAN]
                  Use native endianness.

       [NW_BIG_ENDIAN]
                  The leftmost bytes (those with a lower address) are 
                  most significant. 

       [NW_LITTLE_ENDIAN]
                  The rightmost bytes (those with a higher address) are 
                  most significant

    @description: Endian indication.
 ** ----------------------------------------------------------------------- **/
typedef enum NW_XML_Endianness_e {
  NW_NATIVE_ENDIAN,
  NW_BIG_ENDIAN,
  NW_LITTLE_ENDIAN
} NW_XML_Endianness_t;

/* Remove references to this deprecated typedef */
typedef NW_XML_Endianness_t NW_Endianness_t;

/** ----------------------------------------------------------------------- **
    @struct:      NW_XML_Reader_LineColumn

    @synopsis:    Line and column cursor.

    @scope:       public
    @variables:
       NW_Uint32 crCount
                  Carriage return count.

       NW_Uint32 lfCount
                  Line feed count.

       NW_Uint32 charsSinceLastCR
                  Chars since last CR

       NW_Uint32 charsSinceLastLF
                  Chars since last LF

    @description: Line and column cursor.  

 ** ----------------------------------------------------------------------- **/
typedef struct NW_XML_Reader_LineColumn_s{
    NW_Uint32 crCount;
    NW_Uint32 lfCount;
    NW_Uint32 charsSinceLastCR;
    NW_Uint32 charsSinceLastLF;
}NW_XML_Reader_LineColumn_t;


/** ----------------------------------------------------------------------- **
    @struct:       NW_XML_Reader

    @synopsis:    The XML reader structure.

    @scope:       public
    @variables:
       NW_Uint32 encoding
                  Character set encoding code in force at this index, 
                  may change.

       NW_XML_Endianness_t endianness
                  Big, little or native endianness indicator.

       NW_Uint32 index
                  Current read position in pBuf, a byte index moved
                  by character according to the encoding.

       NW_Uint32 charIndex
                  Similar to index but in terms of characters not bytes.

       NW_XML_Reader_LineColumn_t lineColumn
                  Information for giving text position as line and column,
                  useful for reporting parsing failure.

       NW_Uint32 length
                  Length of data.

       NW_Uint8* pBuf
                  Text is stored in pBuf and the Reader functions provide
                  the interface to the characters in pBuf.

       NW_Bool end
                  End (like EOF): 0 = not at end, 1 = at end.

    @description: Users of Reader should not look directly inside the structure.
                  Instead, use access functions and macros.

 ** ----------------------------------------------------------------------- **/
typedef struct NW_XML_Reader_s{
    NW_Uint32 encoding;
    NW_XML_Endianness_t endianness;
    NW_Uint32 index;
    NW_Uint32 charIndex;
    NW_XML_Reader_LineColumn_t lineColumn;
    NW_Uint32 length;
    NW_Uint8* pBuf;
    NW_Bool end;
} NW_XML_Reader_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_XML_Reader_Interval

    @synopsis:    XML reader interval structure.

    @scope:       public
    @variables:
       NW_Uint32 start
                  Byte index.

       NW_Uint32 stop
                  Byte index.

       NW_Uint32 charStart
                  Character index.

       NW_Uint32 charStop
                  Character index.

    @description: Holds Reader index values defining an interval.
                  Useful to mark a substring in a Reader.  If the text
                  is "this is a sample" and the interval has start = 0,
                  stop = 4 then the substring is "this".  Length is stop
                  - start.  If both values are NW_UINT32_MAX then these
                  are sentinel values meaning an initialized but unset
                  interval, otherwise, if both are the same, it means
                  start has been set but stop hasn't been set.  Don't
                  directly set, use appropriate interval functions.

                  Note: You may also need to record the encoding that
                  applies to this interval if Reader encoding has changed.

 ** ----------------------------------------------------------------------- **/
typedef struct NW_XML_Reader_Interval_s{
    NW_Uint32 start;
    NW_Uint32 stop;
    NW_Uint32 charStart;
    NW_Uint32 charStop;
} NW_XML_Reader_Interval_t;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* ----------------------------------------------------------------------- **
Reader Functions  
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_InitFromBuffer

    @synopsis:    Initialize from buffer.

    @scope:       public

    @parameters:
       [in-out] NW_XML_Reader_t* pT
                  The XML reader.

       [in] NW_Uint32 length
                  Length of buffer.

       [in] unsigned char* pBuf
                  The buffer.

    @description: Pass in pBuf, the pointer (not the data) will be copied 
                  into the NW_XML_Reader_t.pBuf.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_InitFromBuffer(NW_XML_Reader_t* pT, NW_Uint32 length, unsigned char* pBuf);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_DataAddressFromBuffer

    @synopsis:    Gets pointer into buffer.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [in] NW_Uint32 startByteIndex
                  The index.

       [in-out] NW_Uint32* byteLength
                  IN as desired length, OUT as either desired length or
                  remainder of buffer, whichever is less.

       [out] NW_Uint8** ppData
                  Pointer to data.

    @description: Returns a pointer into pBuf in *ppData, clips length 
                  to fit in buffer limits.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Address resolved.

       [NW_STAT_FAILURE]
                  Index out of range.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_DataAddressFromBuffer(NW_XML_Reader_t* pT, NW_Uint32 startByteIndex,
                                 NW_Uint32* byteLength, NW_Uint8** ppData);

#define NW_XML_Reader_GetEncoding(pT) (((const NW_XML_Reader_t*)pT)->encoding)
#define NW_XML_Reader_SetEncoding(pT, e) ((pT)->encoding = (e))

#define NW_XML_Reader_GetEndianness(pT) (((const NW_XML_Reader_t*)pT)->endianness)
#define NW_XML_Reader_SetEndianness(pT, e) ((pT)->endianness = (e))


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_GetPosition

    @synopsis:    Returns current position.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [out] NW_Uint32* pByteIndex
                  Current byte index.

       [out] NW_Uint32* pCharIndex
                  Current character index.

       [out] NW_XML_Reader_LineColumn_t* pLineColumn
                  Current line and column.

    @description: Returns current position.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
void NW_XML_Reader_GetPosition(NW_XML_Reader_t* pT, NW_Uint32* pByteIndex,
                               NW_Uint32* pCharIndex,
                               NW_XML_Reader_LineColumn_t* pLineColumn);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_SetPosition

    @synopsis:    Sets position in reader.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [in] NW_Uint32 byteIndex
                  The byte index.

       [in] NW_Uint32 charIndex
                  The character index.

       [in] const pLineColumn
                  The line and column.

    @description: Setting the position (similar to seeking in a file) is
                  in general not possible without reading the characters
                  (usually reading forward) because character encoding
                  may use a variable numbers of bytes per character. This
                  is here so that if you have defined a valid interval, 
                  then you can reposition to the beginning of the interval.
                  Setting to the position to a bad value will not always be
                  caught immediately.
 ** ----------------------------------------------------------------------- **/
IMPORT_C
void NW_XML_Reader_SetPosition(NW_XML_Reader_t* pT, NW_Uint32 byteIndex,
                               NW_Uint32 charIndex,
                               const NW_XML_Reader_LineColumn_t* pLineColumn);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_GetLineColumn

    @synopsis:    Gets estimate of line and column.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [out] NW_Uint32* pLine
                  The current line.

       [out] NW_Uint32* pColumn
                  The current column.

    @description: Returns an estimate of the current line and column 
                  position in the text. It is an estimate because it has
                  to guess at what the intended line ending sequence is 
                  using a count of CR and LF characters.  Line and Column
                  indices are 1-based not 0-based.
 ** ----------------------------------------------------------------------- **/
IMPORT_C
void NW_XML_Reader_GetLineColumn(NW_XML_Reader_t* pT, NW_Uint32* pLine,
                                 NW_Uint32* pColumn);

#define NW_XML_Reader_AtEnd(pT) (((const NW_XML_Reader_t*)pT)->end)


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_PeekOffset

    @synopsis:    Peeks at offset.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [in] NW_Uint32 nChars
                  Offset in characters.

       [out] NW_Uint32* pC
                  Address of character at offset.

    @description: Fails on encountering illegal char anywhere from current 
                  read position to offset position or if position is illegal
                  (e.g., off end).  Leaves read position unchanged.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Address returned.

       [NW_STAT_FAILURE]
                  Invalid offset or at end of file.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_PeekOffset(NW_XML_Reader_t* pT, NW_Uint32 nChars, NW_Uint32* pC);

#define NW_XML_Reader_Peek(pT, pC) NW_XML_Reader_PeekOffset((pT), 0, (pC))


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_AdvanceOffset

    @synopsis:    Advances offset in reader.

    @scope:       public

    @parameters:
       [in-out] NW_XML_Reader_t* pT
                  The XML reader.

       [in] NW_Uint32 nChars
                  Offset to move in characters.

    @description: Fails on encountering illegal char anywhere from 
                  current read position to offset position or if 
                  position is illegal (e.g., off end).  Moves read
                  position to current + offset.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Read position advanced nChars.

       [NW_STAT_FAILURE]
                  Invalid offset or at end of file, reader not advanced.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_AdvanceOffset(NW_XML_Reader_t* pT, NW_Uint32 nChars);

#define NW_XML_Reader_Advance(pT) NW_XML_Reader_AdvanceOffset((pT), 1)


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_AsciiCharMatch

    @synopsis:    Tests for character match.
    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [in] NW_Uint32 c
                  The character to match.

       [out] NW_Uint32* pMatch
                  default

    @description: pMatch is 1 if ASCII character c matches Reader char in its encoding.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Match results are valid.

       [NW_STAT_FAILURE]
                  Match results are not valid. Invalid offset or at end of file.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_AsciiCharMatch(NW_XML_Reader_t* pT, NW_Uint32 c, NW_Uint32* pMatch);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_AsciiStringMatch

    @synopsis:    Tests for character match.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [in] NW_Uint32 length
                  String length.

       [in] const NW_Uint8* pString,
                  String characters.

       [out] NW_Uint32* pMatch
                  1 if match, otherwise 0.

    @description: pMatch is 1 if ASCII string matches Reader sequence in its encoding.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Match results are valid.

       [NW_STAT_FAILURE]
                  Match results are not valid. Invalid offset or at end of file.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_AsciiStringMatch(NW_XML_Reader_t* pT, NW_Uint32 length, const NW_Uint8* pString,
                               NW_Uint32* pMatch);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_SkipSpace

    @synopsis:    Skip a space.

    @scope:       public

    @parameters:
       [in-out] NW_XML_Reader_t* pT
                  The XML reader.

    @description: Skip a space. For XML, whitespace is only ASCII 
                  0x20 (space), 0x09 (tab), 0x0d (CR), 0x0a (LF).
                  The base test used here, NW_Str_Isspace(), includes
                  two other forms of whitespace.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Skip successful.

       [NW_STAT_FAILURE]
                  Skip not done. Invalid offset or at end of file.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_SkipSpace(NW_XML_Reader_t* pT);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_IsSpace

    @synopsis:    Tests for space.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [out] NW_Uint32* pMatch
                  1 if space, otherwise 0.

    @description: Tests for space. For XML, whitespace is only ASCII 
                  0x20 (space), 0x09 (tab), 0x0d (CR), 0x0a (LF).  The 
                  base test used here, NW_Str_Isspace(), includes two 
                  other forms of whitespace. 

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Test results are valid.

       [NW_STAT_FAILURE]
                  Test results are not valid. Invalid offset or at end of file.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_IsSpace(NW_XML_Reader_t* pT, NW_Uint32* pMatch);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_IsLetter

    @synopsis:    Tests for letter.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [out] NW_Uint32* pMatch
                  1 if letter, otherwise 0.

    @description: Tests for letter.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Test results are valid.

       [NW_STAT_FAILURE]
                  Test results are not valid. Invalid offset or at end of file.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_IsLetter(NW_XML_Reader_t* pT, NW_Uint32* pMatch);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_IsDigit

    @synopsis:    Tests for digit.

    @scope:       public

    @parameters:
       [in] NW_XML_Reader_t* pT
                  The XML reader.

       [out] NW_Uint32* pMatch
                  1 if letter, otherwise 0.

    @description: Tests for digit. Digits include not only the ASCII
                  digits but other language forms of digits.  The base
                  test used here, NW_Str_Isdigit() only tests for ASCII 
                  digits.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Test results are valid.

       [NW_STAT_FAILURE]
                  Test results are not valid. Invalid offset or at end of file.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_XML_Reader_IsDigit(NW_XML_Reader_t* pT, NW_Uint32* pMatch);


/* ----------------------------------------------------------------------- **
   Reader Interval Functions  
** ----------------------------------------------------------------------- **/


/* Always initialize the interval. */
#define NW_XML_Reader_Interval_Init(pI) \
    ((pI)->start = (pI)->stop = (pI)->charStart = (pI)->charStop = NW_UINT32_MAX)

/* Useful in error tests and asserts. */
#define NW_XML_Reader_Interval_IsWellFormed(pI) \
    (((pI)->stop > (pI)->start) && ((pI)->charStop > (pI)->charStart))


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_Interval_Start

    @synopsis:    Initializes interval to current position.

    @scope:       public

    @parameters:
       [out] NW_XML_Reader_Interval_t* pI
                  The XML reader interval.

       [in] NW_XML_Reader_t* pT
                  The XML reader.

    @description: Sets start, stop, charStart, charStop to current 
                  Reader position.
 ** ----------------------------------------------------------------------- **/
IMPORT_C void
NW_XML_Reader_Interval_Start(NW_XML_Reader_Interval_t* pI, NW_XML_Reader_t* pT);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_Reader_Interval_Stop

    @synopsis:    Sets stop to Reader read index.

    @scope:       public

    @parameters:
       [out] NW_XML_Reader_Interval_t* pI
                  The XML reader interval.

       [in] NW_XML_Reader_t* pT
                  The XML reader.

    @description: Sets stop to Reader read index.

 ** ----------------------------------------------------------------------- **/
IMPORT_C void
NW_XML_Reader_Interval_Stop(NW_XML_Reader_Interval_t* pI, NW_XML_Reader_t* pT);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif /* NW_XML_READER_H */
