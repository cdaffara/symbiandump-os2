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
* <<feof>>---test for end of file
* INDEX
* feof
* ANSI_SYNOPSIS
* #include <stdio.h>
* int feof(FILE *<[fp]>);
* TRAD_SYNOPSIS
* #include <stdio.h>
* int feof(<[fp]>)
* FILE *<[fp]>;
* <<feof>> tests whether or not the end of the file identified by <[fp]>
* has been reached.
* RETURNS
* <<feof>> returns <<0>> if the end of file has not yet been reached; if
* at end of file, the result is nonzero.
* PORTABILITY
* <<feof>> is required by ANSI C.
* No supporting OS subroutines are required.
* 
*
*/



#include <stdio.h>
#include "LOCAL.H"

#undef feof

/**
Check if End Of File has been reached.
@return A non-zero value is returned in the case that the position
indicator reached the End Of File in the last input operation with
the specified stream, otherwise 0 is returned.
@param fp pointer to an open file.
*/
EXPORT_C int 
feof (FILE * fp)
{
  return __sfeof (fp);
}
