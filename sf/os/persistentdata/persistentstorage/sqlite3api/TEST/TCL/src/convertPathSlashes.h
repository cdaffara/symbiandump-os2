// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
typedef enum {
    TO_TCL,	    /* convert to unix-style for tcl (\\ -> /) */
    TO_SYMBIAN,	/* convert to Windows-style fir Symbian (/ -> \\) */    
} PathConversionDirection;



char* tclSymbianPathSlashConversion(PathConversionDirection direction, char* pTo, const char* pFrom);
void tclCopySymbianPathSlashConversion(PathConversionDirection direction, char* toStrPtr, const char* fromStrPtr);
