/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef FRMCONST_INTERNAL_H_
#define FRMCONST_INTERNAL_H_

#include <w32std.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <frmconst.h>
#endif

/** 
Visible versions of control characters.
@internalComponent
*/
const TText KVisibleParagraphBreak = 0x00B6;		// paragraph sign (pilcrow)
const TText KVisibleLineBreak = 0x2193;				// down arrow
const TText KVisibleNonBreakSpace = 0x00B0;			// degree sign
const TText KVisiblePotentialHyphen = 0x00AC;		// not sign
const TText KVisibleNonBreakHyphen = 0x007E;		// spacing tilde
const TText KVisibleSpace = 0x00B7;					// middle dot
const TText KVisibleTab = 0x2192;					// right arrow
const TText KVisiblePicture = 0x25A3;				// white square containing small black square

/**
Control characters to allow or inhibit line breaks.
@internalComponent
*/
const TText KZeroWidthSpace = 0x200B;
const TText KZeroWidthNoBreakSpace = 0xFEFF;

/**
@internalComponent
*/
void FormPanic(TFormPanic aPanic);

#endif
