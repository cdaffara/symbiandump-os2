/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* TWindowSlider class implementation
*
*/


/**
 @file 
 @internalComponent
*/
 
#ifndef __WINDOWSLIDER_H__
#define __WINDOWSLIDER_H__

#include <e32std.h>

class TInteger; 
/** Utility class used by our fast exponentiation routines.\n\n
 * See HAC 14.85 for an explanation of how window sliding helps speed things up.
 * @internalComponent
 */
class TWindowSlider
{
public:
	/** 
	 * @param aExp The exponent you are using.
	 * @param aWindowSize The window size.  Leave as default value for the
	 * constructor to pick a window size appropriate for the given aExp
	 * @internalComponent
	 */
	TWindowSlider(const TInteger& aExp, TUint aWindowSize=0);
	/**
	 * Finds the next "window" as defined by HAC 14.85.  The actual bitstring
	 * value is in iValue and it's length in iLength.  These remain valid until
	 * the next call to FindNextWindow()
	 */
	void FindNextWindow(TUint aBegin);
	inline TUint WindowSize(void) {return iSize;}
	inline TUint Value(void) {return iValue;}
	inline TUint Length(void) {return iLength;}

private:
	const TInteger& iExp; ///the exponent all this is being calculated on
	TUint iSize;  ///The size of the sliding window
	TUint iValue; ///the value found by the most recent FindNextWindow() call
	TUint iLength;///the bit length of the iValue
};

#endif
