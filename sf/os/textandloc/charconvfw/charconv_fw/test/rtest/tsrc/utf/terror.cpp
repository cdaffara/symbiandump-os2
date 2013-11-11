/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <utf.h>
#include "t_error.h"

#define test(cond)                                          \
    {                                                       \
    TBool __bb = (cond);                                    \
    TEST(__bb);                                             \
    if (!__bb)                                              \
        {                                                   \
        ERR_PRINTF1(_L("ERROR: Test Failed"));              \
        User::Leave(1);                                     \
        }                                                   \
    }


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

//typedef TInt (*FConvertFromUtfX)(TDes16& aUnicode, const TDesC8& aUtfX, TInt& aState);

TInt ConvertToUnicodeFromUtf7(TDes16& aUnicode, const TDesC8& aUtf7, TInt& aState)
	{
	return CnvUtfConverter::ConvertToUnicodeFromUtf7(aUnicode, aUtf7, aState);
	}

TInt ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8, TInt&)
	{
	return CnvUtfConverter::ConvertToUnicodeFromUtf8(aUnicode, aUtf8);
	}

void CT_ERROR::ExhaustivelyLengthTest(const TDesC16& aUnicode, const TDesC8& aUtfX, FConvertFromUtfX aConvertFromUtfX)
	{
	enum {ECharacterCodeThatCannotOccurInUnicode=0xffff};
	enum {ELengthOfWorkSpace=256};

	TUint16 workSpace[ELengthOfWorkSpace];

	const TInt lengthOfUnicode=aUnicode.Length();
	test(lengthOfUnicode<ELengthOfWorkSpace);
	TInt i=0;
	FOREVER
		{
  		TPtr16 unicode(workSpace, 0, i);

		const TUint16* const lastCharacterOfWorkSpace=&workSpace[ELengthOfWorkSpace-1];

		{TUint16* characterPointer=&workSpace[i];
		FOREVER
			{
			*characterPointer=ECharacterCodeThatCannotOccurInUnicode;
			if (characterPointer==lastCharacterOfWorkSpace)
				{
				break;
				}
			++characterPointer;
			}}
		TInt state=CnvUtfConverter::KStateDefault;

		const TInt returnValue=(*aConvertFromUtfX)(unicode, aUtfX, state);
		test((returnValue!=0) || (state==CnvUtfConverter::KStateDefault));
		{const TUint16* characterPointer=&workSpace[i];
		FOREVER
			{
			test(*characterPointer==ECharacterCodeThatCannotOccurInUnicode);
			if (characterPointer==lastCharacterOfWorkSpace)
				{
				break;
				}
			++characterPointer;
			}}
		if (i==lengthOfUnicode)
			{
			test(unicode==aUnicode);
			if (returnValue!=0)
				{
				test(returnValue>0);
				test((*aConvertFromUtfX)(unicode, aUtfX.Right(returnValue), state)==0);
				test(unicode.Length()==0);
				test(state==CnvUtfConverter::KStateDefault);
				}
			break;
			}
		test(returnValue>0);
		++i;
		}
	}

//typedef TInt (*FConvertToUtfX)(TDes8& aUtfX, const TDesC16& aUnicode);

TInt ConvertFromUnicodeToUtf7WithOptionalDirectCharactersEncodedInBase64(TDes8& aUtf7, const TDesC16& aUnicode)
	{
	return CnvUtfConverter::ConvertFromUnicodeToUtf7(aUtf7, aUnicode, ETrue);
	}

TInt ConvertFromUnicodeToUtf7WithOptionalDirectCharactersEncodedDirectly(TDes8& aUtf7, const TDesC16& aUnicode)
	{
	return CnvUtfConverter::ConvertFromUnicodeToUtf7(aUtf7, aUnicode, EFalse);
	}

TInt ConvertFromUnicodeToUtf8(TDes8& aUtf8, const TDesC16& aUnicode)
	{
	return CnvUtfConverter::ConvertFromUnicodeToUtf8(aUtf8, aUnicode);
	}

void CT_ERROR::ExhaustivelyLengthTest(const TDesC8& aUtfX, const TDesC16& aUnicode, FConvertToUtfX aConvertToUtfX)
	{
	enum {EByteValueThatCannotOccurInEitherUtf7OrUtf8=0xff};
	enum {ELengthOfWorkSpace=512};
	TUint8 workSpace[ELengthOfWorkSpace];
	const TInt lengthOfUtfX=aUtfX.Length();
	test(lengthOfUtfX<ELengthOfWorkSpace);
	TInt i=0;
	FOREVER
		{
		TPtr8 utfX(workSpace, 0, i);
		const TUint8* const lastByteOfWorkSpace=&workSpace[ELengthOfWorkSpace-1];
		{TUint8* bytePointer=&workSpace[i];
		FOREVER
			{
			*bytePointer=EByteValueThatCannotOccurInEitherUtf7OrUtf8;
			if (bytePointer==lastByteOfWorkSpace)
				{
				break;
				}
			++bytePointer;
			}}
		const TInt returnValue=(*aConvertToUtfX)(utfX, aUnicode);
		{const TUint8* bytePointer=&workSpace[i];
		FOREVER
			{
			test(*bytePointer==EByteValueThatCannotOccurInEitherUtf7OrUtf8);
			if (bytePointer==lastByteOfWorkSpace)
				{
				break;
				}
			++bytePointer;
			}}
		if (i==lengthOfUtfX)
			{
			test(returnValue==0);
			test(utfX==aUtfX);
			break;
			}
		test(returnValue>0);
		++i;
		}
	}

void CT_ERROR::ExhaustivelyLengthTest(const TDesC16& aUnicode, const TDesC8& aUtf7WithOptionalDirectCharactersEncodedInBase64, const TDesC8& aUtf7WithOptionalDirectCharactersEncodedDirectly, const TDesC8& aUtf8)
	{
	ExhaustivelyLengthTest(aUnicode, aUtf7WithOptionalDirectCharactersEncodedInBase64, ConvertToUnicodeFromUtf7);
	ExhaustivelyLengthTest(aUnicode, aUtf7WithOptionalDirectCharactersEncodedDirectly, ConvertToUnicodeFromUtf7);
	ExhaustivelyLengthTest(aUnicode, aUtf8, ConvertToUnicodeFromUtf8);
	ExhaustivelyLengthTest(aUtf7WithOptionalDirectCharactersEncodedInBase64, aUnicode, ConvertFromUnicodeToUtf7WithOptionalDirectCharactersEncodedInBase64);
	ExhaustivelyLengthTest(aUtf7WithOptionalDirectCharactersEncodedDirectly, aUnicode, ConvertFromUnicodeToUtf7WithOptionalDirectCharactersEncodedDirectly);
	ExhaustivelyLengthTest(aUtf8, aUnicode, ConvertFromUnicodeToUtf8);
	}

void CT_ERROR::ExhaustivelyLengthTest(const TDesC16& aUnicode, const TDesC8& aUtf7, const TDesC8& aUtf8)
	{
	ExhaustivelyLengthTest(aUnicode, aUtf7, aUtf7, aUtf8);
	}

void ResetDescriptorsForNullTest(TDes8& aEmpty8, TDes16& aEmpty16, TDes8& aNonEmpty8, TDes16& aNonEmpty16)
	{
	aEmpty8.SetLength(0);
	aEmpty16.SetLength(0);
	aNonEmpty8=_L8("abc");
	aNonEmpty16=_L16("xyz");
	}

void CT_ERROR::LengthTestGeneratingUtf7()
	{
	TBuf16<7> unicode;
	unicode.Format(_L16("+%c+&+a+"), 0x52ff);
	TPtr8 generatedUtf7(NULL, 0, 0);
	TUint8 utf7[15];
	TInt i;
	generatedUtf7.Set(utf7, 0, 15);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, unicode, EFalse)==0);
	test(generatedUtf7==_L8("+-+Uv8-+-&+-a+-"));
	for (i=14; i>=13; --i)
		{
		generatedUtf7.Set(utf7, 0, i);
		test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, unicode, EFalse)==1);
		test(generatedUtf7==_L8("+-+Uv8-+-&+-a"));
		}
	generatedUtf7.Set(utf7, 0, 12);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, unicode, EFalse)==2);
	test(generatedUtf7==_L8("+-+Uv8-+-&+-"));
	for (i=11; i>=10; --i)
		{
		generatedUtf7.Set(utf7, 0, i);
		test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, unicode, EFalse)==3);
		test(generatedUtf7==_L8("+-+Uv8-+-&"));
		}
	generatedUtf7.Set(utf7, 0, 9);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, unicode, EFalse)==4);
	test(generatedUtf7==_L8("+-+Uv8-+-"));
	for (i=8; i>=7; --i)
		{
		generatedUtf7.Set(utf7, 0, i);
		test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, unicode, EFalse)==5);
		test(generatedUtf7==_L8("+-+Uv8-"));
		}
	for (i=6; i>=2; --i)
		{
		generatedUtf7.Set(utf7, 0, i);
		test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, unicode, EFalse)==6);
		test(generatedUtf7==_L8("+-"));
		}
	for (i=1; i>=0; --i)
		{
		generatedUtf7.Set(utf7, 0, i);
		test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, unicode, EFalse)==7);
		test(generatedUtf7==KNullDesC8);
		}
	}

void CT_ERROR::LengthTestGeneratingUtf8()
	{
	TBuf16<5> unicode;
	unicode.Format(_L16("J%c%c%c%c"), 0x027e, 0xb205, 0xdbab, 0xde4d);
	TPtr8 generatedUtf8(NULL, 0, 0);
	TUint8 utf8[10];
	TInt i;
	generatedUtf8.Set(utf8, 0, 10);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("J\xc9\xbe\xeb\x88\x85\xf3\xba\xb9\x8d"));
	for (i=9; i>=6; --i)
		{
		generatedUtf8.Set(utf8, 0, i);
		test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==2);
		test(generatedUtf8==_L8("J\xc9\xbe\xeb\x88\x85"));
		}
	for (i=5; i>=3; --i)
		{
		generatedUtf8.Set(utf8, 0, i);
		test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==3);
		test(generatedUtf8==_L8("J\xc9\xbe"));
		}
	for (i=2; i>=1; --i)
		{
		generatedUtf8.Set(utf8, 0, i);
		test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==4);
		test(generatedUtf8==_L8("J"));
		}
	generatedUtf8.Set(utf8, 0, 0);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==5);
	test(generatedUtf8==KNullDesC8);
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0569
@SYMTestCaseDesc        CnvUtfConverter class functionality test
@SYMTestPriority        Medium
@SYMTestActions        	Checking that error conditions yield appropriate error values
                        Tests exhaustively for length-testing UTF-8 2,3,4- byte Unicode character
						Tests for converting bad UTF-7 to Unicode
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_ERROR::TestERROR()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0569 Checking that error conditions yield appropriate error values "));
	ExhaustivelyLengthTest(KNullDesC16, KNullDesC8, KNullDesC8);
	ExhaustivelyLengthTest(_L16("Hello!"), _L8("Hello+ACE-"), _L8("Hello!"), _L8("Hello!"));

	HBufC * unicodeHBuf = HBufC::NewLC(32);
    TPtr16 unicode = unicodeHBuf->Des();
    HBufC * generatedUnicodeHBuf = HBufC::NewLC(32);
    TPtr16 generatedUnicode = generatedUnicodeHBuf->Des();
    HBufC8 * utf8HBuf = HBufC8::NewLC(64);
    TPtr8 utf8 = utf8HBuf->Des();
    HBufC8 * generatedUtf8HBuf = HBufC8::NewLC(64);
    TPtr8 generatedUtf8 = generatedUtf8HBuf->Des();

	const TPtrC16 percentC_16=_L16("%c");
	const TPtrC16 percentCPercentC_16=_L16("%c%c");
	const TPtrC8 percentC_8=_L8("%c");
	const TPtrC8 percentCPercentC_8=_L8("%c%c");
	const TPtrC8 percentCPercentCPercentC_8=_L8("%c%c%c");
	const TPtrC8 percentCPercentCPercentCPercentC_8=_L8("%c%c%c%c");
	TInt state=CnvUtfConverter::KStateDefault;
	//
	INFO_PRINTF1(_L("Exhaustively length-testing a UTF-8 2-byte Unicode character"));
	unicode.Format(_L16("%c"), 0x0635);
	utf8.Format(_L8("%c%c"), 0xd8, 0xb5);
	ExhaustivelyLengthTest(unicode, _L8("+BjU-"), utf8);
	INFO_PRINTF1(_L("Exhaustively length-testing a UTF-8 3-byte Unicode character"));
	unicode.Format(_L16("%c"), 0x679e);
	utf8.Format(_L8("%c%c%c"), 0xe6, 0x9e, 0x9e);
	ExhaustivelyLengthTest(unicode, _L8("+Z54-"), utf8);
	INFO_PRINTF1(_L("Exhaustively length-testing a UTF-8 4-byte Unicode surrogate-pair"));
	unicode.Format(_L16("%c%c"), 0xdb0d, 0xdcab);
	utf8.Format(_L8("%c%c%c%c"), 0xf3, 0x93, 0x92, 0xab);
	ExhaustivelyLengthTest(unicode, _L8("+2w3cqw-"), utf8);
	INFO_PRINTF1(_L("Exhaustively length-testing a UTF-7 various strings"));
	unicode.Format(_L16("a%c"), 0x0105);
	utf8.Format(_L8("a%c%c"), 0xc4, 0x85);
	ExhaustivelyLengthTest(unicode, _L8("a+AQU-"), utf8);
	unicode.Format(_L16("b%c%c"), 0x0431, 0x0431);
	utf8.Format(_L8("b%c%c%c%c"), 0xd0, 0xb1, 0xd0, 0xb1);
	ExhaustivelyLengthTest(unicode, _L8("b+BDEEMQ-"), utf8);
	unicode.Format(_L16("c%c%c%c"), 0x05e6, 0x05e6, 0x05e6);
	utf8.Format(_L8("c%c%c%c%c%c%c"), 0xd7, 0xa6, 0xd7, 0xa6, 0xd7, 0xa6);
	ExhaustivelyLengthTest(unicode, _L8("c+BeYF5gXm-"), utf8);
	unicode.Format(_L16(" %c%c%c%c"), 0xd86a, 0xdfa7, 0xd9e3, 0xde08);
	utf8.Format(_L8(" %c%c%c%c%c%c%c%c"), 0xf0, 0xaa, 0xae, 0xa7, 0xf2, 0x88, 0xb8, 0x88);
	ExhaustivelyLengthTest(unicode, _L8(" +2Grfp9nj3gg-"), utf8);
	unicode.Format(_L16("%cd"), 0x0636);
	utf8.Format(_L8("%c%cd"), 0xd8, 0xb6);
	ExhaustivelyLengthTest(unicode, _L8("+BjY-d"), utf8);
	unicode.Format(_L16("%c%ce"), 0x090f, 0x090f);
	utf8.Format(_L8("%c%c%c%c%c%ce"), 0xe0, 0xa4, 0x8f, 0xe0, 0xa4, 0x8f);
	ExhaustivelyLengthTest(unicode, _L8("+CQ8JDw-e"), utf8);
	unicode.Format(_L16("%c%c%cf"), 0x6cd5, 0x6cd5, 0x6cd5);
	utf8.Format(_L8("%c%c%c%c%c%c%c%c%cf"), 0xe6, 0xb3, 0x95, 0xe6, 0xb3, 0x95, 0xe6, 0xb3, 0x95);
	ExhaustivelyLengthTest(unicode, _L8("+bNVs1WzV-f"), utf8);
	unicode.Format(_L16("%c%c%c%c "), 0xdbe9, 0xdcfe, 0xdb52, 0xddb4);
	utf8.Format(_L8("%c%c%c%c%c%c%c%c "), 0xf4, 0x8a, 0x93, 0xbe, 0xf3, 0xa4, 0xa6, 0xb4);
	ExhaustivelyLengthTest(unicode, _L8("+2+nc/ttS3bQ- "), utf8);
	INFO_PRINTF1(_L("Converting to short UTF-7 descriptors"));
	LengthTestGeneratingUtf7();
	INFO_PRINTF1(_L("Converting to short UTF-8 descriptors"));
	LengthTestGeneratingUtf8();
	{
	INFO_PRINTF1(_L("Testing converting bad UTF-7 to Unicode"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8(")+"), state)==1);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==')');
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+ "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+>"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+-"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==_L16("+"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+A"), state)==0);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+A "), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==_L16(" "));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+A-"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+B"), state)==1);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+B "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+B-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+AA"), state)==0);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+AA "), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==_L16(" "));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+AA-"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+AB"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+AB "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+AB-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+BA"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+BA "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+BA-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+BB"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+BB "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+BB-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnY"), state)==0);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnY "), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==2);
	test(generatedUnicode[0]==0xb676);
	test(generatedUnicode[1]==0x0020);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnY-"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZ"), state)==3);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZ "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZ-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tna"), state)==3);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tna "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tna-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnb"), state)==3);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnb "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnb-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYA"), state)==0);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYA "), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==2);
	test(generatedUnicode[0]==0xb676);
	test(generatedUnicode[1]==0x0020);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYA-"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYB"), state)==1);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYB "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYB-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZA"), state)==1);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZA "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZA-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZB"), state)==1);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZB "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZB-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYAA"), state)==0);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYAA "), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==2);
	test(generatedUnicode[0]==0xb676);
	test(generatedUnicode[1]==0x0020);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYAA-"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYAB"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYAB "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYAB-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYBA"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYBA "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYBA-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYBB"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYBB "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnYBB-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZAA"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZAA "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZAA-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZAB"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZAB "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZAB-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZBA"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZBA "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZBA-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZBB"), state)==2);
	test(state!=CnvUtfConverter::KStateDefault);
	state=CnvUtfConverter::KStateDefault;
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xb676);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZBB "), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+tnZBB-"), state)==CnvUtfConverter::EErrorIllFormedInput);
	test(state==CnvUtfConverter::KStateDefault);
	}
	{
	INFO_PRINTF1(_L("Testing converting bad UTF-8 to Unicode"));
	utf8.Format(percentC_8, 0x80);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentC_8, 0xbf);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentC_8, 0xa1);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentC_8, 0xc0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentC_8, '=', 0xc0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	test(generatedUnicode.Length()==2);
	test(generatedUnicode[0]=='=');
	test(generatedUnicode[1]==0xFFFD);
	utf8.Format(percentCPercentC_8, 0xc0, 0x00);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentC_8, 0xc0, 0x80);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	test(generatedUnicode.Length()==2);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	utf8.Format(percentC_8, 0xdf);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentC_8, '*', 0xdf);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==1);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='*');
	utf8.Format(percentCPercentC_8, 0xdf, 0x7f);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentC_8, 0xdf, 0xbf);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0x07ff);
	utf8.Format(percentC_8, 0xd7);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentC_8, '\'', 0xd7);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==1);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='\'');
	utf8.Format(percentCPercentC_8, 0xd7, 0xd9);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==1);
	utf8.Format(percentCPercentC_8, 0xd7, 0x99);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0x05d9);
	utf8.Format(percentC_8, 0xe0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentC_8, '@', 0xe0);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==1);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='@');
	utf8.Format(percentCPercentC_8, 0xe0, 0x80);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentC_8, ':', 0xe0, 0x80);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==2);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==':');
	utf8.Format(percentCPercentCPercentC_8, 0xe0, 0x80, 0x80);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	test(generatedUnicode.Length()==3);
	test(generatedUnicode[0]==0xfffd);
	test(generatedUnicode[1]==0xfffd);
	test(generatedUnicode[2]==0xfffd);
	utf8.Format(percentCPercentC_8, 0xef, 0x3f);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentC_8, '~', 0xef, 0x3f);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==2);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='~');
	utf8.Format(percentCPercentCPercentC_8, 0xef, 0x3f, 0xbf);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentC_8, 0xef, 0x7f);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentC_8, 'L', 0xef, 0x7f);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==2);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='L');
	utf8.Format(percentCPercentCPercentC_8, 0xef, 0x7f, 0xbf);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentC_8, 0xef, 0xff);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentC_8, '^', 0xef, 0xff);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==2);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='^');
	utf8.Format(percentCPercentCPercentC_8, 0xef, 0xff, 0xbf);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentC_8, 0xef, 0xbf, 0x3f);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentC_8, 0xef, 0xbf, 0x7f);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentC_8, 0xef, 0xbf, 0xff);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentC_8, 0xef, 0xbf, 0xbf);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xffff);
	utf8.Format(percentC_8, 0xf4);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentC_8, ',', 0xf4);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==1);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==',');
	utf8.Format(percentCPercentC_8, 0xf4, 0x06);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentC_8, 'J', 0xf4, 0x06);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==2);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='J');
	utf8.Format(percentCPercentCPercentC_8, 0xf4, 0x06, 0xab);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentCPercentC_8, ']', 0xf4, 0x06, 0xab);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==3);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==']');
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x06, 0xab, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentC_8, 0xf4, 0x46);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentC_8, 'o', 0xf4, 0x46);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==2);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='o');
	utf8.Format(percentCPercentCPercentC_8, 0xf4, 0x46, 0xab);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentCPercentC_8, '!', 0xf4, 0x46, 0xab);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==3);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='!');
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x46, 0xab, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentC_8, 0xf4, 0xc6);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentC_8, 'm', 0xf4, 0xc6);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==2);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='m');
	utf8.Format(percentCPercentCPercentC_8, 0xf4, 0xc6, 0xab);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentCPercentC_8, '&', 0xf4, 0xc6, 0xab);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==3);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='&');
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0xc6, 0xab, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentC_8, 0xf4, 0x86, 0x2b);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentCPercentC_8, 'v', 0xf4, 0x86, 0x2b);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==3);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='v');
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x86, 0x2b, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentC_8, 0xf4, 0x86, 0x6b);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentCPercentC_8, 'Q', 0xf4, 0x86, 0x6b);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==3);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='Q');
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x86, 0x6b, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentC_8, 0xf4, 0x86, 0xeb);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==CnvUtfConverter::EErrorIllFormedInput);
	utf8.Format(percentCPercentCPercentCPercentC_8, '?', 0xf4, 0x86, 0xeb);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==3);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]=='?');
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x86, 0xeb, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==2);
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x86, 0xab, 0x1c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x86, 0xab, 0x5c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x86, 0xab, 0xdc);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==1);
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x86, 0xab, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	test(generatedUnicode.Length()==2);
	test(generatedUnicode[0]==0xdbda);
	test(generatedUnicode[1]==0xdedc);

	// Additional tests added for INC117111...

	utf8.Format(_L8("%c%c%c%c"), 0xf4, 0xc6, 0xab, 0x9c);     // Invalid 4 bit
	TInt returnval = CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);
	test(generatedUnicode.Length()==3);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0x01AB);
	test(generatedUnicode[2]==0xFFFD);

	unicode.Format(percentCPercentC_16, 0xd800, 0xdc00); // a surrogate pair
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);

	test(generatedUtf8==_L8("\xf0\x90\x80\x80")); // round trip test
	returnval = CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8);
	test(generatedUnicode.Length()==2);
	test(generatedUnicode[0]==0xd800);
	test(generatedUnicode[1]==0xdc00);

	unicode.Format(percentCPercentC_16, 0xd7ff, 0xdfff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf\xed\xbf\xbf"));

	utf8.Format(_L8("%c"), 0xc0); // An invalid char.
	returnval = CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);
	test(generatedUnicode.Length()==1);
	test(generatedUnicode[0]==0xFFFD);

	utf8.Format(_L8("%c%c%c%c%c"), 0xf4, 0x20, 0x01, 0x02, 0x10); // Invalid 4 bit and a ASCI
	returnval = CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);
	test(generatedUnicode.Length()==5);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0x0020);
	test(generatedUnicode[2]==0x0001);
	test(generatedUnicode[3]==0x0002);
	test(generatedUnicode[4]==0x0010);

	utf8.Format(_L8("%c%c%c%c"), 0xed, 0xa0, 0x80, 0x10);		// Invalid 3 bit and a ASCI
	returnval = CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);
	test(generatedUnicode.Length()==4);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0x0010);

	utf8.Format(_L8("%c%c%c%c%c"), 0xf4, 0x81, 0x81, 0x81, 0x10);     // Valid 4 bit and a ASCI
	returnval = CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);
	test(generatedUnicode.Length()==3);
	test(generatedUnicode[0]==0xdbc4);
	test(generatedUnicode[1]==0xdc41);
	test(generatedUnicode[2]==0x0010);

	utf8.Format(_L8("%c%c%c%c%c"), 0xf4, 0xc6, 0xab, 0x9c, 0x10);     // Invalid 4 bit and a ASCI
	returnval = CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);
	test(generatedUnicode.Length()==4);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0x01AB);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0x0010);

	utf8.Format(_L8("%c%c%c%c%c"), 0xf4, 0x8c, 0x81, 0x81, 0x10);     // Valid 4 bit and a ASCI
	returnval = CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);

	//INC044750
	utf8.Format(_L8("%c%c%c%c%c"),0x3C, 0xFC, 0xDC, 0x3C,0x3C);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	test(generatedUnicode==_L("<\xFFFD\xFFFD<<"));
	}
	INFO_PRINTF1(_L("Testing bad uuuuu (wwww+1) bits in 4-byte UTF-8"));
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf4, 0x96, 0xab, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	utf8.Format(percentCPercentCPercentCPercentC_8, 0xf0, 0x86, 0xab, 0x9c);
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8)==0);
	{
	INFO_PRINTF1(_L("Testing converting bad Unicode surrogate-pairs to UTF-8"));
	unicode.Format(percentC_16, 0xd800);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, '>', 0xd800);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==1);
	test(generatedUtf8.Length()==1);
	test(generatedUtf8[0]=='>');
	unicode.Format(percentC_16, 0xdbff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 'Z', 0xdbff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==1);
	test(generatedUtf8.Length()==1);
	test(generatedUtf8[0]=='Z');
	unicode.Format(percentC_16, 0xdaaa);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, '|', 0xdaaa);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==1);
	test(generatedUtf8.Length()==1);
	test(generatedUtf8[0]=='|');
	unicode.Format(percentC_16, 0xd7ff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf"));
	unicode.Format(percentC_16, 0xdc00);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xb0\x80"));
	unicode.Format(percentCPercentC_16, 0xd800, 0xe000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xdbff, 0xe000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xdaaa, 0xe000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xd7ff, 0xe000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf\xee\x80\x80"));
	unicode.Format(percentCPercentC_16, 0xdc00, 0xe000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xb0\x80\xee\x80\x80"));
	unicode.Format(percentCPercentC_16, 0xd800, 0xdbff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xdbff, 0xdbff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xdaaa, 0xdbff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xd7ff, 0xdbff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==1);
	test(generatedUtf8==_L8("\xed\x9f\xbf"));
	unicode.Format(percentCPercentC_16, 0xdc00, 0xdbff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==1);
	test(generatedUtf8==_L8("\xed\xb0\x80"));
	unicode.Format(percentCPercentC_16, 0xd800, 0xd7ff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xdbff, 0xd7ff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xdaaa, 0xd7ff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==CnvUtfConverter::EErrorIllFormedInput);
	unicode.Format(percentCPercentC_16, 0xd7ff, 0xd7ff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf\xed\x9f\xbf"));
	unicode.Format(percentCPercentC_16, 0xdc00, 0xd7ff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xb0\x80\xed\x9f\xbf"));
	unicode.Format(percentCPercentC_16, 0xd800, 0xdfff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf0\x90\x8f\xbf"));
	unicode.Format(percentCPercentC_16, 0xdbff, 0xdfff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf4\x8f\xbf\xbf"));
	unicode.Format(percentCPercentC_16, 0xdaaa, 0xdfff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf2\xba\xaf\xbf"));
	unicode.Format(percentCPercentC_16, 0xd7ff, 0xdfff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf\xed\xbf\xbf"));
	unicode.Format(percentCPercentC_16, 0xdc00, 0xdfff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xb0\x80\xed\xbf\xbf"));
	unicode.Format(percentCPercentC_16, 0xd800, 0xdc00);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf0\x90\x80\x80"));
	unicode.Format(percentCPercentC_16, 0xdbff, 0xdc00);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf4\x8f\xb0\x80"));
	unicode.Format(percentCPercentC_16, 0xdaaa, 0xdc00);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf2\xba\xa0\x80"));
	unicode.Format(percentCPercentC_16, 0xd7ff, 0xdc00);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf\xed\xb0\x80"));
	unicode.Format(percentCPercentC_16, 0xdc00, 0xdc00);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xb0\x80\xed\xb0\x80"));
	unicode.Format(percentCPercentC_16, 0xd800, 0xde37);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf0\x90\x88\xb7"));
	unicode.Format(percentCPercentC_16, 0xdbff, 0xde37);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf4\x8f\xb8\xb7"));
	unicode.Format(percentCPercentC_16, 0xdaaa, 0xde37);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf2\xba\xa8\xb7"));
	unicode.Format(percentCPercentC_16, 0xd7ff, 0xde37);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf\xed\xb8\xb7"));
	unicode.Format(percentCPercentC_16, 0xdc00, 0xde37);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xb0\x80\xed\xb8\xb7"));
	}
	TUint i;
	for (i=0; i<=0x1f; ++i)
		{
		utf8.Format(_L8("%c%c\x99\xb1"), 0xf0|(i>>2), 0x85|((i&0x03)<<4));
		const TInt returnValue=CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);
		if ((i<1) || ((i-1)&~0x0f))
			{
			test(returnValue==0);
			}
		else
			{
			test(returnValue==0);
			test(generatedUnicode.Length()==2);
			test((0xd815&~((i-1)<<6))==0xd815); // sanity check ((i-1)<<6)
			test(generatedUnicode[0]==(0xd815|((i-1)<<6)));
			test(generatedUnicode[1]==0xde71);
			}
		}
	for (i=0; i<=0x1f; ++i)
		{
		utf8.Format(_L8("%c%c\x80\x80"), 0xf0|(i>>2), 0x80|((i&0x03)<<4));
		const TInt returnValue=CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, utf8);
		if ((i<1) || ((i-1)&~0x0f))
			{
			test(returnValue==0);
			}
		else
			{
			test(returnValue==0);
			test(generatedUnicode.Length()==2);
			test((0xd800&~((i-1)<<6))==0xd800); // sanity check ((i-1)<<6)
			test(generatedUnicode[0]==(0xd800|((i-1)<<6)));
			test(generatedUnicode[1]==0xdc00);
			}
		}
	INFO_PRINTF1(_L("Testing passing in descriptors that are empty and have NULL pointers"));
	enum {EBufferLength8=32};
	TUint8 buffer8[EBufferLength8];
	TPtr8 null8(NULL, 0, 0);
	TPtr8 trash8(REINTERPRET_CAST(TUint8*, 1), 0, 0);
	TPtr8 empty8(buffer8, 0, EBufferLength8);
	TPtr8 nonEmpty8(buffer8, 0, EBufferLength8);
	enum {EBufferLength16=16};
	TUint16 buffer16[EBufferLength16];
	TPtr16 null16(NULL, 0, 0);
	TPtr16 trash16(REINTERPRET_CAST(TUint16*, 2), 0, 0);
	TPtr16 empty16(buffer16, 0, EBufferLength16);
	TPtr16 nonEmpty16(buffer16, 0, EBufferLength16);
	for (i=0; ; ++i)
		{
		const TUint j=i/4;
		const TUint k=j/4;
		TDes8& descriptor8=(i%4==0)? null8: (i%4==1)? trash8: (i%4==2)? empty8: nonEmpty8;
		TDes16& descriptor16=(j%4==0)? null16: (j%4==1)? trash16: (j%4==2)? empty16: nonEmpty16;
		ResetDescriptorsForNullTest(empty8, empty16, nonEmpty8, nonEmpty16);
		const TBool inCompleteConversion8=((descriptor8.MaxLength()==0) && (descriptor16.Length()>0));
		const TBool inCompleteConversion16=((descriptor16.MaxLength()==0) && (descriptor8.Length()>0));
		TInt returnValue;
		returnValue=CnvUtfConverter::ConvertFromUnicodeToUtf7(descriptor8, descriptor16, k&0x1);
		test(inCompleteConversion8? (returnValue>0): (returnValue==0));
		ResetDescriptorsForNullTest(empty8, empty16, nonEmpty8, nonEmpty16);
		returnValue=CnvUtfConverter::ConvertFromUnicodeToUtf8(descriptor8, descriptor16);
		test(inCompleteConversion8? (returnValue>0): (returnValue==0));
		ResetDescriptorsForNullTest(empty8, empty16, nonEmpty8, nonEmpty16);
		state=CnvUtfConverter::KStateDefault;
		returnValue=CnvUtfConverter::ConvertToUnicodeFromUtf7(descriptor16, descriptor8, state);
		test(inCompleteConversion16? (returnValue>0): (returnValue==0));
		test(inCompleteConversion16 || (state==CnvUtfConverter::KStateDefault));
		ResetDescriptorsForNullTest(empty8, empty16, nonEmpty8, nonEmpty16);
		returnValue=CnvUtfConverter::ConvertToUnicodeFromUtf8(descriptor16, descriptor8);
		test(inCompleteConversion16? (returnValue>0): (returnValue==0));
		if (k&0x2)
			{
			break;
			}
		}
	state=CnvUtfConverter::KStateDefault; // in case any more tests are added below

	// Pop and destroy: unicodeHBuf, generatedUnicode, utf8, generatedUtf8
	CleanupStack::PopAndDestroy(4);
	}


CT_ERROR::CT_ERROR()
    {
    SetTestStepName(KTestStep_T_ERROR);
    }

TVerdict CT_ERROR::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestERROR());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
