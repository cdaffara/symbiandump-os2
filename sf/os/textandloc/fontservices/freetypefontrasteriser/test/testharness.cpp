/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testharness.h"
#include <bitdev.h>
#include <fntstore.h>
#include <fbs.h>
#include <test/testexecutestepbase.h>
#include <graphics/fbsdefs.h>

CTestHarness::CTestHarness(CTestStep* aStep) :
		iStep(aStep)
	{
	iStep -> INFO_PRINTF1(iStep -> TestStepName());
	}

CTestHarness* CTestHarness::NewL(CTestStep* aStep)
	{
	CTestHarness* t = new (ELeave)CTestHarness (aStep);
	CleanupStack::PushL(t);
	t->ConstructL();
	CleanupStack::Pop();
	return t;
	}

void CTestHarness::ConstructL()
	{
	User::LeaveIfError(FbsStartup());
	
	RFbsSession::Connect();
	iFbs = RFbsSession::GetSession();
	if (iFbs == NULL)
		User::Leave(KErrGeneral);

	TRAPD(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EColor16M));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EColor16MA));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EColor16MU));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EColor64K));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EColor4K));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EColor256));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EColor16));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EGray256));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EGray16));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EGray4));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EGray2));
	if (error == KErrNotSupported)
		TRAP(error, iDev = CFbsScreenDevice::NewL(_L("scdv"),EColor16MAP));
	
	if(error == KErrNone)
		{
		iDev->ChangeScreenDevice(NULL);
		iDev->SetAutoUpdate(ETrue);
		iDev->CreateContext(iGc);

		TFontSpec fs(_L("DejaVu Sans Condensed"),16);
		error = iDev->GetNearestFontToDesignHeightInPixels((CFont*&)iStandardFont,fs);
		if (error)
			User::Panic(_L("Could not create 16-pixel DejaVu Sans Condensed"),error);
		}
	}

CTestHarness::~CTestHarness()
	{
	if (iStandardFont)
		iDev->ReleaseFont(iStandardFont);
	delete iGc;
	delete iDev;
	RFbsSession::Disconnect();
	}

void CTestHarness::PerformTests()
	{
	iStep->INFO_PRINTF1(_L("Test started"));
	
	TInt typefaces = iDev->NumTypefaces();
	for (int typeface = 0; typeface < typefaces; typeface++)
		{
		TestFont(typeface);
		}
		
	iStep->INFO_PRINTF1(_L("Test ended"));
	}
	
void CTestHarness::TestFont(TInt aTypeface)
	{
	TTypefaceSupport support;
	iDev->TypefaceSupport(support,aTypeface);
	
	TBuf<50> testInfo;
	testInfo.Append(_L("Testing typeface: "));
	testInfo.Append(support.iTypeface.iName);
	iStep->INFO_PRINTF1(testInfo);
	
	const TInt pointsizes = 9;
	static TInt pointsize[pointsizes] = { 8, 10, 12, 14, 16, 20, 24, 48, 200 };
	TInt i = 0;
	for (i = 0; i < (pointsizes-2); i++)
		{
		TestFontDisplayAtSize(support, pointsize[i], i & 1);
		}
		
	TestFontMetricsDiagram(support, pointsize[i], i & 1);
	i++;
	
	TestFontHugeCharacters(support, pointsize[i], i & 1);
	}
	
void CTestHarness::TestFontDisplayAtSize(TTypefaceSupport& aSupport, TInt aPointSize, TBool aItalic)
	{
	TBuf<50> testInfo;
	testInfo.Append(_L("Testing font size: "));
	TBuf<10> num;
	num.Num(aPointSize);
	testInfo.Append(num);
	iStep->INFO_PRINTF1(testInfo);
	
	TFontSpec fs;

	fs.iTypeface = aSupport.iTypeface;
	fs.iHeight = aPointSize * 20;
	if (aItalic)
		{
		fs.iFontStyle.SetPosture(EPostureItalic);
		}
	iTestFont = NULL;
	TInt error = iDev->GetNearestFontToDesignHeightInTwips((CFont*&)iTestFont,fs);
	if (error)
		User::Panic(_L("Could not create font"),error);
	if (!iTestFont->IsOpenFont())
		{
		iDev->ReleaseFont(iTestFont);
		iTestFont = NULL;
		return;
		}
	PrintMetrics();
	DisplayFont();
	iDev->ReleaseFont(iTestFont);
	}

void CTestHarness::TestFontMetricsDiagram(TTypefaceSupport& aSupport, TInt aPointSize, TBool aItalic)
	{
	TBuf<50> testInfo;
	testInfo.Append(_L("Testing font metrics: "));
	TBuf<10> num;
	num.Num(aPointSize);
	testInfo.Append(num);
	iStep->INFO_PRINTF1(testInfo);
	
	TFontSpec fs;
	fs.iTypeface = aSupport.iTypeface;
	fs.iHeight = aPointSize * 20;
	if (aItalic)
		{
		fs.iFontStyle.SetPosture(EPostureItalic);
		}
	iTestFont = NULL;
	TInt error = iDev->GetNearestFontToDesignHeightInTwips((CFont*&)iTestFont,fs);
	if (error)
		User::Panic(_L("Could not create font"),error);
	if (!iTestFont->IsOpenFont())
		{
		iDev->ReleaseFont(iTestFont);
		iTestFont = NULL;
		return;
		}
	PrintMetrics();
	DrawMetricsDiagram();
	iDev->ReleaseFont(iTestFont);
	}
	
void CTestHarness::TestFontHugeCharacters(TTypefaceSupport& aSupport, TInt aPointSize, TBool aItalic)
	{
	TBuf<50> testInfo;
	testInfo.Append(_L("Testing huge characters: "));
	TBuf<10> num;
	num.Num(aPointSize);
	testInfo.Append(num);
	iStep->INFO_PRINTF1(testInfo);
	
	TFontSpec fs;
	fs.iTypeface = aSupport.iTypeface;
	fs.iHeight = aPointSize * 20;
	if (aItalic)
		fs.iFontStyle.SetPosture(EPostureItalic);
	iTestFont = NULL;
	TInt error = iDev->GetNearestFontToDesignHeightInTwips((CFont*&)iTestFont,fs);
	if (error)
		User::Panic(_L("Could not create font"),error);
	if (!iTestFont->IsOpenFont())
		{
		iDev->ReleaseFont(iTestFont);
		iTestFont = NULL;
		return;
		}
	PrintMetrics();
	DrawHugeCharacters();
	iDev->ReleaseFont(iTestFont);
	}
	
static void Hellenize(TDes16& text)
	{
	static const TUint16* roman = (TUint16*)(L"ABGDEZJQIKLMNXOPRVSTUFHCW");
	
	for (int i = 0; i < text.Length(); i++)
		for (int j = 0; j < 25; j++)
			if (roman[j] == text[i])
				{
				text[i] = (TUint16)(0x391 + j);
				break;
				}
			else if (roman[j] + 32 == text[i])
				{
				text[i] = (TUint16)(0x3B1 + j);
				break;
				}
	}

void CTestHarness::PrintMetrics(TBool isClear)
	{
	if(isClear)
		{
		Clear();	
		}
	
	UseFont(iStandardFont);
	TFontSpec fs = iTestFont->FontSpecInTwips();
	TOpenFontMetrics metrics;
	iTestFont->GetFontMetrics(metrics);
	TOpenFontFaceAttrib attrib;
	iTestFont->GetFaceAttrib(attrib);
	TPtrC name = attrib.LocalFullName();
	iBuffer.Format(_L("%S %dpt size=%d ascent=%d descent=%d maxheight=%d maxdepth=%d maxwidth=%d"),
				  &name,(fs.iHeight + 10) / 20,metrics.Size(),metrics.Ascent(),metrics.Descent(),
				  metrics.MaxHeight(),metrics.MaxDepth(),metrics.MaxWidth());
	Print(iBuffer);
	}

void CTestHarness::DisplayFont()
	{
	UseFont(iStandardFont);
	TOpenFontFaceAttrib attrib;
	iTestFont->GetFaceAttrib(attrib);
	iBuffer.SetLength(0);
	if (attrib.HasLatin())
		iBuffer.Append(_L("Latin, "));			  
	if (attrib.HasGreek())
		iBuffer.Append(_L("Greek, "));			  
	if (attrib.HasCyrillic())
		iBuffer.Append(_L("Cyrillic, "));			  
	if (attrib.HasKana())
		iBuffer.Append(_L("Kana (Japanese syllabaries), "));
	if (attrib.HasHangul())
		iBuffer.Append(_L("Hangul (Korean writing system), "));
	if (attrib.HasCJK())
		iBuffer.Append(_L("CJK (Chinese, Japanese & Korean ideographs), "));
	if (iBuffer.Length() == 0)
		iBuffer.Append(_L("(character sets unknown)"));
	else
		iBuffer.SetLength(iBuffer.Length() - 2);
	Print(iBuffer);

	UseFont(iTestFont);

	// Benchmark the rasterization of Ascii printable characters.
	const int count = 126 - 33 + 1;
	iBuffer.SetLength(count);
	int i;
	for (i = 0; i < count; i++)
		iBuffer[i] = (TText)(i + 33);
	TTime start, end;
	start.HomeTime();
	iTestFont->RawTextWidthInPixels(iBuffer);
	end.HomeTime();
	TTimeIntervalMicroSeconds time64 = end.MicroSecondsFrom(start);
	int time = I64INT(time64.Int64());

	start.HomeTime();
	for (i = 0; i < 100; i++)
		iTestFont->RawTextWidthInPixels(iBuffer);
	end.HomeTime();
	time64 = end.MicroSecondsFrom(start);
	int overhead = I64INT(time64.Int64()) / 100;
	time -= overhead;
	if (time <= 0)
		time = 1;

	iBuffer.Format(_L("%d characters rasterized per second"),
				  count * 1000000 / time);
	Print(iBuffer);
	iBuffer.UpperCase();
	Print(iBuffer);
	iBuffer = _L("sm\xf6rg\xe5sbord soup\xe7on na\xefvet\xe9");
	Print(iBuffer);
	iBuffer.UpperCase();
	Print(iBuffer);

	if (attrib.HasGreek())
		{
		iBuffer = _L("agjwmetrjsewv mjdeiv eisitw");
		Hellenize(iBuffer);
		Print(iBuffer);
		iBuffer.UpperCase();
		Print(iBuffer);
		}
	if (attrib.HasCyrillic())
		{
		iBuffer.SetLength(32);
		for (int i = 0; i < 32; i++)
			iBuffer[i] = (TText)(0x430 + i);
		Print(iBuffer);
		iBuffer.UpperCase();
		Print(iBuffer);
		}
	if (attrib.HasKana())
		{
		iBuffer.SetLength(32);
		int i;
		for (i = 0; i < 32; i++)
			iBuffer[i] = (TText)(0x3041 + i);
		Print(iBuffer);
		for (i = 0; i < iBuffer.Length(); i++)
			iBuffer[i] = (TText)User::Fold(iBuffer[i],TChar::EFoldKana);	// fold to katakana
		Print(iBuffer);
		}
	if (attrib.HasHangul())
		{
		iBuffer.SetLength(32);
		for (int i = 0; i < 32; i++)
			iBuffer[i] = (TText)(0x3131 + i);
		Print(iBuffer);
		}

	// Display characters from 4e00 that are found in all 6 encodings given in the Unicode table.
	if (attrib.HasCJK())
		{
		iBuffer.SetLength(15);
		iBuffer[0] = 0x4e00;
		iBuffer[1] = 0x4e01;
		iBuffer[2] = 0x4e03;
		iBuffer[3] = 0x4e07;
		iBuffer[4] = 0x4e08;
		iBuffer[5] = 0x4e09;
		iBuffer[6] = 0x4e0a;
		iBuffer[7] = 0x4e0b;
		iBuffer[8] = 0x4e0d;
		iBuffer[9] = 0x4e11;
		iBuffer[10] = 0x4e14;
		iBuffer[11] = 0x4e15;
		iBuffer[12] = 0x4e16;
		iBuffer[13] = 0x4e18;
		iBuffer[14] = 0x4e19;
		Print(iBuffer);
		}

	iGc->DiscardFont();
	}

void CTestHarness::DrawCharMetrics(TPoint& aPos,TInt aCode)
	{
	TOpenFontCharMetrics metrics;
	const TUint8* bytes;
	TSize size;
	iTestFont->GetCharacterData(aCode,metrics,bytes,size);
	TRect r;
	r.iTl = aPos;
	r.iTl.iX += metrics.HorizBearingX();
	r.iTl.iY += iBaseLine;
	r.iTl.iY -= metrics.HorizBearingY();
	r.iBr = r.iTl + TSize(metrics.Width(),metrics.Height());
	iGc->DrawRect(r);
	aPos.iX += metrics.HorizAdvance();
	}

void CTestHarness::DrawMetricsDiagram()
	{
	UseFont(iTestFont);
	iBuffer = _L("Dig my crazy metrics!");
	iPrintPos.iX += 8;
	TPoint pos = iPrintPos;
	Print(iBuffer);
	TPoint baseline_start = pos;
	baseline_start.iY += iBaseLine;
	for (int i = 0; i < iBuffer.Length(); i++)
		DrawCharMetrics(pos,iBuffer[i]);
	TPoint baseline_end = pos;
	baseline_end.iY += iBaseLine;
	iGc->DrawLine(baseline_start,baseline_end);
	iGc->DiscardFont();
	//iTest.Getch();
	}

void CTestHarness::DrawHugeCharacters()
	{
	UseFont(iTestFont);
	iBuffer = _L("ABC123");
	TOpenFontCharMetrics metrics;
	const TUint8* bytes;
	TSize size;
	iTestFont->GetCharacterData('A',metrics,bytes,size);
	iBaseLine = metrics.HorizBearingY();;
	Print(iBuffer);
	iGc->DiscardFont();
	//iTest.Getch();
	}

void CTestHarness::Clear()
	{
	TRect r(iDev->SizeInPixels());
	iGc->Clear(r);
	iPrintPos = TPoint(0,0);
	}

void CTestHarness::Print(const TDesC& aText)
	{
	TPoint p = iPrintPos;
	p.iY += iBaseLine;
	iGc->DrawText(aText,p);
	iPrintPos.iX = 0;
	iPrintPos.iY += iLineHeight;
	}

void CTestHarness::UseFont(CFbsFont* aFont)
	{
	iGc->UseFont(aFont);
	TOpenFontMetrics metrics;
	aFont->GetFontMetrics(metrics);
	iBaseLine = metrics.MaxHeight();
	iLineHeight = metrics.MaxHeight() + metrics.MaxDepth();
	}

/*
@SYMTestCaseID GRAPHICS-FREETYPE-0001
@SYMTestCaseDesc Supplementary characters rasterization Test
@SYMTestPriority High
@SYMTestActions  This test checks that the free type rasterizer rasterizes supplementary characters 
properly.
It prints the descriptor containing supplementary characters with 2 Fonts. One font does not support
supplementary characters and the other does.
@SYMTestExpectedResults This step should be executed without error and the user should be able to see the 
above descriptor is displayed as a square with the first font and in the correct shape with the other font.
@SYMDEF DEF120018 Security issue in FreeType rasterizer! Supplementary characters rendered as BMP
*/
void CTestHarness::SurrogateRasterizedTestsL()
	{
	// The literal descriptor consisting of common letter 'A' and Supplementary character 0x20000
	// which is represented by surrogate pair 0xD840 0xDC00
	_LIT16(KSurrogatePairCode, "A\xD840\xDC00");
	
	// Testing Fonts: DejaVu Sans Condensed does not support Supplementary character.
	// Test2 only supports letter "A" and Supplementary character 0x20000
	TFontSpec fsDejaVuSans(_L("DejaVu Sans Condensed"), 16 * 20);
	TFontSpec fsTest2(_L("Test2"), 16 * 20);
	
	Clear();

	// Prints test descriptor with font SwissA
	iTestFont = NULL;
	TInt error = iDev->GetNearestFontToDesignHeightInTwips((CFont*&)iTestFont,fsDejaVuSans);
	iStep->TESTL(KErrNone == error);
	
	if (!iTestFont->IsOpenFont())
		{
		iDev->ReleaseFont(iTestFont);
		iTestFont = NULL;
		User::Leave(KErrGeneral);
		}
	PrintMetrics(EFalse);
	
	UseFont(iTestFont);
	Print(KSurrogatePairCode);
	iDev->ReleaseFont(iTestFont);

	// Prints test descriptor with font Test2
	iTestFont = NULL;
	error = iDev->GetNearestFontToDesignHeightInTwips((CFont*&)iTestFont,fsTest2);
	iStep->TESTL(KErrNone == error);
	
	if (!iTestFont->IsOpenFont())
		{
		iDev->ReleaseFont(iTestFont);
		iTestFont = NULL;
		User::Leave(KErrGeneral);
		}
	PrintMetrics(EFalse);

	UseFont(iTestFont);
	Print(KSurrogatePairCode);
	iDev->ReleaseFont(iTestFont);
	iTestFont = NULL;
	
//	User::After(10000000);
	}

