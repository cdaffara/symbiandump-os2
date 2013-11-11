// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implement case-insensitive comparison using the full folded comparison
// functions of E32
// 
//

#include <e32std.h>
#include <string.h>

/**
Compares the null-terminated strings left and right and returns an integer 
greater than,equal to, or less than zero (0), accordingly as left 
is lexicographically greater than,equal to, or less than right after
translation of each corresponding character to lowercase.
@return an integer greater than, equal to, or less than zero (0),
accordingly as the string left is greater than, equal to,
or less than the string right.
@param left The first string to compare.
@param right The second string to compare.
*/
extern "C" EXPORT_C int strcasecmp (const char *left, const char *right)
	{
	TPtrC8 Left((const TText8*)left);
	TPtrC8 Right((const TText8*)right);
	return Left.CompareF(Right);
	}

extern "C" EXPORT_C int strncasecmp (const char *left, const char *right, size_t length)
	{
	TUint leftlength=strlen(left);
	TUint rightlength=strlen(right);
	// The length parameter is the maximum amount of the string to be searched,
	// so truncate the descriptors appropriately
	if (leftlength>length)
		leftlength=length;
	if (rightlength>length)
		rightlength=length;
	TPtrC8 Left((const TText8*)left,leftlength);
	TPtrC8 Right((const TText8*)right,rightlength);
	return Left.CompareF(Right);
	}
