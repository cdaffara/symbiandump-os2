/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <gdi.h>
#include "TXTFRMAT.H"

#include "TXTSTD.H"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TXTFRMATTraces.h"
#endif



const TInt KParaDefaultLanguage=0;
const TInt KParaDefaultLeftMargin=0;
const TInt KParaDefaultRightMargin=0;
const TInt KParaDefaultIndent=0;
const CParaFormat::TAlignment KParaDefaultHorizAlign=CParaFormat::ELeftAlign;
const CParaFormat::TAlignment KParaDefaultVertAlign=CParaFormat::EUnspecifiedAlign;
const TInt KParaDefaultLineSpacing=200; // 200 twips = 10pt
const CParaFormat::TLineSpacingControl KParaDefaultLineSpacingControl=CParaFormat::ELineSpacingAtLeastInTwips;
const TInt KParaDefaultSpaceBefore=0;
const TInt KParaDefaultSpaceAfter=0;
const TBool KParaDefaultKeepTogether=EFalse;
const TBool KParaDefaultKeepWithNext=EFalse;
const TBool KParaDefaultStartNewPage=EFalse;
const TBool KParaDefaultWidowOrphan=EFalse;
const TBool KParaDefaultWrap=ETrue;
const TInt KParaDefaultBorderMargin=0;
const TInt KParaDefaultTabWidth=360; // 360 twips = 0.25"
const TUint KParFormatBits = (2 << EAttTabStop) - 1;
const TUint KCharFormatBits = (2 << (EAttFontHiddenText - EAttCharLanguage)) - 1;

	
EXPORT_C TTabStop::TTabStop():
	iTwipsPosition(0),
	iType(ELeftTab)
/** The default C++ constructor constructs a TTabStop. The twips position is 
initialised to zero and the alignment to ELeftTab. */
 	{
	}

EXPORT_C TTabStop::TTabStop(const TTabStop& aTabStop):
	iTwipsPosition(aTabStop.iTwipsPosition),
	iType(aTabStop.iType)
	{
	}

EXPORT_C TTabStop& TTabStop::operator=(const TTabStop& aTabStop)
/** Assigns the twips position and alignment of aTabStop to the current TTabStop.

@param aTabStop The tab stop to assign to the current tab stop. 
@return The current tab stop. */
	{
	iTwipsPosition=aTabStop.iTwipsPosition;
	iType=aTabStop.iType;
	return *this;
	}

EXPORT_C TBool TTabStop::operator==(const TTabStop& aTabStop)const
/** Compares two tab stops for equality. To be equal, they must have the same 
twips position and alignment.

@param aTabStop The tab stop to compare with the current tab stop. 
@return ETrue if both tab stops have the same twips position and alignment. 
EFalse if not. */
	{
	if (iTwipsPosition!=aTabStop.iTwipsPosition)
		return EFalse;
	if (iType!=aTabStop.iType)
		return EFalse;
	return ETrue;
	}

EXPORT_C TParaBorder::TParaBorder():
	iLineStyle(ENullLineStyle),
	iThickness(0),
	iColor(TLogicalRgb::ESystemForegroundColor),
	iAutoColor(ETrue)
/** The default C++ constructor constructs a TParaBorder, initializing its line 
style to ENullLineStyle, its line thickness to zero, its colour to KRgbBlack 
and iAutocolor to ETrue. */
	{
	}

EXPORT_C TBool TParaBorder::operator==(const TParaBorder& aParaBorder)const
/** Compares two paragraph border sides for equality. For two paragraph border 
sides to be equal, all data members must be equal.

@param aBorder The paragraph border to compare with the current border.
@return ETrue if the two paragraph border sides are equal, EFalse if not. */
	{
	if (iLineStyle!=aParaBorder.iLineStyle)
		return EFalse;
	if (iThickness!=aParaBorder.iThickness)
		return EFalse;
	if (iColor!=aParaBorder.iColor)
		return EFalse;
	if (iAutoColor!=aParaBorder.iAutoColor)
		return EFalse;
	return ETrue;
	}

EXPORT_C TBullet::TBullet():
	iCharacterCode(0x2022),
	iHeightInTwips(0),
	iHangingIndent(TRUE),
	iColor(TLogicalRgb::ESystemForegroundColor),
	iStyle(EBulletStyle),
	iStartNumber(1),
	iAlignment(ELeftAlign)
/** The default C++ constructor constructs a TBullet, initializing the 
character code to 0x2022, the height to zero, the colour to the system's 
default foreground colour and the hanging indent to ETrue. The typeface 
is not initialised. */
	{
	}

EXPORT_C TBool TBullet::operator==(const TBullet& aBullet)const
/** Compares two bullet points for equality. For two bullet points to be equal, 
all data members must be equal.

@param aBullet The bullet point to compare. 
@return ETrue if the two bullet points are equal, EFalse if not. */
	{
	return iCharacterCode == aBullet.iCharacterCode &&
		   iHeightInTwips == aBullet.iHeightInTwips &&
		   iHangingIndent == aBullet.iHangingIndent &&
		   iColor == aBullet.iColor &&
		   iStyle == aBullet.iStyle &&
		   iStartNumber == aBullet.iStartNumber &&
		   iAlignment == aBullet.iAlignment &&
		   iTypeface == aBullet.iTypeface;
	}

EXPORT_C CParaFormat* CParaFormat::NewL()
/** Allocates and constructs a CParaFormat object. All attributes are 
initialised with default values.

@return The new CParaFormat object. */
	{
	return new(ELeave) CParaFormat;
	}

EXPORT_C CParaFormat* CParaFormat::NewLC()
/** Allocates and constructs a CParaFormat object. All attributes are 
initialised with default values. Leaves the object on the cleanup stack.

@return The new CParaFormat object. */
	{
	CParaFormat* self=new(ELeave) CParaFormat;
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CParaFormat* CParaFormat::NewL(const CParaFormat& aFormat)
/** Allocates and constructs a new CParaFormat. All attributes are initialised 
to the values contained in the aFormat argument.

@param aFormat Paragraph format container whose values are used to initialise 
the new CParaFormat. 
@return The new CParaFormat object. */
	{
	CParaFormat* self = new(ELeave) CParaFormat(aFormat);
	CleanupStack::PushL(self);
	self->CopyL(aFormat);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CParaFormat::CParaFormat():
	iTabList(NULL),
	iParaBorderArray(NULL),
	iFillColor(TLogicalRgb::ESystemBackgroundColor),
	iLanguage(KParaDefaultLanguage),
	iLeftMarginInTwips(KParaDefaultLeftMargin),
	iRightMarginInTwips(KParaDefaultRightMargin),
	iIndentInTwips(KParaDefaultIndent),
	iHorizontalAlignment(KParaDefaultHorizAlign),
	iVerticalAlignment(KParaDefaultVertAlign),
	iLineSpacingInTwips(KParaDefaultLineSpacing),
	iLineSpacingControl(KParaDefaultLineSpacingControl),
	iSpaceBeforeInTwips(KParaDefaultSpaceBefore),
	iSpaceAfterInTwips(KParaDefaultSpaceAfter),
	iKeepTogether(KParaDefaultKeepTogether),
	iKeepWithNext(KParaDefaultKeepWithNext),
	iStartNewPage(KParaDefaultStartNewPage),
	iWidowOrphan(KParaDefaultWidowOrphan),
	iWrap(KParaDefaultWrap),
	iBorderMarginInTwips(KParaDefaultBorderMargin),
	iBullet(NULL),
	iDefaultTabWidthInTwips(KParaDefaultTabWidth)
/** The default C++ constructor constructs a new CParaFormat initialising all 
attributes to the default settings.

Note: This function allows a CParaFormat object to be created on the stack. This 
should only be done if it is known in advance that the object will not be 
used to store tab stops, bullets or borders. */
	{
	}


CParaFormat::CParaFormat(const CParaFormat& aFormat):
	iFillColor(aFormat.iFillColor),
	iLanguage(aFormat.iLanguage),
	iLeftMarginInTwips(aFormat.iLeftMarginInTwips),
	iRightMarginInTwips(aFormat.iRightMarginInTwips),
	iIndentInTwips(aFormat.iIndentInTwips),
	iHorizontalAlignment(aFormat.iHorizontalAlignment),
	iVerticalAlignment(aFormat.iVerticalAlignment),
	iLineSpacingInTwips(aFormat.iLineSpacingInTwips),
	iLineSpacingControl(aFormat.iLineSpacingControl),
	iSpaceBeforeInTwips(aFormat.iSpaceBeforeInTwips),
	iSpaceAfterInTwips(aFormat.iSpaceAfterInTwips),
	iKeepTogether(aFormat.iKeepTogether),
	iKeepWithNext(aFormat.iKeepWithNext),
	iStartNewPage(aFormat.iStartNewPage),
	iWidowOrphan(aFormat.iWidowOrphan),
	iWrap(aFormat.iWrap),
	iBorderMarginInTwips(aFormat.iBorderMarginInTwips),
	iDefaultTabWidthInTwips(aFormat.iDefaultTabWidthInTwips)
	{
	}

void CParaFormat::CreateTabListL()
	{
	if (!iTabList)
		iTabList = new(ELeave) CArrayFixFlat<TTabStop>(ETabStoreGranularity);
	}
 
EXPORT_C CParaFormat::~CParaFormat()
/** The destructor frees all resources owned by the paragraph format container 
(tabs, borders and bullets), prior to its destruction. 

Note that Strip() also sets the resource pointers to NULL. This is important 
in case CParaFormat is on the stack and gets deleted twice: once by the cleanup 
stack and once by exceptions being unwound. */
	{
	Strip();
	}

EXPORT_C void CParaFormat::CopyL(const CParaFormat& aFormat)
/** Copies all attribute values from another paragraph format container.

@param aFormat Contains the attribute values to copy. */
	{
	TParaFormatMask mask;
	mask.SetAll();
	CopyL(aFormat,mask);
	}

EXPORT_C void CParaFormat::CopyL(const CParaFormat& aFormat,const TParaFormatMask& aMask)
/** Copies selected attribute values from another paragraph format container. 
Only the attributes which are set in the mask are copied.

@param aFormat Contains the attribute values to copy. 
@param aMask Bitmask specifying the attributes to copy. */
	{
	if (aMask.AttribIsSet(EAttParaLanguage))
		iLanguage=aFormat.iLanguage;
	if (aMask.AttribIsSet(EAttFillColor))
		iFillColor=aFormat.iFillColor;
	if (aMask.AttribIsSet(EAttLeftMargin))
		iLeftMarginInTwips=aFormat.iLeftMarginInTwips;
	if (aMask.AttribIsSet(EAttRightMargin))
		iRightMarginInTwips=aFormat.iRightMarginInTwips;
	if (aMask.AttribIsSet(EAttIndent))
		iIndentInTwips=aFormat.iIndentInTwips;
	if (aMask.AttribIsSet(EAttAlignment))
		iHorizontalAlignment=aFormat.iHorizontalAlignment;
	if (aMask.AttribIsSet(EAttVerticalAlignment))
		iVerticalAlignment=aFormat.iVerticalAlignment;
	if (aMask.AttribIsSet(EAttLineSpacing))
		iLineSpacingInTwips=aFormat.iLineSpacingInTwips;
	if (aMask.AttribIsSet(EAttLineSpacingControl))
		iLineSpacingControl=aFormat.iLineSpacingControl;
	if (aMask.AttribIsSet(EAttSpaceBefore))
		iSpaceBeforeInTwips=aFormat.iSpaceBeforeInTwips;
	if (aMask.AttribIsSet(EAttSpaceAfter))
		iSpaceAfterInTwips=aFormat.iSpaceAfterInTwips;
	if (aMask.AttribIsSet(EAttKeepTogether))
		iKeepTogether=aFormat.iKeepTogether;
	if (aMask.AttribIsSet(EAttKeepWithNext))
		iKeepWithNext=aFormat.iKeepWithNext;
	if (aMask.AttribIsSet(EAttStartNewPage))
		iStartNewPage=aFormat.iStartNewPage;
	if (aMask.AttribIsSet(EAttWidowOrphan))
		iWidowOrphan=aFormat.iWidowOrphan;
	if (aMask.AttribIsSet(EAttWrap))
		iWrap=aFormat.iWrap;

	// Copy borders; create a border array only if necessary.
	if (iParaBorderArray || aFormat.iParaBorderArray)
		{
		if (!iParaBorderArray)
			iParaBorderArray = new(ELeave) TParaBorderArray;
		if (aMask.AttribIsSet(EAttTopBorder))
			iParaBorderArray->iBorder[EParaBorderTop] = aFormat.ParaBorder(EParaBorderTop);
		if (aMask.AttribIsSet(EAttBottomBorder))
			iParaBorderArray->iBorder[EParaBorderBottom] = aFormat.ParaBorder(EParaBorderBottom);
		if (aMask.AttribIsSet(EAttLeftBorder))
			iParaBorderArray->iBorder[EParaBorderLeft] = aFormat.ParaBorder(EParaBorderLeft);
		if (aMask.AttribIsSet(EAttRightBorder))
			iParaBorderArray->iBorder[EParaBorderRight] = aFormat.ParaBorder(EParaBorderRight);
		TParaBorder default_border;
		TBool borders_are_default = TRUE;
		for (int i = 0; i < 4; i++)
			if (iParaBorderArray->iBorder[i] != default_border)
				{
				borders_are_default = FALSE;
				break;
				}
		if (borders_are_default)
			RemoveAllBorders();
		}

	if (iBullet || aFormat.iBullet)
		{
		if (aMask.AttribIsSet(EAttBullet))
			{
			if (aFormat.iBullet && aFormat.iBullet->iStyle != TBullet::ENullStyle)
				{
				if (!iBullet)
					iBullet = new(ELeave) TBullet;
				*iBullet = *aFormat.iBullet;
				}
			else
				{
				delete iBullet;
				iBullet = NULL;
				}
			}
		}

	if (aMask.AttribIsSet(EAttDefaultTabWidth))
		iDefaultTabWidthInTwips = aFormat.iDefaultTabWidthInTwips;

	if (iTabList || aFormat.iTabList)
		{
		if (aMask.AttribIsSet(EAttTabStop))
			{
			RemoveAllTabs();
			if (aFormat.iTabList && aFormat.iTabList->Count())
				{
				CreateTabListL();
				iTabList->InsertL(0,&(aFormat.iTabList->At(0)),aFormat.iTabList->Count());
				}
			}
		}
	}

EXPORT_C TBool CParaFormat::IsEqual(const CParaFormat& aFormat,const TParaFormatMask& aMask) const
/** Compares selected attribute values for equality. Only the attributes 
specified in the mask are involved in the comparison.

@param aFormat Contains the attribute values to compare. 
@param aMask Bitmask specifying the attributes to compare. 
@return ETrue if the two format containers have the same values for the
attributes specified in the mask, EFalse if not. */
	{
	if (aMask.AttribIsSet(EAttParaLanguage))
		{
		if (iLanguage!=aFormat.iLanguage)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFillColor))
		{
		if (iFillColor!=aFormat.iFillColor)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttLeftMargin))
		{
		if (iLeftMarginInTwips!=aFormat.iLeftMarginInTwips)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttRightMargin))
		{
		if (iRightMarginInTwips!=aFormat.iRightMarginInTwips)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttIndent))
		{
		if (iIndentInTwips!=aFormat.iIndentInTwips)	
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttAlignment))
		{
		if (iHorizontalAlignment!=aFormat.iHorizontalAlignment)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttVerticalAlignment))
		{
		if (iVerticalAlignment!=aFormat.iVerticalAlignment)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttLineSpacing))
		{
		if (iLineSpacingInTwips!=aFormat.iLineSpacingInTwips)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttLineSpacingControl))
		{
		if (iLineSpacingControl!=aFormat.iLineSpacingControl)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttSpaceBefore))
		{
		if (iSpaceBeforeInTwips!=aFormat.iSpaceBeforeInTwips)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttSpaceAfter))
		{
		if (iSpaceAfterInTwips!=aFormat.iSpaceAfterInTwips)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttKeepTogether))
		{
		if (iKeepTogether!=aFormat.iKeepTogether)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttKeepWithNext))
		{
		if (iKeepWithNext!=aFormat.iKeepWithNext)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttStartNewPage))
		{
		if (iStartNewPage!=aFormat.iStartNewPage)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttWidowOrphan))
		{
		if (iWidowOrphan!=aFormat.iWidowOrphan)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttWrap))
		{
		if (iWrap!=aFormat.iWrap)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttBorderMargin))
		{
		if (iBorderMarginInTwips!=aFormat.iBorderMarginInTwips)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttTopBorder))
		{
		if (iParaBorderArray)
			{
			if (ParaBorder(EParaBorderTop)!=aFormat.ParaBorder(EParaBorderTop))
				return EFalse;
			}
		else
			{
			if (aFormat.ParaBorder(EParaBorderTop).iLineStyle!=TParaBorder::ENullLineStyle)
				return EFalse;
			}
		}
	if (aMask.AttribIsSet(EAttBottomBorder))
		{
		if (iParaBorderArray)
			{
			if (ParaBorder(EParaBorderBottom)!=aFormat.ParaBorder(EParaBorderBottom))
				return EFalse;
			}
		else
			{
			if (aFormat.ParaBorder(EParaBorderBottom).iLineStyle!=TParaBorder::ENullLineStyle)
				return EFalse;
			}
		}
	if (aMask.AttribIsSet(EAttLeftBorder))
		{
		if (iParaBorderArray)
			{
			if (ParaBorder(EParaBorderLeft)!=aFormat.ParaBorder(EParaBorderLeft))
				return EFalse;
			}
		else
			{
			if (aFormat.ParaBorder(EParaBorderLeft).iLineStyle!=TParaBorder::ENullLineStyle)
				return EFalse;
			}
		}
	if (aMask.AttribIsSet(EAttRightBorder))
		{
		if (iParaBorderArray)
			{
			if (ParaBorder(EParaBorderRight)!=aFormat.ParaBorder(EParaBorderRight))
				return EFalse;
			}
		else
			{
			if (aFormat.ParaBorder(EParaBorderRight).iLineStyle!=TParaBorder::ENullLineStyle)
				return EFalse;
			}
		}
	if (aMask.AttribIsSet(EAttBullet))
		{
		if (iBullet)
			{
			if (!aFormat.iBullet)
				return EFalse;
			if (*iBullet!=*aFormat.iBullet)
				return EFalse;
			}
		else
			{
			if (aFormat.iBullet)
				return EFalse;
			}
		}
	if (aMask.AttribIsSet(EAttDefaultTabWidth))
		{
		if (iDefaultTabWidthInTwips!=aFormat.iDefaultTabWidthInTwips)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttTabStop))
		{
		if (TabCount()>0)
			{// Check the tablists are the same.
			if (TabCount()!=aFormat.TabCount())
				return EFalse;
			TInt tabCount=TabCount();
			for (TInt index=0;index<tabCount;index++)
				{// Check each stored tab is the same
				if (TabStop(index)!=aFormat.TabStop(index))
					return EFalse;
				}
			}
		else if (aFormat.TabCount()>0)
			return EFalse;
		}
	return ETrue;
	}

EXPORT_C TBool CParaFormat::IsEqual(const CParaFormat& aFormat) const
/** Compares all attribute values for equality.

@param aFormat Contains the attribute values to compare. 
@return ETrue if the two format containers have the same values for all 
attributes, EFalse if not. */
	{
	TParaFormatMask mask;
	mask.SetAll();
	return IsEqual(aFormat,mask);
	}
 
EXPORT_C void CParaFormat::Strip()
/** Deletes all paragraph borders, bullets and tab stops. No other 
attributes are affected. */
	{
	RemoveAllTabs();
	RemoveAllBorders();
	delete iBullet;
	iBullet = NULL;
	}

EXPORT_C void CParaFormat::Reset()
/** Resets all paragraph format attributes to their default values. All tab 
stops, paragraph borders and bullet points which have been allocated are 
deleted and set to NULL. */
	{
	ResetNonDestructive();
	Strip();
	}
	
EXPORT_C void CParaFormat::ResetNonDestructive()
/** Resets all paragraph format attributes to their default values, but any 
allocated tab stops, bullet points and paragraph borders are preserved. */
	{
	iLanguage=KParaDefaultLanguage;
	iFillColor=TLogicalRgb::ESystemBackgroundColor;
	iLeftMarginInTwips=KParaDefaultLeftMargin;
	iRightMarginInTwips=KParaDefaultRightMargin;
	iIndentInTwips=KParaDefaultIndent;
	iHorizontalAlignment=KParaDefaultHorizAlign;
	iVerticalAlignment=KParaDefaultVertAlign;
	iLineSpacingInTwips=KParaDefaultLineSpacing;
	iLineSpacingControl=KParaDefaultLineSpacingControl;
	iSpaceBeforeInTwips=KParaDefaultSpaceBefore;
	iSpaceAfterInTwips=KParaDefaultSpaceAfter;
	iKeepTogether=KParaDefaultKeepTogether;
	iKeepWithNext=KParaDefaultKeepWithNext;
	iStartNewPage=KParaDefaultStartNewPage;
	iWidowOrphan=KParaDefaultWidowOrphan;
	iWrap=KParaDefaultWrap;
	iBorderMarginInTwips=KParaDefaultBorderMargin;
	iDefaultTabWidthInTwips=KParaDefaultTabWidth;
	}

EXPORT_C void CParaFormat::StoreTabL(const TTabStop& aTabStop)
/** Adds a tab stop to the list of tab stops, maintaining the ordering of the 
list, (ascending order of twips position). Multiple tabs with the same twips 
position are not allowed, so that if aTabStop shares the same twips position 
as an existing tab stop, regardless of its alignment, the existing tab stop is 
replaced by aTabStop.

@param aTabStop The tab stop to be stored. */
	{
	CreateTabListL();
	TKeyArrayFix tabKey(_FOFF(TTabStop,iTwipsPosition),ECmpTUint32);
	TInt tabNumber;
	CArrayFixFlat<TTabStop>& tabs=*iTabList;
	if (tabs.FindIsq(aTabStop,tabKey,tabNumber)==0)
		tabs[tabNumber]=aTabStop;		// found one at this position
	else
		tabs.InsertL(tabNumber,aTabStop);	// add the new one
	}

EXPORT_C void CParaFormat::RemoveTab(TInt aTabPosition)
/** Deletes a tab stop identified by its twips position. If the specified 
tab stop does not exist, the function has no effect.
	
@param aTabTwipsPosition The twips position of the tab stop to remove. */

	{
	TInt tabNumber=LocateTab(aTabPosition);  // will return KTabNotFound if no tab list present
	if (tabNumber!=KTabNotFound)
		{
		iTabList->Delete(tabNumber);
		iTabList->Compress();
		}
	}

EXPORT_C const TTabStop CParaFormat::TabStop(TInt aTabIndex) const
/** Gets the tab stop located at the specified index within the tab list 
(counting from zero). Tab stops are ordered in ascending order of twips 
position. If the object has no tab list, then a default tab stop is returned.

@param aTabIndex The offset of the tab stop in the tab list. Must be less 
than the total number of tab stops, or a panic occurs. To find the total number 
of tab stops, use TabCount(). 
@return The tab stop located at the specified index. */
	{
	if (aTabIndex>=TabCount())
	    {
	    OstTrace0( TRACE_FATAL, CPARAFORMAT_TABSTOP, "ETabNotFound" );
	    }
	__ASSERT_ALWAYS(aTabIndex<TabCount(),Panic(ETabNotFound));
	
	return (iTabList)
		? TTabStop((*iTabList)[aTabIndex])
		: TTabStop();
	}

EXPORT_C TInt CParaFormat::LocateTab(TInt aTabPosition) const
/** Locates the tab stop specified by its twips position, and returns its
offset in the tab list.

@param aTabPosition The twips position of the tab stop.
@return The tab stop's index within the tab list (counting from zero). 
KTabNotFound indicates that  no tab stop has the specified twips 
position, or that no tab list has been allocated. */
	{
	if (!iTabList)
		return KTabNotFound;
	TKeyArrayFix tabKey(_FOFF(TTabStop,iTwipsPosition),ECmpTUint32);
	TInt tabNumber=0;
	TTabStop tab;
	tab.iTwipsPosition=aTabPosition;
	TInt error=iTabList->FindIsq(tab,tabKey,tabNumber);
	return error ?KTabNotFound :tabNumber;
	}

EXPORT_C void CParaFormat::SetParaBorderL(TParaBorderSide aSide,const TParaBorder& aBorder)
/** Sets one side of the object's paragraph border. If a border on the specified 
side already exists, it is replaced.

Note: Setting a single side of the object's paragraph border incurs the overhead 
of allocating storage for the three other sides, which are assigned default 
values.

@param aSide The side for the paragraph border. 
@param aBorder Specification for the paragraph border. */
	{
	TParaBorderArray* borders=iParaBorderArray;
	if (!borders)
		iParaBorderArray=borders=new(ELeave) TParaBorderArray;
	borders->iBorder[aSide]=aBorder;
	}

EXPORT_C const TParaBorder CParaFormat::ParaBorder(TParaBorderSide aSide)const
/** Gets the paragraph border on the side specified. If no paragraph border 
array has been allocated, returns a default paragraph border.

@param aSide The side for the paragraph border. 
@return The paragraph border on the specified side. */
	{
	if (iParaBorderArray)
		return iParaBorderArray->iBorder[aSide];
	return TParaBorder();
	}

EXPORT_C void CParaFormat::RemoveAllBorders()
/** Deletes all paragraph borders. */
	{
	delete iParaBorderArray;
	iParaBorderArray = NULL;
	}

EXPORT_C TBool CParaFormat::AllBordersEqual(const CParaFormat& aFormat)const
/** Tests whether all paragraph borders in the specified paragraph format 
container are identical to the paragraph borders of this paragraph format 
container.

@param aFormat Contains the set of paragraph borders to compare. 
@return ETrue if both objects have exactly the same set of paragraph borders. 
EFalse if not. */
	{
	if (aFormat.ParaBorder(EParaBorderTop)!=ParaBorder(EParaBorderTop))
		return EFalse;
	if (aFormat.ParaBorder(EParaBorderBottom)!=ParaBorder(EParaBorderBottom))
		return EFalse;
	if (aFormat.ParaBorder(EParaBorderLeft)!=ParaBorder(EParaBorderLeft))
		return EFalse;
	if (aFormat.ParaBorder(EParaBorderRight)!=ParaBorder(EParaBorderRight))
		return EFalse;
	return ETrue;
	}
 
EXPORT_C TBool CParaFormat::IsBorderEqual(TParaBorderSide aSide,const CParaFormat& aFormat)const
/** Tests whether the paragraph border located on the specified side is the same 
as the border on the corresponding side in this object. For two borders to 
be equal, they must both either be set or unset, and if set, they must have 
the same characteristics.

@param aSide Indicates which side should be compared. 
@param aFormat Contains the paragraph border to compare. 
@return ETrue if the border sides are identical. EFalse if not. */
	{
	return aFormat.ParaBorder(aSide)==ParaBorder(aSide);
	}

 EXPORT_C void TParaFormatMask::SetAll()
/** Sets all attribute flags in the paragraph format mask. */
	{
	iGuard = KMaxTUint;
	}
 
EXPORT_C void TParaFormatMask::ClearAll()
/** Clears all attribute flags in the paragraph format mask. */
	{
	iGuard=0;
	}

EXPORT_C  TBool TParaFormatMask::operator==(const TParaFormatMask& aMask)const
/** Compares two paragraph format masks for equality.

@param aMask The mask to compare. 
@return ETrue if all flags are the same in both masks. EFalse if any differ. */
	{
	return (iGuard & KParFormatBits) == (aMask.iGuard & KParFormatBits);
	}

EXPORT_C TFontPresentation::TFontPresentation():
	iTextColor(TLogicalRgb::ESystemForegroundColor),
	iHighlightColor(TLogicalRgb::ESystemForegroundColor),
	iHighlightStyle(EFontHighlightNone),
	iStrikethrough(EStrikethroughOff),
	iUnderline(EUnderlineOff),
	iHiddenText(EFalse),
	iPictureAlignment(EAlignBaseLine)
/** The default C++ constructor constructs a TFontPresentation object, 
initializing all member data to default values. For details of these values, 
see the table below. */
	{
	}

EXPORT_C TBool TFontPresentation::IsEqual(const TFontPresentation& aFontPresentation,const TCharFormatMask& aMask) const
/** Compares selected attribute values for equality. Only the attributes 
specified in the mask are involved in the comparison.

@param aFontPresentation Contains the attribute values to compare. 
@param aMask Bitmask specifying the attributes involved in the comparison. 
@return ETrue if the two objects have the same values for the attributes 
specified in the mask, EFalse if not. */
	{
	// Compare two font presentations, comparing only the attributes selected by the mask.
	if (aMask.AttribIsSet(EAttColor))
		{
		if (iTextColor!=aFontPresentation.iTextColor)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontHighlightColor))
		{
		if (iHighlightColor!=aFontPresentation.iHighlightColor)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontHighlightStyle))
		{
		if (iHighlightStyle!=aFontPresentation.iHighlightStyle)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontStrikethrough))
		{
		if (iStrikethrough!=aFontPresentation.iStrikethrough)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontUnderline))
		{
		if (iUnderline!=aFontPresentation.iUnderline)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontHiddenText))
		{
		if (iHiddenText!=aFontPresentation.iHiddenText)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontPictureAlignment))
		{
		if (iPictureAlignment!=aFontPresentation.iPictureAlignment)
			return EFalse;
		}
	return ETrue;
	}

EXPORT_C TCharFormat::TCharFormat():
	iLanguage(0)
/** Allocates and constructs a TCharFormat object. The font-independent 
attributes are initialised with default values. The language is initialised 
to zero. The font (iFontSpec) is set to be proportional and serif and to have 
a height of 200 twips. The typeface name is not initialised. */
	{
	iFontSpec.iTypeface.SetAttributes(TTypeface::EProportional | TTypeface::ESerif);
	iFontSpec.iHeight = 200;
	}

EXPORT_C TCharFormat::TCharFormat(const TDesC &aTypefaceName,TInt aHeight):
	iLanguage(0),
	iFontSpec(aTypefaceName,aHeight)
/** The C++ constructor is used to construct the TCharFormat object with a font 
(typeface name and font height). The font-independent attributes are initialised 
to default values; for details, see class TFontPresentation. The language 
is initialised to zero.

@param aTypefaceName Specifies the typeface name. 
@param aHeight Specifies the font height in twips. */
	{
	}

EXPORT_C TBool TCharFormat::IsEqual(const TCharFormat& aFormat,const TCharFormatMask& aMask) const
/** Compares selected attribute values for equality. Only the attributes 
specified in the mask are involved in the comparison.

@param aFormat Contains the attribute values to compare. 
@param aMask Bitmask specifying the attributes to compare. 
@return ETrue if the two format containers have the same values for the 
attributes specified in the mask, EFalse if not. */
	{
	if (aMask.AttribIsSet(EAttCharLanguage))
		{
		if (iLanguage!=aFormat.iLanguage)
			return EFalse;
		}
	if (!iFontPresentation.IsEqual(aFormat.iFontPresentation,aMask))
		return EFalse;
	if (aMask.AttribIsSet(EAttFontHeight))
		{
		if (iFontSpec.iHeight!=aFormat.iFontSpec.iHeight)
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontTypeface))
		{
		if (!(iFontSpec.iTypeface==aFormat.iFontSpec.iTypeface))
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontPosture))
		{
		if (iFontSpec.iFontStyle.Posture()!=aFormat.iFontSpec.iFontStyle.Posture())
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontStrokeWeight))
		{
		if (iFontSpec.iFontStyle.StrokeWeight()!=aFormat.iFontSpec.iFontStyle.StrokeWeight())
			return EFalse;
		}
	if (aMask.AttribIsSet(EAttFontPrintPos))
		{
		if (iFontSpec.iFontStyle.PrintPosition()!=aFormat.iFontSpec.iFontStyle.PrintPosition())
			return EFalse;
		}
	return ETrue;
	}


EXPORT_C TBool TCharFormat::IsEqual(const TCharFormat& aFormat) const
/** Compares all attribute values for equality.

@param aFormat Contains the attribute values to compare. 
@return ETrue if the two format containers have the same values for all 
attributes, EFalse if not. */
	{
	TCharFormatMask mask;
	mask.SetAll();
	return IsEqual(aFormat,mask);
	}

EXPORT_C void TCharFormatMask::SetAll()
/** Sets all attribute flags in the character format mask. */
	{
	iGuard = KMaxTUint;
	}

EXPORT_C void TCharFormatMask::ClearAll()
/** Clears all attribute flags in the character format mask. */
	{
	iGuard = 0;
	}

EXPORT_C  TBool TCharFormatMask::operator==(const TCharFormatMask& aMask)const
/** Compares two character format masks for equality.

@param aMask The mask to compare. 
@return ETrue if all flags are the same in both masks. EFalse if any differ. */
	{
	return (iGuard & KCharFormatBits) == (aMask.iGuard & KCharFormatBits);
	}
