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
* <<fileno>>---return file descriptor associated with stream
* INDEX
* fileno
* ANSI_SYNOPSIS
* #include <stdio.h>
* int fileno(FILE *<[fp]>);
* TRAD_SYNOPSIS
* #include <stdio.h>
* int fileno(<[fp]>)
* FILE *<[fp]>;
* You can use <<fileno>> to return the file descriptor identified by <[fp]>.
* RETURNS
* <<fileno>> returns a non-negative integer when successful.
* If <[fp]> is not an open stream, <<fileno>> returns -1.
* PORTABILITY
* <<fileno>> is not part of ANSI C.
* POSIX requires <<fileno>>.
* Supporting OS subroutines required: none.
* 
*
*/



#include <stdio.h>
#include "LOCAL.H"

EXPORT_C int
fileno (FILE * f)
{
  CHECK_INIT (f);
  return __sfileno (f);
}
