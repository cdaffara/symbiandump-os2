/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/

#include "tbrokenrandom.h"

/* CRandomSetSeed */
CRandomSetSource::CRandomSetSource(const TDesC8& aSource)
    {
	SetSource(aSource);
    }

void CRandomSetSource::GenerateBytesL(TDes8& aDest) 
    {
	if(aDest.Size() <= iValue.Size() - (TInt)iCounter)
		{
		for (int i = 0 ; i < aDest.Length() ; ++i)
			{
			aDest[i] = iValue[iCounter];
			iCounter++;
			} 
		}
	else
		{
		TRandom::RandomL(aDest);
		}
    }

void CRandomSetSource::SetSource(const TDesC8& aSource)
	{
	iValue.Set(aSource);
	iCounter = 0;
	}

/* CRandomIncrementing */
CRandomIncrementing::CRandomIncrementing(TUint aInitialValue)
    {
	iValue = aInitialValue;
    }

void CRandomIncrementing::GenerateBytesL(TDes8& aDest) 
    {
    for (int i = 0 ; i < aDest.Length() ; ++i)
        {
        aDest[i] = (char)(iValue)++;
        }
    }


