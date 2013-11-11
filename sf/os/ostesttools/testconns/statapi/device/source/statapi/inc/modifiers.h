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
*
*/


#if !defined(__MODIFIERS_H__)
#define __MODIFIERS_H__

#include <e32keys.h>

//------------------------------------------------------------------------------

#define NO_MORE_MODIFIERKEYS	_S("NO_MORE_MODIFIERKEYS")	// end of array

const TText* const ENUM_TEXTMODIFIERSArray[] = 
{
	_S("Autorepeatable"),
	_S("Keypad"),
	_S("LeftAlt"),
	_S("RightAlt"),
	_S("Alt"),
	_S("LeftCtrl"),
	_S("RightCtrl"),
	_S("Ctrl"),
	_S("LeftShift"),
	_S("RightShift"),
	_S("Shift"),
	_S("LeftFunc"),
	_S("RightFunc"),
	_S("Func"),
	_S("CapsLock"),
	_S("NumLock"),
	_S("ScrollLock"),
	_S("KeyUp"),
	_S("Special"),
	_S("DoubleClick"),
    _S("PureKeycode"),
	_S("EAllModifiers"),
	NO_MORE_MODIFIERKEYS
};

//------------------------------------------------------------------------------

const TEventModifier ENUM_VALMODIFIERSArray[] =
{
	EModifierAutorepeatable,
	EModifierKeypad,
	EModifierLeftAlt,
	EModifierRightAlt,
	EModifierAlt,
	EModifierLeftCtrl,
	EModifierRightCtrl,
	EModifierCtrl,
	EModifierLeftShift,
	EModifierRightShift,
	EModifierShift,
	EModifierLeftFunc,
	EModifierRightFunc,
	EModifierFunc,
	EModifierCapsLock,
	EModifierNumLock,
	EModifierScrollLock,
	EModifierKeyUp,
	EModifierSpecial,
	EModifierDoubleClick,
    EModifierPureKeycode,
	EAllModifiers
};

#endif
