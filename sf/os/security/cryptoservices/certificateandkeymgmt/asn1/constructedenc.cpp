/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Methods common to all constructed-data-type encoding classes
*
*/


#include <asn1enc.h>


CASN1EncConstructed::CASN1EncConstructed(const TTagType aType, const TASN1Class aClass)
: CASN1EncBase(aType, aClass)
	{
	}


TBool CASN1EncConstructed::IsConstructed() const
	{
	return ETrue;
	}


void CASN1EncConstructed::CalculateContentsLengthDER()
	{
	iContentsLengthDER = 0;
	TUint numChildren = NumChildren();
	for (TUint index = 0; index < numChildren; ++index)
		{
		iContentsLengthDER += Child(index).LengthDER();
		}
	}


void CASN1EncConstructed::WriteContentsDERL(TDes8& aBuf) const
	{
	TUint pos = 0;
	TUint numChildren = NumChildren();
	for (TUint index = 0; index < numChildren; ++index)
		{
		Child(index).WriteDERL(aBuf, pos);
		}
	}
