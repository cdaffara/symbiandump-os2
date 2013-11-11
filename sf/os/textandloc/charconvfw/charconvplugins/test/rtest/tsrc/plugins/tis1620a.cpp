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
* Implement the dummy charconv plugin CTIS1620aImplementation
*
*/


#include <convgeneratedcpp.h>
#include <ecom/implementationproxy.h>
#include "shiftjis.h"
#include "tis1620.h"


const TDesC8& CTIS1620aImplementation::ReplacementForUnconvertibleUnicodeCharacters()
{
	return CnvShiftJis::ReplacementForUnconvertibleUnicodeCharacters();
}

TInt CTIS1620aImplementation::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness /*aDefaultEndiannessOfForeignCharacters*/,
 const TDesC8& /*aReplacementForUnconvertibleUnicodeCharacters*/, TDes8& /*aForeign*/, const TDesC16& /*aUnicode*/, 
CCnvCharacterSetConverter::TArrayOfAscendingIndices& /*aIndicesOfUnconvertibleCharacters*/)
{
	return 0;
}

TInt CTIS1620aImplementation::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness /*aDefaultEndiannessOfForeignCharacters*/,
 TDes16& /*aUnicode*/, const TDesC8& /*aForeign*/, TInt&, TInt& /*aNumberOfUnconvertibleCharacters*/, TInt& /*aIndexOfFirstByteOfFirstUnconvertibleCharacter*/)
{
	return 0;
}

TBool CTIS1620aImplementation::IsInThisCharacterSetL(TBool& /*aSetToTrue*/, TInt& /*aConfidenceLevel*/, const TDesC8&)
{
	return EFalse;
}

CTIS1620aImplementation* CTIS1620aImplementation::NewL()
{
CTIS1620aImplementation* self = new (ELeave) CTIS1620aImplementation;
return self;
}


CTIS1620aImplementation::CTIS1620aImplementation()
{
//default constructor.. do nothing
}

CTIS1620aImplementation::~ CTIS1620aImplementation()
{
//default destructor .. do nothing
}

// THE FOLLOWING FROM ECOM CREATION FUNCTION

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KTIS1620aImplUid,	CTIS1620aImplementation::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KTIS1620bImplUid,	CTIS1620bImplementation::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

