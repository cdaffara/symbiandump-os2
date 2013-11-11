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
* FUNCTION
* <<fputc>>---write a character on a stream or file
* INDEX
* fputc
* ANSI_SYNOPSIS
* #include <stdio.h>
* int fputc(int <[ch]>, FILE *<[fp]>);
* TRAD_SYNOPSIS
* #include <stdio.h>
* int fputc(<[ch]>, <[fp]>)
* int <[ch]>;
* FILE *<[fp]>;
* <<fputc>> converts the argument <[ch]> from an <<int>> to an
* <<unsigned char>>, then writes it to the file or stream identified by
* <[fp]>.
* If the file was opened with append mode (or if the stream cannot
* support positioning), then the new character goes at the end of the
* file or stream.  Otherwise, the new character is written at the
* current value of the position indicator, and the position indicator
* advances by one.
* For a macro version of this function, see <<putc>>.
* RETURNS
* If successful, <<fputc>> returns its argument <[ch]>.  If an error
* intervenes, the result is <<EOF>>.  You can use `<<ferror(<[fp]>)>>' to
* query for errors.
* PORTABILITY
* <<fputc>> is required by ANSI C.
* Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
* <<lseek>>, <<read>>, <<sbrk>>, <<write>>.
* 
*
*/



#include <stdio.h>
/**
Write character to stream.
@return If there are no errors the written character is returned.
If an error occurs, EOF is returned.
@param ch Character to be written. 
The function casts the int parameter to its unsigned char equivalent before writing it. 
@param file pointer to an open file.
*/
EXPORT_C int
fputc (int ch, FILE * file)
{
  return putc ((unsigned char)ch, file);
}
