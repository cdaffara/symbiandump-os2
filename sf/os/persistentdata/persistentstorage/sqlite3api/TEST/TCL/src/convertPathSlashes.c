// Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
// This file converts a linux/tcl path to or from a windows path.
// 
//

/*
 * convert to a windows path.  required because PIPS
 * is mostly posix but uses Windows paths and tcl uses unix paths internally (see FsJoinPath())
 *
 * inspired by TclWinNoBackslash()
 *
 * July 30, 2007
 */

#include "convertPathSlashes.h"
  
/**  
 * Switch the separator chars inside a given string. both pointers can point
 *   to the same string or different strings.
 *
 * @param direction  direction to convert to.
 * @param pTo        pointer of string to be converted.
 * @param pFrom      pointer to string to be converted from.
 *
 * @ret   pTo        pointer to last char+1 of converted buffer.
 */
char* tclSymbianPathSlashConversion(direction, pTo, pFrom)
	PathConversionDirection direction;   // TO_TCL - '\\' to '/', TO_SYMBIAN - '/' to '\\'
	char *pTo;       // string to be converted.
	const char *pFrom;     // string to be converted.
{
	char  toChar;
	char  fromChar;
	
	if (direction == TO_TCL) {
		fromChar = '\\';
		toChar = '/';
	}
	else {        // TO_SYMBIAN
		fromChar = '/';
		toChar = '\\';		
	}
	
   	for (; *pFrom != '\0'; pTo++, pFrom++) {
		if (*pFrom == fromChar) {
    		*pTo = toChar;
		}		
		else {
			*pTo = *pFrom;	
		}
	}//for
		
	return pTo;	
}

/**  
 * Copies the source path to the destination path and null 
 *   terminates the destination string.
 *
 * @param direction  direction to convert to.
 * @param pTo        pointer of string to be converted.
 * @param pFrom      pointer to string to be converted from.
 */
// copy from fromStrPtr into a buffer supplied by toStrPtr.
void tclCopySymbianPathSlashConversion(direction, toStrPtr, fromStrPtr)
	PathConversionDirection direction;   // TO_TCL - '\\' to '/', TO_SYMBIAN - '/' to '\\'
	char *toStrPtr;    // string to be converted.
	const char *fromStrPtr;// string to convert from.
{
	char* pTo;
	const char* pFrom;

	
	pTo = toStrPtr;
	pFrom = fromStrPtr;
	pTo = tclSymbianPathSlashConversion(direction, pTo, pFrom);
	*pTo = '\0';		
}

