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
#include <e32test.h>
#include <e32cons.h>
#include <e32base.h>
#include <e32svr.h>

#include <f32file.h>

#include "NumberConversion.h"

RTest test(_L("NumberConversion Test"));
CConsoleBase* console;


void DriveEngineL();
void SetupConsoleL();

const TInt KCalypsoConsoleHeight = 15;
const TInt KCalypsoConsoleWidth = 20;


void RunTestsL();


_LIT(KStringToNumber1, "123");	// plain western
_LIT(KStringToNumber2, "\x0661\x0662\x0663");  // plain arabic
_LIT(KStringToNumber3, "\x06F1\x06F2\x06F3");	// plain eastern-arabic
_LIT(KStringToNumber4, "12\x0663");	// mixed western/arabic

/**
* Work around for DEF063276.
* This literal is now loaded from a z:\test\data\KStringToNumber5.dat
* Bullseye Coverage corrupts this literal, to avoid this it is stored in a file as to not be touched by Bullseye Coverage.
*/

//_LIT(KStringToNumber5, "1\x06F2\x0033");	// mixed western/eastern-arabic
_LIT(KStringToNumber6, "\x0661\x0662\x06F3");	// mixed arabic/eastern-arabic
_LIT(KStringToNumber7, "1\x0662\x06F3");	// mixed western/arabic/eastern-arabic
_LIT(KStringToNumber8, "\x0967\x0968\x0969"); // plain devanagari
_LIT(KStringToNumber9, "\x0E51\x0E52\x0E53"); // plain thai
_LIT(KStringToNumber10, "1\x0968\x0969"); // mixed western/devanagari
_LIT(KStringToNumber11, "1\x0E52\x0E53"); // mixed western/thai
_LIT(KStringToNumber12, "\x0967\x0E52\x0E53"); // mixed devanagari/thai
_LIT(KStringToNumber13, "1\x0E52\x0969"); // mixed western/thai/devanagari
_LIT(KStringToNumber14, "xxx");	// unknown symbols

// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KStringToNumber15, "\x09E7\x09E8\x09E9"); // plain bengali
_LIT(KStringToNumber16, "\x0A67\x0A68\x0A69"); // plain gurmukhi
_LIT(KStringToNumber17, "\x0AE7\x0AE8\x0AE9"); // plain gujarati
_LIT(KStringToNumber18, "\x0B67\x0B68\x0B69"); // plain oriya
_LIT(KStringToNumber19, "\x0BE7\x0BE8\x0BE9"); // plain tamil
_LIT(KStringToNumber20, "\x0C67\x0C68\x0C69"); // plain telegu
_LIT(KStringToNumber21, "\x0CE7\x0CE8\x0CE9"); // plain kannada
_LIT(KStringToNumber22, "\x0D67\x0D68\x0D69"); // plain malayalam
_LIT(KStringToNumber23, "\x0ED1\x0ED2\x0ED3"); // plain lao
_LIT(KStringToNumber24, "\x0F21\x0F22\x0F32"); // plain tibetan
_LIT(KStringToNumber25, "\x01041\x01042\x01043"); // plain mayanmar
_LIT(KStringToNumber26, "\x017E1\x017E2\x017E3"); // plain khmer
_LIT(KStringToNumber27, "1\x0968\x0969"); // mixed western/bengali
_LIT(KStringToNumber28, "\x0CE7\x0E52\x0E53"); // mixed kannada/thai
_LIT(KStringToNumber29, "1\x0E52\x0969"); // mixed western/thai/tibetan
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

_LIT(KPositionOfNextNumber1, "test1");	//western
_LIT(KPositionOfNextNumber2, "testa\x0661");	//arabic
_LIT(KPositionOfNextNumber3, "testaa\x06F1");	//eastern arabic
_LIT(KPositionOfNextNumber4, "testaaa\x0967");	// devanagari
_LIT(KPositionOfNextNumber5, "testaaaa\x0E51");	// thai
_LIT(KPositionOfNextNumber6, "test");	// none

// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KPositionOfNextNumber7, "testaaaaa\x09E7");	// bengali
_LIT(KPositionOfNextNumber8, "testaaaaaa\x0A67");	// gurmukhi
_LIT(KPositionOfNextNumber9, "testaaaaaaa\x0AE7");	// gujarati
_LIT(KPositionOfNextNumber10, "testaaaaaaaa\x0B67");	// oriya
_LIT(KPositionOfNextNumber11, "testaaaaaaaaa\x0BE7");	// tamil
_LIT(KPositionOfNextNumber12, "testaaaaaaaaaa\x0C67");	// telegu
_LIT(KPositionOfNextNumber13, "testaaaaaaaaaaa\x0CE7");	// kannada
_LIT(KPositionOfNextNumber14, "testaaaaaaaaaaaa\x0D67");	// malayalam
_LIT(KPositionOfNextNumber15, "testaaaaaaaaaaaaa\x0ED1");	// lao
_LIT(KPositionOfNextNumber16, "testaaaaaaaaaaaaaa\x0F20");	// tibetan
_LIT(KPositionOfNextNumber17, "testaaaaaaaaaaaaaaa\x01041");	// mayanmar
_LIT(KPositionOfNextNumber18, "testaaaaaaaaaaaaaaaa\x017E1");	// khmer



_LIT(KFormatNumber1, "123");	// plain western
_LIT(KFormatNumber2, "1230");	// plain western
_LIT(KFormatNumber3, "\x0661\x0662\x0663");  // plain arabic
_LIT(KFormatNumber4, "\x06F1\x06F2\x06F3");	// plain eastern-arabic
_LIT(KFormatNumber5, "\x0967\x0968\x0969");	// plain devanagari
_LIT(KFormatNumber6, "\x0E51\x0E52\x0E53");	// plain thai

// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KFormatNumber7, "\x09E7\x09E8\x09E9"); // plain bengali
_LIT(KFormatNumber8, "\x0A67\x0A68\x0A69"); // plain gurmukhi
_LIT(KFormatNumber9, "\x0AE7\x0AE8\x0AE9"); // plain gujarati
_LIT(KFormatNumber10, "\x0B67\x0B68\x0B69"); // plain oriya
_LIT(KFormatNumber11, "\x0BE7\x0BE8\x0BE9"); // plain tamil
_LIT(KFormatNumber12, "\x0C67\x0C68\x0C69"); // plain telegu
_LIT(KFormatNumber13, "\x0CE7\x0CE8\x0CE9"); // plain kannada
_LIT(KFormatNumber14, "\x0D67\x0D68\x0D69"); // plain malayalam
_LIT(KFormatNumber15, "\x0ED1\x0ED2\x0ED3"); // plain lao
_LIT(KFormatNumber16, "\x0F21\x0F22\x0F23"); // plain tibetan
_LIT(KFormatNumber17, "\x01041\x01042\x01043"); // plain mayanmar
_LIT(KFormatNumber18, "\x017E1\x017E2\x017E3"); // plain khmer
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

_LIT(KAppendFormatNumber1, "test123");	// plain western
_LIT(KAppendFormatNumber2, "test1231230");	// plain western
_LIT(KAppendFormatNumber3, "test1231230\x0661\x0662\x0663");  // plain arabic
_LIT(KAppendFormatNumber4, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3");	// plain eastern-arabic
_LIT(KAppendFormatNumber5, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969");	// plain devanagari
_LIT(KAppendFormatNumber6, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53");	// plain thai

// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KAppendFormatNumber7, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9");	// plain bengali
_LIT(KAppendFormatNumber8, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69");	// plain gurmukhi
_LIT(KAppendFormatNumber9, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9");	// plain gujarati
_LIT(KAppendFormatNumber10, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69");	// plain oriya
_LIT(KAppendFormatNumber11, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69\x0BE7\x0BE8\x0BE9");	// plain tamil
_LIT(KAppendFormatNumber12, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69\x0BE7\x0BE8\x0BE9\x0C67\x0C68\x0C69");	// plain telegu
_LIT(KAppendFormatNumber13, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69\x0BE7\x0BE8\x0BE9\x0C67\x0C68\x0C69\x0CE7\x0CE8\x0CE9");	// plain kannada
_LIT(KAppendFormatNumber14, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69\x0BE7\x0BE8\x0BE9\x0C67\x0C68\x0C69\x0CE7\x0CE8\x0CE9\x0D67\x0D68\x0D69");	// plain malayalam
_LIT(KAppendFormatNumber15, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69\x0BE7\x0BE8\x0BE9\x0C67\x0C68\x0C69\x0CE7\x0CE8\x0CE9\x0D67\x0D68\x0D69\x0ED1\x0ED2\x0ED3");	// plain lao
_LIT(KAppendFormatNumber16, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69\x0BE7\x0BE8\x0BE9\x0C67\x0C68\x0C69\x0CE7\x0CE8\x0CE9\x0D67\x0D68\x0D69\x0ED1\x0ED2\x0ED3\x0F21\x0F22\x0F23");	// plain tibetan
_LIT(KAppendFormatNumber17, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69\x0BE7\x0BE8\x0BE9\x0C67\x0C68\x0C69\x0CE7\x0CE8\x0CE9\x0D67\x0D68\x0D69\x0ED1\x0ED2\x0ED3\x0F21\x0F22\x0F23\x01041\x01042\x01043");	// plain mayanmar
_LIT(KAppendFormatNumber18, "test1231230\x0661\x0662\x0663\x06F1\x06F2\x06F3\x0967\x0968\x0969\x0E51\x0E52\x0E53\x09E7\x09E8\x09E9\x0A67\x0A68\x0A69\x0AE7\x0AE8\x0AE9\x0B67\x0B68\x0B69\x0BE7\x0BE8\x0BE9\x0C67\x0C68\x0C69\x0CE7\x0CE8\x0CE9\x0D67\x0D68\x0D69\x0ED1\x0ED2\x0ED3\x0F21\x0F22\x0F23\x01041\x01042\x01043\x017E1\x017E2\x017E3");	// plain khmer
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co


_LIT(KConvertDigits0, "xyz0000123xyz0");  // western to arabic / eastern-arabic / devanagari / thai / bengali / gurmukhi
										  // gujarati / oriya / tamil / telegu / kannada / malayalam / lao / tibetan
										  // mayanmar / khmer
_LIT(KConvertDigits0Result1, "xyz\x0660\x0660\x0660\x0660\x0661\x0662\x0663xyz\x0660");
_LIT(KConvertDigits0Result2, "xyz\x06F0\x06F0\x06F0\x06F0\x06F1\x06F2\x06F3xyz\x06F0");
_LIT(KConvertDigits0Result3, "xyz\x0966\x0966\x0966\x0966\x0967\x0968\x0969xyz\x0966");
_LIT(KConvertDigits0Result4, "xyz\x0E50\x0E50\x0E50\x0E50\x0E51\x0E52\x0E53xyz\x0E50");
// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KConvertDigits0Result5, "xyz\x09E6\x09E6\x09E6\x09E6\x09E7\x09E8\x09E9xyz\x09E6");
_LIT(KConvertDigits0Result6, "xyz\x0A66\x0A66\x0A66\x0A66\x0A67\x0A68\x0A69xyz\x0A66");
_LIT(KConvertDigits0Result7, "xyz\x0AE6\x0AE6\x0AE6\x0AE6\x0AE7\x0AE8\x0AE9xyz\x0AE6");
_LIT(KConvertDigits0Result8, "xyz\x0B66\x0B66\x0B66\x0B66\x0B67\x0B68\x0B69xyz\x0B66");
//_LIT(KConvertDigits0Result9, "xyz\x0BE6\x0BE6\x0BE6\x0BE6\x0BE7\x0BE8\x0BE9xyz\x0BE6");
_LIT(KConvertDigits0Result10, "xyz\x0C66\x0C66\x0C66\x0C66\x0C67\x0C68\x0C69xyz\x0C66");
_LIT(KConvertDigits0Result11, "xyz\x0CE6\x0CE6\x0CE6\x0CE6\x0CE7\x0CE8\x0CE9xyz\x0CE6");
_LIT(KConvertDigits0Result12, "xyz\x0D66\x0D66\x0D66\x0D66\x0D67\x0D68\x0D69xyz\x0D66");
_LIT(KConvertDigits0Result13, "xyz\x0ED0\x0ED0\x0ED0\x0ED0\x0ED1\x0ED2\x0ED3xyz\x0ED0");
_LIT(KConvertDigits0Result14, "xyz\x0F20\x0F20\x0F20\x0F20\x0F21\x0F22\x0F23xyz\x0F20");
_LIT(KConvertDigits0Result15, "xyz\x01040\x01040\x01040\x01040\x01041\x01042\x01043xyz\x01040");
_LIT(KConvertDigits0Result16, "xyz\x017E0\x017E0\x017E0\x017E0\x017E1\x017E2\x017E3xyz\x017E0");
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

_LIT(KConvertDigits1, "xyz123xyz");  // western to arabic / eastern-arabic / devanagari / thai / bengali / gurmukhi
									 // gujarati / oriya / tamil / telegu / kannada / malayalam / lao / tibetan
									 // mayanmar / khmer 
_LIT(KConvertDigits1Result1, "xyz\x0661\x0662\x0663xyz");
_LIT(KConvertDigits1Result2, "xyz\x06F1\x06F2\x06F3xyz");
_LIT(KConvertDigits1Result3, "xyz\x0967\x0968\x0969xyz");
_LIT(KConvertDigits1Result4, "xyz\x0E51\x0E52\x0E53xyz");
// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KConvertDigits1Result5, "xyz\x09E7\x09E8\x09E9xyz");
_LIT(KConvertDigits1Result6, "xyz\x0A67\x0A68\x0A69xyz");
_LIT(KConvertDigits1Result7, "xyz\x0AE7\x0AE8\x0AE9xyz");
_LIT(KConvertDigits1Result8, "xyz\x0B67\x0B68\x0B69xyz");
_LIT(KConvertDigits1Result9, "xyz\x0BE7\x0BE8\x0BE9xyz");
_LIT(KConvertDigits1Result10, "xyz\x0C67\x0C68\x0C69xyz");
_LIT(KConvertDigits1Result11, "xyz\x0CE7\x0CE8\x0CE9xyz");
_LIT(KConvertDigits1Result12, "xyz\x0D67\x0D68\x0D69xyz");
_LIT(KConvertDigits1Result13, "xyz\x0ED1\x0ED2\x0ED3xyz");
_LIT(KConvertDigits1Result14, "xyz\x0F21\x0F22\x0F23xyz");
_LIT(KConvertDigits1Result15, "xyz\x01041\x01042\x01043xyz");
_LIT(KConvertDigits1Result16, "xyz\x017E1\x017E2\x017E3xyz");
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

_LIT(KConvertDigits2, "xyz\x0661\x0662\x0663xyz");  // arabic to western / eastern-arabic/ devanagari/ thai / bengali / gurmukhi
									 				// gujarati / oriya / tamil / telegu / kannada / malayalam / lao / tibetan
									 				// mayanmar / khmer 
_LIT(KConvertDigits2Result1, "xyz123xyz");
_LIT(KConvertDigits2Result2, "xyz\x06F1\x06F2\x06F3xyz");
_LIT(KConvertDigits2Result3, "xyz\x0967\x0968\x0969xyz");
_LIT(KConvertDigits2Result4, "xyz\x0E51\x0E52\x0E53xyz");
// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KConvertDigits2Result5, "xyz\x09E7\x09E8\x09E9xyz");
_LIT(KConvertDigits2Result6, "xyz\x0A67\x0A68\x0A69xyz");
_LIT(KConvertDigits2Result7, "xyz\x0AE7\x0AE8\x0AE9xyz");
_LIT(KConvertDigits2Result8, "xyz\x0B67\x0B68\x0B69xyz");
_LIT(KConvertDigits2Result9, "xyz\x0BE7\x0BE8\x0BE9xyz");
_LIT(KConvertDigits2Result10, "xyz\x0C67\x0C68\x0C69xyz");
_LIT(KConvertDigits2Result11, "xyz\x0CE7\x0CE8\x0CE9xyz");
_LIT(KConvertDigits2Result12, "xyz\x0D67\x0D68\x0D69xyz");
_LIT(KConvertDigits2Result13, "xyz\x0ED1\x0ED2\x0ED3xyz");
_LIT(KConvertDigits2Result14, "xyz\x0F21\x0F22\x0F23xyz");
_LIT(KConvertDigits2Result15, "xyz\x01041\x01042\x01043xyz");
_LIT(KConvertDigits2Result16, "xyz\x017E1\x017E2\x017E3xyz");
//End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

_LIT(KConvertDigits3, "xyz\x06F1\x06F2\x06F3xyz");	// eastern arabic to western / arabic / devenagari/ thai / bengali / gurmukhi
									 				// gujarati / oriya / tamil / telegu / kannada / malayalam / lao / tibetan
									 				// mayanmar / khmer 
_LIT(KConvertDigits3Result1, "xyz123xyz");
_LIT(KConvertDigits3Result2, "xyz\x0661\x0662\x0663xyz");
_LIT(KConvertDigits3Result3, "xyz\x0967\x0968\x0969xyz");
_LIT(KConvertDigits3Result4, "xyz\x0E51\x0E52\x0E53xyz");
// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KConvertDigits3Result5, "xyz\x09E7\x09E8\x09E9xyz");
_LIT(KConvertDigits3Result6, "xyz\x0A67\x0A68\x0A69xyz");
_LIT(KConvertDigits3Result7, "xyz\x0AE7\x0AE8\x0AE9xyz");
_LIT(KConvertDigits3Result8, "xyz\x0B67\x0B68\x0B69xyz");
_LIT(KConvertDigits3Result9, "xyz\x0BE7\x0BE8\x0BE9xyz");
_LIT(KConvertDigits3Result10, "xyz\x0C67\x0C68\x0C69xyz");
_LIT(KConvertDigits3Result11, "xyz\x0CE7\x0CE8\x0CE9xyz");
_LIT(KConvertDigits3Result12, "xyz\x0D67\x0D68\x0D69xyz");
_LIT(KConvertDigits3Result13, "xyz\x0ED1\x0ED2\x0ED3xyz");
_LIT(KConvertDigits3Result14, "xyz\x0F21\x0F22\x0F23xyz");
_LIT(KConvertDigits3Result15, "xyz\x01041\x01042\x01043xyz");
_LIT(KConvertDigits3Result16, "xyz\x017E1\x017E2\x017E3xyz");
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co


_LIT(KConvertDigits4, "xyz123xyz123xyz");  // western to arabic / eastern-arabic
_LIT(KConvertDigits4Result1, "xyz\x0661\x0662\x0663xyz\x0661\x0662\x0663xyz");
_LIT(KConvertDigits4Result2, "xyz\x06F1\x06F2\x06F3xyz\x06F1\x06F2\x06F3xyz");
_LIT(KConvertDigits5, "xyz\x0661\x0662\x0663xyz\x0661\x0662\x0663xyz");  // arabic to western / eastern-arabic
_LIT(KConvertDigits5Result1, "xyz123xyz123xyz");
_LIT(KConvertDigits5Result2, "xyz\x06F1\x06F2\x06F3xyz\x06F1\x06F2\x06F3xyz");
_LIT(KConvertDigits6, "xyz\x06F1\x06F2\x06F3xyz\x06F1\x06F2\x06F3xyz");	// eastern arabic to western / arabic
_LIT(KConvertDigits6Result1, "xyz123xyz123xyz");
_LIT(KConvertDigits6Result2, "xyz\x0661\x0662\x0663xyz\x0661\x0662\x0663xyz");

_LIT(KConvertDigits7,"xyzxyz");

_LIT(KConvertDigits8, "123");  // western to arabic simple 
_LIT(KConvertDigits8Result1, "\x0661\x0662\x0663");

_LIT(KConvertDigits9, "35840-1234567890123");
_LIT(KConvertDigits9Result1, "35840-1234567890123");


// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KConvertDigits10, "xyz\x0967\x0968\x0969xyz");	// devanagari to western / arabic / eastern arabic / thai / bengali / gurmukhi
									 				// gujarati / oriya / tamil / telegu / kannada / malayalam / lao / tibetan
									 				// mayanmar / khmer
_LIT(KConvertDigits10Result1, "xyz123xyz");
_LIT(KConvertDigits10Result2, "xyz\x0661\x0662\x0663xyz");
_LIT(KConvertDigits10Result3, "xyz\x06F1\x06F2\x06F3xyz");
_LIT(KConvertDigits10Result4, "xyz\x0E51\x0E52\x0E53xyz");
_LIT(KConvertDigits10Result5, "xyz\x09E7\x09E8\x09E9xyz");
_LIT(KConvertDigits10Result6, "xyz\x0A67\x0A68\x0A69xyz");
_LIT(KConvertDigits10Result7, "xyz\x0AE7\x0AE8\x0AE9xyz");
_LIT(KConvertDigits10Result8, "xyz\x0B67\x0B68\x0B69xyz");
_LIT(KConvertDigits10Result9, "xyz\x0BE7\x0BE8\x0BE9xyz");
_LIT(KConvertDigits10Result10, "xyz\x0C67\x0C68\x0C69xyz");
_LIT(KConvertDigits10Result11, "xyz\x0CE7\x0CE8\x0CE9xyz");
_LIT(KConvertDigits10Result12, "xyz\x0D67\x0D68\x0D69xyz");
_LIT(KConvertDigits10Result13, "xyz\x0ED1\x0ED2\x0ED3xyz");
_LIT(KConvertDigits10Result14, "xyz\x0F21\x0F22\x0F23xyz");
_LIT(KConvertDigits10Result15, "xyz\x01041\x01042\x01043xyz");
_LIT(KConvertDigits10Result16, "xyz\x017E1\x017E2\x017E3xyz");




_LIT(KConvertDigits11, "xyz\x0CE7\x0CE8\x0CE9xyz");	// kannada to western / arabic / eastern arabic / devanagari /thai / bengali / gurmukhi
									 				// gujarati / oriya / tamil / telegu / malayalam / lao / tibetan
									 				// mayanmar / khmer
_LIT(KConvertDigits11Result1, "xyz123xyz");
_LIT(KConvertDigits11Result2, "xyz\x0661\x0662\x0663xyz");
_LIT(KConvertDigits11Result3, "xyz\x06F1\x06F2\x06F3xyz");
_LIT(KConvertDigits11Result4, "xyz\x0967\x0968\x0969xyz");
_LIT(KConvertDigits11Result5, "xyz\x0E51\x0E52\x0E53xyz");
_LIT(KConvertDigits11Result6, "xyz\x09E7\x09E8\x09E9xyz");
_LIT(KConvertDigits11Result7, "xyz\x0A67\x0A68\x0A69xyz");
_LIT(KConvertDigits11Result8, "xyz\x0AE7\x0AE8\x0AE9xyz");
_LIT(KConvertDigits11Result9, "xyz\x0B67\x0B68\x0B69xyz");
_LIT(KConvertDigits11Result10, "xyz\x0BE7\x0BE8\x0BE9xyz");
_LIT(KConvertDigits11Result11, "xyz\x0C67\x0C68\x0C69xyz");
_LIT(KConvertDigits11Result12, "xyz\x0D67\x0D68\x0D69xyz");
_LIT(KConvertDigits11Result13, "xyz\x0ED1\x0ED2\x0ED3xyz");
_LIT(KConvertDigits11Result14, "xyz\x0F21\x0F22\x0F23xyz");
_LIT(KConvertDigits11Result15, "xyz\x01041\x01042\x01043xyz");
_LIT(KConvertDigits11Result16, "xyz\x017E1\x017E2\x017E3xyz");



_LIT(KConvertDigits12, "xyz\x0AE7\x0AE8\x0AE9xyz");	// gujarati to western / arabic / eastern arabic / devanagari /thai / bengali / gurmukhi
									 				// oriya / tamil / telegu / kannada / malayalam / lao / tibetan
									 				// mayanmar / khmer
_LIT(KConvertDigits12Result1, "xyz123xyz");
_LIT(KConvertDigits12Result2, "xyz\x0661\x0662\x0663xyz");
_LIT(KConvertDigits12Result3, "xyz\x06F1\x06F2\x06F3xyz");
_LIT(KConvertDigits12Result4, "xyz\x0967\x0968\x0969xyz");
_LIT(KConvertDigits12Result5, "xyz\x0E51\x0E52\x0E53xyz");
_LIT(KConvertDigits12Result6, "xyz\x09E7\x09E8\x09E9xyz");
_LIT(KConvertDigits12Result7, "xyz\x0A67\x0A68\x0A69xyz");
_LIT(KConvertDigits12Result8, "xyz\x0B67\x0B68\x0B69xyz");
_LIT(KConvertDigits12Result9, "xyz\x0BE7\x0BE8\x0BE9xyz");
_LIT(KConvertDigits12Result10, "xyz\x0C67\x0C68\x0C69xyz");
_LIT(KConvertDigits12Result11, "xyz\x0CE7\x0CE8\x0CE9xyz");
_LIT(KConvertDigits12Result12, "xyz\x0D67\x0D68\x0D69xyz");
_LIT(KConvertDigits12Result13, "xyz\x0ED1\x0ED2\x0ED3xyz");
_LIT(KConvertDigits12Result14, "xyz\x0F21\x0F22\x0F23xyz");
_LIT(KConvertDigits12Result15, "xyz\x01041\x01042\x01043xyz");
_LIT(KConvertDigits12Result16, "xyz\x017E1\x017E2\x017E3xyz");

_LIT(KConvertDigits13, "xyz\x017E1\x017E2\x017E3xyz");	// khmer to mayanmar / tibetan / lao / malayalam / kannada / telugu / tamil / oriya 
													// gujarati / gurmukhi / bengali / thai / devanagari / eastern arabic / arabic / wetern

_LIT(KConvertDigits13Result1, "xyz123xyz");
_LIT(KConvertDigits13Result2, "xyz\x0661\x0662\x0663xyz");
_LIT(KConvertDigits13Result3, "xyz\x06F1\x06F2\x06F3xyz");
_LIT(KConvertDigits13Result4, "xyz\x0967\x0968\x0969xyz");
_LIT(KConvertDigits13Result5, "xyz\x0E51\x0E52\x0E53xyz");
_LIT(KConvertDigits13Result6, "xyz\x09E7\x09E8\x09E9xyz");
_LIT(KConvertDigits13Result7, "xyz\x0A67\x0A68\x0A69xyz");
_LIT(KConvertDigits13Result8, "xyz\x0AE7\x0AE8\x0AE9xyz");
_LIT(KConvertDigits13Result9, "xyz\x0B67\x0B68\x0B69xyz");
_LIT(KConvertDigits13Result10, "xyz\x0BE7\x0BE8\x0BE9xyz");
_LIT(KConvertDigits13Result11, "xyz\x0C67\x0C68\x0C69xyz");
_LIT(KConvertDigits13Result12, "xyz\x0CE7\x0CE8\x0CE9xyz");
_LIT(KConvertDigits13Result13, "xyz\x0D67\x0D68\x0D69xyz");
_LIT(KConvertDigits13Result14, "xyz\x0ED1\x0ED2\x0ED3xyz");
_LIT(KConvertDigits13Result15, "xyz\x0F21\x0F22\x0F23xyz");
_LIT(KConvertDigits13Result16, "xyz\x01041\x01042\x01043xyz");
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co




_LIT(KFormatString1,"%d");
_LIT(KFormatResult1a,"123");
_LIT(KFormatResult1b,"\x0661\x0662\x0663");
_LIT(KFormatResult1c,"\x06F1\x06F2\x06F3");
_LIT(KFormatResult1d,"\x0967\x0968\x0969");
_LIT(KFormatResult1e,"\x0E51\x0E52\x0E53");
// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KFormatResult1f, "\x09E7\x09E8\x09E9"); 
_LIT(KFormatResult1g, "\x0A67\x0A68\x0A69"); 
_LIT(KFormatResult1h, "\x0AE7\x0AE8\x0AE9"); 
_LIT(KFormatResult1i, "\x0B67\x0B68\x0B69"); 
_LIT(KFormatResult1j, "\x0BE7\x0BE8\x0BE9"); 
_LIT(KFormatResult1k, "\x0C67\x0C68\x0C69"); 
_LIT(KFormatResult1l, "\x0CE7\x0CE8\x0CE9"); 
_LIT(KFormatResult1m, "\x0D67\x0D68\x0D69"); 
_LIT(KFormatResult1n, "\x0ED1\x0ED2\x0ED3"); 
_LIT(KFormatResult1o, "\x0F21\x0F22\x0F23"); 
_LIT(KFormatResult1p, "\x01041\x01042\x01043"); 
_LIT(KFormatResult1q, "\x017E1\x017E2\x017E3"); 
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

_LIT(KFormatString2,"abc%dxyz");
_LIT(KFormatResult2a,"abc123xyz");
_LIT(KFormatResult2b,"abc\x0661\x0662\x0663xyz");
_LIT(KFormatResult2c,"abc\x06F1\x06F2\x06F3xyz");
_LIT(KFormatResult2d,"abc\x0967\x0968\x0969xyz");
_LIT(KFormatResult2e,"abc\x0E51\x0E52\x0E53xyz");
// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
_LIT(KFormatResult2f, "abc\x09E7\x09E8\x09E9xyz"); 
_LIT(KFormatResult2g, "abc\x0A67\x0A68\x0A69xyz"); 
_LIT(KFormatResult2h, "abc\x0AE7\x0AE8\x0AE9xyz"); 
_LIT(KFormatResult2i, "abc\x0B67\x0B68\x0B69xyz"); 
_LIT(KFormatResult2j, "abc\x0BE7\x0BE8\x0BE9xyz"); 
_LIT(KFormatResult2k, "abc\x0C67\x0C68\x0C69xyz"); 
_LIT(KFormatResult2l, "abc\x0CE7\x0CE8\x0CE9xyz"); 
_LIT(KFormatResult2m, "abc\x0D67\x0D68\x0D69xyz"); 
_LIT(KFormatResult2n, "abc\x0ED1\x0ED2\x0ED3xyz"); 
_LIT(KFormatResult2o, "abc\x0F21\x0F22\x0F23xyz"); 
_LIT(KFormatResult2p, "abc\x01041\x01042\x01043xyz"); 
_LIT(KFormatResult2q, "abc\x017E1\x017E2\x017E3xyz");
// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

_LIT(KFormatString3,"abc%dxyz%dxyz");
_LIT(KFormatResult3a,"abc123xyz456xyz");
_LIT(KFormatResult3b,"abc\x0661\x0662\x0663xyz\x0664\x0665\x0666xyz");
_LIT(KFormatResult3c,"abc\x06F1\x06F2\x06F3xyz\x06F4\x06F5\x06F6xyz");
_LIT(KFormatResult3d,"abc\x0967\x0968\x0969xyz\x096A\x096B\x096Cxyz");
_LIT(KFormatResult3e,"abc\x0E51\x0E52\x0E53xyz\x0E54\x0E55\x0E56xyz");

_LIT(KString, "xyz");
_LIT(KFormatString4,"abc%d%S%d%S");
_LIT(KFormatResult4a,"abc123xyz456xyz");
_LIT(KFormatResult4b,"abc\x0661\x0662\x0663xyz\x0664\x0665\x0666xyz");
_LIT(KFormatResult4c,"abc\x06F1\x06F2\x06F3xyz\x06F4\x06F5\x06F6xyz");

/**
* Utility for DEF063276 fix.
*/

_LIT(KStringToNumber5Filename, "z:\\test\\data\\KStringToNumber5.dat");

static void ReadDesc(TDes& aDes, const TDesC& aFilename, RFs& aFs)
	{
	RFile file;
	TInt err = file.Open(aFs, aFilename, EFileRead);
	test(err == KErrNone);
	CleanupClosePushL(file);
	
	TPtr8 ptr(reinterpret_cast<TUint8*>(const_cast<TUint16*>(aDes.Ptr())), aDes.MaxSize());
	err = file.Read(ptr);
	test(err == KErrNone);
	aDes.SetLength(ptr.Length() / sizeof(TText));
	CleanupStack::PopAndDestroy(&file);
	}


void TestStringToNumber()
	{
	TInt number = 0;
	TInt length = 0;
	TDigitType digitType = EDigitTypeUnknown;
	number = NumberConversion::ConvertFirstNumber(KStringToNumber1, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeWestern);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber2, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeArabicIndic);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber3, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeEasternArabicIndic);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber4, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeAllTypes);
	
	// Fix for DEF063276
	RFs fs;
	test(fs.Connect() == KErrNone);
	CleanupClosePushL(fs);
	
	TBuf<3> buf;
	ReadDesc(buf, KStringToNumber5Filename, fs);
	
	CleanupStack::PopAndDestroy(&fs);
	
	number = NumberConversion::ConvertFirstNumber(buf, length, digitType);
	// End fix
	
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeAllTypes);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber6, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeAllTypes);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber7, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeAllTypes);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber8, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeDevanagari);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber9, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeThai);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber10, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeAllTypes);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber11, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeAllTypes);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber12, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeAllTypes);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber13, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeAllTypes);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber14, length, digitType);
	test(length == 0);
	test(digitType == EDigitTypeUnknown);
	
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	number = NumberConversion::ConvertFirstNumber(KStringToNumber15, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeBengali);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber16, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeGurmukhi);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber17, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeGujarati);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber18, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeOriya);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber19, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeTamil);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber20, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeTelugu);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber21, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeKannada);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber22, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeMalayalam);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber23, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeLao);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber24, length, digitType);
	test(number == 138);
	test(length == 3);
	test(digitType == EDigitTypeTibetan);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber25, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeMayanmar);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber26, length, digitType);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeKhmer);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co


	// Test matching a single type only
	number = NumberConversion::ConvertFirstNumber(KStringToNumber1, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeWestern);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber2, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeArabicIndic);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber3, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeEasternArabicIndic);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber4, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 12);
	test(length == 2);
	test(digitType == EDigitTypeWestern);
	// Fix for DEF063276
	number = NumberConversion::ConvertFirstNumber(buf, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	// End fix

	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeWestern);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber6, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 12);
	test(length == 2);
	test(digitType == EDigitTypeArabicIndic);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber7, length, digitType,NumberConversion:: EMatchSingleTypeOnly);
	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeWestern);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber8, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeDevanagari);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber9, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeThai);
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	number = NumberConversion::ConvertFirstNumber(KStringToNumber15, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeBengali);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber16, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeGurmukhi);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber17, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeGujarati);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber18, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeOriya);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber19, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeTamil);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber20, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeTelugu);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber21, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeKannada);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber22, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeMalayalam);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber23, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeLao);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber24, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 138);
	test(length == 3);
	test(digitType == EDigitTypeTibetan);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber25, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeMayanmar);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber26, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 123);
	test(length == 3);
	test(digitType == EDigitTypeKhmer);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co	
	
	number = NumberConversion::ConvertFirstNumber(KStringToNumber10, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeWestern);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber11, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeWestern);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber12, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeDevanagari);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber13, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeWestern);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber14, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(length == 0);
	test(digitType == EDigitTypeUnknown);
	
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	number = NumberConversion::ConvertFirstNumber(KStringToNumber27, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeWestern);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber28, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeKannada);
	number = NumberConversion::ConvertFirstNumber(KStringToNumber29, length, digitType, NumberConversion::EMatchSingleTypeOnly);
	test(number == 1);
	test(length == 1);
	test(digitType == EDigitTypeWestern);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	}


void TestPositionOfNextNumber()
	{
	TDigitType type = EDigitTypeUnknown;
	TInt position = 0;
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber1, type);
	test(position == 4);
	test(type == EDigitTypeWestern);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber2, type);
	test(position == 5);
	test(type == EDigitTypeArabicIndic);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber3, type);
	test(position == 6);
	test(type == EDigitTypeEasternArabicIndic);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber4, type);
	test(position == 7);
	test(type == EDigitTypeDevanagari);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber5, type);
	test(position == 8);
	test(type == EDigitTypeThai);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber6, type);
	test(position == KErrNotFound);
	test(type == EDigitTypeUnknown);
	
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber7, type);
	test(position == 9);
	test(type == EDigitTypeBengali);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber8, type);
	test(position == 10);
	test(type == EDigitTypeGurmukhi);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber9, type);
	test(position == 11);
	test(type == EDigitTypeGujarati);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber10, type);
	test(position == 12);
	test(type == EDigitTypeOriya);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber11, type);
	test(position == 13);
	test(type == EDigitTypeTamil);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber12, type);
	test(position == 14);
	test(type == EDigitTypeTelugu);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber13, type);
	test(position == 15);
	test(type == EDigitTypeKannada);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber14, type);
	test(position == 16);
	test(type == EDigitTypeMalayalam);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber15, type);
	test(position == 17);
	test(type == EDigitTypeLao);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber16, type);
	test(position == 18);
	test(type == EDigitTypeTibetan);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber17, type);
	test(position == 19);
	test(type == EDigitTypeMayanmar);
	position = NumberConversion::PositionAndTypeOfNextNumber(KPositionOfNextNumber18, type);
	test(position == 20);
	test(type == EDigitTypeKhmer);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	
	
	}


void TestFormatNumber()
	{
	TBuf<80> text;
	NumberConversion::FormatNumber(text, 123, EDigitTypeWestern);
	test(text.Compare(KFormatNumber1) == 0);
	NumberConversion::FormatNumber(text, 1230, EDigitTypeWestern);
	test(text.Compare(KFormatNumber2) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeArabicIndic);
	test(text.Compare(KFormatNumber3) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeEasternArabicIndic);
	test(text.Compare(KFormatNumber4) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeDevanagari);
	test(text.Compare(KFormatNumber5) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeThai);
	test(text.Compare(KFormatNumber6) == 0);
	
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	NumberConversion::FormatNumber(text, 123, EDigitTypeBengali);
	test(text.Compare(KFormatNumber7) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeGurmukhi);
	test(text.Compare(KFormatNumber8) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeGujarati);
	test(text.Compare(KFormatNumber9) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeOriya);
	test(text.Compare(KFormatNumber10) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeTamil);
	test(text.Compare(KFormatNumber11) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeTelugu);
	test(text.Compare(KFormatNumber12) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeKannada);
	test(text.Compare(KFormatNumber13) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeMalayalam);
	test(text.Compare(KFormatNumber14) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeLao);
	test(text.Compare(KFormatNumber15) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeTibetan);
	test(text.Compare(KFormatNumber16) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeMayanmar);
	test(text.Compare(KFormatNumber17) == 0);
	NumberConversion::FormatNumber(text, 123, EDigitTypeKhmer);
	test(text.Compare(KFormatNumber18) == 0);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	
	}

void TestAppendFormatNumber()
	{
	TBuf<80> text(_L("test"));
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeWestern);
	test(text.Compare(KAppendFormatNumber1) == 0);
	NumberConversion::AppendFormatNumber(text, 1230, EDigitTypeWestern);
	test(text.Compare(KAppendFormatNumber2) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeArabicIndic);
	test(text.Compare(KAppendFormatNumber3) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeEasternArabicIndic);
	test(text.Compare(KAppendFormatNumber4) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeDevanagari);
	test(text.Compare(KAppendFormatNumber5) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeThai);
	test(text.Compare(KAppendFormatNumber6) == 0);
	
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeBengali);
	test(text.Compare(KAppendFormatNumber7) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeGurmukhi);
	test(text.Compare(KAppendFormatNumber8) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeGujarati);
	test(text.Compare(KAppendFormatNumber9) == 0);
	
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeOriya);
	test(text.Compare(KAppendFormatNumber10) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeTamil);
	test(text.Compare(KAppendFormatNumber11) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeTelugu);
	test(text.Compare(KAppendFormatNumber12) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeKannada);
	test(text.Compare(KAppendFormatNumber13) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeMalayalam);
	test(text.Compare(KAppendFormatNumber14) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeLao);
	test(text.Compare(KAppendFormatNumber15) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeTibetan);
	test(text.Compare(KAppendFormatNumber16) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeMayanmar);
	test(text.Compare(KAppendFormatNumber17) == 0);
	NumberConversion::AppendFormatNumber(text, 123, EDigitTypeKhmer);
	test(text.Compare(KAppendFormatNumber18) == 0);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	}

void TestConvertDigits()
	{
	TBuf<80> text;

	text = KConvertDigits0;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits0Result1) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits0Result2) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeDevanagari);
	test(text.Compare(KConvertDigits0Result3) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeThai);
	test(text.Compare(KConvertDigits0Result4) == 0);
	NumberConversion::ConvertDigits (text,EDigitTypeWestern);
	test(text.Compare(KConvertDigits0) == 0);
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	NumberConversion::ConvertDigits(text, EDigitTypeBengali);
	test(text.Compare(KConvertDigits0Result5) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeGurmukhi);
	test(text.Compare(KConvertDigits0Result6) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeGujarati);
	test(text.Compare(KConvertDigits0Result7) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeOriya);
	test(text.Compare(KConvertDigits0Result8) == 0);
	/* Can't test TAMIL DIGIT 0 because it wasn't introduced until Unicode 4.1
	   We currently use Unicode 2.0, and hence can only test this once we upgrade to 4.1 or higher
	NumberConversion::ConvertDigits(text, EDigitTypeTamil);
	test(text.Compare(KConvertDigits0Result9) == 0);*/
	NumberConversion::ConvertDigits(text, EDigitTypeTelugu);
	test(text.Compare(KConvertDigits0Result10) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeKannada);
	test(text.Compare(KConvertDigits0Result11) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeMalayalam);
	test(text.Compare(KConvertDigits0Result12) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeLao);
	test(text.Compare(KConvertDigits0Result13) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeTibetan);
	test(text.Compare(KConvertDigits0Result14) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeMayanmar);
	test(text.Compare(KConvertDigits0Result15) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeKhmer);
	test(text.Compare(KConvertDigits0Result16) == 0);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits1Result1) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits1Result2) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeDevanagari);
	test(text.Compare(KConvertDigits1Result3) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeThai);
	test(text.Compare(KConvertDigits1Result4) == 0);
	text = KConvertDigits1;
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	NumberConversion::ConvertDigits(text, EDigitTypeBengali);
	test(text.Compare(KConvertDigits1Result5) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeGurmukhi);
	test(text.Compare(KConvertDigits1Result6) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeGujarati);
	test(text.Compare(KConvertDigits1Result7) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeOriya);
	test(text.Compare(KConvertDigits1Result8) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeTamil);
	test(text.Compare(KConvertDigits1Result9) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeTelugu);
	test(text.Compare(KConvertDigits1Result10) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeKannada);
	test(text.Compare(KConvertDigits1Result11) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeMalayalam);
	test(text.Compare(KConvertDigits1Result12) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeLao);
	test(text.Compare(KConvertDigits1Result13) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeTibetan);
	test(text.Compare(KConvertDigits1Result14) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeMayanmar);
	test(text.Compare(KConvertDigits1Result15) == 0);
	text = KConvertDigits1;
	NumberConversion::ConvertDigits(text, EDigitTypeKhmer);
	test(text.Compare(KConvertDigits1Result16) == 0);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits2Result1) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits2Result2) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeDevanagari);
	test(text.Compare(KConvertDigits2Result3) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeThai);
	test(text.Compare(KConvertDigits2Result4) == 0);
	text = KConvertDigits2;
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	NumberConversion::ConvertDigits(text, EDigitTypeBengali);
	test(text.Compare(KConvertDigits2Result5) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeGurmukhi);
	test(text.Compare(KConvertDigits2Result6) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeGujarati);
	test(text.Compare(KConvertDigits2Result7) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeOriya);
	test(text.Compare(KConvertDigits2Result8) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeTamil);
	test(text.Compare(KConvertDigits2Result9) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeTelugu);
	test(text.Compare(KConvertDigits2Result10) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeKannada);
	test(text.Compare(KConvertDigits2Result11) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeMalayalam);
	test(text.Compare(KConvertDigits2Result12) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeLao);
	test(text.Compare(KConvertDigits2Result13) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeTibetan);
	test(text.Compare(KConvertDigits2Result14) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeMayanmar);
	test(text.Compare(KConvertDigits2Result15) == 0);
	text = KConvertDigits2;
	NumberConversion::ConvertDigits(text, EDigitTypeKhmer);
	test(text.Compare(KConvertDigits2Result16) == 0);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits3Result1) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits3Result2) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeDevanagari);
	test(text.Compare(KConvertDigits3Result3) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeThai);
	test(text.Compare(KConvertDigits3Result4) == 0);
	text = KConvertDigits3;
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	NumberConversion::ConvertDigits(text, EDigitTypeBengali);
	test(text.Compare(KConvertDigits3Result5) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeGurmukhi);
	test(text.Compare(KConvertDigits3Result6) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeGujarati);
	test(text.Compare(KConvertDigits3Result7) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeOriya);
	test(text.Compare(KConvertDigits3Result8) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeTamil);
	test(text.Compare(KConvertDigits3Result9) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeTelugu);
	test(text.Compare(KConvertDigits3Result10) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeKannada);
	test(text.Compare(KConvertDigits3Result11) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeMalayalam);
	test(text.Compare(KConvertDigits3Result12) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeLao);
	test(text.Compare(KConvertDigits3Result13) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeTibetan);
	test(text.Compare(KConvertDigits3Result14) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeMayanmar);
	test(text.Compare(KConvertDigits3Result15) == 0);
	text = KConvertDigits3;
	NumberConversion::ConvertDigits(text, EDigitTypeKhmer);
	test(text.Compare(KConvertDigits3Result16) == 0);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co

	text = KConvertDigits4;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits4Result1) == 0);
	text = KConvertDigits4;
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits4Result2) == 0);

	text = KConvertDigits5;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits5Result1) == 0);
	text = KConvertDigits5;
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits5Result2) == 0);

	text = KConvertDigits6;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits6Result1) == 0);
	text = KConvertDigits6;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits6Result2) == 0);

	text = KConvertDigits7;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits7) == 0);

	text = KConvertDigits8;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits8Result1) == 0);

	text = KConvertDigits9;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits9Result1) == 0);
	
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits10Result1) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits10Result2) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits10Result3) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeThai);
	test(text.Compare(KConvertDigits10Result4) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeBengali);
	test(text.Compare(KConvertDigits10Result5) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeGurmukhi);
	test(text.Compare(KConvertDigits10Result6) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeGujarati);
	test(text.Compare(KConvertDigits10Result7) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeOriya);
	test(text.Compare(KConvertDigits10Result8) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeTamil);
	test(text.Compare(KConvertDigits10Result9) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeTelugu);
	test(text.Compare(KConvertDigits10Result10) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeKannada);
	test(text.Compare(KConvertDigits10Result11) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeMalayalam);
	test(text.Compare(KConvertDigits10Result12) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeLao);
	test(text.Compare(KConvertDigits10Result13) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeTibetan);
	test(text.Compare(KConvertDigits10Result14) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeMayanmar);
	test(text.Compare(KConvertDigits10Result15) == 0);
	text = KConvertDigits10;
	NumberConversion::ConvertDigits(text, EDigitTypeKhmer);
	test(text.Compare(KConvertDigits10Result16) == 0);
	
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits11Result1) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits11Result2) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits11Result3) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeDevanagari);
	test(text.Compare(KConvertDigits11Result4) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeThai);
	test(text.Compare(KConvertDigits11Result5) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeBengali);
	test(text.Compare(KConvertDigits11Result6) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeGurmukhi);
	test(text.Compare(KConvertDigits11Result7) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeGujarati);
	test(text.Compare(KConvertDigits11Result8) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeOriya);
	test(text.Compare(KConvertDigits11Result9) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeTamil);
	test(text.Compare(KConvertDigits11Result10) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeTelugu);
	test(text.Compare(KConvertDigits11Result11) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeMalayalam);
	test(text.Compare(KConvertDigits11Result12) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeLao);
	test(text.Compare(KConvertDigits11Result13) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeTibetan);
	test(text.Compare(KConvertDigits11Result14) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeMayanmar);
	test(text.Compare(KConvertDigits11Result15) == 0);
	text = KConvertDigits11;
	NumberConversion::ConvertDigits(text, EDigitTypeKhmer);
	test(text.Compare(KConvertDigits11Result16) == 0);
	
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits12Result1) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits12Result2) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits12Result3) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeDevanagari);
	test(text.Compare(KConvertDigits12Result4) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeThai);
	test(text.Compare(KConvertDigits12Result5) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeBengali);
	test(text.Compare(KConvertDigits12Result6) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeGurmukhi);
	test(text.Compare(KConvertDigits12Result7) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeOriya);
	test(text.Compare(KConvertDigits12Result8) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeTamil);
	test(text.Compare(KConvertDigits12Result9) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeTelugu);
	test(text.Compare(KConvertDigits12Result10) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeKannada);
	test(text.Compare(KConvertDigits12Result11) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeMalayalam);
	test(text.Compare(KConvertDigits12Result12) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeLao);
	test(text.Compare(KConvertDigits12Result13) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeTibetan);
	test(text.Compare(KConvertDigits12Result14) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeMayanmar);
	test(text.Compare(KConvertDigits12Result15) == 0);
	text = KConvertDigits12;
	NumberConversion::ConvertDigits(text, EDigitTypeKhmer);
	test(text.Compare(KConvertDigits12Result16) == 0);
	
	text = KConvertDigits13;
	NumberConversion::ConvertDigits(text, EDigitTypeMayanmar);
	test(text.Compare(KConvertDigits13Result16) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeTibetan);
	test(text.Compare(KConvertDigits13Result15) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeLao);
	test(text.Compare(KConvertDigits13Result14) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeMalayalam);
	test(text.Compare(KConvertDigits13Result13) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeKannada);
	test(text.Compare(KConvertDigits13Result12) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeTelugu);
	test(text.Compare(KConvertDigits13Result11) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeTamil);
	test(text.Compare(KConvertDigits13Result10) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeOriya);
	test(text.Compare(KConvertDigits13Result9) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeGujarati);
	test(text.Compare(KConvertDigits13Result8) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeGurmukhi);
	test(text.Compare(KConvertDigits13Result7) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeBengali);
	test(text.Compare(KConvertDigits13Result6) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeThai);
	test(text.Compare(KConvertDigits13Result5) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeDevanagari);
	test(text.Compare(KConvertDigits13Result4) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeEasternArabicIndic);
	test(text.Compare(KConvertDigits13Result3) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeArabicIndic);
	test(text.Compare(KConvertDigits13Result2) == 0);
	NumberConversion::ConvertDigits(text, EDigitTypeWestern);
	test(text.Compare(KConvertDigits13Result1) == 0);
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	}


void TestLengthOfConvertedText()
	{
	TBuf<80> text;
	text = KConvertDigits1;
	test(NumberConversion::LengthOfConvertedText(text, EDigitTypeArabicIndic) == text.Length());
	}

void TestFormat()
	{
	TBuf<80> text;
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeWestern, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1a) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeArabicIndic, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1b) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeEasternArabicIndic, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1c) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeDevanagari, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1d) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeThai, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1e) == 0);
	text = KFormatString1;
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	NumberConversion::Format(EDigitTypeBengali, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1f) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeGurmukhi, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1g) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeGujarati, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1h) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeOriya, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1i) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeTamil, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1j) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeTelugu, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1k) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeKannada, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1l) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeMalayalam, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1m) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeLao, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1n) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeTibetan, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1o) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeMayanmar, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1p) == 0);
	text = KFormatString1;
	NumberConversion::Format(EDigitTypeKhmer, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult1q) == 0);
	text = KFormatString1;
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeWestern, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2a) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeArabicIndic, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2b) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeEasternArabicIndic, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2c) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeDevanagari, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2d) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeThai, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2e) == 0);
	text = KFormatString2;
	// DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co
	NumberConversion::Format(EDigitTypeBengali, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2f) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeGurmukhi, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2g) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeGujarati, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2h) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeOriya, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2i) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeTamil, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2j) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeTelugu, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2k) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeKannada, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2l) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeMalayalam, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2m) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeLao, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2n) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeTibetan, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2o) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeMayanmar, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2p) == 0);
	text = KFormatString2;
	NumberConversion::Format(EDigitTypeKhmer, TRefByValue<TDes>(text), 123);
	test(text.Compare(KFormatResult2q) == 0);
	text = KFormatString2;
	// End of DEF101816: TDigitType enum needs to be extended to inlcude Indic digits to enable number co


	text = KFormatString3;
	NumberConversion::Format(EDigitTypeWestern, TRefByValue<TDes>(text), 123, 456);
	test(text.Compare(KFormatResult3a) == 0);
	text = KFormatString3;
	NumberConversion::Format(EDigitTypeArabicIndic, TRefByValue<TDes>(text), 123, 456);
	test(text.Compare(KFormatResult3b) == 0);
	text = KFormatString3;
	NumberConversion::Format(EDigitTypeEasternArabicIndic, TRefByValue<TDes>(text), 123, 456);
	test(text.Compare(KFormatResult3c) == 0);
	text = KFormatString3;
	NumberConversion::Format(EDigitTypeDevanagari, TRefByValue<TDes>(text), 123, 456);
	test(text.Compare(KFormatResult3d) == 0);
	text = KFormatString3;
	NumberConversion::Format(EDigitTypeThai, TRefByValue<TDes>(text), 123, 456);
	test(text.Compare(KFormatResult3e) == 0);


//	text.Format(KFormatString4, 123, &KString, 456, &KString);

	text = KFormatString4;
	NumberConversion::Format(EDigitTypeWestern, TRefByValue<TDes>(text), 123, &KString, 456, &KString);
	test(text.Compare(KFormatResult4a) == 0);
	text = KFormatString4;
	NumberConversion::Format(EDigitTypeArabicIndic, TRefByValue<TDes>(text), 123, &KString, 456, &KString);
	test(text.Compare(KFormatResult4b) == 0);
	text = KFormatString4;
	NumberConversion::Format(EDigitTypeEasternArabicIndic, TRefByValue<TDes>(text), 123, &KString, 456, &KString);
	test(text.Compare(KFormatResult4c) == 0);

	}

void TestLocale()
	{
	TLocale testLocale1;
	TLocale testLocale2;
	TDigitType testDigitType;

	testLocale1.SetDigitType(EDigitTypeThai);
	testLocale2.SetDigitType(EDigitTypeArabicIndic);

	testDigitType = testLocale1.DigitType();
	test(testDigitType==EDigitTypeThai);
	testDigitType = testLocale2.DigitType();
	test(testDigitType==EDigitTypeArabicIndic);
	}

void TestConvertDigit()
	{
	struct TDigitInfo
		{
		TDigitInfo(TDigitType aDigitType, const TDesC& aName) :
			iDigitType(aDigitType),
			iName(aName)
			{
			}
		TDigitType iDigitType;
		TBuf<32> iName;
		} digitTypes[] = 
			{
			TDigitInfo(EDigitTypeWestern, _L("Western")),
			TDigitInfo(EDigitTypeArabicIndic, _L("ArabicIndic")),
			TDigitInfo(EDigitTypeEasternArabicIndic, _L("EasternArabicIndic")),
			TDigitInfo(EDigitTypeDevanagari, _L("Devanagari")),
			TDigitInfo(EDigitTypeThai, _L("Thai")),
			};

	test.Printf(_L("\r\nConverDigit()\r\n"));
	for(TInt i=0;i<TInt(sizeof(digitTypes)/sizeof(digitTypes[0]));++i)
		{
		test.Printf(_L("%S\r\n"), &(digitTypes[i].iName));
		for(TInt j=0;j<TInt(sizeof(digitTypes)/sizeof(digitTypes[0]));++j)
			{
			test.Printf(_L("...to %S\r\n"), &(digitTypes[j].iName));
			for(TInt k=0;k<10;++k)
				{
				TChar c = TChar(digitTypes[i].iDigitType + k);
				test(NumberConversion::ConvertDigit(c, digitTypes[j].iDigitType) == 
					TChar(digitTypes[j].iDigitType + k));
				}
			}
		}
	}

void RunTestsL()
	{
	test.Start(_L(" @SYMTestCaseID:SYSLIBS-NUMBERCONVERSION-LEGACY-TESTCONV-0001 Testing NumberConversion "));
	__UHEAP_MARK;
	test.Next(_L("Test String->Number"));
	TestStringToNumber();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Test PositionOfNextNumber"));
	TestPositionOfNextNumber();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Test FormatNumber"));
	TestFormatNumber();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Test AppendFormatNumber"));
	TestAppendFormatNumber();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Test ConvertDigits"));
	TestConvertDigits();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Test LengthOfConvertedText"));
	TestLengthOfConvertedText();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Test Format"));
	TestFormat();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Test Locale"));
	TestLocale();
	__UHEAP_MARKEND;
	__UHEAP_MARK;
	test.Next(_L("Test ConvertDigit"));
	TestConvertDigit();
	__UHEAP_MARKEND;

	}


void SetupConsoleL() // initialize and call example code under cleanup stack
	{
	console=Console::NewL(_L("NumberConversion Test"),	TSize(KCalypsoConsoleWidth,KCalypsoConsoleHeight));
	CleanupStack::PushL(console);
	test.SetConsole(console);

	// Set up an active scheduler, for multitap testing
	CActiveScheduler*  scheduler = new (ELeave) CActiveScheduler;
	  // Push onto the cleanup stack
	CleanupStack::PushL(scheduler);
	 
	  // Install as the active scheduler
	CActiveScheduler::Install(scheduler);

	// DriveEngineL is the real thing being tested here
	TRAPD(error,RunTestsL()); // perform example function

	test(error == KErrNone);
	test.End();

	CleanupStack::PopAndDestroy(2); // close console, scheduler
	}


GLDEF_C TInt E32Main() // main function called by E32
	{
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
	TRAPD(error,SetupConsoleL()); // more initialization, then do example
	__ASSERT_ALWAYS(!error,User::Panic(_L("NumberConversion Test"),error));
	delete cleanup; // destroy clean-up stack
	return 0; // and return
	}
