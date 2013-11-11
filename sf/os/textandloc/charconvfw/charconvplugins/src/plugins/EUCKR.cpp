/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    This file is a source code file for a charconv plug-in.
*                This plug-in supports EUC-KR.
*
*/



// INCLUDES
#include <e32std.h>
#include <charconv.h>
#include <convgeneratedcpp.h>
#include <ecom/implementationproxy.h>
#include "cp949table.h"
#include "charactersetconverter.h"
// New Interface class
class CEUCKRImplementation : public CCharacterSetConverterPluginInterface
{
    public:
        virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();

        virtual TInt ConvertFromUnicode(
            CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
            const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
            TDes8& aForeign, 
            const TDesC16& aUnicode, 
            CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters );

        virtual TInt ConvertToUnicode(
            CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
            TDes16& aUnicode, 
            const TDesC8& aForeign, 
            TInt&, 
            TInt& aNumberOfUnconvertibleCharacters, 
            TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter );

        virtual TBool IsInThisCharacterSetL(
            TBool& aSetToTrue, 
            TInt& aConfidenceLevel, 
            const TDesC8& );

        static CEUCKRImplementation* NewL();

        virtual ~CEUCKRImplementation();

    private:
        CEUCKRImplementation();
};

// FUNCTION DEFINITIONS
const TDesC8& CEUCKRImplementation::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvCp949Table::ReplacementForUnconvertibleUnicodeCharacters();
	}

TInt CEUCKRImplementation::ConvertFromUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
    const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
    TDes8& aForeign, 
    const TDesC16& aUnicode, 
    CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	return CCnvCharacterSetConverter::DoConvertFromUnicode(CnvCp949Table::ConversionData(), aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters);
	}

TInt CEUCKRImplementation::ConvertToUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
    TDes16& aUnicode, 
    const TDesC8& aForeign, 
    TInt&, 
    TInt& aNumberOfUnconvertibleCharacters, 
    TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
    return CCnvCharacterSetConverter::DoConvertToUnicode(CnvCp949Table::ConversionData(), aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	}


TBool CEUCKRImplementation::IsInThisCharacterSetL(
    TBool& aSetToTrue, 
    TInt& aConfidenceLevel, 
    const TDesC8& /*aBuf*/)
	{
/*
	aSetToTrue=ETrue;
    aConfidenceLevel=50;
    
    TUint8 ch(0);
    for (TInt i=0;i<aBuf.Length();i++)
        {
        ch=aBuf[i];
        if (ch<0x7F)
            {
            continue;
            }
        else if (0xa1<=ch&&ch<=0xfe)
            {
            i++;
            __ASSERT_DEBUG(i<aBuf.Length(),User::Panic(_L("EUCKR"),__LINE__));
            }
        else
            {
            aConfidenceLevel=0;
            aSetToTrue=EFalse;
            break;
            }
        }    
	return aSetToTrue;
*/
	aSetToTrue=ETrue;
	aConfidenceLevel=0;
	return EFalse;
	}

CEUCKRImplementation* CEUCKRImplementation::NewL()
    {
    CEUCKRImplementation* self = new(ELeave) CEUCKRImplementation;
    return self;
    }

CEUCKRImplementation::CEUCKRImplementation()
    {
    //default constructor.. do nothing
    }

CEUCKRImplementation::~CEUCKRImplementation()
    {
    //default destructor .. do nothing
    }

// ECOM CREATION FUNCTION
const TImplementationProxy ImplementationTable[] = 
    {
    // Note: This is the same UID as defined in old mmp-file
    // Used also in 12221212.rss ( implementation_uid )
    IMPLEMENTATION_PROXY_ENTRY( 0x2000E526, CEUCKRImplementation::NewL )
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

