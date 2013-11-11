/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*                This plug-in supports CP949.
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
class CCP949Implementation : public CCharacterSetConverterPluginInterface
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

        static CCP949Implementation* NewL();

        virtual ~CCP949Implementation();

    private:
        CCP949Implementation();
};

// FUNCTION DEFINITIONS
const TDesC8& CCP949Implementation::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvCp949Table::ReplacementForUnconvertibleUnicodeCharacters();
	}

TInt CCP949Implementation::ConvertFromUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
    const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
    TDes8& aForeign, 
    const TDesC16& aUnicode, 
    CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	return CCnvCharacterSetConverter::DoConvertFromUnicode(CnvCp949Table::ConversionData(), aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters);
	}

TInt CCP949Implementation::ConvertToUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
    TDes16& aUnicode, 
    const TDesC8& aForeign, 
    TInt&, 
    TInt& aNumberOfUnconvertibleCharacters, 
    TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
    return CCnvCharacterSetConverter::DoConvertToUnicode(CnvCp949Table::ConversionData(), aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	}


TBool CCP949Implementation::IsInThisCharacterSetL(
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
            __ASSERT_DEBUG(i<aBuf.Length(),User::Panic(_L("CP949"),__LINE__));
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

CCP949Implementation* CCP949Implementation::NewL()
    {
    CCP949Implementation* self = new(ELeave) CCP949Implementation;
    return self;
    }

CCP949Implementation::CCP949Implementation()
    {
    //default constructor.. do nothing
    }

CCP949Implementation::~CCP949Implementation()
    {
    //default destructor .. do nothing
    }

// ECOM CREATION FUNCTION
const TImplementationProxy ImplementationTable[] = 
    {
    // Note: This is the same UID as defined in old mmp-file
    // Used also in 12221212.rss ( implementation_uid )
    IMPLEMENTATION_PROXY_ENTRY( 0x200100FF, CCP949Implementation::NewL )
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

