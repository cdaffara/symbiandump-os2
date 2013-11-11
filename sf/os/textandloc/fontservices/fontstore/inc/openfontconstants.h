/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* A header for constants for the open font system, which allows
* SymbianOS to use fonts of arbitrary types, including TrueType/OpenType
* and other outline font formats.
*
*/


#ifndef OPENFONTCONSTANTS_H
#define OPENFONTCONSTANTS_H

/**
The OpenFont ECOM Plug-in Interface Definition UID.

@internalTechnology
*/
const TInt KUidOpenFontRasterizerPlunginInterface = 0x101F7F5D;

/**
The Shaper Factory ECOM Plug-in Interface Definition UID.

@internalTechnology
*/
const TInt KUidShaperFactoryPlunginInterface = 0x10279726;


/**
Replacement character code.

This is a Unicode private use area codepoint, which is reserved in the Symbian 
OS to represent characters for which a glyph does not exist in a font (either 
a bitmap or scalable font). If a glyph exists in a font with this character 
code, it will be used for nonexistent characters, otherwise, the rasterizer's 
default replacement character glyph will be used. For example, TrueType fonts 
return glyph index 0 for nonexistent characters, and that is usually mapped 
to an empty rectangle.
@internalTechnology
*/
const TUint KReplacementCharacter = 0xF6DB;

/**
KFillCharacterOffset is a significant offset that is set when a character within a code section is 
not in the font. This means that for these fill characters nothing is stored within the binary
data part of the code section.
@internalTechnology
*/
const TInt KFillCharacterOffset = 0x7FFF;

#endif /*OPENFONTCONSTANTS_H*/
