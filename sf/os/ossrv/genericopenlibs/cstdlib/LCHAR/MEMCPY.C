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
* <<memcpy>>---copy memory regions
* ANSI_SYNOPSIS
* #include <string.h>
* void* memcpy(void *<[out]>, const void *<[in]>, size_t <[n]>);
* This function copies <[n]> bytes from the memory region
* pointed to by <[src]> to the memory region pointed to by
* <[dest]>.
* If the regions overlap, the behavior is undefined.
* RETURNS
* <<memcpy>> A pointer to a location length bytes beyond 
* dest (i.e. the location dest+length).
* PORTABILITY
* <<memcpy>> is ANSI C.
* <<memcpy>> requires no supporting OS subroutines.
* QUICKREF
* memcpy ansi pure
* 
*
*/



#include <string.h>
#include <stddef.h>

#ifndef __GCC32__
#undef  memcpy
#endif

IMPORT_C void* memcpy(void* aTrg, const void* aSrc, unsigned int aLength);	

EXPORT_C void* _e32memcpy (void* dest, const void* src, size_t length)
	{
  	return (char*)memcpy(dest, src, length);		
	}

