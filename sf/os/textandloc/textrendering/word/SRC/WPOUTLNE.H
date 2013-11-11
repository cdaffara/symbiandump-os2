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


#if !defined(__WPOUTLNE_H__)
#define __WPOUTLNE_H__


#if !defined(__EIKHLBM_H__)
#include <techview/eikhlbm.h>
#endif
#if !defined(__EIKHLBX_H__)
#include <techview/eikhlbx.h>
#endif
#if !defined(__EIKHLBI_H__)
#include <techview/eikhlbi.h>
#endif
#if !defined(__EIKHLBV_H__)
#include <techview/eikhlbv.h>
#endif
#if !defined(__TXTRICH_H__)
#include <txtrich.h>
#endif
#if !defined(__TXTFMLYR_H__)
#include <txtfmlyr.h>
#endif
#if !defined(__BARSREAD_H__)
#include <barsread.h>
#endif


class CRichText;
class COutlineHListBox;	
class COutlineHListBoxView;
class TResourceReader;

/**
@internalComponent
*/
const TInt KErrNoOutline=-50;


class TOutlineEntry
/**
TOutlineEntry
An entry in the Outline Table - used to store info for each paragraph in the document
which has an outline level 
@internalComponent
*/
{
public:
	TOutlineEntry(TInt aParagraphNo, TInt aOutlineLevel, TInt aParaLen, TInt aParaPos, TBool aChild, TBool aLast);
	TOutlineEntry();
public:
	TInt iParagraphNo;		// The pargagraph number for this entry
	TInt iOutlineLevel;		// The outline level for this paragraph
	TInt iParagraphLength;	// No. of chars in this paragraph
	TInt iParagraphPos;		// Char position of start of paragraph
	TBool iIsChild;			// Is this a child of the previous paragraph
	TBool iLastEntry;		// Is this the last entry at this level
};



class TOutlineMap
/**
TOutlineMap
An entry in the Outline Map Table - maps an index into the Outline Table to 
an entry in the hierarchical list box
@internalComponent
*/
{
public:
	TOutlineMap(TInt aIndex, CHierListItem* aPtr);
public:
	TInt iIndex;			// Index reference
	CHierListItem*	iPtr;	// Pointer to a list item
};

/**
@internalComponent
*/
typedef CArrayFixFlat<TOutlineEntry> COutlineTable;
typedef CArrayFixFlat<TOutlineMap> COutlineMapTable;


class COutlineMap : public CBase
/**
COutlineMap
Contains an array of TOutlineMap objects, with functions to bind entries
together.
@internalComponent
*/
{
public:
static COutlineMap* NewL();
	~COutlineMap();
	void BindL(TInt aIndex, CHierListItem* aPtr);	// Bind a list item with the paragraph index
	TInt Index(CHierListItem* aPtr);				// Given a pointer, return the index
	CHierListItem* Ptr(TInt aIndex);				// Given an index, return the pointer
	TBool IndexPresent(TInt aIndex);				// Has this index been added yet?
private:
	void ConstructL();
private:
	COutlineMapTable* iOutlineMapTable;
};



class COutlineHListBoxModel : public CHierarchicalListBoxModel
/**
COutlineHListBoxModel
This is the list box model which defines the behaviour of an outline list box
@internalComponent
*/
{
public:
	static COutlineHListBoxModel* NewL(COutlineHListBox* aParent,CRichText* aText);
	~COutlineHListBoxModel();
	void CreateRootListL();
	void ExpandItemL(TInt aItemIndex);
	TInt ExpandCurrentPositionL(TInt aCursorPos);
	TInt CurrentCharPosition();
	TInt GetItemByParagraph(TInt aPara);
private:
	void ConstructL();
	void CreateEntryTableL();
	void ScanHeading(TDes& aText);
	TInt ExpandParagraphL(TInt aPara);
public:
	COutlineTable* iOutlineTable;	// Table of outline levels in the document
	COutlineMap*	iOutlineMap;	// Table to map between the outline table and the listbox items
private:
	CRichText* iText;
	TInt iCursorPos;
	COutlineHListBox* iParent;
};



class COutlineHListBox : public CEikHierarchicalListBox
/**
COutlineHListBox
Parent control - owns the model and view of the listbox
@internalComponent
*/
{
public:
	COutlineHListBox(CRichText* aText, TInt aCursorPos);
	void ConstructL();
	void ConstructFromResourceL(TResourceReader& aReader);
	COutlineHListBoxModel* OutlineModel() const;
	TSize MinimumSize();
	CListBoxView* MakeViewClassInstanceL();
	COutlineHListBoxView* HlView() const;
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	void ExpandAllItemsL();
	void CollapseAllItemsL();
private:
	CRichText* iText;
	TInt iCursorPos;
};


class COutlineHListItemDrawer : public CHierarchicalListItemDrawer
/**
COutlineHListItemDrawer
Contains bitmap information about each drawer in the list box
@internalComponent
*/
	{
public:
	COutlineHListItemDrawer(COutlineHListBoxModel* aModel, const CFont* aFont, CArrayPtrFlat<CGulIcon>* aIcons);

protected: // from CHierarchicalListItemDrawer
	CGulIcon* ItemIcon(TInt aItemIndex) const;
	};


class COutlineHListBoxView : public CHierarchicalListBoxView
/**
Overriden functions from CHierarchicalListBoxView
@internalComponent
*/
{
public:
	void DrawItem(TInt aItemIndex) const;
	void DrawItemLinks(TInt aItemIndex, TInt aParagraphLevel) const;
	TSize ItemSize(TInt aItemIndex, TInt aParagraphLevel) const;
	TPoint ItemPos(TInt aItemIndex, TInt aParagraphLevel) const;
	void ClearPreceedingItemRect(TInt aItemIndex, TInt aParagraphLevel) const;
private:
	TInt GetParagraphLevel(CHierListItem* aItem) const;

};


// Inline functions

inline COutlineHListBoxModel* COutlineHListBox::OutlineModel() const { return (COutlineHListBoxModel*)iModel; }



#endif

