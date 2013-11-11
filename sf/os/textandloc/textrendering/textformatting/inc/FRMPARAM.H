/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* FORM global parameters; held in thread-local storage.
*
*/


#ifndef __FRMPARAM_H__
#define __FRMPARAM_H__

#include <gdi.h>


/** 
Specifies the protocol for getting system colours for use in text views. A
class which implements this protocol allows an application (or a thread,
because a pointer to the object is held in thread-local storage) to use system
colours in text views.

You should create an object of an MFormParam-derived class which implements the
SystemColor() function. Then, pass a pointer to the object to the static
function MFormParam::Set(). The MFormParam-derived object is owned by the
caller of Set(). When system colours are no longer needed, you should cancel
them by calling MFormParam::Set(NULL). It is the caller's responsibility to
cause the text view to be redrawn after system colours are changed.

CTextLayout uses the colour translation function SystemColor() (if an
MFormParam-derived object is present) to set the pen and brush colours for
drawing components of the text view. It calls MFormParam::Get() before each
draw. This enables an application to introduce system colours, change the
parameter object, or abolish system colours, and redraw everything correctly;
no information is left over from the last draw.
@publishedAll
@released
*/
class MFormParam

	{
	public:

	/** Translates 8-bit logical colour indices to physical colours pure
	virtual function.

	The colour index is stored in a TLogicalRgb object and is returned by
	TLogicalRgb::SystemColorIndex().

	There are three reserved index values:

	Zero means the logical colour does not map to a system colour: the logical
	colour is in fact an ordinary TRgb. This function should not normally be
	called with this value.

	254 and 255 are default foreground and background colours respectively, and
	should be converted to the GUI's standard foreground and background colours

	The values 1-253 are reserved for the use of the GUI and can be interpreted
	as convenient. These can be interpreted as
	TLogicalColor::EColorWindowBackground upward; this mapping can be done by
	subtracting one from the index number aColorIndex.

	@param aColorIndex Logical colour index. Between 1 and 255 inclusive.
	@param aDefaultColor The default colour. This is the colour to be used if
	no translation is performed. This allows translation to change certain
	indices but not others, (by passing the default colour back unchanged).
	@return The physical colour which maps to the logical colour index. */
	virtual TRgb SystemColor(TUint aColorIndex,TRgb aDefaultColor) const = 0;
	IMPORT_C virtual void Reserved();
	IMPORT_C static void Set(const MFormParam* aParam);
	IMPORT_C static const MFormParam* Get();
	};

#endif // __FRMPARAM_H__
