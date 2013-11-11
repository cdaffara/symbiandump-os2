/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implement the dummy charconv plugin CTIS1620bImplementation
*
*/


#include <convgeneratedcpp.h>
#include <ecom/implementationproxy.h>
#include "shiftjis.h"
#include "tis1620.h"


const TDesC8& CTIS1620bImplementation::ReplacementForUnconvertibleUnicodeCharacters()
{
	return CnvShiftJis::ReplacementForUnconvertibleUnicodeCharacters();
}

TInt CTIS1620bImplementation::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness /*aDefaultEndiannessOfForeignCharacters*/,
 const TDesC8& /*aReplacementForUnconvertibleUnicodeCharacters*/, TDes8& /*aForeign*/, const TDesC16& /*aUnicode*/, 
CCnvCharacterSetConverter::TArrayOfAscendingIndices& /*aIndicesOfUnconvertibleCharacters*/)
{
	return 0;
}

TInt CTIS1620bImplementation::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness /*aDefaultEndiannessOfForeignCharacters*/,
 TDes16& /*aUnicode*/, const TDesC8& /*aForeign*/, TInt&, TInt& /*aNumberOfUnconvertibleCharacters*/, TInt& /*aIndexOfFirstByteOfFirstUnconvertibleCharacter*/)
{
	return 0;
}

TBool CTIS1620bImplementation::IsInThisCharacterSetL(TBool& /*aSetToTrue*/, TInt& /*aConfidenceLevel*/, const TDesC8&)
{
	return EFalse;
}

CTIS1620bImplementation* CTIS1620bImplementation::NewL()
{
CTIS1620bImplementation* self = new (ELeave) CTIS1620bImplementation;
return self;
}


CTIS1620bImplementation::CTIS1620bImplementation()
{
//default constructor.. do nothing
}

CTIS1620bImplementation::~ CTIS1620bImplementation()
{
//default destructor .. do nothing
}

// The ImplementationTable is in TIS1620a.cpp

