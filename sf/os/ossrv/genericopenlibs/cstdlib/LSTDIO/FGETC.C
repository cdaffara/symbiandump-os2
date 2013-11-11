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
* <<fgetc>>---get a character from a file or stream
* INDEX
* fgetc
* ANSI_SYNOPSIS
* #include <stdio.h>
* int fgetc(FILE *<[fp]>);
* TRAD_SYNOPSIS
* #include <stdio.h>
* int fgetc(<[fp]>)
* FILE *<[fp]>;
* Use <<fgetc>> to get the next single character from the file or stream
* identified by <[fp]>.  As a side effect, <<fgetc>> advances the file's
* current position indicator.
* For a macro version of this function, see <<getc>>.
* RETURNS
* The next character (read as an <<unsigned char>>, and cast to
* <<int>>), unless there is no more data, or the host system reports a
* read error; in either of these situations, <<fgetc>> returns <<EOF>>.
* You can distinguish the two situations that cause an <<EOF>> result by
* using the <<ferror>> and <<feof>> functions.
* PORTABILITY
* ANSI C requires <<fgetc>>.
* Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
* <<lseek>>, <<read>>, <<sbrk>>, <<write>>.
* 
*
*/



#include <stdio.h>
#include "LOCAL.H"

/**
Get the next character from a stream.
Returns the next character of the stream and increases the file pointer
to point to the following one.
@return   The character read is returned as an int value.
If the End Of File has been reached or there has been an error reading,
the function returns EOF. 
@param fp pointer to an open file.
*/
EXPORT_C int
fgetc (FILE * fp)
{
  return __sgetc (fp);
}
