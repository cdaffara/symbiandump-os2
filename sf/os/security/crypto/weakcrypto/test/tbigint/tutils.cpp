/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tutils.h"
#include <bigint.h>
#include "t_output.h"

void Utils::DumpInteger(Output& aOut, const TDesC& aDesc, 
	const RInteger& aThat)
	{
	aOut.writeString(aDesc);
	HBufC8* buf = aThat.BufferLC();
	aOut.writeOctetString(*buf);
	aOut.writeNewLine();
	CleanupStack::PopAndDestroy(buf);
	}


/* CRandomSetSource */
CRandomSetSource::CRandomSetSource(const TDesC8& aSource)
    {
	SetSource(aSource);
    }

void CRandomSetSource::GenerateBytesL(TDes8& aDest)
    {
	TInt i=0;
	//Reverse the string by bytes up to the point of the end of the given string
	//of "random" bytes.
    for (i=0; iCounter >= 0 && i< aDest.Length(); ++i,--iCounter)
        {
        aDest[i] = iValue[iCounter];
        }
	//Then fill the remaining (if any) bytes in aDest with 0's.  This is all to
	//allow the stuff to work with bigint style integers; we're writing straight
	//into a RInteger.Ptr() here
	for(;i<aDest.Length(); ++i)
		{
        aDest[i] = 0x00;
		}
    }

void CRandomSetSource::SetSource(const TDesC8& aSource)
	{
	iValue.Set(aSource);
	Reset();
	}

void CRandomSetSource::Reset(void) 
	{
	iCounter = iValue.Length() - 1;
	}
