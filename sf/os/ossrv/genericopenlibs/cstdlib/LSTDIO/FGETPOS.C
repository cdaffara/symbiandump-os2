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
* <<fgetpos>>---record position in a stream or file
* INDEX
* fgetpos
* ANSI_SYNOPSIS
* #include <stdio.h>
* int fgetpos(FILE *<[fp]>, fpos_t *<[pos]>);
* TRAD_SYNOPSIS
* #include <stdio.h>
* int fgetpos(<[fp]>, <[pos]>)
* FILE *<[fp]>;
* fpos_t *<[pos]>;
* Objects of type <<FILE>> can have a ``position'' that records how much
* of the file your program has already read.  Many of the <<stdio>> functions
* depend on this position, and many change it as a side effect.
* You can use <<fgetpos>> to report on the current position for a file
* identified by <[fp]>; <<fgetpos>> will write a value
* representing that position at <<*<[pos]>>>.  Later, you can
* use this value with <<fsetpos>> to return the file to this
* position.
* In the current implementation, <<fgetpos>> simply uses a character
* count to represent the file position; this is the same number that
* would be returned by <<ftell>>.
* RETURNS
* <<fgetpos>> returns <<0>> when successful.  If <<fgetpos>> fails, the
* result is <<1>>.  Failure occurs on streams that do not support
* positioning; the global <<errno>> indicates this condition with the
* value <<ESPIPE>>.
* PORTABILITY
* <<fgetpos>> is required by the ANSI C standard, but the meaning of the
* value it records is not specified beyond requiring that it be
* acceptable as an argument to <<fsetpos>>.  In particular, other
* conforming C implementations may return a different result from
* <<ftell>> than what <<fgetpos>> writes at <<*<[pos]>>>.
* No supporting OS subroutines are required.
* 
*
*/



#include <stdio.h>
/**
Get position in a stream.
@return 0 value indicates success. non-zero value indicates error.
@param fp pointer to an open file. 
@param pos pointer to a fpos_t object where the position will be stored. 
*/
EXPORT_C int
fgetpos (FILE * fp, fpos_t * pos)
{
  *pos = ftell (fp);

  if (*pos != -1)
    return 0;
  return 1;
}
