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
* <<abs>>---integer absolute value (magnitude)
* INDEX
* abs
* ANSI_SYNOPSIS
* #include <stdlib.h>
* int abs(int <[i]>);
* TRAD_SYNOPSIS
* #include <stdlib.h>
* int abs(<[i]>)
* int <[i]>;
* <<abs>> returns
* @tex
* $|x|$,
* @end tex
* the absolute value of <[i]> (also called the magnitude
* of <[i]>).  That is, if <[i]> is negative, the result is the opposite
* of <[i]>, but if <[i]> is nonnegative the result is <[i]>.
* The similar function <<labs>> uses and returns <<long>> rather than <<int>> values.
* RETURNS
* The result is a nonnegative integer.
* PORTABILITY
* <<abs>> is ANSI.
* No supporting OS subroutines are required.
* 
*
*/



#include <stdlib.h>

/**
Return absolute value of integer parameter.
@return   The absolute value of n.
@param i Integer value.
*/
EXPORT_C int abs (int i)
{
  return (i < 0) ? -i : i;
}
