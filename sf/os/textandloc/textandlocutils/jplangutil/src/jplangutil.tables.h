/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     Conversion tables used in JPLangUtil.cpp.
* 
*
*/


#ifndef __JPLANGUTIL_TABLES_H__
#define __JPLANGUTIL_TABLES_H__

// INCLUDES
#include <e32std.h>

// CONSTANTS
const TText KMiddleDotMap[] =      { 0xff65 , 0x30fb };
const TText KLetterWoMap[] =       { 0xff66 , 0x30f2 };
const TText KSmallLetterAMap[] =   { 0xff67 , 0x30a1 };
const TText KSmallLetterIMap[] =   { 0xff68 , 0x30a3 };
const TText KSmallLetterUMap[] =   { 0xff69 , 0x30a5 };
const TText KSmallLetterEMap[] =   { 0xff6a , 0x30a7 };
const TText KSmallLetterOMap[] =   { 0xff6b , 0x30a9 };
const TText KSmallLetterYaMap[] =  { 0xff6c , 0x30e3 };
const TText KSmallLetterYuMap[] =  { 0xff6d , 0x30e5 };
const TText KSmallLetterYoMap[] =  { 0xff6e , 0x30e7 };
const TText KSmallLetterTsuMap[] = { 0xff6f , 0x30c3 };
const TText KProlongedSoundMap[] = { 0xff70 , 0x30fc };
const TText KLetterAMap[] =   { 0xff71 , 0x30a2 };
const TText KLetterIMap[] =   { 0xff72 , 0x30a4 };
const TText KLetterUMap[] =   { 0xff73 , 0x30a6 };
const TText KLetterEMap[] =   { 0xff74 , 0x30a8 };
const TText KLetterOMap[] =   { 0xff75 , 0x30aa };
const TText KLetterKaMap[] =  { 0xff76 , 0x30ab };
const TText KLetterKiMap[] =  { 0xff77 , 0x30ad };
const TText KLetterKuMap[] =  { 0xff78 , 0x30af };
const TText KLetterKeMap[] =  { 0xff79 , 0x30b1 };
const TText KLetterKoMap[] =  { 0xff7a , 0x30b3 };
const TText KLetterSaMap[] =  { 0xff7b , 0x30b5 };
const TText KLetterShiMap[] = { 0xff7c , 0x30b7 };
const TText KLetterSuMap[] =  { 0xff7d , 0x30b9 };
const TText KLetterSeMap[] =  { 0xff7e , 0x30bb };
const TText KLetterSoMap[] =  { 0xff7f , 0x30bd };
const TText KLetterTaMap[] =  { 0xff80 , 0x30bf };
const TText KLetterTiMap[] =  { 0xff81 , 0x30c1 };
const TText KLetterTsuMap[] = { 0xff82 , 0x30c4 };
const TText KLetterTeMap[] =  { 0xff83 , 0x30c6 };
const TText KLetterToMap[] =  { 0xff84 , 0x30c8 };
const TText KLetterNaMap[] =  { 0xff85 , 0x30ca };
const TText KLetterNiMap[] =  { 0xff86 , 0x30cb };
const TText KLetterNuMap[] =  { 0xff87 , 0x30cc };
const TText KLetterNeMap[] =  { 0xff88 , 0x30cd };
const TText KLetterNoMap[] =  { 0xff89 , 0x30ce };
const TText KLetterHaMap[] =  { 0xff8a , 0x30cf };
const TText KLetterHiMap[] =  { 0xff8b , 0x30d2 };
const TText KLetterFuMap[] =  { 0xff8c , 0x30d5 };
const TText KLetterHeMap[] =  { 0xff8d , 0x30d8 };
const TText KLetterHoMap[] =  { 0xff8e , 0x30db };
const TText KLetterMaMap[] =  { 0xff8f , 0x30de };
const TText KLetterMiMap[] =  { 0xff90 , 0x30df };
const TText KLetterMuMap[] =  { 0xff91 , 0x30e0 };
const TText KLetterMeMap[] =  { 0xff92 , 0x30e1 };
const TText KLetterMoMap[] =  { 0xff93 , 0x30e2 };
const TText KLetterYaMap[] =  { 0xff94 , 0x30e4 };
const TText KLetterYuMap[] =  { 0xff95 , 0x30e6 };
const TText KLetterYoMap[] =  { 0xff96 , 0x30e8 };
const TText KLetterRaMap[] =  { 0xff97 , 0x30e9 };
const TText KLetterRiMap[] =  { 0xff98 , 0x30ea };
const TText KLetterRuMap[] =  { 0xff99 , 0x30eb };
const TText KLetterReMap[] =  { 0xff9a , 0x30ec };
const TText KLetterRoMap[] =  { 0xff9b , 0x30ed };
const TText KLetterWaMap[] =  { 0xff9c , 0x30ef };
const TText KLetterNMap[] =   { 0xff9d , 0x30f3 };
const TText KVoicedSoundMarkMap[] =     { 0xff9e , 0x309b };
const TText KSemiVoicedSoundMarkMap[] = { 0xff9f , 0x309c };

const TText* const KHalfToFullWidthKatakanaCharTable[] =
    {
    KMiddleDotMap,
    KLetterWoMap,
    KSmallLetterAMap,
    KSmallLetterIMap,
    KSmallLetterUMap,
    KSmallLetterEMap,
    KSmallLetterOMap,
    KSmallLetterYaMap,
    KSmallLetterYuMap,
    KSmallLetterYoMap,
    KSmallLetterTsuMap,
    KProlongedSoundMap,
    KLetterAMap,
    KLetterIMap,
    KLetterUMap,
    KLetterEMap,
    KLetterOMap,
    KLetterKaMap,
    KLetterKiMap,
    KLetterKuMap,
    KLetterKeMap,
    KLetterKoMap,
    KLetterSaMap,
    KLetterShiMap,
    KLetterSuMap,
    KLetterSeMap,
    KLetterSoMap,
    KLetterTaMap,
    KLetterTiMap,
    KLetterTsuMap,
    KLetterTeMap,
    KLetterToMap,
    KLetterNaMap,
    KLetterNiMap,
    KLetterNuMap,
    KLetterNeMap,
    KLetterNoMap,
    KLetterHaMap,
    KLetterHiMap,
    KLetterFuMap,
    KLetterHeMap,
    KLetterHoMap,
    KLetterMaMap,
    KLetterMiMap,
    KLetterMuMap,
    KLetterMeMap,
    KLetterMoMap,
    KLetterYaMap,
    KLetterYuMap,
    KLetterYoMap,
    KLetterRaMap,
    KLetterRiMap,
    KLetterRuMap,
    KLetterReMap,
    KLetterRoMap,
    KLetterWaMap,
    KLetterNMap,
    KVoicedSoundMarkMap,
    KSemiVoicedSoundMarkMap
    };

const TText KUToVUMap[] =   { 0xff73 , 0x30f4 };
const TText KKAToGAMap[] =  { 0xff76 , 0x30ac };
const TText KKIToGIMap[] =  { 0xff77 , 0x30ae };
const TText KKUToGUMap[] =  { 0xff78 , 0x30b0 };
const TText KKEToGEMap[] =  { 0xff79 , 0x30b2 };
const TText KKOToGOMap[] =  { 0xff7a , 0x30b4 };
const TText KSAToZAMap[] =  { 0xff7b , 0x30b6 };
const TText KSHIToJIMap[] = { 0xff7c , 0x30b8 };
const TText KSUToZUMap[] =  { 0xff7d , 0x30ba };
const TText KSEToZEMap[] =  { 0xff7e , 0x30bc };
const TText KSOToZOMap[] =  { 0xff7f , 0x30be };
const TText KTAToDAMap[] =  { 0xff80 , 0x30c0 };
const TText KCHIToZIMap[] = { 0xff81 , 0x30c2 };
const TText KTSUToZUMap[] = { 0xff82 , 0x30c5 };
const TText KTEToDEMap[] =  { 0xff83 , 0x30c7 };
const TText KTOToDOMap[] =  { 0xff84 , 0x30c9 };
const TText KHAToBAMap[] =  { 0xff8a , 0x30d0 };
const TText KHIToBIMap[] =  { 0xff8b , 0x30d3 };
const TText KFUToBUMap[] =  { 0xff8c , 0x30d6 };
const TText KHEToBEMap[] =  { 0xff8d , 0x30d9 };
const TText KHOToBOMap[] =  { 0xff8e , 0x30dc };
const TText KWAToVAMap[] =  { 0xff9c , 0x30f7 };
const TText KWOToVOMap[] =  { 0xff66 , 0x30fa };

const TText* const KHalfWidthBaseToFullWidthVoicedKatakanaTable[] =
    {
    KUToVUMap,
    KKAToGAMap,
    KKIToGIMap,
    KKUToGUMap,
    KKEToGEMap,
    KKOToGOMap,
    KSAToZAMap,
    KSHIToJIMap,
    KSUToZUMap,
    KSEToZEMap,
    KSOToZOMap,
    KTAToDAMap,
    KCHIToZIMap,
    KTSUToZUMap,
    KTEToDEMap,
    KTOToDOMap,
    KHAToBAMap,
    KHIToBIMap,
    KFUToBUMap,
    KHEToBEMap,
    KHOToBOMap,
    KWAToVAMap,
    KWOToVOMap
    };

const TText KHAToPAMap[] = { 0xff8a , 0x30d1 };
const TText KHIToPIMap[] = { 0xff8b , 0x30d4 };
const TText KFUToPUMap[] = { 0xff8c , 0x30d7 };
const TText KHEToPEMap[] = { 0xff8d , 0x30da };
const TText KHOToPOMap[] = { 0xff8e , 0x30dd };

const TText* const KHalfWidthBaseToFullWidthSemiVoicedKatakanaTable[] =
    {
    KHAToPAMap,
    KHIToPIMap,
    KFUToPUMap,
    KHEToPEMap,
    KHOToPOMap
    };

const TText KSpaceMap[] =     { 0x0020 , 0x3000 };
const TText KCentSignMap[] =  { 0x00a2 , 0xffe0 };
const TText KPoundSignMap[] = { 0x00a3 , 0xffe1 };
const TText KNotSignMap[] =   { 0x00ac , 0xffe2 };
const TText KMacronMap[] =    { 0x00af , 0xffe3 };
const TText KBrokenBarMap[] = { 0x00a6 , 0xffe4 };
const TText KYenSignMap[] =   { 0x00a5 , 0xffe5 };
const TText KWonSignMap[] =   { 0x00a9 , 0xffe6 };
const TText KLeftWhiteParenthesisMap[] =  { 0x2985 , 0xff5f };
const TText KRightWhiteParenthesisMap[] = { 0x2986 , 0xff60 };
const TText KIdeographicFullStopMap[] =   { 0xff61 , 0x3002 };
const TText KLeftCornerBracketMap[] =     { 0xff62 , 0x300c };
const TText KRightCornerBracketMap[] =    { 0xff63 , 0x300d };
const TText KIdeographicCommaMap[] =      { 0xff64 , 0x3001 };
const TText KFormsLightVerticalMap[] =    { 0xffe8 , 0x2502 };
const TText KLeftwardsArrowMap[] =  { 0xffe9 , 0x2190 };
const TText KUpwardsArrowMap[] =    { 0xffea , 0x2191 };
const TText KRightwardsArrowMap[] = { 0xffeb , 0x2192 };
const TText KDownwardsArrowMap[] =  { 0xffec , 0x2193 };
const TText KBlackSquareMap[] =     { 0xffed , 0x25a0 };
const TText KWhiteCircleMap[] =     { 0xffee , 0x25cb };

const TText* const KHalfToFullWidthSpecialCharTable[] =
    {
    KSpaceMap,
    KCentSignMap,
    KPoundSignMap,
    KNotSignMap,
    KMacronMap,
    KBrokenBarMap,
    KYenSignMap,
    KWonSignMap,
    KLeftWhiteParenthesisMap,
    KRightWhiteParenthesisMap,
    KIdeographicFullStopMap,
    KLeftCornerBracketMap,
    KRightCornerBracketMap,
    KIdeographicCommaMap,
    KFormsLightVerticalMap,
    KLeftwardsArrowMap,
    KUpwardsArrowMap,
    KRightwardsArrowMap,
    KDownwardsArrowMap,
    KBlackSquareMap,
    KWhiteCircleMap
    };

#endif // __JPLANGUTIL_TABLES_H__

// End of File
