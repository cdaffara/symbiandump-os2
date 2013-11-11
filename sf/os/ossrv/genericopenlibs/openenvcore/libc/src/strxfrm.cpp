// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : strxfrm.cpp
// Part of     : MRT
// Implementation for wcpcpy API
// Version     : 1.0
//


 
#include <string.h>
#include <e32std.h>

extern "C" {

EXPORT_C
size_t strxfrm(char *dest, const char *src, size_t n)
{
	TInt i;	
	unsigned char temp;
	if(n == 0 && dest == NULL)
		return(strlen(src));
	for(i = 0; src[i] != '\0' && i!= n; i++)	
		{
		temp = src[i];
		dest[i] = (char) User :: Collate(temp);	
		}
	return i;			
}

} //extern "C"
