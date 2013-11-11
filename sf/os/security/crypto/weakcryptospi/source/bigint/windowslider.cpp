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
*
*/


#include <bigint.h>
#include "windowslider.h"
#include "../common/inlines.h"

TWindowSlider::TWindowSlider(const TInteger& aExp, TUint aWindowSize)
	: iExp(aExp), iSize(aWindowSize)
	{
	if(iSize == 0)
		{
		TUint expLen = iExp.BitCount();
		//These numbers are more or less arbitrary and can be tuned for empirical
		//performance results if desired.  It's a trade off between amount of
		//precomputation (more if larger iSize) and number of iterations
		//(more if smaller iSize).  The current defaults were obtained
		//from crypto++
		if( expLen <= 17 )
			iSize = 1;
		else if( expLen <= 24 )
			iSize = 2;
		else if( expLen <= 70 )
			iSize = 3;
		else if( expLen <= 197 )
			iSize = 4;
		else if( expLen <= 539 )
			iSize = 5;
		else if( expLen <= 1434 )
			iSize = 6;
		else 
			iSize = 7;
		}
	assert(iSize>=1 && iSize<=7);
	}

void TWindowSlider::FindNextWindow(TUint aBegin)
	{
	assert(iExp.Bit(aBegin)); //initial bit must be 1
	TInt end = aBegin;
	TUint temp = 0;
	iValue = 0;
	TUint j = 0;
	for(TInt i=aBegin; i>=0 && j<iSize; i--, j++)
		{
		TUint saveMask = WORD_BITS - j - 1;
		temp |= iExp.Bit(i) << saveMask;
		if( temp & (1 << saveMask) )
			{
			end = i;
			iValue |= temp; //last valid substring
			}
		}

	iLength = aBegin - end + 1;
	iValue >>= (WORD_BITS - iLength);
	}

