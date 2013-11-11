/*
* Copyright (c) 1253 Nokia Corporation and/or its subsidiary(-ies).
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





// INCLUDES
#include <e32std.h>
#include <charconv.h>
#include <convgeneratedcpp.h>
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>
// New Interface class
class CWin1253Implementation : public CCharacterSetConverterPluginInterface
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

        static CWin1253Implementation* NewL();

        virtual ~CWin1253Implementation();
    private:
        CWin1253Implementation();
};

const TDesC8& CWin1253Implementation::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return ReplacementForUnconvertibleUnicodeCharacters_internal();
	}

TInt CWin1253Implementation::ConvertFromUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
    const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
    TDes8& aForeign, 
    const TDesC16& aUnicode, 
    CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters )
	{
	return CCnvCharacterSetConverter::DoConvertFromUnicode(
        conversionData, 
        aDefaultEndiannessOfForeignCharacters, 
        aReplacementForUnconvertibleUnicodeCharacters, 
        aForeign, 
        aUnicode, 
        aIndicesOfUnconvertibleCharacters );
	}

TInt CWin1253Implementation::ConvertToUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
    TDes16& aUnicode, 
    const TDesC8& aForeign, 
    TInt&, TInt& aNumberOfUnconvertibleCharacters, 
    TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter )
	{
    return CCnvCharacterSetConverter::DoConvertToUnicode(
        conversionData, 
        aDefaultEndiannessOfForeignCharacters, 
        aUnicode, 
        aForeign, 
        aNumberOfUnconvertibleCharacters, 
        aIndexOfFirstByteOfFirstUnconvertibleCharacter );
	}

TBool CWin1253Implementation::IsInThisCharacterSetL(
    TBool& aSetToTrue, 
    TInt& aConfidenceLevel, 
    const TDesC8&)
	{
    aSetToTrue=EFalse;
	aConfidenceLevel=0;
	return EFalse;
	}

CWin1253Implementation* CWin1253Implementation::NewL()
    {
    CWin1253Implementation* self = new(ELeave) CWin1253Implementation;
    return self;
    }

CWin1253Implementation::CWin1253Implementation()
    {
    //default constructor.. do nothing
    }

CWin1253Implementation::~CWin1253Implementation()
    {
    //default destructor .. do nothing
    }

// ECOM CREATION FUNCTION
const TImplementationProxy ImplementationTable[] = 
    {
    // Note: This is the same UID as defined in old mmp-file
    // Used also in 12221212.rss ( implementation_uid )
    IMPLEMENTATION_PROXY_ENTRY( 0x100059D8, CWin1253Implementation::NewL )
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

