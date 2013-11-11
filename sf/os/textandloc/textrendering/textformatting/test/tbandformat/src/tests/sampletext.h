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
*
*/


#ifndef __SAMPLETEXT_H
#define __SAMPLETEXT_H

/**
@file
@internalComponent 
*/
#include <e32std.h>

/*
This file contains text strings to initialise the various document types 
used to test band formatting and reformatting. 

(1) Constants for special characters, like the paragraph delimiter and bidirectional markup

(2) Various types of string to insert into a document, either to test insertion or to 
construct particular documents.
	- KSpace: a single space
	- KSpaces: 8 consecutive spaces
	- KEnglishMoreThanALine_*: some English text that occupies more than a line
	- KArabicMoreThanALine: some Arabic text that occupies more than a line
	- KEnglishLessThanALine_*: several strings of English text which each occupy less than a line
	- KArabicLessThanALine: some Arabic text which occupies less than a line	
	- KNumbers: some numbers that occupy more than a line
	- KPuncs: some punctutation that occupies less than a line
	- KEnglishLongWords_*: some English text which consists of pathologically long words (i.e. we expect to have to break lines mid-word)
	- KEnglishShortWords_*: some English text which consists only of 1-letter words
Some specific English and Arabic single words, used for the bidirectionality tests:
	- KFirstWord
	- KSecondWord
	- KFirstWordArabic
	- KSecondWordArabic 
	
(3) Some paragraphs
	- KEnglishShortParagraph: an English paragraph, more than a line but less than the visible band
	- KBleak1, KBleak2, KBleak3: some average-to-fairly-large English paragraphs
	- KEnglishLargeParagraph: an English paragraph which is too big to fit the formatted band
	- KLTRWithRTL: a paragraph which is predominantly LTR, containing some RTL text
	- KLTRWithSmallRTL: a paragraph which is predominantly LTR, containing just a few RTL characters
	- KLTRWithLargeRTL: a paragraph which is predominantly LTR, containing several lines worth of RTL text
	- KRTLWithLargeLTR: a paragraph which is predominantly RTL, containing several lines worth of LTR text
	- KRTLWithSmallLTR: a paragraph which is predominantly RTL, containing just a few LTR characters
	- KRTLWithLTR: a paragraph which is predominantly RTL, containing some LTR text
	- KVeryLarge: a very big paragraph, predominantly LTR, containing a few RTL runs
*/
/*
(1) Special characters
*/
_LIT(KRLO, "\x202E");
_LIT(KRLM, "\x200F");
_LIT(KRLE, "\x202B");
_LIT(KPDF, "\x202C");
_LIT(KParagraphDelimiter, "\x2029");

/*
(2) Strings
*/

_LIT(KSpace, " ");
_LIT(KSpaces, "        ");
_LIT(KOpenBracket, "(");
_LIT(KCloseBracket, ")");

_LIT(KEnglishMoreThanALine_1, "Fog on the Essex marshes, fog on the Kentish heights. \
Fog creeping into the cabooses of collier-brigs");

_LIT(KEnglishMoreThanALine_2, "This scarecrow of a suit has, in course of time, become \
so complicated that no man alive knows what it means.");

_LIT(KEnglishMoreThanALine_3, "She carries some small litter in a reticule which she calls \
her documents, principally consisting of paper matches and dry lavender.");

_LIT(KEnglishMoreThanALine_4, "A few lawyers' clerks and others who know this suitor by \
sight linger on the chance of his furnishing some fun.");

_LIT(KEnglishMoreThanALine_5, "Thus, in the midst of the mud and at the heart of the fog, \
sits the Lord High Chancellor in his High Court of Chancery.");

_LIT(KArabicMoreThanALine,"\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648 \
\x623\x635\x644\x629 \x641\x64A \x627\x644\x623\x633\x631\x629 \x627\x644\x628 \
\x634\x631\x64A\x629 \x648\x627\x644\x633\x644\x627\x645. ");

_LIT(KEnglishLessThanALine_1, "Holborn");
_LIT(KEnglishLessThanALine_2, "Fog on");
_LIT(KEnglishLessThanALine_3, "the essex");
_LIT(KEnglishLessThanALine_4, "as if the");
_LIT(KEnglishLessThanALine_5, "time--as");

_LIT(KArabicLessThanALine,"\x627\x633\x627\x644\x633\x627\x644");
_LIT(KArabicInParenthesis,"(\x627\x644\x623\x633\x631)");

_LIT(KNumbers, "12341455422 48875 66458752 4521 88 54 788821364457889 78552147 52 \
457 85 1225478889961 5224 455 5 ");

_LIT(KPuncs, "!? (<>)");

_LIT(KEnglishLongWords_1, "FogontheEssexmarshesfogontheKentishheights\
Fogcreeping");

_LIT(KEnglishLongWords_2, "Thisscarecrowofasuithasincourseoftimebecome\
socomplicated");

_LIT(KEnglishLongWords_3, "Shecarriessomesmalllitterinareticulewhichshecalls\
her documentsprincipallyconsistingofpapermatchesanddrylavender");

_LIT(KEnglishLongWords_4, "Afewlawyersclerksandotherswhoknowthissuitorbysightlingeronthechance \
ofhisfurnishingsomefun");

_LIT(KEnglishLongWords_5, "Thusinthemidstofthemudandattheheartofthefog \
sitstheLordHighChancellor inhisHighCourtofChancery.");

_LIT(KEnglishShortWords_1, "F o g o n t h e E s s e x m a r s h e s f o g o n t h e K e n \
t i s h h e i g h t s F o g c r e e p i n g ");

_LIT(KEnglishShortWords_2, "T h i s s c a r e c r o w o f a s u i t h a s i n c o u r s e o f \
t i m e b e c o m e s o c o m p l i c a t e d ");

_LIT(KEnglishShortWords_3, "S h e c a r r i e s s o m e s m a l l l i t t e r i n a r e t i \
c u l e w h i c h s h e c a l l s h e r   d o c u m e n t s p r i n c i p a l l y c o n s i \
s t i n g o f p a p e r m a t c h e s a n d d r y l a v e n d e r ");

_LIT(KEnglishShortWords_4, "A f e w l a w y e r s c l e r k s a n d o t h e r s w h o k n o w t \
h i s s u i t o r b y s i g h t l i n g e r o n t h e c h a n c e");

_LIT(KEnglishShortWords_5, "T h u s i n t h e m i d s t o f t h e m u d a n d a t t h e h e a r t \
o f t h e f o g  s i t s t h e L o r d H i g h C h a n c e l l o r   i n h i s H i g h C o u r t \
o f C h a n c e r y . ");

_LIT(KFirstWord,"First ");

_LIT(KSecondWord,"Second ");

_LIT(KFirstWordArabic, "\x627\x633\x627\x644 ");

_LIT(KSecondWordArabic, "\x648\x627\x644\x633 ");

_LIT(KThirdWordArabic, "\x62D\x631\x64A\x629 ");

/*
(3) Paragraphs
*/

_LIT(KEnglishShortParagraph, "London.  Michaelmas term lately over, and the Lord Chancellor \
sitting in Lincoln's Inn Hall. ");

_LIT(KBleak1, "London.  Michaelmas term lately over, and the Lord Chancellor \
sitting in Lincoln's Inn Hall.  Implacable November weather.  As \
much mud in the streets as if the waters had but newly retired from \
the face of the earth, and it would not be wonderful to meet a \
Megalosaurus, forty feet long or so, waddling like an elephantine \
lizard up Holborn Hill.  Smoke lowering down from chimney-pots, \
making a soft black drizzle, with flakes of soot in it as big as \
full-grown snowflakes--gone into mourning, one might imagine, for \
the death of the sun.  Dogs, undistinguishable in mire.  Horses, \
scarcely better; splashed to their very blinkers.  Foot passengers, \
jostling one another's umbrellas in a general infection of ill \
temper, and losing their foot-hold at street-corners, where tens of \
thousands of other foot passengers have been slipping and sliding \
since the day broke (if this day ever broke), adding new deposits \
to the crust upon crust of mud, sticking at those points tenaciously \
to the pavement, and accumulating at compound interest.");

_LIT(KBleak2, "Gas looming through the fog in divers places in the streets, much \
as the sun may, from the spongey fields, be seen to loom by \
husbandman and ploughboy.  Most of the shops lighted two hours \
before their time--as the gas seems to know, for it has a haggard \
and unwilling look.");
  
_LIT(KBleak3, "Fog everywhere. Fog up the river, where it flows among green aits \
and meadows; fog down the river, where it rolls deified among the \
tiers of shipping and the waterside pollutions of a great (and \
dirty) city. Fog on the Essex marshes, fog on the Kentish heights. \
Fog creeping into the cabooses of collier-brigs; fog lying out on \
the yards and hovering in the rigging of great ships; fog drooping \
on the gunwales of barges and small boats.  Fog in the eyes and \
throats of ancient Greenwich pensioners, wheezing by the firesides \
of their wards; fog in the stem and bowl of the afternoon pipe of \
the wrathful skipper, down in his close cabin; fog cruelly pinching \
the toes and fingers of his shivering little 'prentice boy on deck. \
Chance people on the bridges peeping over the parapets into a \
nether sky of fog, with fog all round them, as if they were up in a \
balloon and hanging in the misty clouds.");

_LIT(KEnglishLargeParagraph, "This is the start of a long paragraph. London.\
  Michaelmas term lately over, and the Lord Chancellor \
sitting in Lincoln's Inn Hall.  Implacable November weather.  As \
much mud in the streets as if the waters had but newly retired from \
the face of the earth, and it would not be wonderful to meet a \
Megalosaurus, forty feet long or so, waddling like an elephantine \
lizard up Holborn Hill.  Smoke lowering down from chimney-pots, \
making a soft black drizzle, with flakes of soot in it as big as \
full-grown snowflakes--gone into mourning, one might imagine, for \
the death of the sun.  Dogs, undistinguishable in mire.  Horses, \
scarcely better; splashed to their very blinkers.  Foot passengers, \
jostling one another's umbrellas in a general infection of ill \
temper, and losing their foot-hold at street-corners, where tens of \
thousands of other foot passengers have been slipping and sliding \
since the day broke (if this day ever broke), adding new deposits \
to the crust upon crust of mud, sticking at those points tenaciously \
to the pavement, and accumulating at compound interest. \
Gas looming through the fog in divers places in the streets, much \
as the sun may, from the spongey fields, be seen to loom by \
husbandman and ploughboy.  Most of the shops lighted two hours \
before their time--as the gas seems to know, for it has a haggard \
and unwilling look. Fog everywhere. Fog up the river, where it flows among green aits \
and meadows; fog down the river, where it rolls deified among the \
tiers of shipping and the waterside pollutions of a great (and \
dirty) city. Fog on the Essex marshes, fog on the Kentish heights. \
Fog creeping into the cabooses of collier-brigs; fog lying out on \
the yards and hovering in the rigging of great ships; fog drooping \
on the gunwales of barges and small boats.  Fog in the eyes and \
throats of ancient Greenwich pensioners, wheezing by the firesides \
of their wards; fog in the stem and bowl of the afternoon pipe of \
the wrathful skipper, down in his close cabin; fog cruelly pinching \
the toes and fingers of his shivering little 'prentice boy on deck. \
Chance people on the bridges peeping over the parapets into a \
nether sky of fog, with fog all round them, as if they were up in a \
balloon and hanging in the misty clouds.");

_LIT(KLTRWithRTL, "Gas looming through the fog in divers places in the streets, much \
as the sun may, from the spongey fields, be seen to loom by \
husbandman and ploughboy.  Most of the shops lighted two hours \
before their time--as the gas seems to know, for it has a haggard \
and unwilling look. London.  Michaelmas term lately over, and the Lord Chancellor \
sitting in Lincoln's Inn Hall.  Implacable November weather.  As \
much mud in the streets as if the waters had but newly retired from \
the face of the earth, and it would not be wonderful to meet a \
Megalosaurus, forty feet long or so, waddling like an elephantine \
lizard up Holborn Hill.  Smoke lowering down from chimney-pots, \
making a soft black drizzle, \x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648 \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 12344 4544 \x627\x644\x628\x634\x631\x64A\x629 \
\x648\x627\x644\x633\x644\x627\x645 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639 \
\x627\x644\x639\x62F\x644 with flakes of soot in it as big as \
full-grown snowflakes--gone into mourning, one might imagine, for \
the death of the sun.  Dogs, undistinguishable in mire.  Horses, \
scarcely better; splashed to their very blinkers.  Foot passengers, \
jostling one another's umbrellas in a general infection of ill \
temper, and losing their foot-hold at street-corners, where tens of \
thousands of other foot passengers have been slipping and sliding \
since the day broke (if this day ever broke), adding new deposits \
to the crust upon crust of mud, sticking at those points tenaciously \
to the pavement, and accumulating at compound interest.Fog everywhere \
Fog up the river, where it flows among green aits \
and meadows; fog down the river, where it rolls deified among the \
tiers of shipping and the waterside pollutions of a great (and \
dirty) city.  Fog on the Essex marshes, fog on the Kentish heights. \
Fog creeping into the cabooses of collier-brigs; fog lying out on \
the yards and hovering in the rigging of great ships; fog drooping \
on the gunwales of barges and small boats.  Fog in the eyes and \
throats of ancient Greenwich pensioners, wheezing by the firesides \
of their wards; fog in the stem and bowl of the afternoon pipe of \
the wrathful skipper, down in his close cabin; fog cruelly pinching \
the toes and fingers of his shivering little 'prentice boy on deck. \
Chance people on the bridges peeping over the parapets into a \
nether sky of fog, with fog all round them, as if they were up in a \
balloon and hanging in the misty clouds.");


_LIT(KLTRWithSmallRTL, "Gas looming through the fog in divers places in the streets, much \
as the sun may, from the spongey fields, be seen to loom by \
husbandman and ploughboy.  Most of the shops lighted two hours \
before their time--as the gas seems to know, for it has a haggard \
and unwilling look. London.  Michaelmas term lately over, and the Lord Chancellor \
sitting in Lincoln's Inn Hall.  Implacable November weather.  As \
much mud in the streets as if the waters had but newly retired from \
the face of the earth, and it would not be wonderful to meet a \
Megalosaurus, forty feet long or so, waddling like an elephantine \
lizard up Holborn Hill.  Smoke lowering down from chimney-pots, \
making \x627(\x633) 12 \x644 a soft black drizzle, with flakes of soot in it as big as \
full-grown snowflakes--gone into mourning, one might imagine, for \
the death of the sun.  Dogs, undistinguishable in mire.  Horses, \
scarcely better; splashed to their very blinkers.  Foot passengers, \
jostling one another's umbrellas in a general infection of ill \
temper, and losing their foot-hold at street-corners, where tens of \
thousands of other foot passengers have been slipping and sliding \
since the day broke (if this day ever broke), adding new deposits \
to the crust upon crust of mud, sticking at those points tenaciously \
to the pavement, and accumulating at compound interest.Fog everywhere. \
Fog up the river, where it flows among green aits \
and meadows; fog down the river, where it rolls deified among the \
tiers of shipping and the waterside pollutions of a great (and \
dirty) city.  Fog on the Essex marshes, fog on the Kentish heights. \
Fog creeping into the cabooses of collier-brigs; fog lying out on \
the yards and hovering in the rigging of great ships; fog drooping \
on the gunwales of barges and small boats.  Fog in the eyes and \
throats of ancient Greenwich pensioners, wheezing by the firesides \
of their wards; fog in the stem and bowl of the afternoon pipe of \
the wrathful skipper, down in his close cabin; fog cruelly pinching \
the toes and fingers of his shivering little 'prentice boy on deck. \
Chance people on the bridges peeping over the parapets into a \
nether sky of fog, with fog all round them, as if they were up in a \
balloon and hanging in the misty clouds.");


_LIT(KLTRWithLargeRTL, "Gas looming through the fog in divers places in the streets, much \
as the sun may, from the spongey fields, be seen to loom by \
husbandman and ploughboy.  Most of the shops lighted two hours \
before their time--as the gas seems to know, for it has a haggard \
and unwilling look. London.  Michaelmas term lately over, and the Lord Chancellor \
sitting in Lincoln's Inn Hall.  Implacable November weather.  As \
much mud in the streets as if the waters had but newly retired from \
the face of the earth, and it would not be wonderful to meet a \
Megalosaurus, forty feet long or so, waddling like an elephantine \
lizard up Holborn Hill.  Smoke lowering down from chimney-pots, \
making a \x644\x645\x651\x627 \x643\x627\x646 \x627\x644\x627\x639\x62A \
\x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A 12 34 49084 \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
\x645.  with flakes of soot in it as big as \
full-grown snowflakes--gone into mourning, one might imagine, for \
the death of the sun.  Dogs, undistinguishable in mire.  Horses, \
scarcely better; splashed to their very blinkers.  Foot passengers, \
jostling one another's umbrellas in a general infection of ill \
temper, and losing their foot-hold at street-corners, where tens of \
thousands of other foot passengers have been slipping and sliding \
since the day broke (if this day ever broke), adding new deposits \
to the crust upon crust of mud, sticking at those points tenaciously \
to the pavement, and accumulating at compound interest.Fog everywhere. \
Fog up the river, where it flows among green aits \
and meadows; fog down the river, where it rolls deified among the \
tiers of shipping and the waterside pollutions of a great (and \
dirty) city.  Fog on the Essex marshes, fog on the Kentish heights. \
Fog creeping into the cabooses of collier-brigs; fog lying out on \
the yards and hovering in the rigging of great ships; fog drooping \
on the gunwales of barges and small boats.  Fog in the eyes and \
throats of ancient Greenwich pensioners, wheezing by the firesides \
of their wards; fog in the stem and bowl of the afternoon pipe of \
the wrathful skipper, down in his close cabin; fog cruelly pinching \
the toes and fingers of his shivering little 'prentice boy on deck. \
Chance people on the bridges peeping over the parapets into a \
nether sky of fog, with fog all round them, as if they were up in a \
balloon and hanging in the misty clouds.");




_LIT(KRTLWithLargeLTR, "\x644\x645\x651\x627 \x643\x627\x646 \x627\x644\x627\x639\x62A \
\x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
\x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631 London.  Michaelmas term lately over, and \
the Lord Chancellor sitting in Lincoln's Inn Hall.  Implacable November weather.\
As much mud in the streets as if the waters had but newly retired from \
the face of the earth, and it would not be wonderful to meet a \
Megalosaurus, forty feet long or so, waddling like an elephantine \
lizard up Holborn Hill.  Smoke lowering down from chimney-pots, \
making a soft black drizzle, with flakes of soot in it as big as \
full-grown snowflakes--gone into mourning, one might imagine, for \
the death of the sun.  Dogs, undistinguishable in mire.  Horses, \
scarcely better; splashed to their very blinkers.  Foot passengers, \
jostling one another's umbrellas in a general infection of ill \
temper, and losing their foot-hold at street-corners, where tens of \
thousands of other foot passengers have been slipping and sliding \
since the day broke (if this day ever broke), adding new deposits \
to the crust upon crust of mud, sticking at those points tenaciously \
to the pavement, and accumulating at compound interest.\x64A\x629 \x648\x627\x644 \
\x639\x62F\x644 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
\x645. \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627 \
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629 \
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644. ");




_LIT(KRTLWithSmallLTR, "\x644\x645\x651\x627 \x643\x627\x646 \x627\x644\x627\x639\x62A\
\x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
\x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627 tiny 12 34 text\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
\x645. \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
\x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644, ");




_LIT(KRTLWithLTR, "\x644\x645\x651\x627 \x643\x627\x646 \x627\x644\x627\x639\x62A\
\x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
\x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
  \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631 This is only a 123 657 8 piece of random text to fill \
up a line and a half but it isn't big enough.\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
\x645. \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633 \
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644 \
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A \
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644\
 \x631\x627\x641 \x628\x627\x644\x643\x631\x627\x645\x629 \x627\x644\x645\x62A\
\x623\x635\x644\x629 \x641\x64A \x62C\x645\x64A\x639 \x623\x639\x636\x627\
\x621 \x627\x644\x623\x633\x631\x629 \x627\x644\x628\x634\x631\x64A\x629\
 \x648\x628\x62D\x642\x648\x642\x647\x645 \x627\x644\x645\x62A\x633\x627\
\x648\x64A\x629 \x627\x644\x62B\x627\x628\x62A\x629 \x647\x648 \x623\x633\
\x627\x633 \x627\x644\x62D\x631\x64A\x629 \x648\x627\x644\x639\x62F\x644\
 \x648\x627\x644\x633\x644\x627\x645 \x641\x64A \x627\x644\x639\x627\x644. ");


#endif
