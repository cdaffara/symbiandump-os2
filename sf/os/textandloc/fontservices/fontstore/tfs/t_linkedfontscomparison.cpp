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


#include "FNTSTD.H"
#include "t_linkedfontscomparison.h"
#include <hal.h>
#include <test/thashreferenceimages.h>
#include <linkedfonts.h>

/**
@file 

@SYMPurpose structures used to define linked typefaces used in this test, create and 
use linked typefaces, and check against a hash value
*/

/** this is for the group entries
 */
struct TTGroup
	{
	CLinkedTypefaceGroup::TBaselineShift iBaselineShift;
	CLinkedTypefaceGroup::TScalingOption iScalingOption;
	TInt iAntiAliasThreshold; //KMinTInt for do not set;
	TInt iBoldnessPercentage; //KMinTInt for do not set;
	TInt iItalicAngle; //KMinTInt for do not set;
	};

/** this is for linked font elements
 */
struct TTLinkedFontElement
	{
	const TPtrC iTypefaceName;
	TInt iGroup;
	TBool iIsCanonical;
	};

/** this is for the linked typeface table
 */
struct TTLinkedTypeface
	{
	const TPtrC iTypefaceName;
	const TTLinkedFontElement* iElements;
	};

/** this is a plain group
 */ 
const TTGroup KPlainGroup =
	{
	CLinkedTypefaceGroup::ENoBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	};

const TTGroup KGroup1 =
	{
	CLinkedTypefaceGroup::ENoBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	KMinTInt, //this value means do not set - antialiased	
	KMinTInt, //this value means do not set	- boldness percentage
	KMinTInt, //this value means do not set	- italic angle
	};

const TTGroup KGroup2 =
	{
	CLinkedTypefaceGroup::EBaselineShift,
	CLinkedTypefaceGroup::EScalingUp,
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	};
const TTGroup KGroup3 =
	{
	CLinkedTypefaceGroup::EBaselineShift,
	CLinkedTypefaceGroup::EScalingUpOrDown,
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	};
const TTGroup KGroup4 =
	{
	CLinkedTypefaceGroup::ENoBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	14, //antialiased	
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	};
const TTGroup KGroup5 =
	{
	CLinkedTypefaceGroup::ENoBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	10, //antialiased	
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	};
const TTGroup KGroup6 =
	{
	CLinkedTypefaceGroup::ENoBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	KMinTInt, //this value means do not set	
	100, //boldness percentage	
	KMinTInt, //this value means do not set	
	};
const TTGroup KGroup7 =
	{
	CLinkedTypefaceGroup::ENoBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	KMinTInt, //this value means do not set	
	10, //boldness percentage	
	KMinTInt, //this value means do not set	
	};
const TTGroup KGroup8 =
	{
	CLinkedTypefaceGroup::ENoBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	10, //italic angle	
	};
const TTGroup KGroup9 =
	{
	CLinkedTypefaceGroup::EBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	};
const TTGroup KGroup10 =
	{
	CLinkedTypefaceGroup::ENoBaselineShift,
	CLinkedTypefaceGroup::EScalingNone,
	KMinTInt, //this value means do not set	
	KMinTInt, //this value means do not set	
	40, //large italic angle	
	};

/** all the groups are added to every linked typeface
if there are too many groups then there may be a need for a group list
per linked typeface.  Can have a maximum of 12 groups
*/
const TTGroup* KGroupList[]=
	{
	&KPlainGroup, /* index 0 */	
	&KGroup1,
	&KGroup2,
	&KGroup3,
	&KGroup4,
	&KGroup5,
	&KGroup6,
	&KGroup7,
	&KGroup8,
	&KGroup9,
	&KGroup10,
	};

/** typeface element definitions
 */

/** Latin font
 */
const TTLinkedFontElement KLatinFontCanonical = 
	{
	_L("DejaVu Sans Condensed"),0, ETrue //name, group zero, is canonical
	};
	
/** 
 */
const TTLinkedFontElement KJapaneseNotCanonical =
	{
	_L("TBKomachiG-R"), 0, EFalse	 //name, group zero, not canonical
	};

/** Japanese - to be updated
 */
const TTLinkedFontElement KJapaneseNotCanonicalGroup1 =
	{
	_L("TBKomachiG-R"), 1, EFalse	 //name, group , not canonical
	};
const TTLinkedFontElement KJapaneseNotCanonicalGroup3 =
	{
	_L("TBKomachiG-R"), 3, EFalse	 //name, group, not canonical
	};
const TTLinkedFontElement KJapaneseNotCanonicalGroup4 =
	{
	_L("TBKomachiG-R"), 4, EFalse	 //name, group, not canonical
	};
const TTLinkedFontElement KJapaneseNotCanonicalGroup6 =
	{
	_L("TBKomachiG-R"), 6, EFalse	 //name, group, not canonical
	};
const TTLinkedFontElement KJapaneseNotCanonicalGroup8 =
	{
	_L("TBKomachiG-R"), 8, EFalse	 //name, group, not canonical
	};
const TTLinkedFontElement KJapaneseNotCanonicalGroup9 =
	{
	_L("TBKomachiG-R"), 9, EFalse	 //name, group, not canonical
	};
const TTLinkedFontElement KJapaneseNotCanonicalGroup10 =
	{
	_L("TBKomachiG-R"), 10, EFalse	 //name, group, not canonical
	};

const TTLinkedFontElement KLatinFontTTNotCanonicalGroup0 = 
	{
	_L("LinkedFontTestFont"), 0, EFalse
	};

const TTLinkedFontElement KLatinFontTTCanonicalGroup0 = 
	{
	_L("LinkedFontTestFont"), 0, ETrue
	};

/* for use case testing */
const TTLinkedFontElement KLatinFontTTCanonicalGroup2 = 
	{
	_L("LinkedFontTestFont"), 2, ETrue
	};
const TTLinkedFontElement KLatinFontTTCanonicalGroup3 = 
	{
	_L("LinkedFontTestFont"), 3, ETrue
	};
const TTLinkedFontElement KLatinFontTTCanonicalGroup5 = 
	{
	_L("LinkedFontTestFont"), 5, ETrue
	};
const TTLinkedFontElement KLatinFontTTCanonicalGroup7 = 
	{
	_L("LinkedFontTestFont"), 7, ETrue
	};
const TTLinkedFontElement KLatinFontTTCanonicalGroup8 = 
	{
	_L("LinkedFontTestFont"), 8, ETrue
	};
const TTLinkedFontElement KLatinFontTTCanonicalGroup9 = 
	{
	_L("LinkedFontTestFont"), 9, ETrue
	};
const TTLinkedFontElement KLatinFontTTCanonicalGroup10 = 
	{
	_L("LinkedFontTestFont"), 10, ETrue
	};
	
/** Chandas 
 */
const TTLinkedFontElement KHindiCanonical = 
	{
	_L("Chandas"),  0, ETrue
	};

/** Chandas 
 */
const TTLinkedFontElement KHindiNotCanonical = 
	{
	_L("Chandas"),  0, EFalse
	};

/** Information about linked fonts
TLinkedFont1.ttf
Typeface name: LinkedFont1
This will be the dominant font. It contains 178 characters.
Some of the characters (13) are deleted from this file and separated in to other three files.
Characters missing are F, D, J, M, P, S, V, X, Z, a, n, l, y.

TLinkedFont2.ttf
Typeface name: LinkedFont2
Characters available (7):	F, J, M, S, Z, a, y.

TLinkedFont3.ttf
Typeface name: LinkedFont3
Characters available (4):	D, P, x, l

TLinkedFont4.ttf
Typeface name: LinkedFont4
Characters available (2):	V, n

TWithoutLinkedFont.ttf
Typeface name: WithoutLinkedFont
Characters available:	It contains 191 characters U+0020-U+007E and U+00A0-U+00FF
*/	

/** LinkedFont1TT
 */
const TTLinkedFontElement KLinkedFont1TT = 
	{
	_L("LinkedFont1TT"),  0, EFalse
	};

/** LinkedFont2TT
 */
const TTLinkedFontElement KLinkedFont2TTCanonical = 
	{
	_L("LinkedFont2TT"),  0, ETrue
	};

/** LinkedFont3TT
 */
const TTLinkedFontElement KLinkedFont3TT = 
	{
	_L("LinkedFont3TT"),  0, EFalse
	};

/** LinkedFont4TT
 */
const TTLinkedFontElement KLinkedFont4TT = 
	{
	_L("LinkedFont4TT"),  0, EFalse
	};

/** KNullDesC
 */
const TTLinkedFontElement KNullElement = 
	{
	_L(""),  0, EFalse
	};
	
/** linked typeface, this list is terminated by KNullElement
 */
const TTLinkedFontElement KLatinFontHindi[]=
	{
	KLatinFontTTNotCanonicalGroup0,
	KHindiCanonical,
	KNullElement,
	};
/** linked typeface, this list is terminated by KNullElement
 */
const TTLinkedFontElement KLatinFontJapanese[]=
	{
	KLatinFontCanonical,
	KJapaneseNotCanonical,
	KNullElement
	};
/** linked typeface, this list is terminated by KNullElement
 */
const TTLinkedFontElement KLinkedFontTestFonts[]=
	{
	KLinkedFont1TT,
	KLinkedFont2TTCanonical,
	KLinkedFont3TT,
	KLinkedFont4TT,
	KNullElement
	};

const TTLinkedFontElement KJapaneseLatinFontTypeface1[]=
	{
	KLatinFontTTCanonicalGroup2,
	KJapaneseNotCanonicalGroup1,
	KNullElement
	};
const TTLinkedFontElement KJapaneseLatinFontTypeface2[]=
	{
	KLatinFontTTCanonicalGroup3,
	KJapaneseNotCanonicalGroup3,
	KNullElement
	};
const TTLinkedFontElement KJapaneseLatinFontTypeface3[]=
	{
	KLatinFontTTCanonicalGroup5,
	KJapaneseNotCanonicalGroup4,
	KNullElement
	};
const TTLinkedFontElement KJapaneseLatinFontTypeface4[]=
	{
	KLatinFontTTCanonicalGroup7,
	KJapaneseNotCanonicalGroup6,
	KNullElement
	};
const TTLinkedFontElement KJapaneseLatinFontTypeface5[]=
	{
	KLatinFontTTCanonicalGroup8,
	KJapaneseNotCanonicalGroup8,
	KNullElement
	};
const TTLinkedFontElement KJapaneseLatinFontTypeface6[]=
	{
	KLatinFontTTCanonicalGroup9,
	KJapaneseNotCanonicalGroup9,
	KNullElement
	};

const TTLinkedFontElement KJapaneseLatinFontTypeface7[]=
	{
	KLatinFontTTCanonicalGroup10,
	KJapaneseNotCanonicalGroup10,
	KNullElement
	};

const TTLinkedFontElement KEnglishJapanseHindiFont[]=
	{
	KLatinFontTTCanonicalGroup0,
	KJapaneseNotCanonical,
	KHindiNotCanonical,
	KNullElement
	};

/** the linked typeface set, no need to null terminate, sizeof is used for this array
 */
const TTLinkedTypeface KLinkedTypefaces[] =
	{
	//name, list of linked font elements
	{_L("LinkedFontTestFont"), KLinkedFontTestFonts},		
	{_L("LatinFontHindi"), KLatinFontHindi}, 
	{_L("LatinFontJapanese"), KLatinFontJapanese},
	{_L("EnglishJapaneseHindi"), KEnglishJapanseHindiFont},
	{_L("UseCaseFont1"), KJapaneseLatinFontTypeface1},
	{_L("UseCaseFont2"), KJapaneseLatinFontTypeface2},
	{_L("UseCaseFont3"), KJapaneseLatinFontTypeface3},
	{_L("UseCaseFont4"), KJapaneseLatinFontTypeface4},
	{_L("UseCaseFont5"), KJapaneseLatinFontTypeface5},
	{_L("UseCaseFont6"), KJapaneseLatinFontTypeface6},
	{_L("UseCaseFont7"), KJapaneseLatinFontTypeface7},
	};

/** Enum which constains bitwise flags for attrbutes which can be set for each font
 */
enum TFontEffectsFlags
	{
	ENoEffect = 0x0,
	EDropShadowOn = 0x1,
	EOutlineOn = 0x2,
	ESetMonochromeGlyphBitmap  = 0x4,
	ESetAntiAliasedGlyphBitmap = 0x8,
	ESetFourColourBlendGlyphBitmap = 0x10,
	EVerticalDraw = 0x20,
	ERotate90Degrees = 0x40,
	EIsStrikethroughOn = 0x80,
	EIsUnderlineOn = 0x100,
	ESoftEdgeOn = 0x200,
	EAlgorithmicBoldOn = 0x400,
	EDrawUp = 0x800,
	EItalicOn = 0x1000,
	ERegularBoldOn = 0x2000,
	EEndOfList = 0x80000000 //a meta flag
	};

const TInt KLengthOfHashValue = 1000;

/** table used to produce hash string from attributes 
 */
const TPtrC KFontEffectsFlagsString[32]=
	{
	_L("ENoEffect"),
	_L("EDropShadowOn"),
	_L("EOutlineOn"),
	_L("ESetMonochromeGlyphBitmap"),
	_L("ESetAntiAliasedGlyphBitmap"),
	_L("ESetFourColourBlendGlyphBitmap"),
	_L("EVerticalDraw"),
	_L("ERotate90Degrees"),
	_L("EIsStrikethroughOn"),
	_L("EIsUnderlineOn"),
	_L("ESoftEdgeOn"),
	_L("EAlgorithmicBoldOn"),
	_L("EDrawUp"),
	_L("EItalicOn"),
	_L("ERegularBoldOn"),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L(""),
	_L("EEndOfList"),
	};


/**this structure is used to define a test case
 */
struct TTestCase
	{
	const TPtrC iTypeFaceName; //typeface name 
	const TDesC* iOutputString; //the output string
	const TDesC* iTestDecription; //test description
	const TPoint iPosition; //the position
	const TInt* iSizes; // a list of sizes
	const TUint32* iFontAttributes; //a list of font attrbutes (bitwise flags, see above
	};

/** an array of sizes used during testing, terminated by zero
 */
const TInt KFontSizeNormalTesting[]=
	{
	10,
	20,
	35,
	0 
	};

const TInt KFontSizeUseCaseTesting[]=
	{
	8,
	12,
	16,
	35,
	0
	};

/** an array of attributes used during font testing, terminated by EEndOfList
 */
const TUint32 KFontAttributesNormalTest[] =// probably need to expand this with position
	{
	ENoEffect,
	EIsUnderlineOn,
	EIsStrikethroughOn,
	ESetFourColourBlendGlyphBitmap,
	EIsUnderlineOn| EIsStrikethroughOn,
	EAlgorithmicBoldOn,
	EVerticalDraw,
	EVerticalDraw | EIsUnderlineOn,
	EVerticalDraw | EIsUnderlineOn | ERotate90Degrees,
	EEndOfList,
	};

const TUint32 KFontAttributesUseCaseTest[] =// probably need to expand this with position
	{
	ENoEffect,
	ERegularBoldOn,
	ESetAntiAliasedGlyphBitmap,
	EItalicOn,
	EEndOfList,
	};

// here are the output strings which are used for testing
_LIT(KHindiText,"\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020");
_LIT(KEnglishText,"The quick brown fox 123");
_LIT(KEnglishHindiText,"TEST 123 \x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020");
_LIT(KEnglishJapaneseText,"T\x308a\x3093\x3054 ESghy LINKED FONTS 123");
_LIT(KEnglishJapaneseHindiText, "T\x308a\x3093\x3054\x306e EST FOnTS LINKED \x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020 123");
_LIT(KLinkedFontTestString,"lInKED FOnTS");


// test descriptions
_LIT(KEnglishTest,"EnglishTest");
_LIT(KHindiTest,"HindiTest");
_LIT(KEnglishHindiTest,"EnglishHindiTest");
_LIT(KEnglishJapaneseTest,"EnglishJapaneseTest");
_LIT(KLinkedFontTest1,"LinkedFontTest-withLinking");
_LIT(KLinkedFontTest2,"LinkedFontTest-unlinkedFont");
_LIT(KEnglishJapaneseHindiTest,"EnglishJapaneseHindiTest");
_LIT(KUseCaseTest,"UseCaseTest");



_LIT(KTestName0001,"TestCase001");
/** this table defines the test cases:
 font name, string, position, sizes(array), attributes(array)
 */
const TTestCase KTestCases[]=
	{
	{_L("LatinFontHindi"), &KHindiText,&KHindiTest, TPoint(50,50),KFontSizeNormalTesting,	KFontAttributesNormalTest},
	{_L("LatinFontHindi"), &KEnglishText, &KEnglishTest, TPoint(50,50),KFontSizeNormalTesting, KFontAttributesNormalTest},
	{_L("LatinFontHindi"), &KEnglishHindiText, &KEnglishHindiTest, TPoint(50,50),KFontSizeNormalTesting, KFontAttributesNormalTest},
	{_L("LatinFontJapanese"),&KEnglishText, &KEnglishTest, TPoint(50,50),KFontSizeNormalTesting, KFontAttributesNormalTest},
	{_L("LatinFontJapanese"),&KEnglishJapaneseText, &KEnglishJapaneseTest, TPoint(50,50),KFontSizeNormalTesting, KFontAttributesNormalTest},
	{_L("LinkedFontTestFont"),&KLinkedFontTestString, &KLinkedFontTest1, TPoint(50,50),KFontSizeNormalTesting, KFontAttributesNormalTest},
	{_L("WithoutLinkedFontTT"),&KLinkedFontTestString, &KLinkedFontTest2, TPoint(50,50),KFontSizeNormalTesting, KFontAttributesNormalTest},
	{_L("EnglishJapaneseHindi"),&KEnglishJapaneseHindiText, &KEnglishJapaneseHindiTest, TPoint(50,50),KFontSizeNormalTesting, KFontAttributesNormalTest},

	//use case testing
	{_L("UseCaseFont1"),&KEnglishJapaneseText, &KUseCaseTest, TPoint(50,50),KFontSizeUseCaseTesting, KFontAttributesUseCaseTest},
	{_L("UseCaseFont2"),&KEnglishJapaneseText, &KUseCaseTest, TPoint(50,50),KFontSizeUseCaseTesting, KFontAttributesUseCaseTest},
	{_L("UseCaseFont3"),&KEnglishJapaneseText, &KUseCaseTest, TPoint(50,50),KFontSizeUseCaseTesting, KFontAttributesUseCaseTest},
	{_L("UseCaseFont4"),&KEnglishJapaneseText, &KUseCaseTest, TPoint(50,50),KFontSizeUseCaseTesting, KFontAttributesUseCaseTest},
	{_L("UseCaseFont5"),&KEnglishJapaneseText, &KUseCaseTest, TPoint(50,50),KFontSizeUseCaseTesting, KFontAttributesUseCaseTest},
	{_L("UseCaseFont6"),&KEnglishJapaneseText, &KUseCaseTest, TPoint(50,50),KFontSizeUseCaseTesting, KFontAttributesUseCaseTest},
	{_L("UseCaseFont7"),&KEnglishJapaneseText, &KUseCaseTest, TPoint(50,50),KFontSizeUseCaseTesting, KFontAttributesUseCaseTest},
	};

/** Class used to register the linked typefaces
 */	
class CTLinkedTypefaceCreator : public CBase
	{
public:
	static CTLinkedTypefaceCreator* NewL(CTestStep* aStep);
	void CreateL(); //creates the linked typefaces - will not leave with a registration error, but try the next
	~CTLinkedTypefaceCreator();
private:
	CTLinkedTypefaceCreator(CTestStep* aStep);
	void CreateGroupsL(const TTGroup* aElement[],TInt aNumberOfElements);
	void CreateElementsL(const TTLinkedFontElement* aElements);
	void ConstructL();

private:	
	CTestStep* iStep; //for recording failures, not owned
	RPointerArray<CLinkedTypefaceGroup> iGroups; //owned
	RPointerArray<CLinkedTypefaceElementSpec> iElements; //owned
	CFbsScreenDevice* iDev; //owned
	};

/** static NewL
@return CTLinkedTypefaceCreator* 
 */
CTLinkedTypefaceCreator* CTLinkedTypefaceCreator::NewL(CTestStep* aStep)
	{
	CTLinkedTypefaceCreator*  ptr = new (ELeave) CTLinkedTypefaceCreator(aStep);
	CleanupStack::PushL(ptr);
	ptr->ConstructL();
	CleanupStack::Pop();
	return ptr;
	}	

CTLinkedTypefaceCreator::CTLinkedTypefaceCreator(CTestStep* aStep):iStep(aStep)
	{
		
	}

void CTLinkedTypefaceCreator::ConstructL()
	{
	TDisplayMode mode[4]= {EColor16MAP, EColor16MA, EColor16MU, EColor64K};

	TInt err=KErrNone;
	for (TInt count=0;count<(sizeof(mode)/sizeof(mode[0]));count++)
		{
		TRAP(err, iDev = CFbsScreenDevice::NewL(KNullDesC, mode[count]));
		if (err!=KErrNotSupported)
			{
			return;
			}
		}

	User::LeaveIfError(err);
	}

CTLinkedTypefaceCreator::~CTLinkedTypefaceCreator()
	{
	delete iDev;
	iGroups.ResetAndDestroy();
	iElements.ResetAndDestroy();
	}
	
/** function used to create the linked typefaces
 */
void CTLinkedTypefaceCreator::CreateL()
	{
	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iDev);
	CleanupStack::PushL(store);

	for (TInt counter=0; counter< sizeof(KLinkedTypefaces)/sizeof(TTLinkedTypeface);counter++)
		{
		//creating the groups needs to be done for every typeface since ownwership is taken
		//after adding the group to one linked typeface specification
		//leave on any failure e.g. out of memory, which is not expected
		CreateGroupsL(KGroupList, sizeof(KGroupList)/sizeof(TTGroup*));

		//create the linked typeface elements for this linked typeface
		CreateElementsL(KLinkedTypefaces[counter].iElements);

		//add the groups and elements, but first create the spec
		CLinkedTypefaceSpecification* spec = CLinkedTypefaceSpecification::NewLC(KLinkedTypefaces[counter].iTypefaceName);
		
		//add the groups
		for (TInt groupCounter=0; groupCounter<iGroups.Count();groupCounter++)
			{
			spec->AddLinkedTypefaceGroupL(*iGroups[groupCounter]);//ownership is taken
			iGroups[groupCounter]=NULL;
			}

		//add the elements
		for (TInt elementCounter=0; elementCounter<iElements.Count();elementCounter++)
			{
			spec->AddTypefaceAtBackL(*iElements[elementCounter]);//ownership is taken
			iElements[elementCounter]=NULL;
			}
		iElements.ResetAndDestroy();
		iGroups.ResetAndDestroy();

		//register
		TInt err=KErrNone;
		TRAP(err,spec->RegisterLinkedTypefaceL(*store)); //may fail if something is wrong with the tables, want to try to continue

		//do not report test failure if already registered
		iStep->testBooleanTrue(err==KErrNone || err==KErrAlreadyExists, (TText8*)__FILE__, __LINE__);

		//delete spec
		CleanupStack::PopAndDestroy();
		}
	CleanupStack::PopAndDestroy();//store
	}

/** function used to create the groups, note all groups are created at once, and 
added to all linked typefaces

@param aElement array of pointers to groups
@param aNumberOfElements the number of elements in the group
*/
void CTLinkedTypefaceCreator::CreateGroupsL(const TTGroup* aElement[],TInt aNumberOfElements)
	{
	for (TInt counter =0; counter<aNumberOfElements;counter++)
		{
		CLinkedTypefaceGroup* groupPtr= CLinkedTypefaceGroup::NewLC(counter);
		groupPtr->SetScalingOption(aElement[counter]->iScalingOption);
		groupPtr->SetBaselineShift(aElement[counter]->iBaselineShift);

		if (aElement[counter]->iBoldnessPercentage!=KMinTInt)
			{
			groupPtr->SetBoldnessPercentage(aElement[counter]->iBoldnessPercentage);
			}
		if (aElement[counter]->iItalicAngle!=KMinTInt)
			{
			groupPtr->SetItalicAngle(aElement[counter]->iItalicAngle);
			}	
		iGroups.AppendL(groupPtr);
		CleanupStack::Pop();//ptr is now on the RArray
		}
	}

/** creates all the elements for one typeface
iElements is updated

@param aElements pointer to a list of elements, terminated by a typeface with null name 
*/

void CTLinkedTypefaceCreator::CreateElementsL(const TTLinkedFontElement* aElements)
	{
	const TTLinkedFontElement* ptrElementSpec = aElements;
	
	//need a loop to go through all the typefaces 
	for (;ptrElementSpec->iTypefaceName!=KNullDesC;ptrElementSpec++)
		{
		CLinkedTypefaceElementSpec* elementSpec = CLinkedTypefaceElementSpec::NewLC(ptrElementSpec->iTypefaceName,ptrElementSpec->iGroup);
		elementSpec->SetCanonical(ptrElementSpec->iIsCanonical);
		iElements.AppendL(elementSpec);
		CleanupStack::Pop();
		}
	}

// Code for creating fonts, drawing text, and comparing hash value

//Considering the intersection of the screen size for both hw and emulator so that same
//baselined hash can be used to test for both
#define	KScreenWidth 320
#define	KScreenHeight 240

const TInt KNumOfDisplayModes = 12;

/** table of strings of display modes
 */
const TPtrC KDisplayModeNames[KNumOfDisplayModes] = 
	{
	_L("EGray2"),
	_L("EGray4"),
	_L("EGray16"),
	_L("EGray256"),
	_L("EColor16"),
	_L("EColor256"),
	_L("EColor64K"),
	_L("EColor16M"),
	_L("EColor4K"),
	_L("EColor16MU"),
	_L("EColor16MA"),
	_L("EColor16MAP"),
	};

/** a mapping table index from DisplayMode into the table of strings
for supported colour modes.  
The tables of strings do not include ERgb, or ENone.  The value in the table is also
needed for compatibility with CTHashReferenceImages
*/
const TInt KSupportDisplayStringIndex[14] = 
	{
	0, //ENone - use EGray2 output string 
	0, //EGray2,
	1, //EGray4,
	2, //EGray16,
	3, //EGray256,
	4, //EColor16,
	5, //EColor256,
	6, //EColor64K,
	7, //EColor16M,
	7, //ERgb
	8, //EColor4K,
	9, //EColor16MU,
	10,//EColor16MA,
	11,//EColor16MAP,
 	};

/** a helper cleanup class
 */
typedef struct
	{
	CBitmapDevice* device;
	CFont* font;
	} SCleanFont;

/** path where the output file are to be stored (if this is configured in chashreferenceimages.cpp
*/
_LIT(KPath,"c:\\fntstoretest\\%S.mbm");

/** constructor
@param aStep the test step
 */
CTLinkedFontsComparison::CTLinkedFontsComparison(CTestStep *aStep):CTGraphicsBase(aStep),iStep(aStep)
	{	
	}

/** ConstructL
 */
void CTLinkedFontsComparison::ConstructL()
	{
	iFbs = RFbsSession::GetSession();
	User::LeaveIfNull(iFbs);
	iHashUtil = CTHashReferenceImages::NewL(iStep,iFbs,&KPath);
	}

/** Destructor
 */
CTLinkedFontsComparison::~CTLinkedFontsComparison()
	{
	FreeScreenDevice();
	delete iHashUtil;
	if (iFbs)
		iFbs->Disconnect();
	}
	
/**
Auxilary function will be called in case of a leave to release the font
@param aCleanFont font to be released
*/
void CleanUpFont(TAny* aCleanFont)
	{
	SCleanFont* cleanFont= static_cast<SCleanFont*>(aCleanFont);
	(cleanFont->device)->ReleaseFont(cleanFont->font);
	}


class CFbsFontEx : public CFbsFont
	{
public:
	static CBitmapFont* FontAddress(CFbsFont* aFont);
	};

CBitmapFont* CFbsFontEx::FontAddress(CFbsFont* aFont)
	{
	return(((CFbsFontEx*)aFont)->Address());
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0107

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926, REQ10927, REQ10942, REQ10943, REQ10947, REQ10949

   @SYMTestCaseDesc			This test registers linked typefaces with defintions coming from tables.

   @SYMTestActions 			Register a number of linked typefaces which are defined in a set of tables. 
      				
      						Groups can have the following attributes:
      							CLinkedTypefaceGroup::TBaselineShift
								CLinkedTypefaceGroup::TScalingOption
								AntiAliasingThreshold (integer value)
								BoldnessPercentage (integer value)
								ItalicAngle (integer value)

							Typeface elements can have the following attributes:
								Name
								Group
								Is canonical

							Linked typefaces can have the following attributes:
								Name
								Linked typeface elements
								Groups
 
 							For each linked typeface to be created
 								create linked typeface specification
 								create groups
 								add groups to linked typeface specification
 								create linked typeface elements
 								add linked typeface elements to linked typeface specification
 								register the specification
 
   							To produce new linked typefaces it should be possible to do this by updating
   							the table entries.   							

   							The linked typefaces should be valid.  This test requires that the internal test
   							fonts be present.

   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1)For each linked typeface, create the groups, create the linked typeface elements,
   							create a linked typeface, add the groups and elements, and 
   						 	register the linked typeface with the font and bitmap server.	

   @SYMTestExpectedResults	(1) - linked font registration succeeds with no panics or leaves.  If the test is run a
   								second time then KErrAlreadyExists is returned, which is not a failure.
*/	
void CTLinkedFontsComparison::RegisterLinkedTypefacesL()
	{
	CTLinkedTypefaceCreator* creator = CTLinkedTypefaceCreator::NewL(iStep);
	CleanupStack::PushL(creator);
	creator->CreateL();
	CleanupStack::PopAndDestroy();
	}
	
/** Create the screen device.
@param aMode display mode for which device has to be created
*/
void CTLinkedFontsComparison::CreateBitmapDeviceL(TDisplayMode aMode)
	{
	iBitmap = new(ELeave) CFbsBitmap;
	//Considering the intersection of the screen size for both hw and emulator so that same
	//baselined hash can be used to test for both
	iBitmap->Create(TSize(KScreenWidth, KScreenHeight), aMode);
	iBitmapDevice = CFbsBitmapDevice::NewL(iBitmap);
	User::LeaveIfError(iBitmapDevice->CreateContext((CGraphicsContext*&)iGc));
	iHashUtil->SetScreenDeviceAndBitmap(iBitmapDevice,iBitmap,iGc);
	}

/**
Delete the devices created and gc.
*/
void CTLinkedFontsComparison::FreeScreenDevice()
	{
	delete iBitmapDevice;
	delete iBitmap;
	delete iGc;
	iBitmapDevice = NULL;
	iBitmap = NULL;
	iGc = NULL;
	}

/** Function to generate a meaningful hash string from the test attributes

@param aSize the size of the font
@param aAttributes the attributes of the font 
@see TFontEffectsFlags
@param aTypefaceName the typeface name

@return the buffer with a text description, used for inclusion in the hash string
*/
HBufC* CTLinkedFontsComparison::GenerateTestDescriptionLC(TInt aSize,TUint32 aAttributes, const TPtrC &aTypefaceName, const TDesC &aTestDescription)
	{
	TBuf<KLengthOfHashValue> tempBuffer;
		
	//append the name
	tempBuffer.Append(_L("_"));
	tempBuffer.Append(aTypefaceName);
	tempBuffer.Append(_L("_"));
	tempBuffer.Append(aTestDescription);
	tempBuffer.Append(_L("_"));
	
	//append the size 
	tempBuffer.AppendFormat(_L("_size-%d"),aSize);
	if (aAttributes == 0)
		{
		//output no attributes
		tempBuffer.Append(_L("_"));
		tempBuffer.Append(KFontEffectsFlagsString[0]); //no attributes string
		}
	else
		{
		for (TInt counter=0;counter<31;counter++)
			{
			TInt bit = 1<<counter; //1,2,4,8 etc
			if (bit&aAttributes)
				{
				tempBuffer.Append(_L("_"));
				tempBuffer.Append(KFontEffectsFlagsString[counter+1]);
				}
			}
		}
	return tempBuffer.AllocLC();	
	}

/**
   @SYMTestCaseID			GRAPHICS-FNTSTORE-LINKEDFONTS-0108

   @SYMPREQ					PREQ2146

   @SYMREQ					REQ10924, REQ10925, REQ10926, REQ10927, REQ10942, REQ10943, REQ10947, REQ10949

   @SYMTestCaseDesc			This test draws fonts and compares a hash value from the output against
   							a value stored in a file.
   							
   @SYMTestActions			The fonts used for this test come from a table, and each font can
   							be tested with a different output string, size, and creation flag.
   							The sizes are typically, 10, 20, 35 pixels.
   							
   							Steps for each font in the test table entry:
   							(1)Create the font
   							(2)For each size
   							  	(3) For each attribute
   							  		(4)Draw the text and compare against the stored hash value
   								  	
   							
   							The font creation parameters are typically:
								ENoEffect : plain font
								EIsUnderlineOn : have underline
								EIsStrikethroughOn : have strikethrough
								ESetFourColourBlendGlyphBitmap : for shadow and outline
								EIsUnderlineOn, EIsStrikethroughOn : both underline and strikethrough	
								EAlgorithmicBoldOn : algorithmic bold on
								EVerticalDraw : vertical text
								EVerticalDraw, EIsUnderlineOn : vertical text and underline 
								
							If more fonts or attribute combinations are required to be tested, then this should be
							achievable by adding table entries.
   							
   							To produce the hash values the test needs to be run with a special version 
   							of test harness where APPEND_NEW_OR_MISSING_HASH_DATA and SAVEBITMAP macros are defined
   							in thashreferenceimages.cpp.  The saved mbm files should be converted to bitmap 
   							for manual checking.  							
   							
   							The fonts used for this tests should have been registered beforehand with 
   							test GRAPHICS-FNTSTORE-LINKEDFONTS-0107.

   @SYMTestPriority			Critical

   @SYMTestStatus			Implemented

   @SYMTestActions			(1) Create the font. Draw text, and compare the hash value against
    							the value in the file tlinkedfontcomparison.ini.  Note that having the same hash
    							value as the ini file does not necessarily mean that the output is correct. 

   @SYMTestExpectedResults	(1) - font creation succeeds
   							(4) - the hash values are the same for each text string output
*/
void CTLinkedFontsComparison::TestDrawTextUsingLinkedFontsL()
	{
	INFO_PRINTF1(_L("TestDrawTextUsingLinkedFontsL()"));
	FreeScreenDevice();
	const TDisplayMode displayMode = EColor16MAP;

	CreateBitmapDeviceL(displayMode);
	
	INFO_PRINTF2(_L("Display mode: %S"), &KDisplayModeNames[KSupportDisplayStringIndex[displayMode]]);

	//for all the tests
	TInt numberOfTestsRun=0;
	for (TInt testCounter = 0; testCounter< sizeof(KTestCases)/sizeof(TTestCase); testCounter++)
		{
		
		//for all the sizes
		const TInt* ptrSizes = KTestCases[testCounter].iSizes;
		for (; *ptrSizes!=0 ;ptrSizes++)
			{
			TInt size = *ptrSizes;
			
			// for all the attributes
			const TUint32 *ptrAttributes = KTestCases[testCounter].iFontAttributes;
			for (;*ptrAttributes!=EEndOfList;ptrAttributes++)
														
				{
				TUint32 attributes = *ptrAttributes;
				
				HBufC* testDescription = GenerateTestDescriptionLC(size,attributes,KTestCases[testCounter].iTypeFaceName,*( KTestCases[testCounter].iTestDecription));
				//this function is used for a fuller description
				
				//conversion from HBufC to TPtrC
				//need this an an argument to the generate hash id function
				TPtrC ptr = TPtrC(testDescription->Des());
				
				//construct a desriptor string, function in hashutility KTestName0001
				HBufC* hashIdString = iHashUtil->GenerateHashIdStringLC(
						KTestName0001,//general name for test
						numberOfTestsRun, //number of sub test
						&ptr,
						0, 				 //fontFaceIndex, index into the name array above, must be zero
						KSupportDisplayStringIndex[static_cast<TInt>(displayMode)],//display mode, converted to text
							//in the hash code this is an index to a string table, which is missing ENone and ERgb
						attributes //was orientation, but just integer at end, no use attributes
						);
				
				DoTestDrawingOutputL(*hashIdString, KTestCases[testCounter].iTypeFaceName, *(KTestCases[testCounter].iOutputString),
								  KTestCases[testCounter].iPosition,size,attributes);
				
				numberOfTestsRun++;
				CleanupStack::PopAndDestroy(2,testDescription);
				}
			}//sizes
		}//test array
	}

/**
Function to create a font, draw the text, and compare the hash value

@param aHashId a string with the hash ID.
@param aFont the font name
@param aOutputString the output string
@param aPosition the position where to draw the text
@param aSize the height of the font in pixels
@param aAttributes the attributes for the font 
@see TFontEffectsFlags
*/
void CTLinkedFontsComparison::DoTestDrawingOutputL(const TDesC& aHashId, const TPtrC& aFont, const TDesC& aOutputString,
		 TPoint aPosition, TInt aSize, TUint32 aAttributes)
	{
	TFontSpec fontSpec;
	PrepareTestEnvironment(fontSpec, aFont, aSize, aAttributes, ETrue, ETrue);
	CFont* font;
	TEST(iBitmapDevice->GetNearestFontToDesignHeightInPixels((CFont*&)font, fontSpec) == KErrNone);
	SCleanFont cleanupFont = {iBitmapDevice, font};
	CleanupStack::PushL(TCleanupItem(CleanUpFont, &cleanupFont));
	
	//may need to add attributes for the font
	CBitmapFont* bitmapFont=CFbsFontEx::FontAddress(static_cast<CFbsFont*>(font));
	if (aAttributes& EItalicOn)
		{
		bitmapFont->iAlgStyle.SetIsItalic(ETrue);
		}
	if (aAttributes& ERegularBoldOn)
		{
		bitmapFont->iAlgStyle.SetIsBold(ETrue);
		}	
	
	iGc->SetBrushColor(KRgbWhite);
	iGc->Clear();
	iGc->SetBrushColor(KRgbYellow);
	iGc->UseFont(font);
	if(EVerticalDraw & aAttributes)
		{
		iGc->DrawTextVertical(aOutputString, aPosition, ERotate90Degrees & aAttributes);
		}
	else
		{
		iGc->DrawText(aOutputString, aPosition);
		}
	iHashUtil->CompareHashValuesL(aHashId);
	CleanupStack::Pop();//font
	iBitmapDevice->ReleaseFont(font);
	iGc->DiscardFont();
	}

/**
Sets the fontspec and gc attributes according to the flags set in STestFontParameters for the test.
Before setting gc attributes it is cleared and reset.
@param aFontSpec holds all effective font settings
@param aSettings holds the flags for setting the fontspec and gc
@param aFontIndex the fonts index of KFontFace to be set as typeface
@param aSetEffects ETrue will set the effects on in fontspec if their flags are enabled. By default it is ETrue.
@param aSetColors ETrue will set the colours in gc from STestFontParameters. By default it is EFalse.
*/
void CTLinkedFontsComparison::PrepareTestEnvironment(TFontSpec &aFontSpec, const TPtrC& aFont, TInt aSize, TUint32 aAttributes,
							TBool aSetEffects, TBool aSetColors)
	{
	//Set the typeface from the font face array
	aFontSpec.iTypeface.iName = aFont;
	aFontSpec.iHeight = aSize;

	//Set the effects on if any of flags for the effects are set in aSettings and aSetEffects is ETrue
	if(aSetEffects)
		{
		aFontSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, EDropShadowOn & aAttributes);
		aFontSpec.iFontStyle.SetEffects(FontEffect::EOutline, EOutlineOn & aAttributes);
		aFontSpec.iFontStyle.SetEffects(FontEffect::ESoftEdge, ESoftEdgeOn & aAttributes);
		aFontSpec.iFontStyle.SetEffects(FontEffect::EAlgorithmicBold, EAlgorithmicBoldOn & aAttributes);
		}

	iGc->Reset();
	iGc->Clear();
	iGc->SetPenColor(KRgbBlack);
	iGc->SetStrikethroughStyle((EIsStrikethroughOn & aAttributes) ? EStrikethroughOn : EStrikethroughOff);
	iGc->SetUnderlineStyle((EIsUnderlineOn & aAttributes) ? EUnderlineOn : EUnderlineOff);

	//Set the glyph bitmap type
	if(ESetMonochromeGlyphBitmap & aAttributes)
		{
		aFontSpec.iFontStyle.SetBitmapType(EMonochromeGlyphBitmap);
		}
	else if(ESetAntiAliasedGlyphBitmap & aAttributes)
		{
		aFontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
		}

	//Set the colours in gc if asked for
	if(aSetColors)
		{
		iGc->SetBrushColor(KRgbYellow);
		iGc->SetPenColor(KRgbBlack);
		iGc->SetShadowColor(KRgbGray);
		}
	}

/** Function to determine if the rasterizer has the linked typeface extended interface.

@return ETrue if the linked fonts rasterizer is present. EFalse otherwise
*/
TBool CTLinkedFontsComparison::CheckMonoTypeInstalledL()
	{
	CreateBitmapDeviceL(EColor16MU); //this is just for the bitmap device to get the font

	CFbsTypefaceStore* store = CFbsTypefaceStore::NewL(iBitmapDevice);
	CleanupStack::PushL(store);
	
	// Create typeface to be linked
	_LIT(KLinkedTypefaceTT, "LinkedFont1TT");

	CLinkedTypefaceSpecification *typefaceSpec;
	_LIT(KLinkedFont1TTLinkingTest, "LinkedFont1TTLinkingTest");
		
	typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedFont1TTLinkingTest);
	
	CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(1);
	typefaceSpec->AddLinkedTypefaceGroupL(*group1);
	CleanupStack::Pop();
	
	// Add first typeface
	CLinkedTypefaceElementSpec *elementSpec1;
	elementSpec1 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypefaceTT, 1);
	elementSpec1->SetCanonical(ETrue);
		
	typefaceSpec->AddTypefaceAtBackL(*elementSpec1);
	CleanupStack::Pop(1, elementSpec1);
	
	// Now try regstering the linked typeface
	TRAPD(ret, typefaceSpec->RegisterLinkedTypefaceL(*store));
	CleanupStack::PopAndDestroy(2, store);
	
	if (ret == KErrNone|| ret== KErrAlreadyExists)
		{
		return ETrue;
		}
	else
		{
		INFO_PRINTF1(_L("Linked fonts comparison: no linked fonts rasterizer found"));
		return EFalse;
		}
	}

/** regular RunTestCaseL, a virtual function called by the test framework
 */
void CTLinkedFontsComparison::RunTestCaseL(TInt aCurTestCase)
	{
	((CTLinkedFontsComparisonStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch (aCurTestCase)
		{
		case 1:
			{
			((CTLinkedFontsComparisonStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			if (!CheckMonoTypeInstalledL())
				{
				TestComplete();
				};
			break;
			}
		case 2:
			{
			((CTLinkedFontsComparisonStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0107"));
			RegisterLinkedTypefacesL();
			break;
			}
		case 3:
			{
			((CTLinkedFontsComparisonStep*)iStep)->SetTestStepID(_L("GRAPHICS-FNTSTORE-LINKEDFONTS-0108"));
			TestDrawTextUsingLinkedFontsL();
			break;
			}
		default:
			{
			((CTLinkedFontsComparisonStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTLinkedFontsComparisonStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
			}
		}
	((CTLinkedFontsComparisonStep*)iStep)->RecordTestResultL();
	}

__CONSTRUCT_STEP__(LinkedFontsComparison)
