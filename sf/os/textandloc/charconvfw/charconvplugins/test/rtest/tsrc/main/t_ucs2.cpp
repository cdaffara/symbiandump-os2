/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <charconv.h>
#include "t_ucs2.h"

#define test(cond)                                  \
    {                                               \
    TBool __bb = (cond);                            \
    TEST(__bb);                                     \
    if (!__bb)                                      \
        {                                           \
        ERR_PRINTF1(_L("ERROR: Test Failed"));      \
        User::Leave(1);                             \
        }                                           \
    }

const TInt KMibValue = 1000;

const TInt KBufferLength1=1;
const TInt KBufferLength6=6;
const TInt KBufferLength8=8;
const TInt KBufferLength40=40;

const TPtrC8 KForeignBufferEven12LE(_S8("\xff\xfe\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a"));
const TPtrC8 KForeignBufferOdd13LE(_S8("\xff\xfe\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b"));
const TPtrC8 KForeignBufferEven12BE(_S8("\xfe\xff\x42\x41\x44\x43\x46\x45\x48\x47\x4a\x49"));
const TPtrC8 KForeignBufferOdd13BE(_S8("\xfe\xff\x42\x41\x44\x43\x46\x45\x48\x47\x4a\x49\x4b"));

const TPtrC8 KForeignBufferEvenWithoutBOM10LE(_S8("\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a"));
const TPtrC8 KForeignBufferOddWithoutBOM11LE(_S8("\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b"));
const TPtrC8 KForeignBufferEvenWithoutBOM10BE(_S8("\x42\x41\x44\x43\x46\x45\x48\x47\x4a\x49"));
const TPtrC8 KForeignBufferOddWithoutBOM11BE(_S8("\x42\x41\x44\x43\x46\x45\x48\x47\x4a\x49\x4b"));

const TPtrC8 KForeignBufferEven24LE(_S8("\xff\xfe\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56"));
const TPtrC8 KForeignBufferOdd25LE(_S8("\xff\xfe\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57"));
const TPtrC8 KForeignBufferEven24BE(_S8("\xfe\xff\x42\x41\x44\x43\x46\x45\x48\x47\x4a\x49\x4c\x4b\x4e\x4d\x50\x4f\x52\x51\x54\x53\x56\x55"));
const TPtrC8 KForeignBufferOdd25BE(_S8("\xfe\xff\x42\x41\x44\x43\x46\x45\x48\x47\x4a\x49\x4c\x4b\x4e\x4d\x50\x4f\x52\x51\x54\x53\x56\x55\x57"));

const TPtrC8 KForeignBufferEvenWithoutBOM22LE(_S8("\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56"));
const TPtrC8 KForeignBufferOddWithoutBOM23LE(_S8("\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57"));
const TPtrC8 KForeignBufferEvenWithoutBOM22BE(_S8("\x42\x41\x44\x43\x46\x45\x48\x47\x4a\x49\x4c\x4b\x4e\x4d\x50\x4f\x52\x51\x54\x53\x56\x55"));
const TPtrC8 KForeignBufferOddWithoutBOM23BE(_S8("\x42\x41\x44\x43\x46\x45\x48\x47\x4a\x49\x4c\x4b\x4e\x4d\x50\x4f\x52\x51\x54\x53\x56\x55\x57"));

const TPtrC16 KUnicodeBufferEven14BE(_S16("\xfeff\x4142\x4344\x4546\x4748\x494a\x4b4c\x4d4e\x4f50\x5152\x5354\x5556\x5758\x595a"));
const TPtrC16 KUnicodeBufferOdd15BE(_S16("\xfeff\x4142\x4344\x4546\x4748\x494a\x4b4c\x4d4e\x4f50\x5152\x5354\x5556\x5758\x595a\x5b5c"));

/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0553
@SYMTestCaseDesc        8 bit to UCS2 Little Endian Conversion - Single pass test
@SYMTestPriority        Medium
@SYMTestActions         Tests a 8 bit string to 16 bit UCS2 conversion.
                        Test with and without Byte Order Mark. Byte Order Mark is set to Little Endian.
                        Output buffer is large enough to contain the complete input string
                        on a single pass
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_UCS2::LEConversionSinglePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter)
{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0553 "));
    TBuf16<KBufferLength40> generatedUnicode;
    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TInt numberOfUnconvertibleCharacters=0;
    TInt indexOfFirstByteOfFirstUnconvertibleCharacter=0;
    TInt numberLeft =0;

    // set the default endianness to little Endian
    aCharacterSetConverter.SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::ELittleEndian);

    // test even number of foreign bytes
    aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEven12LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49"));
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\nLittleEndianConversionSinglePassEvenBOM - OK"));

    // test even number of foreign bytes without BOM
    aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEvenWithoutBOM10LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49"));
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\nLittleEndianConversionSinglePassEvenBOM - OK"));

    // test odd number of foreign bytes
    state=CCnvCharacterSetConverter::KStateDefault;
    numberLeft = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOdd13LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(numberLeft==1);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49"));
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    // test odd number of foreign bytes without BOM
    numberLeft = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOddWithoutBOM11LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49"));
    test(numberLeft==1);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    INFO_PRINTF1(_L("\nLittleEndianConversionSinglePassOddBOM - OK"));
}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0554
@SYMTestCaseDesc        8 bit to UCS2 Little Endian Conversion - Multiple pass test
@SYMTestPriority        Medium
@SYMTestActions         Tests a 8 bit string to 16 bit UCS2 conversion.
                        With and without Byte Order Mark. Byte Order Mark is set to Little Endian.
                        Output buffer is NOT large enough to contain the complete input string
                        on a single pass. Multiple calls are made to complete translation.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_UCS2::LEConversionMultiplePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter)
{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0554 "));
    TBuf16<KBufferLength6> generatedUnicode;
    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TInt numberOfUnconvertibleCharacters=0;
    TInt indexOfFirstByteOfFirstUnconvertibleCharacter=0;

    // set the default endianness to little Endian
    aCharacterSetConverter.SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::ELittleEndian);

    // test even number of foreign bytes
    // first pass
    TInt numberUnconverted;
    numberUnconverted = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEven24LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49\x4c4b"));
    test(numberUnconverted==10);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    // test even number of foreign bytes without BOM
    numberUnconverted = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEvenWithoutBOM22LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49\x4c4b"));
    test(numberUnconverted==10);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    // second pass
    numberUnconverted = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEven24LE.Right(numberUnconverted), state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(numberUnconverted==0);
    test(generatedUnicode==_L16("\x4e4d\x504f\x5251\x5453\x5655"));
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\nLittleEndianConversionMultiplePassOddEven - OK"));

    // without BOM
    numberOfUnconvertibleCharacters=10;
    aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEvenWithoutBOM22LE.Right(numberOfUnconvertibleCharacters), state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4e4d\x504f\x5251\x5453\x5655"));
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\nLittleEndianConversionMultiplePassOddEven - OK"));


    // test odd number of foreign bytes
    // first pass
    state=CCnvCharacterSetConverter::KStateDefault;
    numberUnconverted = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOdd25LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(numberUnconverted==11);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49\x4c4b"));

    // without BOM
    numberUnconverted = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOddWithoutBOM23LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(numberUnconverted==11);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49\x4c4b"));

    // second pass
    numberUnconverted = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOdd25LE.Right(numberUnconverted), state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4e4d\x504f\x5251\x5453\x5655"));
    test(numberUnconverted==1);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    // without BOM
    numberUnconverted = 11;
    numberUnconverted = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOddWithoutBOM23LE.Right(numberUnconverted), state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(numberUnconverted==1);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    test(generatedUnicode==_L16("\x4e4d\x504f\x5251\x5453\x5655"));


    INFO_PRINTF1(_L("\nLittleEndianConversionMultiplePassOddBOM - OK"));
}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0555
@SYMTestCaseDesc        8 bit to UCS2 Big Endian Conversion - Single pass
@SYMTestPriority        Medium
@SYMTestActions         Tests a 8 bit string to 16 bit UCS2 conversion.
                        With and without Byte Order Mark. Byte Order Mark is set to Big Endian.
                        Output buffer is large enough to contain the complete input string
                        on a single pass
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_UCS2::BEConversionSinglePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter)
{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0555 "));
    TBuf16<KBufferLength40> generatedUnicode;
    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TInt numberOfUnconvertibleCharacters=0;
    TInt indexOfFirstByteOfFirstUnconvertibleCharacter=0;
    TInt numberLeft=0;

    // set the default endianness to Big Endian
    aCharacterSetConverter.SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::EBigEndian);

    // test even number of foreign bytes
    aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEven12BE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49"));
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\nBigEndianConversionSinglePassEvenBOM - OK"));

    // test even number of foreign bytes without BOM
    aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEvenWithoutBOM10BE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49"));
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\nBigEndianConversionSinglePassEvenBOM - OK"));


    // test odd number of foreign bytes
    state=CCnvCharacterSetConverter::KStateDefault;
    numberLeft = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOdd13BE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49"));
    test(numberLeft==1);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    // test odd number of foreign bytes without BOM
    numberLeft = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOddWithoutBOM11BE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49"));
    test(numberLeft==1);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    INFO_PRINTF1(_L("\nBigEndianConversionSinglePassOddBOM - OK"));
}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0556
@SYMTestCaseDesc        8 bit to UCS2 Big Endian Conversion - Multiple pass test
@SYMTestPriority        Medium
@SYMTestActions         Tests a 8 bit string to 16 bit UCS2 conversion.
                        With and without Byte Order Mark. Byte Order Mark is set to Big Endian.
                        Output buffer is NOT large enough to contain the complete input string
                        on a single pass. Multiple calls are made to complete translation.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_UCS2::BEConversionMultiplePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter)
{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0556 "));
    TBuf16<KBufferLength6> generatedUnicode;
    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TInt numberOfUnconvertibleCharacters=0;
    TInt indexOfFirstByteOfFirstUnconvertibleCharacter=0;
    TInt numberUnconverted=0;

    // set the default endianness to Big Endian
    aCharacterSetConverter.SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::EBigEndian);

    // test even number of foreign bytes
    // first pass
    numberUnconverted=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEven24BE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49\x4c4b"));
    test(numberUnconverted==10);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    // without BOM
    numberUnconverted=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEvenWithoutBOM22BE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49\x4c4b"));
    test(numberUnconverted==10);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    // second pass
    numberUnconverted=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEven24BE.Right(numberUnconverted), state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4e4d\x504f\x5251\x5453\x5655"));
    test(numberUnconverted==0);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\nBigEndianConversionMultiplePassOddEven - OK"));

    // without BOM
    numberOfUnconvertibleCharacters=10;
    numberUnconverted=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferEvenWithoutBOM22BE.Right(numberOfUnconvertibleCharacters), state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4e4d\x504f\x5251\x5453\x5655"));
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\nBigEndianConversionMultiplePassOddEven - OK"));

    // test odd number of foreign bytes
    // first pass
    state=CCnvCharacterSetConverter::KStateDefault;
    numberUnconverted=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOdd25BE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(numberUnconverted==11);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49\x4c4b"));

    // without BOM
    numberUnconverted=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOddWithoutBOM23BE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(numberUnconverted==11);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    test(generatedUnicode==_L16("\x4241\x4443\x4645\x4847\x4a49\x4c4b"));

    // second pass
    numberUnconverted=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOdd25BE.Right(numberUnconverted), state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4e4d\x504f\x5251\x5453\x5655"));
    test(numberUnconverted==1);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);

    // without BOM
    numberOfUnconvertibleCharacters=11;
    numberUnconverted=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KForeignBufferOddWithoutBOM23BE.Right(numberOfUnconvertibleCharacters), state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode==_L16("\x4e4d\x504f\x5251\x5453\x5655"));
    test(numberUnconverted==1);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);


    INFO_PRINTF1(_L("\nBigEndianConversionMultiplePassOddBOM - OK"));
}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0557
@SYMTestCaseDesc        UCS2 to 8 bit Big Endian Conversion - Single pass test
@SYMTestPriority        Medium
@SYMTestActions         Tests a 16 bit UCS2 to 8 bit descriptor conversion.
                        Byte Order Mark is present and is set to Big Endian.
                        Output buffer is large enough to contain the complete input string
                        on a single pass
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_UCS2::UCS2BEConversionSinglePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter)
{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0557 "));
    TBuf8<KBufferLength40> generatedForeign;
    CCnvCharacterSetConverter::TArrayOfAscendingIndices indicesOfUnconvertibleCharacters;

    // set the default endianness to Big Endian
    aCharacterSetConverter.SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::EBigEndian);

    // test even number of foreign bytes
    aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferEven14BE,
                                                        indicesOfUnconvertibleCharacters );

    test(generatedForeign==_L8("\xfe\xff\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a"));
    INFO_PRINTF1(_L("\nUCS2BEConversionSinglePassEvenBOM - OK"));

    // test odd number of foreign bytes
    aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferOdd15BE,
                                                        indicesOfUnconvertibleCharacters );

    test(generatedForeign==_L8("\xfe\xff\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c"));
    INFO_PRINTF1(_L("\nUCS2BEConversionSinglePassOddBOM - OK"));
}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0558
@SYMTestCaseDesc        UCS2 to 8 bit Big Endian Conversion - Multiple pass test
@SYMTestPriority        Medium
@SYMTestActions         Tests a 16 bit UCS2 to 8 bit descriptor conversion.
                        Byte Order Mark is present and is set to Big Endian.
                        Output buffer is NOT large enough to contain the complete input string
                        on a single pass
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_UCS2::UCS2BEConversionMultiplePassBOM (CCnvCharacterSetConverter& aCharacterSetConverter)
{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0558 "));
    TBuf8<KBufferLength6> generatedForeign;
    TBuf8<KBufferLength8> generatedForeign2;
    TInt result=0;
    CCnvCharacterSetConverter::TArrayOfAscendingIndices indicesOfUnconvertibleCharacters;

    // set the default endianness to Big Endian
    aCharacterSetConverter.SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::EBigEndian);

    // *** Test even number of bytes ***
    // test even number of foreign bytes - pass 1
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferEven14BE,
                                                        indicesOfUnconvertibleCharacters );
    test(generatedForeign==_L8("\xfe\xff\x41\x42\x43\x44"));
    test(result==11);

    // test even number of foreign bytes - pass 2
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferEven14BE.Right(result),
                                                        indicesOfUnconvertibleCharacters );
    test(result==8);
    test(generatedForeign==_L8("\x45\x46\x47\x48\x49\x4A"));

    // test even number of foreign bytes - pass 3
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferEven14BE.Right(result),
                                                        indicesOfUnconvertibleCharacters );
    test(result==5);
    test(generatedForeign==_L8("\x4b\x4c\x4d\x4e\x4f\x50"));


    // test even number of foreign bytes - pass 4
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferEven14BE.Right(result),
                                                        indicesOfUnconvertibleCharacters );
    test(result==2);
    test(generatedForeign==_L8("\x51\x52\x53\x54\x55\x56"));

    // test even number of foreign bytes - pass 5
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign2,
                                                        KUnicodeBufferEven14BE.Right(result),
                                                        indicesOfUnconvertibleCharacters );
    test(generatedForeign2==_L8("\x57\x58\x59\x5a"));
    test(result==0);

    INFO_PRINTF1(_L("\nUCS2LEConversionMultiplePassEvenBOM - OK"));

    // *** Test odd numer of bytes ***
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferOdd15BE,
                                                        indicesOfUnconvertibleCharacters );
    test(generatedForeign==_L8("\xfe\xff\x41\x42\x43\x44"));

        // test even number of foreign bytes - pass 2
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferOdd15BE.Right(result),
                                                        indicesOfUnconvertibleCharacters );
    test(generatedForeign==_L8("\x45\x46\x47\x48\x49\x4a"));

    // test even number of foreign bytes - pass 3
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferOdd15BE.Right(result),
                                                        indicesOfUnconvertibleCharacters );
    test(generatedForeign==_L8("\x4b\x4c\x4d\x4e\x4f\x50"));

    // test even number of foreign bytes - pass 4
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferOdd15BE.Right(result),
                                                        indicesOfUnconvertibleCharacters );
    test(generatedForeign==_L8("\x51\x52\x53\x54\x55\x56"));


    // test even number of foreign bytes - pass 5
    result = aCharacterSetConverter.ConvertFromUnicode( generatedForeign,
                                                        KUnicodeBufferOdd15BE.Right(result),
                                                        indicesOfUnconvertibleCharacters );
    test(generatedForeign==_L8("\x57\x58\x59\x5a\x5b\x5c"));
    test(result==0);

    INFO_PRINTF1(_L("\nUCS2LEConversionMultiplePassOddBOM - OK"));
}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0559
@SYMTestCaseDesc        Tests a 16 bit UCS2 to 8 bit & 16 bit UCS2 to 8 bit conversion with empty buffers.
@SYMTestPriority        Medium
@SYMTestActions         Attempt for conversion when input and output buffers are small
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_UCS2::TooSmallBufferConversion (CCnvCharacterSetConverter& aCharacterSetConverter)
    {
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0559 "));
    TBuf16<KBufferLength1> generatedUnicode1;
    TBuf8<KBufferLength1> generatedForeign1;

    TInt state=CCnvCharacterSetConverter::KStateDefault;
    TInt numberOfUnconvertibleCharacters=0;
    TInt indexOfFirstByteOfFirstUnconvertibleCharacter=0;

    CCnvCharacterSetConverter::TArrayOfAscendingIndices indicesOfUnconvertibleCharacters;

        // set the default endianness to little Endian
    aCharacterSetConverter.SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::ELittleEndian);

    // convert to UCS2 using an output buffer that is too small
    TInt result = aCharacterSetConverter.ConvertToUnicode(generatedUnicode1, KForeignBufferEven12LE, state, numberOfUnconvertibleCharacters, indexOfFirstByteOfFirstUnconvertibleCharacter);
    test(generatedUnicode1==_L16("\x4241"));
    test(result==8);
    test(numberOfUnconvertibleCharacters==0);
    test(indexOfFirstByteOfFirstUnconvertibleCharacter==-1);
    INFO_PRINTF1(_L("\n8bit->UCS2 output buffer size 1 - OK"));

    INFO_PRINTF1(_L("\nConversionTooSmallBuffer - OK"));
    }
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0560
@SYMTestCaseDesc        Conversion tests from UCS2 GUID to MIB
@SYMTestPriority        Medium
@SYMTestActions         Tests retreiving the MIB value from the UCS2 GUID
                        Tests retreiving the UCS2 GUID from the MIB value
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_UCS2::CharacterSetValueAndMIBTests(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession)
{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0560 "));
    // check that the character set value of '1000' is converted to the UCS2 GUID (0x101ff492)
    test(aCharacterSetConverter.ConvertMibEnumOfCharacterSetToIdentifierL(KMibValue,aFileServerSession)==KCharacterSetIdentifierUcs2);
    INFO_PRINTF1(_L("\nMIB->Char Set GUID - OK"));

    // check that the UCS2 GUID (0x101ff492) is converted to the character set value of '1000'
    test(aCharacterSetConverter.ConvertCharacterSetIdentifierToMibEnumL(KCharacterSetIdentifierUcs2,aFileServerSession)==KMibValue);
    INFO_PRINTF1(_L("\nChar Set GUID->MIB - OK"));
}


void CT_UCS2::DoE32MainL()
    {
    RFs fileServerSession;
    CleanupClosePushL(fileServerSession);
    User::LeaveIfError(fileServerSession.Connect());
    CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
    CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
    TInt i;
    for (i=arrayOfCharacterSetsAvailable->Count()-1; i>=0; --i)
        {
        const CCnvCharacterSetConverter::SCharacterSet& charactersSet=(*arrayOfCharacterSetsAvailable)[i];
        characterSetConverter->PrepareToConvertToOrFromL(charactersSet.Identifier(), *arrayOfCharacterSetsAvailable, fileServerSession);
        TPtrC charactersSetName(charactersSet.Name());
        if (charactersSet.NameIsFileName())
            {
            charactersSetName.Set(TParsePtrC(charactersSetName).Name());
            }
        INFO_PRINTF2(_L("    %S\n"), &charactersSetName);
        }

    // test MIB to Character Set ID (and visa versa) conversion
    CharacterSetValueAndMIBTests(*characterSetConverter, fileServerSession);

    // prepare for conversion
    INFO_PRINTF1(_L("\nTesting 8 bit -> UCS2 conversion"));
    TRAPD( error, characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierUcs2, *arrayOfCharacterSetsAvailable, fileServerSession));
    if ( error != KErrNone )
        {
        RDebug::Print(_L("PrepareToConvertToOrFromL failed code:%d\n"),error);
        RDebug::Print(_L("KCharacterSetIdentifierUcs2 is not available so tests failed\n"));
        User::LeaveIfError(error);
        }


    // *** 8 bit to UCS2 conversion ***
    INFO_PRINTF1(_L("\n8->UCS2 Little Endian Conversion - Single pass"));
    LEConversionSinglePassBOM(*characterSetConverter);

    INFO_PRINTF1(_L("\n8->UCS2 Little Endian Conversion - Multiple pass"));
    LEConversionMultiplePassBOM(*characterSetConverter);

    INFO_PRINTF1(_L("\n8->UCS2 Big Endian Conversion - Single pass"));
    BEConversionSinglePassBOM(*characterSetConverter);

    INFO_PRINTF1(_L("\n8->UCS2 Big Endian Conversion - Multiple pass"));
    BEConversionMultiplePassBOM(*characterSetConverter);


    // *** UCS2 to 8 bit conversion ***
    INFO_PRINTF1(_L("\nUCS2->8 Big Endian Conversion - Single pass"));
    UCS2BEConversionSinglePassBOM(*characterSetConverter);

    INFO_PRINTF1(_L("\nUCS2->8 Big Endian Conversion - Multiple pass"));
    UCS2BEConversionMultiplePassBOM(*characterSetConverter);

    // *** attempted conversion when input/output buffers are too small
    INFO_PRINTF1(_L("\nTooSmallBufferConversion"));
    TooSmallBufferConversion(*characterSetConverter);

    INFO_PRINTF1(_L("\n"));
    CleanupStack::PopAndDestroy(3);// arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
    }

CT_UCS2::CT_UCS2()
    {
    SetTestStepName(KTestStep_T_UCS2);
    }

TVerdict CT_UCS2::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;
    TRAPD(error1, DoE32MainL());
    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
