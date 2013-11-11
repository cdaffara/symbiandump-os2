/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Utility functions and constants required to use automatically generated
* lookup tables used by alternate charconv conversion implementation
* Currently only used by ShiftJis_Shared and ShiftJis_Docomo shared.
*
*/


#include <e32std.h>

// Functions used by lookup table driven charconv implementations
// to access lookup tables 
TUint16 lookupUnicodeChar(TUint aRow, TUint aColumn) ;
TUint16 lookupForeignChar(TUint aRow, TUint aColumn) ; 
TUint16 getUnicodeUnmappedCharacter(void) ;
TUint16 getForeignUnmappedCharacter(void) ;

// Offsets into lookup tables used by above functions.
const TUint32 KlowestIndex = 0 ;
const TUint32 KhighestIndex = 1 ;
const TUint32 KFirstDataIndex = 2 ;

