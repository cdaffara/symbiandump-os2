/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_builtin.h"

/**
* Work around for DEF063276.
* This literal is now loaded from a z:\test\data\KOriginalSms7Bit.dat
* Bullseye Coverage corrupts this literal at position \x1b\x30 to \x1b\x37
* to avoid this literal is stored in a file as to not be touched by Bullseye Coverage.
*/

/*
_LIT8(KOriginalSms7Bit, "\
\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\
\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a    \x1c\x1d\x1e\x1f\
\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\
\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\
\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\
\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\
\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\
\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f\
\x1b\x00\x1b\x01\x1b\x02\x1b\x03\x1b\x04\x1b\x05\x1b\x06\x1b\x07\x1b\x08\x1b\x09\x1b\x0a\x1b\x0b\x1b\x0c\x1b\x0d\x1b\x0e\x1b\x0f\
\x1b\x10\x1b\x11\x1b\x12\x1b\x13\x1b\x14\x1b\x15\x1b\x16\x1b\x17\x1b\x18\x1b\x19\x1b\x1a\x1b\x1b\x1b\x1c\x1b\x1d\x1b\x1e\x1b\x1f\
\x1b\x20\x1b\x21\x1b\x22\x1b\x23\x1b\x24\x1b\x25\x1b\x26\x1b\x27\x1b\x28\x1b\x29\x1b\x2a\x1b\x2b\x1b\x2c\x1b\x2d\x1b\x2e\x1b\x2f\
\x1b\x30\x1b\x31\x1b\x32\x1b\x33\x1b\x34\x1b\x35\x1b\x36\x1b\x37\x1b\x38\x1b\x39\x1b\x3a\x1b\x3b\x1b\x3c\x1b\x3d\x1b\x3e\x1b\x3f\
\x1b\x40\x1b\x41\x1b\x42\x1b\x43\x1b\x44\x1b\x45\x1b\x46\x1b\x47\x1b\x48\x1b\x49\x1b\x4a\x1b\x4b\x1b\x4c\x1b\x4d\x1b\x4e\x1b\x4f\
\x1b\x50\x1b\x51\x1b\x52\x1b\x53\x1b\x54\x1b\x55\x1b\x56\x1b\x57\x1b\x58\x1b\x59\x1b\x5a\x1b\x5b\x1b\x5c\x1b\x5d\x1b\x5e\x1b\x5f\
\x1b\x60\x1b\x61\x1b\x62\x1b\x63\x1b\x64\x1b\x65\x1b\x67\x1b\x67\x1b\x68\x1b\x69\x1b\x6a\x1b\x6b\x1b\x6c\x1b\x6d\x1b\x6e\x1b\x6f\
\x1b\x70\x1b\x71\x1b\x72\x1b\x73\x1b\x74\x1b\x75\x1b\x76\x1b\x77\x1b\x78\x1b\x79\x1b\x7a\x1b\x7b\x1b\x7c\x1b\x7d\x1b\x7e\x1b\x7f");
*/


_LIT8(KExpectedSms7Bit, "\
\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\
\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a    \x1c\x1d\x1e\x1f\
\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\
\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\
\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\
\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\
\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\
\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f\
\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x1b\x0a\x0b\x0c\x0d\x0e\x0f\
\x10\x11\x12\x13\x1b\x14\x15\x16\x17\x18\x19\x1a\x3f\x1c\x1d\x1e\x1f\
\x20\x21\x22\x23\x24\x25\x26\x27\x1b\x28\x1b\x29\x2a\x2b\x2c\x2d\x2e\x1b\x2f\
\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3a\x3b\x1b\x3c\x1b\x3d\x1b\x3e\x3f\
\x1b\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\
\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\
\x60\x61\x62\x63\x64\x1b\x65\x67\x67\x68\x69\x6a\x6b\x6c\x6d\x6e\x6f\
\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7a\x7b\x7c\x7d\x7e\x7f");
_LIT16(KExpectedUnicodeFromSms7Bit, "\
\x0040\x00a3$\x00a5\x00e8\x00e9\x00f9\x00ec\x00f2\x00e7\x000a\x00d8\x00f8\x000d\x00c5\x00e5\
\x0394\x005f\x03a6\x0393\x039b\x03a9\x03a0\x03a8\x03a3\x0398\x039e    \x00c6\x00e6\x00df\x00c9\
\x0020!\"#\x00a4%&'()*+,-./0123456789:;<=>?\
\x00a1\x0041\x0042\x0043\x0044\x0045\x0046GHIJKLMNOPQRSTUVWXYZ\x00c4\x00d6\x00d1\x00dc\x00a7\
\x00bf\x0061\x0062\x0063\x0064\x0065\x0066ghijklmnopqrstuvwxyz\x00e4\x00f6\x00f1\x00fc\x00e0\
\x0040\x00a3$\x00a5\x00e8\x00e9\x00f9\x00ec\x00f2\x00c7\x000c\x00d8\x00f8\x000d\x00c5\x00e5\
\x0394\x005f\x03a6\x0393\x005e\x03a9\x03a0\x03a8\x03a3\x0398\x039e\xfffd\x00c6\x00e6\x00df\x00c9\
\x0020!\"#\x00a4%&'{}*+,-.\\0123456789:;[~]?\
|\x0041\x0042\x0043\x0044\x0045\x0046GHIJKLMNOPQRSTUVWXYZ\x00c4\x00d6\x00d1\x00dc\x00a7\
\x00bf\x0061\x0062\x0063\x0064\x20ac\x0067ghijklmnopqrstuvwxyz\x00e4\x00f6\x00f1\x00fc\x00e0");



// 1 line of the original defect data from email in unicode
	_LIT16(KEmailToConvert, "\
\x006C\x0069\x006E\x0065\x0020\x0065\x006E\x0064\x0073\x0020\x0069\
\x006E\x0020\x006A\x0061\x0070\x0061\x006E\x0065\x0073\x0065\x0020\
\x3075\x3064\x3059\x3057\x306B\x3072\x3080\x000d\x000a");



	// the expected result when converted to ISO2022JP
_LIT8(KExpectedIso2022jp, "\
\x6C\x69\x6E\x65\x20\x65\x6E\x64\x73\x20\x69\x6E\x20\
\x6A\x61\x70\x61\x6E\x65\x73\x65\x20\x1B\x24\x42\x24\x55\x24\x44\
\x24\x39\x24\x37\x24\x4B\x24\x52\x24\x60\x1B\x28\x42\x0D\x0A");
//\x1B\x28\x4A");

// 1 line of the original defect data from email in unicode
	_LIT16(KEmailEndingInJapanese, "\
\x006C\x0069\x006E\x0065\x0020\x0065\x006E\x0064\x0073\x0020\x0069\
\x006E\x0020\x006A\x0061\x0070\x0061\x006E\x0065\x0073\x0065\x0020\
\x3075\x3064\x3059\x3057\x306B\x3072\x3080");

	// the expected result when converted to ISO2022JP
_LIT8(KExpectedEmailEndingInJapanese, "\
\x6C\x69\x6E\x65\x20\x65\x6E\x64\x73\x20\x69\x6E\x20\
\x6A\x61\x70\x61\x6E\x65\x73\x65\x20\x1B\x24\x42\x24\x55\x24\x44\
\x24\x39\x24\x37\x24\x4B\x24\x52\x24\x60\x1B\x28\x42");

_LIT(KOriginalSms7BitFile, "z:\\test\\data\\KOriginalSms7Bit.dat");

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


// this is the test plugin implementation ID in rather than KCharacterSetIdentifierShiftJis
const TUint KCharacterSetTestShiftJisKddiau=0x01000001;

/**
* Utility for DEF063276 fix.
*/

void CT_BUILTIN::ReadDesc(TDes8& aDes, const TDesC& aFilename, RFs& aFs)
	{
	RFile file;
	TInt err = file.Open(aFs, aFilename, EFileRead);
	test(err == KErrNone);
	CleanupClosePushL(file);

	err = file.Read(aDes);
	test(err == KErrNone);
	CleanupStack::PopAndDestroy(&file);
	}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0499
@SYMTestCaseDesc        Tests for conversion of SMS 7 bit data to Unicode, and from Unicode to original data
@SYMTestPriority        Medium
@SYMTestActions         Tests for CCnvCharacterSetConverter::ConvertToUnicode(),CCnvCharacterSetConverter::ConvertFromUnicode()
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_BUILTIN::TestFix_EDNJJUN_4RWH2A(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession, CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0499 "));
	TBuf16<300> generatedUnicode;
	TBuf8<300> generatedSms7Bit;
	TRAPD(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, aArrayOfCharacterSetsAvailable, aFileServerSession));
	test(errCode == KErrNone);
	TInt state=CCnvCharacterSetConverter::KStateDefault;

	// Fix for DEF063276.

	TBuf8<400> buf;
	ReadDesc(buf, KOriginalSms7BitFile, aFileServerSession);

	test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, buf, state)==0);
	// End fix.

	test(generatedUnicode.Length()==259);
	test(generatedUnicode==KExpectedUnicodeFromSms7Bit);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(aCharacterSetConverter.ConvertFromUnicode(generatedSms7Bit, generatedUnicode)==0);
	test(generatedSms7Bit.Length()==269);
	test(generatedSms7Bit==KExpectedSms7Bit);
	}

/**
Test_DEF049749 test code for DEF049749 - Char convs don t handle correcly Shift JIS emoji

@SYMTestCaseID          SYSLIB-CHARCONV-CT-0500
@SYMTestCaseDesc        Tests for defect number DEF049749
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion from Shift JIS Emoji character set to Unicode
                        Tests for CCnvCharacterSetConverter::EErrorIllFormedInput error flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_BUILTIN::Test_DEF049749(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession, CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable)
	{
	// original defect data
	_LIT8(KCodeToConvert, "\xF8\x9F\xF8\x9F\x81\x40\x3B\x3B\x3B\x3B");
	_LIT16(KExpectedUnicode, "\xFFFD\xFFFD\x3000\x003b\x003b\x003b\x003b");
	TBuf16<300> generatedUnicode;

	// check now able to convert the original defect data
	TRAPD(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
	test(errCode == KErrNone);
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TInt ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, KCodeToConvert, state);

	// in DEF049749 CCnvCharacterSetConverter::EErrorIllFormedInput was incorrectly returned.
	// rather than the number of unconverted bytes left, which should be 0
	test(ret ==0);
	test(generatedUnicode==KExpectedUnicode);

	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0500 User-defined ShiftJIS region->FFFD "));
	/* check the user defined region (ref CJKV Ken Lunde page206)
	F040-F97E and F080-F9FC */
	TBuf8<300> sampleUserDefined;
	for (TInt leadByte = 0xF0; leadByte != 0xF9; leadByte++)
		{
		// a leadbyte missing the trail byte then another character
		sampleUserDefined.Format(_L8("%c\x3F"), leadByte);
		_LIT16(KExpectedUnicode4, "\xFFFD\x3F");
		TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
		test(errCode == KErrNone);
		ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
		test(ret ==0);
		test(generatedUnicode==KExpectedUnicode4);

		// a single leadbyte missing the trail byte
		sampleUserDefined.Format(_L8("%c"), leadByte);
		TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
		test(errCode == KErrNone);
		ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
		test(ret == CCnvCharacterSetConverter::EErrorIllFormedInput);

		// Default version of Shift-JIS converter now contains mappings for ezweb private use
		// in range 0xF7BC to 0xF7FC so following tests would fail in this area - JG 30/06/2005
		if ((leadByte < 0xF3) || (leadByte > 0xF7))
			{
			for (TInt trailbyte = 0x40; trailbyte != 0xFD;
				trailbyte += (trailbyte == 0x7E? 2 : 1))
				{
				TInt errCode = KErrNone;

				// emoji character on its own
				sampleUserDefined.Format(_L8("%c%c"), leadByte, trailbyte);
				_LIT16(KExpectedUnicode1, "\xFFFD");
				TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
				test(errCode == KErrNone);
				ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
				test(ret ==0);
				test(generatedUnicode==KExpectedUnicode1);

				// followed by a single character
				sampleUserDefined.Format(_L8("%c%cx"), leadByte, trailbyte);
				_LIT16(KExpectedUnicode2, "\xFFFDx");
				TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
				test(errCode == KErrNone);
				ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
				test(ret ==0);
				test(generatedUnicode==KExpectedUnicode2);

				// preceeded by a single character
				sampleUserDefined.Format(_L8("x%c%c"), leadByte, trailbyte);
				_LIT16(KExpectedUnicode3, "x\xFFFD");
				TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
				test(errCode == KErrNone);
				ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
				test(ret ==0);
				test(generatedUnicode==KExpectedUnicode3);
				}
			}
		}

	INFO_PRINTF1(_L("All double-byte characters convert to single Unicode"));
	TText8 LeadBytes[] = { 0x81, 0x9F, 0xE0, 0xEA, 0xEB, 0xF9, 0xFA, 0xFC };
	TText8 TrailBytes[] = { 0x40, 0x7E, 0x80, 0xFC };
	TText8* leadp;
	for (leadp = LeadBytes;
		leadp != LeadBytes + sizeof(LeadBytes)/sizeof(LeadBytes[0]);
		++leadp)
		{
		// a single leadbyte missing the trail byte
		sampleUserDefined.Format(_L8("%c"), *leadp);
		TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
		test(errCode == KErrNone);
		ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
		test(ret == CCnvCharacterSetConverter::EErrorIllFormedInput);

		// lead byte then illegal trail byte, which is a legal byte on its own
		sampleUserDefined.Format(_L8("%c%c"), *leadp, 0x3F);
		TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
		test(errCode == KErrNone);
		ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
		test(ret == 0);
		test(generatedUnicode == _L16("\xFFFD\x3F"));

		for (TText8* trailp = TrailBytes;
			trailp != TrailBytes + sizeof(TrailBytes)/sizeof(TrailBytes[0]);
			++trailp)
			{
			sampleUserDefined.Format(_L8("%c%c"), *leadp, *trailp);
			TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
			test(errCode == KErrNone);
			ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
			test(ret == 0);
			test(generatedUnicode.Length() == 1);
			}
		}

	INFO_PRINTF1(_L("All single-byte characters convert to single Unicode"));
	TText8 SingleBytes[] = { 0, 0x7F, 0x80, 0xA0, 0xA1, 0xFD, 0xFE, 0xFF };
	for (TText8* singlep = SingleBytes;
		singlep != SingleBytes + sizeof(SingleBytes)/sizeof(SingleBytes[0]);
		++singlep)
		{
		// a single leadbyte missing the trail byte
		sampleUserDefined.Format(_L8("%c"), *singlep);
		TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
		test(errCode == KErrNone);
		ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
		test(ret == 0);
		test(generatedUnicode.Length() == 1);
		}

	INFO_PRINTF1(_L("Valid lead byte + invalid trail byte -> FFFD + something"));
	TText8 NotTrailBytes[] = { 0x3F, 0x7F, 0xFD, 0xFE, 0xFF };
	for (leadp = LeadBytes;
		leadp != LeadBytes + sizeof(LeadBytes)/sizeof(LeadBytes[0]);
		++leadp)
		{
		for (TText8* trailp = NotTrailBytes;
			trailp != NotTrailBytes + sizeof(NotTrailBytes)/sizeof(NotTrailBytes[0]);
			++trailp)
			{
			sampleUserDefined.Format(_L8("%c%c"), *leadp, *trailp);
			TRAP(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetTestShiftJisKddiau, aArrayOfCharacterSetsAvailable, aFileServerSession));
			test(errCode == KErrNone);
			ret = aCharacterSetConverter.ConvertToUnicode(generatedUnicode, sampleUserDefined, state);
			test(ret == 0);
			test(generatedUnicode.Length() == 2);
			test(generatedUnicode[0] == 0xFFFD);
			}
		}
	}

/**
Test_DEF053900 test code for DEF053900  - Problems encoding Japanese emails to ISO-2022-JP

@SYMTestCaseID          SYSLIB-CHARCONV-CT-1306
@SYMTestCaseDesc        Tests for defect number DEF053900
@SYMTestPriority        Medium
@SYMTestActions         Attempt for converting from Japanese character set email to Unicode.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_BUILTIN::Test_DEF053900(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession, CArrayFix<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable)
	{
	// check now able to convert the original defect data
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-1306 "));
	TBuf8<300> generatedIso2022;
	TRAPD(errCode, aCharacterSetConverter.PrepareToConvertToOrFromL(KCharacterSetIdentifierIso2022Jp, aArrayOfCharacterSetsAvailable, aFileServerSession));
	test(errCode == KErrNone);
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TInt ret = aCharacterSetConverter.ConvertFromUnicode(generatedIso2022, KEmailToConvert, state);
	// check the converted data is as expected
	test(ret ==0);
	test(generatedIso2022==KExpectedIso2022jp);
	ret = aCharacterSetConverter.ConvertFromUnicode(generatedIso2022, KEmailEndingInJapanese, state);
	test(ret ==0);
	test(generatedIso2022==KExpectedEmailEndingInJapanese);
}

/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0501
@SYMTestCaseDesc        Tests for CCnvCharacterSetConverter class
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion from empty descriptors to different character sets
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_BUILTIN::DoE32MainL()
	{
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
	INFO_PRINTF1(_L("Available:\n"));
	for (TInt i=arrayOfCharacterSetsAvailable->Count()-1; i>=0; --i)
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
	TBuf16<128> originalUnicode;
	TBuf8<128> generatedForeign;
	TBuf16<128> generatedUnicode;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TUint characterCode;
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0501 Testing empty descriptors (Code Page 1252) "));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, *arrayOfCharacterSetsAvailable, fileServerSession);
	originalUnicode=KNullDesC16;
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==KNullDesC8);
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Testing empty descriptors (ISO-8859-1)"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierIso88591, *arrayOfCharacterSetsAvailable, fileServerSession);
	originalUnicode=KNullDesC16;
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==KNullDesC8);
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Testing empty descriptors (ASCII)"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, *arrayOfCharacterSetsAvailable, fileServerSession);
	originalUnicode=KNullDesC16;
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==KNullDesC8);
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Testing empty descriptors (SMS 7-bit)"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, *arrayOfCharacterSetsAvailable, fileServerSession);
	originalUnicode=KNullDesC16;
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==KNullDesC8);
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Testing exhaustive single-character Code Page 1252 round trips"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, *arrayOfCharacterSetsAvailable, fileServerSession);
	for (characterCode=0x00; characterCode<=0x7f; ++characterCode)
		{
		originalUnicode.Format(_L16("%c"), characterCode);
		test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
		TBuf8<1> expectedForeign;
		expectedForeign.Format(_L8("%c"), characterCode);
		test(generatedForeign==expectedForeign);
		test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		test(generatedUnicode==originalUnicode);
		}
	originalUnicode.Format(_L16("%c"), 0x20ac);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x201a);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x82"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x0192);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x83"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x201e);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x84"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2026);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x85"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2020);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x86"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2021);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x87"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x02c6);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x88"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2030);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x89"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x0160);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x8a"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2039);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x8b"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x0152);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x8c"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x017d);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x8e"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x017e);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x9e"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2018);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x91"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2019);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x92"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x201c);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x93"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x201d);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x94"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2022);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x95"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2013);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x96"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2014);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x97"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x02dc);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x98"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x2122);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x99"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x0161);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x9a"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x203a);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x9b"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x0153);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x9c"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c"), 0x0178);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x9f"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	for (characterCode=0xa0; characterCode<0xff; ++characterCode)
		{
		originalUnicode.Format(_L16("%c"), characterCode);
		test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
		TBuf8<1> expectedForeign;
		expectedForeign.Format(_L8("%c"), characterCode);
		test(generatedForeign==expectedForeign);
		test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		test(generatedUnicode==originalUnicode);
		}
	INFO_PRINTF1(_L("Testing exhaustive single-character ISO-8859-1 round trips"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierIso88591, *arrayOfCharacterSetsAvailable, fileServerSession);
	for (characterCode=0x00; characterCode<=0xff; ++characterCode)
		{
		const TBool unconvertibleCharacter=(characterCode==0x80);
		TInt numberOfUnconvertibleCharacters;
		TInt indexOfFirstUnconvertibleCharacter;
		originalUnicode.Format(_L16("%c"), characterCode);
		test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode, numberOfUnconvertibleCharacters, indexOfFirstUnconvertibleCharacter)==0);
		if (characterCode != 0x80)
			test(numberOfUnconvertibleCharacters==(unconvertibleCharacter? 1: 0));
		if (numberOfUnconvertibleCharacters>0)
			{
			test(indexOfFirstUnconvertibleCharacter==0);
			}
		TBuf8<1> expectedForeign;
		expectedForeign.Format(_L8("%c"), unconvertibleCharacter? 0x1a: characterCode);
		test(generatedForeign==expectedForeign);
		TBuf8<1> originalForeign;
		originalForeign.Format(_L8("%c"), characterCode);
		test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalForeign, state, numberOfUnconvertibleCharacters, indexOfFirstUnconvertibleCharacter)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		if (characterCode != 0x80)
			test(numberOfUnconvertibleCharacters==(unconvertibleCharacter? 1: 0));
		if (numberOfUnconvertibleCharacters>0)
			{
			test(indexOfFirstUnconvertibleCharacter==0);
			}
		if (unconvertibleCharacter)
			{
			test(generatedUnicode.Length()==1);
			if (characterCode==0x80)
			    {
				test(generatedUnicode[0]==0x20ac);
			    }
			else
			    {
				test(generatedUnicode[0]==0xfffd);
			    }
			}
		else
			{
			test(generatedUnicode==originalUnicode);
			}
		}
	INFO_PRINTF1(_L("Testing exhaustive single-character ASCII round trips"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, *arrayOfCharacterSetsAvailable, fileServerSession);
	for (characterCode=0x00; characterCode<=0xff; ++characterCode)
		{
		const TBool unconvertibleCharacter=(characterCode>=0x80);
		TInt numberOfUnconvertibleCharacters;
		originalUnicode.Format(_L16("%c"), characterCode);
		test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode, numberOfUnconvertibleCharacters)==0);
		test(numberOfUnconvertibleCharacters==(unconvertibleCharacter? 1: 0));
		TBuf8<1> expectedForeign;
		expectedForeign.Format(_L8("%c"), unconvertibleCharacter? 0x1a: characterCode);
		test(generatedForeign==expectedForeign);
		TBuf8<1> originalForeign;
		originalForeign.Format(_L8("%c"), characterCode);
		test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalForeign, state, numberOfUnconvertibleCharacters)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		test(numberOfUnconvertibleCharacters==(unconvertibleCharacter? 1: 0));
		if (unconvertibleCharacter)
			{
			test(generatedUnicode.Length()==1);
			test(generatedUnicode[0]==0xfffd);
			}
		else
			{
			test(generatedUnicode==originalUnicode);
			}
		}
	INFO_PRINTF1(_L("Testing exhaustive SMS 7-bit round trip (default table)"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, fileServerSession);
	originalUnicode.SetLength(0);
	const TPtrC16 formatOfSixteenCharacters=_L16("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c");
	originalUnicode.AppendFormat(formatOfSixteenCharacters, 0x0040, 0x00a3, 0x0024, 0x00a5, 0x00e8, 0x00e9, 0x00f9, 0x00ec, 0x00f2, 0x00e7, 0x000a, 0x00d8, 0x00f8, 0x000d, 0x00c5, 0x00e5);
	originalUnicode.AppendFormat(formatOfSixteenCharacters, 0x0394, 0x005f, 0x03a6, 0x0393, 0x039b, 0x03a9, 0x03a0, 0x03a8, 0x03a3, 0x0398, 0x039e,    '!', 0x00c6, 0x00e6, 0x00df, 0x00c9);
	originalUnicode.AppendFormat(formatOfSixteenCharacters, 0x0020, 0x0021, 0x0022, 0x0023, 0x00a4, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f);
	originalUnicode.AppendFormat(formatOfSixteenCharacters, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f);
	originalUnicode.AppendFormat(formatOfSixteenCharacters, 0x00a1, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f);
	originalUnicode.AppendFormat(formatOfSixteenCharacters, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x00c4, 0x00d6, 0x00d1, 0x00dc, 0x00a7);
	originalUnicode.AppendFormat(formatOfSixteenCharacters, 0x00bf, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f);
	originalUnicode.AppendFormat(formatOfSixteenCharacters, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x00e4, 0x00f6, 0x00f1, 0x00fc, 0x00e0);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign.Length()==0x80);
	for (characterCode=0x00; characterCode<0x80; ++characterCode)
		{
		test(generatedForeign[characterCode]==(characterCode==0x1b)? '!': characterCode);
		}
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Testing Greek range of SMS 7-bit"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, *arrayOfCharacterSetsAvailable, fileServerSession);
	TBuf16<16> expectedUnicode;
	originalUnicode.Format(formatOfSixteenCharacters.Left(7*2), 0x0386, 0x0388, 0x0389, 0x038a, 0x038c, 0x038e, 0x038f);
	expectedUnicode.Format(formatOfSixteenCharacters.Left(7*2), 0x0041, 0x0045, 0x0048, 0x0049, 0x004f, 0x0059, 0x03a9);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x41\x45\x48\x49\x4f\x59\x15"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==expectedUnicode);
	originalUnicode.Format(formatOfSixteenCharacters, 0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f);
	expectedUnicode.Format(formatOfSixteenCharacters, 0x0049, 0x0041, 0x0042, 0x0393, 0x0394, 0x0045, 0x005a, 0x0048, 0x0398, 0x0049, 0x004b, 0x039b, 0x004d, 0x004e, 0x039e, 0x004f);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x49\x41\x42\x13\x10\x45\x5a\x48\x19\x49\x4b\x14\x4d\x4e\x1a\x4f"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==expectedUnicode);
	originalUnicode.Format(formatOfSixteenCharacters.Left(15*2), 0x03a0, 0x03a1, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7, 0x03a8, 0x03a9, 0x03aa, 0x03ab, 0x03ac, 0x03ad, 0x03ae, 0x03af);
	expectedUnicode.Format(formatOfSixteenCharacters.Left(15*2), 0x03a0, 0x0050, 0x03a3, 0x0054, 0x0059, 0x03a6, 0x0058, 0x03a8, 0x03a9, 0x0049, 0x0059, 0x0041, 0x0045, 0x0048, 0x0049);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x16\x50\x18\x54\x59\x12\x58\x17\x15\x49\x59\x41\x45\x48\x49"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==expectedUnicode);
	originalUnicode.Format(formatOfSixteenCharacters, 0x03b0, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf);
	expectedUnicode.Format(formatOfSixteenCharacters, 0x0059, 0x0041, 0x0042, 0x0393, 0x0394, 0x0045, 0x005a, 0x0048, 0x0398, 0x0049, 0x004b, 0x039b, 0x004d, 0x004e, 0x039e, 0x004f);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x59\x41\x42\x13\x10\x45\x5a\x48\x19\x49\x4b\x14\x4d\x4e\x1a\x4f"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==expectedUnicode);
	originalUnicode.Format(formatOfSixteenCharacters.Left(15*2), 0x03c0, 0x03c1, 0x03c2, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7, 0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03cc, 0x03cd, 0x03ce);
	expectedUnicode.Format(formatOfSixteenCharacters.Left(15*2), 0x03a0, 0x0050, 0x03a3, 0x03a3, 0x0054, 0x0059, 0x03a6, 0x0058, 0x03a8, 0x03a9, 0x0049, 0x0059, 0x004f, 0x0059, 0x03a9);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x16\x50\x18\x18\x54\x59\x12\x58\x17\x15\x49\x59\x4f\x59\x15"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==expectedUnicode);
	originalUnicode.Format(formatOfSixteenCharacters.Left(6*2), 0x03d0, 0x03d1, 0x03d2, 0x03d3, 0x03d4, 0x03d5);
	expectedUnicode.Format(formatOfSixteenCharacters.Left(6*2), 0x0042, 0x0398, 0x0059, 0x0059, 0x0059, 0x03a6);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x42\x19\x59\x59\x59\x12"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==expectedUnicode);
	{
	INFO_PRINTF1(_L("Testing converting from Internet-standard names to UIDs"));
	test(characterSetConverter->ConvertStandardNameOfCharacterSetToIdentifierL(_L8("US-ASCII"), fileServerSession)==KCharacterSetIdentifierAscii);
	HBufC8* standardName=characterSetConverter->ConvertCharacterSetIdentifierToStandardNameL(KCharacterSetIdentifierAscii, fileServerSession);
	test(*standardName==_L8("US-ASCII"));
	delete standardName;
	}
	{
	INFO_PRINTF1(_L("Testing fix for defect EDNEMUA-4KGFEY in \"Symbian Defect Tracking v3.0\""));
	CCnvCharacterSetConverter* characterSetConverter2=CCnvCharacterSetConverter::NewLC();
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, *arrayOfCharacterSetsAvailable, fileServerSession);
	characterSetConverter2->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, *arrayOfCharacterSetsAvailable, fileServerSession);
	characterSetConverter2->SetDowngradeForExoticLineTerminatingCharacters(CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed);
	originalUnicode.Format(_L16("Line1%cLine2%cLine3\x0aLine4\x0d\x0a"), 0x2029, 0x2028);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("Line1\x0d\x0aLine2\x0d\x0aLine3\x0aLine4\x0d\x0a"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==_L16("Line1\x0d\x0aLine2\x0d\x0aLine3\x0aLine4\x0d\x0a"));
	test(characterSetConverter2->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==_L16("Line1\x0d\x0aLine2\x0d\x0aLine3\x0aLine4\x0d\x0a"));
	test(characterSetConverter2->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("Line1\x0aLine2\x0aLine3\x0aLine4\x0d\x0a"));
	test(characterSetConverter2->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==_L16("Line1\x0aLine2\x0aLine3\x0aLine4\x0d\x0a"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==_L16("Line1\x0aLine2\x0aLine3\x0aLine4\x0d\x0a"));
	CleanupStack::PopAndDestroy(); // characterSetConverter2
	}
	{
	INFO_PRINTF1(_L("Testing fix for defect EDNPSTN-4L5K2G in \"Symbian Defect Tracking v3.0\" (i.e. the extension table)"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, *arrayOfCharacterSetsAvailable, fileServerSession);
	originalUnicode.Format(_L16("%c^{}\\abc[~]|%c"), 0x000c, 0x20ac);
	test(characterSetConverter->ConvertFromUnicode(generatedForeign, originalUnicode)==0);
	test(generatedForeign==_L8("\x1b\x0a\x1b\x14\x1b\x28\x1b\x29\x1b\x2f""abc\x1b\x3c\x1b\x3d\x1b\x3e\x1b\x40\x1b\x65"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedForeign, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	}
	{
	INFO_PRINTF1(_L("Testing fix for defect EDNJJUN-4RWH2A in \"Symbian Defect Tracking v3.0\" (SMS 7-bit [0x1b, 0x41] should be converted to 0x0041, etc.)"));
	TestFix_EDNJJUN_4RWH2A(*characterSetConverter, fileServerSession, *arrayOfCharacterSetsAvailable);
	}

	//
	INFO_PRINTF1(_L("Testing fix for defect DEF049749"));
	Test_DEF049749(*characterSetConverter, fileServerSession, *arrayOfCharacterSetsAvailable);

	INFO_PRINTF1(_L("Testing fix for defect DEF053900"));
	Test_DEF053900(*characterSetConverter, fileServerSession, *arrayOfCharacterSetsAvailable);

	CleanupStack::PopAndDestroy(3); // arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
	}

CT_BUILTIN::CT_BUILTIN()
    {
    SetTestStepName(KTestStep_T_BUILTIN);
    }

TVerdict CT_BUILTIN::doTestStepL()
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
