/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TCUSTOMWRAP_H_

#include <frmtlay.h>
#include <linebreak.h>

// ------------ TTestCustomWrap ---------

/**
 * A custom wrapping class that allows line breaks anywhere, but allows (Latin)
 * commas, full-stops and closing brackets to "wrap-up" to the previous line.
 */
class TTestCustomWrap : public MFormCustomWrap
	{
	// All characters are given the same class. It does not matter which it is
	// as long as it is not ESpLineBreakClass. Spaces must not be treated
	// differently from other characters.
	TUint LineBreakClass(TUint /*aCode*/,TUint& aRangeStart,TUint& aRangeEnd) const
		{
		// All characters have the same class..
		aRangeStart = 0;
		aRangeEnd = 0x110000;
		// ..this class is Ideographic
		return MTmCustom::EIdLineBreakClass;
		}
	TBool LineBreakPossible(TUint /*aPrevClass*/,
		TUint /*aNextClass*/, TBool /*aHaveSpaces*/) const
		{
		// Line breaking allowed between any class of characters.
		// In practice they will all be EIdLineBreakClass, because we have
		// fixed it so that they will be.
		return ETrue;
		}
	TBool IsHangingCharacter(TUint aChar) const
		{
		// Allow commas, full stops and closing brackets to hang over the
		// margins. In practice, ideographic commas, full-stops and brackets
		// should be allowed as well as many other types of closing punctuation.
		if (aChar == ',' || aChar == '.' || aChar == ')')
			return ETrue;
		return EFalse;
		}
	};
	
/**
 * A custom wrapping class that allows line breaks between alphabetic and 
 * break after classes
 */	
class TTestCustomWrap2 : public MFormCustomWrap
	{

	TBool LineBreakPossible(TUint aPrevClass,
		TUint aNextClass, TBool aHaveSpaces) const
		{
		TBool breakAllowed;
		breakAllowed = MFormCustomWrap::LineBreakPossible(aPrevClass,aNextClass,aHaveSpaces);
			
		if(!breakAllowed)
			{
			//Specifically allow breaking between alphabetic and break after
			//classes (e.g. Tab character)
			breakAllowed = ((aPrevClass == MLineBreaker::EAlLineBreakClass) 
							&& (aNextClass == MLineBreaker::EBaLineBreakClass));
			}
		
		return breakAllowed;
		}

	};
	
/**
 * A custom wrapping class that allows line breaks between alphabetic and 
 * break after classes, but allows (Latin) commas, full-stops,
 * closing brackets and tabs to "wrap-up" to the previous line.
 */		
class TTestCustomWrap3 : public MFormCustomWrap
	{

	TBool LineBreakPossible(TUint aPrevClass,
		TUint aNextClass, TBool aHaveSpaces) const
		{

		TBool breakAllowed;
		breakAllowed = MFormCustomWrap::LineBreakPossible(aPrevClass,aNextClass,aHaveSpaces);
		
		if(!breakAllowed)
			{
			//Specifically allow breaking between alphabetic and break after
			//classes (e.g. Tab character)
			breakAllowed = ((aPrevClass == MLineBreaker::EAlLineBreakClass) 
							&& (aNextClass == MLineBreaker::EBaLineBreakClass));
			}
		
		return breakAllowed;
		}
	
	TBool IsHangingCharacter(TUint aChar) const
		{
		// Allow commas, full stops, closing brackets and tabs to hang over the
		// margins. In practice, ideographic commas, full-stops and brackets
		// should be allowed as well as many other types of closing punctuation.
		if (aChar == ',' || aChar == '.' || aChar == ')' || aChar == '\t')
			return ETrue;
		return EFalse;
		}
	};
	
/**
 * A custom wrapping class that sets tabs as space breaking characters
 */	
class TTestCustomWrap4 : public MFormCustomWrap
	{
	TUint LineBreakClass(TUint aCode,TUint& aRangeStart,TUint& aRangeEnd) const
		{
		TUint breakClass;
		
		//Set tabs to be space breaking class
		if(aCode == 0x09)
			{
			breakClass = MLineBreaker::ESpLineBreakClass;	
			}
		else
			{
			breakClass = MFormCustomWrap::LineBreakClass(aCode,aRangeStart, aRangeEnd);
			}
		return breakClass;
		}
	};


#endif
