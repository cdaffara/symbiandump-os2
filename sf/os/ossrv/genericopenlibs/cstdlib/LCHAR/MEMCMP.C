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
* <<memcmp>>---compare two memory areas
* INDEX
* memcmp
* ANSI_SYNOPSIS
* #include <string.h>
* int memcmp(const void *<[s1]>, const void *<[s2]>, size_t <[n]>);
* TRAD_SYNOPSIS
* #include <string.h>
* int memcmp(<[s1]>, <[s2]>, <[n]>)
* void *<[s1]>;
* void *<[s2]>;
* size_t <[n]>;
* This function compares not more than <[n]> characters of the
* object pointed to by <[s1]> with the object pointed to by <[s2]>.
* RETURNS
* The function returns an integer greater than, equal to or
* less than zero 	according to whether the object pointed to by
* <[s1]> is greater than, equal to or less than the object
* pointed to by <[s2]>.
* PORTABILITY
* <<memcmp>> is ANSI C.
* <<memcmp>> requires no supporting OS subroutines.
* QUICKREF
* memcmp ansi pure
* 
*
*/



#include <string.h>
/**
Compare two buffers.
Compares the fisrt num bytes of two memory blocks pointed by m1 and m1.
@return a value indicating the relationship between the buffers
@param m1 Pointer to buffer. 
@param m2 Pointer to buffer. 
@param n  Number of bytes to compare.
*/

IMPORT_C signed int memcompare(const unsigned char*, signed int, const unsigned char*, signed int);


EXPORT_C int
memcmp (const void* m1, const void* m2, size_t n)
{
  return memcompare((const unsigned char*)m1, n, (const unsigned char*)m2, n); 
}
