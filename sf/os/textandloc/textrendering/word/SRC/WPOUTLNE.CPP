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


#include <badesca.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikon.mbg>

#include "WPOUTLNE.H"
#include <word.rsg>
#include <word.mbg>


const TInt KExpandedBitmapIndex = 0;
const TInt KClosedBitmapIndex = 1;
const TInt KIndentPerLevel=12;
const TInt KMaxParagraphLength = 128;  // If a paragraph is longer than this length, truncate it
const TInt KWaitingTime = 500000;




// TOutlineEntry - constructors
////////////////

TOutlineEntry::TOutlineEntry(TInt aParagraphNo, TInt aOutlineLevel, TInt aParaLen, TInt aParaPos, TBool aChild, TBool aLast)
	: iParagraphNo(aParagraphNo),
	iOutlineLevel(aOutlineLevel),
	iParagraphLength(aParaLen),
	iParagraphPos(aParaPos),
	iIsChild(aChild),
	iLastEntry(aLast)
{
}


TOutlineEntry::TOutlineEntry()
	: iParagraphNo(0),
	iOutlineLevel(0),
	iParagraphLength(0),
	iParagraphPos(0),
	iIsChild(EFalse),
	iLastEntry(EFalse)
{
}

// TOutlineMap - constructor
//////////////

TOutlineMap::TOutlineMap(TInt aIndex, CHierListItem* aPtr)
	: iIndex(aIndex),
	iPtr(aPtr)
{
}


// COutlineMap
//////////////

COutlineMap* COutlineMap::NewL()
{
	COutlineMap* self = new(ELeave) COutlineMap();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}


void COutlineMap::ConstructL()
{
	// Create table with granularity of 5
	iOutlineMapTable=new(ELeave) COutlineMapTable(5);
}

COutlineMap::~COutlineMap()
{
	// delete the table
	delete iOutlineMapTable;
}


void COutlineMap::BindL(TInt aRef, CHierListItem* aPtr)
{
	TOutlineMap entry(aRef, aPtr);

	// Check to see if either of these already exists
	// When re-expanding elements, the same references can be bound to
	// different pointers, and the same pointers can be bound to different
	// references.

	TInt items = iOutlineMapTable->Count();
	TInt count;

	for (count = 0; count<items; count++)
	{
		TOutlineMap* entry = &(*iOutlineMapTable)[count];
		if (entry->iPtr == aPtr || entry->iIndex == aRef) 
		{
			iOutlineMapTable->Delete(count);
			items--;
			count--;
		}
	}

	iOutlineMapTable->AppendL(entry);
}


TInt COutlineMap::Index(CHierListItem* aPtr)
{
	// Given an item pointer from the list box, return an index into the
	// Outline Table, or -1 if it does not exist

	const TInt items = iOutlineMapTable->Count();
	TInt count;

	for (count=0; count<items; count++)
	{
		TOutlineMap* entry = &(*iOutlineMapTable)[count];
		if (entry->iPtr == aPtr) return entry->iIndex;
	}
	return KErrNotFound;
}


CHierListItem* COutlineMap::Ptr(TInt aIndex)
{
	// Given an index into the Outline table, return a pointer to its
	// entry in the list box, or NULL if it does not exist

	const TInt items = iOutlineMapTable->Count();
	TInt count;

	for (count=0; count<items; count++)
	{
		TOutlineMap* entry = &(*iOutlineMapTable)[count];
		if (entry->iIndex == aIndex) return entry->iPtr;
	}
	return NULL;
}

TBool COutlineMap::IndexPresent(TInt aIndex)
{
	// Returns ETrue if the given index is present in the table

	TInt items = iOutlineMapTable->Count();
	TInt count;

	for (count=0; count<items; count++)
	{
		TOutlineMap* entry = &(*iOutlineMapTable)[count];
		if (entry->iIndex == aIndex) return ETrue;
	}
	return EFalse;

}



/// COutlineHListBox 
////////////////////


COutlineHListBox::COutlineHListBox(CRichText* aText, TInt aCursorPos)
	: iText(aText),
	iCursorPos(aCursorPos)
{
}


TSize COutlineHListBox::MinimumSize()
	{
	// Specify the minimum size of this control 
	// Essential to make the control visible
	TSize size(iSize.iWidth, iSize.iHeight);
	if (!(iListBoxFlags&EScrollBarSizeExcluded) && iSBFrame && iSBFrame->HScrollBarVisibility()!=CEikScrollBarFrame::EOff)
		size.iHeight += CEikScrollBar::DefaultScrollBarBreadth();
	size+=iBorder.SizeDelta();
	return size;
	}


void COutlineHListBox::ConstructFromResourceL(TResourceReader& aReader)
{
	 // Called when creating the control from a dialog
	// Reads the size of the control, then constructs the object
	iSize.iWidth = aReader.ReadInt16();
	iSize.iHeight = aReader.ReadInt16();

	ConstructL();

}


void COutlineHListBox::ConstructL()
	{
	// The NewL constructor will leave if there is nothing to show in outline view,
	// so trap it, display a message, and leave without any error

	TRAPD(ret, iModel = COutlineHListBoxModel::NewL(this, iText);)
	if (ret == KErrNoOutline)
	{
		CEikonEnv::Static()->InfoMsg(R_WORD_NO_OUTLINE);
		User::Leave(KErrNone);
	}

	//  Get the open/close icons from the system
	TFileName resource = _L("*");
	CGulIcon* icon = iEikonEnv->CreateIconL(resource, EMbmWordOpenfdr);
	CleanupStack::PushL(icon);

	CArrayPtrFlat<CGulIcon>* icons = new(ELeave) CArrayPtrFlat<CGulIcon>(2);
	CleanupStack::PushL(icons);

	icons->AppendL(icon);
	icon = iEikonEnv->CreateIconL(resource, EMbmWordClsdfdr);

	CleanupStack::PushL(icon);
	icons->AppendL(icon);
	
	iItemDrawer = new(ELeave) COutlineHListItemDrawer((COutlineHListBoxModel*)iModel, iEikonEnv->NormalFont(), icons);

	CleanupStack::Pop(3);  // Cleanup the two icons and the array

	// Create the initial entries of the list box
	OutlineModel()->CreateRootListL();

	CreateViewL();

	// Expand the tree to show the current cursor position
	TInt item = OutlineModel()->ExpandCurrentPositionL(iCursorPos);

	if (item != -1)
		{
		// This updates the view to show the requested item no.
		iView->SetCurrentItemIndex(item);
		iView->SelectItemL(item);
		iListBoxFlags |= EStateChanged;
		}

	CreateScrollBarFrameL();
	ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
	}


CListBoxView* COutlineHListBox::MakeViewClassInstanceL()
	{
	return (new(ELeave) COutlineHListBoxView);
	}


TKeyResponse COutlineHListBox::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	TInt currentItemIndex = iView->CurrentItemIndex();
	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;
	CHierListItem* itemToBeExpanded=listModel->Item(currentItemIndex);

	TInt keyCode = aKeyEvent.iCode;

	TBool expandKey = (keyCode == EKeyRightArrow || keyCode == '+');
	TBool collapseKey = (keyCode == EKeyLeftArrow || keyCode == '-');

	const TInt index=CurrentItemIndex();
	CArrayFix<CHierListItem*>* listArray=HlModel()->HierListArray();
	TBool itemHasChildren=EFalse;


	if (expandKey)
	{
		if (!itemToBeExpanded->IsExpanded()) 
		{
			listModel->ExpandItemL(index);
			CArrayFix<CHierListItem*>* newListArray=HlModel()->HierListArray();
			if (index<newListArray->Count()-1)
				itemHasChildren=((*newListArray)[index]->Level()==(*newListArray)[index+1]->Level()-1);

		}
	}

	if (collapseKey)
	{	
		if (itemToBeExpanded->IsExpanded()) 
		{
			if (index<HlModel()->NumberOfItems()-1)
				itemHasChildren=((*listArray)[index]->Level()==(*listArray)[index+1]->Level()-1);
			listModel->CollapseItem(index);

		}
	}

	if (expandKey || collapseKey)
	{
		iView->CalcDataWidth();
		iView->CalcBottomItemIndex();
		UpdateScrollBarsL();
		if (itemHasChildren)
			((CHierarchicalListBoxView*)iView)->DrawFromItem(index);
		else
			((CHierarchicalListBoxView*)iView)->DrawItem(index);
		ReportListBoxEventL(MEikListBoxObserver::EEventItemActioned);

		return EKeyWasConsumed;
	}

	return CEikHierarchicalListBox::OfferKeyEventL(aKeyEvent,aType);
	
}



void COutlineHListBox::ExpandAllItemsL()
{
	CEikonEnv* eikonEnv=CEikonEnv::Static();
	eikonEnv->BusyMsgL(R_WORD_EXPAND_OUTLINE, KWaitingTime);

	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;
	TInt originalIndex = CurrentItemIndex();
	CHierListItem* itemPtr=listModel->Item(originalIndex);
	TInt paragraphIndex = listModel->iOutlineMap->Index(itemPtr);

	TInt item=0;



	TInt index = 0;
	iView->SetCurrentItemIndex(0);

	while (index < listModel->NumberOfItems() )
	{
		CArrayFix<CHierListItem*>* listArray=listModel->HierListArray();
		if (!(*listArray)[index]->IsExpanded())
			{
			listModel->ExpandItemL(index);
			CArrayFix<CHierListItem*>* newListArray=listModel->HierListArray();
			}
		item = listModel->GetItemByParagraph(paragraphIndex);
		iView->SetCurrentItemIndex(item);

		index++;
	}


	iView->CalcDataWidth();
	iView->CalcBottomItemIndex();

	item = listModel->GetItemByParagraph(paragraphIndex);
	((CHierarchicalListBoxView*)iView)->ScrollToMakeItemVisible(item);
	((CHierarchicalListBoxView*)iView)->DrawFromItem(iView->TopItemIndex());

	UpdateScrollBarsL();
	ReportListBoxEventL(MEikListBoxObserver::EEventItemActioned);

	eikonEnv->BusyMsgCancel();

}

void COutlineHListBox::CollapseAllItemsL()
{

	CEikonEnv* eikonEnv=CEikonEnv::Static();
	eikonEnv->BusyMsgL(R_WORD_COLLAPSE_OUTLINE, KWaitingTime);

	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;
	TInt originalIndex = CurrentItemIndex();
    CHierListItem* itemPtr=listModel->Item(originalIndex);
	TInt paragraphIndex = listModel->iOutlineMap->Index(itemPtr);

	//TInt itemCount = listModel->NumberOfItems();
	TInt index = 0;
	TInt item = 0;

	// Get root parent of this node
	TInt rootParent = 0;
	TInt whilecount = 0;

	TInt childCount = 0;

	while (whilecount<=paragraphIndex)
	{
		if (listModel->iOutlineTable->At(whilecount).iIsChild)
			childCount++;

		if (childCount==0)
			rootParent = whilecount;

		if (listModel->iOutlineTable->At(whilecount).iLastEntry)
			childCount--;

		whilecount++;
	}

	

	while (index < listModel->NumberOfItems() )
	{
		CArrayFix<CHierListItem*>* listArray=listModel->HierListArray();
	if ((*listArray)[index]->IsExpanded())
		{
		HlModel()->CollapseItem(index);
		}

		item = listModel->GetItemByParagraph(rootParent);
		iView->SetCurrentItemIndex(item);


		index++;
	}

	iView->CalcDataWidth();
	iView->CalcBottomItemIndex();

	item = listModel->GetItemByParagraph(paragraphIndex);
	((CHierarchicalListBoxView*)iView)->ScrollToMakeItemVisible(item);
	((CHierarchicalListBoxView*)iView)->DrawFromItem(iView->TopItemIndex());
				
	UpdateScrollBarsL();
	ReportListBoxEventL(MEikListBoxObserver::EEventItemActioned);

	eikonEnv->BusyMsgCancel();

}




// COutlineHListItemDrawer
//////////////////////////

COutlineHListItemDrawer::COutlineHListItemDrawer(COutlineHListBoxModel* aModel, const CFont* aFont,	CArrayPtrFlat<CGulIcon>* aIcons)
	: CHierarchicalListItemDrawer(aModel, aFont, aIcons)
	{}

CGulIcon* COutlineHListItemDrawer::ItemIcon(TInt aItemIndex) const
	{
	if (!iIconArray || aItemIndex <= -1 || aItemIndex >= iModel->NumberOfItems())
		return NULL;
	TInt iconIndex;

	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;
    CHierListItem* itemPtr = listModel->Item(aItemIndex);
	TInt paragraphIndex = listModel->iOutlineMap->Index(itemPtr);

	iconIndex = KClosedBitmapIndex;

	if (paragraphIndex < listModel->iOutlineTable->Count() - 1)
	{
		if (listModel->iOutlineTable->At(paragraphIndex+1).iIsChild)
			iconIndex = KExpandedBitmapIndex;
	}
	return (*iIconArray)[iconIndex];
	}


// COutlineHListBoxView
////////////////////////


TInt COutlineHListBoxView::GetParagraphLevel(CHierListItem* aItem) const
{
	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;
	TInt paragraphIndex = listModel->iOutlineMap->Index(aItem);
	if (paragraphIndex != KErrNotFound)
	    return (listModel->iOutlineTable->At(paragraphIndex).iOutlineLevel);
	else 
		return 0;
}


void COutlineHListBoxView::DrawItem(TInt aItemIndex) const
	{
	// Modify this items level so that it is draw offset according to its doc level

	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;

	CHierListItem* itemToBeDrawn = listModel->Item(aItemIndex);
	TInt paragraphLevel = GetParagraphLevel(itemToBeDrawn) - 1;

	if (RedrawDisabled())
		return;
	const TInt count = iModel->NumberOfItems();

	if (aItemIndex >= 0 && aItemIndex < count && ItemIsVisible(aItemIndex))
		{
		iGc->SetClippingRect(iViewRect);
		const TSize itemSize = ItemSize(aItemIndex, paragraphLevel);
		iItemDrawer->SetItemCellSize(itemSize);
		ClearPreceedingItemRect(aItemIndex, paragraphLevel);
		const TPoint pos=ItemPos(aItemIndex, paragraphLevel);
		TPoint markPos(iViewRect.iTl.iX, pos.iY);
		iItemDrawer->DrawItemMark(ItemIsSelected(aItemIndex), iFlags & EDimmed,markPos);

		DrawItemLinks(aItemIndex, paragraphLevel);

		STATIC_CAST(CHierarchicalListItemDrawer*, iItemDrawer)->DrawActualItem(aItemIndex, TRect(pos,itemSize),
										aItemIndex == iCurrentItemIndex, iFlags & EEmphasized, iFlags & EDimmed, ItemIsSelected(aItemIndex));
		iGc->CancelClippingRect();
		}		
	}


TSize COutlineHListBoxView::ItemSize(TInt aItemIndex, TInt aParagraphLevel) const
	{
	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;

	if (! listModel->Item(aItemIndex))
		return TSize(0, 0);
	const TInt indent=aParagraphLevel*KIndentPerLevel;
	return TSize(Max(iViewRect.Width()-indent, DataWidth()), iItemHeight);
	}

TPoint COutlineHListBoxView::ItemPos(TInt aItemIndex, TInt aParagraphLevel) const
	{
	// assumes aItemIndex is currently visible
	TInt indentLevel=0;
	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;

	if (aItemIndex>-1 && aItemIndex<listModel->NumberOfItems())
		indentLevel=aParagraphLevel;
	TPoint pos(-iHScrollOffset + iViewRect.iTl.iX + indentLevel*KIndentPerLevel,
					iViewRect.iTl.iY + (aItemIndex-iTopItemIndex)*iItemHeight);
	return pos;
	}

void COutlineHListBoxView::ClearPreceedingItemRect(TInt aItemIndex, TInt aParagraphLevel) const
	{
	const TInt indentLevel=aParagraphLevel;
	TPoint startPoint=ItemPos(aItemIndex, aParagraphLevel);
	startPoint.iX-=indentLevel*KIndentPerLevel;
	const TRect blankRect(startPoint,TPoint(startPoint.iX+indentLevel*KIndentPerLevel,startPoint.iY+iItemHeight));
	iItemDrawer->ClearRect(blankRect);
	}


void COutlineHListBoxView::DrawItemLinks(TInt aItemIndex, TInt aParagraphLevel) const
	{
	
	COutlineHListBoxModel* listModel = (COutlineHListBoxModel*)iModel;
	
	CHierListItem* itemToBeDrawn=listModel->Item(aItemIndex);
	const TInt itemLevel=itemToBeDrawn->Level();
	if (!itemLevel)
		return;
	TPoint startPoint=ItemPos(aItemIndex, itemLevel);

	TInt parentLevel = itemLevel;

	CHierListItem* parent = itemToBeDrawn->Parent();
	if (parent)
	{
		parentLevel = GetParagraphLevel(parent);
		startPoint=ItemPos(aItemIndex, parentLevel);
	}

	startPoint.iX-=KIndentPerLevel/2;


	iGc->SetPenStyle(CGraphicsContext::ESolidPen);
	iGc->SetPenColor(0);
	if (itemLevel)
		{
		const TPoint horizLineStart=startPoint+TPoint(0,iItemHeight/2);
		iGc->MoveTo(horizLineStart);
		iGc->DrawLineTo(TPoint(horizLineStart.iX+(KIndentPerLevel*(aParagraphLevel-parentLevel))+KIndentPerLevel/2,horizLineStart.iY));
		}


	TInt lineIndentLevel=itemLevel;
	TInt count=-1;



	while (lineIndentLevel--)
		{
		count++;
		
		if (lineIndentLevel+1<itemLevel)
			{
			parent=itemToBeDrawn->Parent();
			if (count>1)
				{
				TInt parentsToFind=count-1;
				while (parentsToFind--)
					parent=parent->Parent();
				}
			if (!(parent->HasFurtherSibling()))
				{
				parentLevel = GetParagraphLevel(parent);
				startPoint=ItemPos(aItemIndex,parentLevel);	
				startPoint.iX-=KIndentPerLevel/2;
				continue;
				}
			else
				{
				if (parent->Parent()) parent=parent->Parent();
				parentLevel = GetParagraphLevel(parent);
				startPoint=ItemPos(aItemIndex,parentLevel);	
				startPoint.iX-=KIndentPerLevel/2;
				}
			}



		TInt vertLineLength=iItemHeight;
		if (lineIndentLevel+1==itemLevel)
			{
			if (!itemToBeDrawn->HasFurtherSibling())
				vertLineLength/=2;
			}
		iGc->MoveTo(startPoint);
		iGc->DrawLineTo(TPoint(startPoint.iX,startPoint.iY+vertLineLength));
		
		startPoint.iX-=KIndentPerLevel;
		}
		
	}





// COutlineHListBoxModel
////////////////////////


COutlineHListBoxModel* COutlineHListBoxModel::NewL(COutlineHListBox* aParent,CRichText* aText)
{
	COutlineHListBoxModel* self = new (ELeave) COutlineHListBoxModel;

	CleanupStack::PushL(self);

	self->iParent=aParent;
	self->iText=aText;
	self->ConstructL();

	CleanupStack::Pop();
	return self;
}


void COutlineHListBoxModel::ConstructL()
{
	 iOutlineTable = new (ELeave)COutlineTable(2);
	 iOutlineMap=COutlineMap::NewL();

	 // Construct the Outline table from the document
	 CreateEntryTableL();

}

COutlineHListBoxModel::~COutlineHListBoxModel()
	{
	delete iOutlineMap;

	delete iOutlineTable;

	}


TInt COutlineHListBoxModel::CurrentCharPosition()
{
	// This returns the character position in the document of the currently
	// selected item in the listbox

	// Get the current item's entry
	TInt item = iParent->CurrentItemIndex();

	CHierListItem* itemPtr=Item(item);
	TInt paragraphIndex = iOutlineMap->Index(itemPtr);
	
	return (iOutlineTable->At(paragraphIndex).iParagraphPos);
}


void COutlineHListBoxModel::CreateEntryTableL()
{
	/* The scans through the document and creates the table which is then
	   used by the outline control.
	   Each entry in the table references one paragraph in the document that contains
	   an outline level greater than zero. (Only styles have outline levels).
	   For each of these paragraphs, the following information is stored:
	   Paragraph no. in the document,
		Character position of start of paragraph, length of paragraph (or a maximum of
		128 - used to generate the text of each node in the control),
		the outline level of the paragraph.
	   Two boolean values are also stored:
		isChild is True if the entry is the first child of a previous entry
		lastEntry is True if this entry is the last entry at the current outline level
		These two values are used to decide the structure of the tree
	*/

	   

	TInt paragraphPos=0;
	TInt paragraphLength=0;
	TInt outlineLevel=0;
	TInt lastOutlineLevel = 0;
	TBool isChild=EFalse;
//	TBool lastEntry=ETrue;
	TBool styleChanges=EFalse;

	TInt entryCount = 0;

	TInt paragraphCount = iText->ParagraphCount();

	TInt count;
	TInt whileCount;
	TInt parentLevel;


 	const CParaFormatLayer* paragraphStyle;
	//
	CEikonEnv* eikonEnv=CEikonEnv::Static();

	eikonEnv->BusyMsgL(R_WORD_OUTLINE, KWaitingTime);

	for (count=0; count<paragraphCount; count++)
	{
		// Get position of this paragraph
		paragraphPos = iText->CharPosOfParagraph(paragraphLength, count);
		if (paragraphLength>KMaxParagraphLength) paragraphLength=KMaxParagraphLength;

		// Discard paragraphs with no text (length will be 1 because of paragraph delimiter)
		if (paragraphLength>1) 
		{
		paragraphStyle=iText->ParagraphStyle(styleChanges, paragraphPos, paragraphLength-1);
		if (paragraphStyle->SenseBase())
		{
 		 outlineLevel=((CParagraphStyle*)paragraphStyle)->OutlineLevel();
		 if (outlineLevel>0) 
		 {
			isChild = (lastOutlineLevel < outlineLevel && lastOutlineLevel>0);

			iOutlineTable->AppendL(TOutlineEntry(count,outlineLevel,paragraphLength-1,paragraphPos,isChild,EFalse));	
			entryCount++;
			lastOutlineLevel=outlineLevel;
		 }
		}

		}

	}


	// Now look through to find the final nodes at each level

	if (entryCount) 
	{
		TInt childCount;

		for (count=1; count<entryCount-1; count++)
		{
			lastOutlineLevel = iOutlineTable->At(count).iOutlineLevel;
			
			// Get parent level
			whileCount = count;
			childCount = 0;
			while (whileCount>0)
			{	
				if (iOutlineTable->At(whileCount).iIsChild) childCount++;
				if (iOutlineTable->At(whileCount).iLastEntry) childCount--;
				if (childCount == 1) break;
				whileCount--;
			}
			// Check if this node is at the top level already
			if (whileCount==0) continue;
			else parentLevel = iOutlineTable->At(whileCount-1).iOutlineLevel;

			whileCount = count+1;

			while (whileCount<entryCount)
			{
				outlineLevel = iOutlineTable->At(whileCount).iOutlineLevel;
				if (outlineLevel <= lastOutlineLevel && outlineLevel>parentLevel) break;
				if (outlineLevel <= parentLevel) 
				{
					iOutlineTable->At(count).iLastEntry = ETrue;
					break;
				}
				whileCount++;
			}
		}

		iOutlineTable->At(entryCount-1).iLastEntry = ETrue;


	}
	else 
		{
		eikonEnv->BusyMsgCancel();
		User::Leave(KErrNoOutline);
		}

	eikonEnv->BusyMsgCancel();

}

void COutlineHListBoxModel::ScanHeading(TDes& aText)
	{
	// Scan the heading
	for (TInt pos=0; pos < aText.Length(); pos++)
		// Replace this charater with a normal space
		if (aText[pos] == CEditableText::ENonBreakingSpace)
			aText[pos] = ' ';
	}

void COutlineHListBoxModel::CreateRootListL()
{
	// This creates entries in the list box for all entries in the outline
	// table which are not children of any other nodes 

	iHierListArray=new(ELeave)CArrayFixSeg<CHierListItem*>(5);
	CHierListItem* item;

	TInt count;

	TBuf<128> text;

	TInt paragraphCount = iOutlineTable->Count();
	TInt paragraphPos;
	TInt paragraphLength;
	TBool lastEntry;
	TBool isChild;

	TInt childCount = 0;

	for (count=0; count<paragraphCount; count++)
	{
		paragraphPos= iOutlineTable->At(count).iParagraphPos;
		paragraphLength= iOutlineTable->At(count).iParagraphLength;
		lastEntry= iOutlineTable->At(count).iLastEntry;
		isChild= iOutlineTable->At(count).iIsChild;

		if (isChild) childCount++;

		if (!isChild && childCount < 1)
		{
				iText->Extract(text, paragraphPos, paragraphLength);
				// Scan th heading
				ScanHeading(text);
				item = new(ELeave)CHierListItem((TInt16)0);
				CleanupStack::PushL(item);
				item->SetTextL(text);
	
				AddItemL(item, -1, -1);
				iOutlineMap->BindL(count, item);
				CleanupStack::Pop(); // Pop item
		}
		if (lastEntry) childCount--;

	}

}


void COutlineHListBoxModel::ExpandItemL(TInt aItemIndex)
{
	// This creates nodes for all immediate children of the given index

	CHierListItem* itemToBeExpanded=Item(aItemIndex);
	if (itemToBeExpanded->IsExpanded())
		return;

	TInt newItemIndex = aItemIndex;

	TInt index;
	TInt count;
	TBuf<128> text;

	TInt paragraphCount = iOutlineTable->Count();
	TInt paragraphPos;
	TInt paragraphLength;
	TBool lastEntry;
	TBool isChild;
	TInt childCount = 0;
	CHierListItem* item;

	index = iOutlineMap->Index(itemToBeExpanded);


	if (index != KErrNotFound)
	{
			for (count=index+1; count<paragraphCount; count++)
			{
				lastEntry= iOutlineTable->At(count).iLastEntry;
				isChild= iOutlineTable->At(count).iIsChild;


				if (isChild) childCount++;

				if (childCount == 1)
				{
						paragraphPos= iOutlineTable->At(count).iParagraphPos;
						paragraphLength= iOutlineTable->At(count).iParagraphLength;

						iText->Extract(text, paragraphPos, paragraphLength);
						ScanHeading(text);
						item = new(ELeave)CHierListItem((TInt16)0);
						CleanupStack::PushL(item);
						item->SetTextL(text);

						AddItemL(item, aItemIndex, newItemIndex++);

						iOutlineMap->BindL(count, item);
						CleanupStack::Pop(); // pop item
						itemToBeExpanded->SetExpanded();	
				}
				if (lastEntry) childCount--;	
				if (childCount == 0) break;
			}
	}
}



TInt COutlineHListBoxModel::GetItemByParagraph(TInt aPara)
{
	// Given a paragraph number in the Outline Table, this returns
	// the item number in the list box.
	
		CHierListItem* thisPtr = iOutlineMap->Ptr(aPara);
		TInt size = HierListArray()->Count();
		TInt count;
		TInt item = -1;

		for (count=0; count<size; count++)
		{
			if (HierListArray()->At(count) == thisPtr) 
			{
				item = count;
				break;
			}
		}
		return item;
}


TInt COutlineHListBoxModel::ExpandCurrentPositionL(TInt aCursorPos)
{
	// This finds out which paragraph the cursor is currently at
	// then expands its parents

	TInt paragraphNo = iText->ParagraphNumberForPos(aCursorPos);
	TInt count=0;
	TInt stylePara = -1;
	TInt paragraphCount = iOutlineTable->Count();

	// Find out which style entry is above this position

	for (count=0; count<paragraphCount; count++)
	{
		if (iOutlineTable->At(count).iParagraphNo > paragraphNo) break;
		stylePara = count;
	}


	return (ExpandParagraphL(stylePara));

}

TInt COutlineHListBoxModel::ExpandParagraphL(TInt aPara)
{
	// This looks to see if the paragraphs parent node is expand, and if not,
	// expand it by recursion.
	// The maximum iteration level is the number of levels deep the initial node is,
	// and is unlikely to be more than 2 or 3 levels.
	// The theoretical maximum is 99 levels, (as the UI only support entry of level 0-99)


	// Check that this paragraph has not been expanded

	if (iOutlineMap->IndexPresent(aPara) || aPara == -1) return GetItemByParagraph(aPara);

	// If this node is a first child, then try to open the previous node
	// since it must be it's parent

	if (iOutlineTable->At(aPara).iIsChild)
	{
		ExpandParagraphL(aPara-1); 
		// Node aPara-1 is now guaranteed to exist, so open it up
		TInt item = GetItemByParagraph(aPara-1);

		if (item != -1) ExpandItemL(item);

		return GetItemByParagraph(aPara);
	}
		

	// Need to look a bit further up since this node has siblings

	TInt childCount = 0;
	TInt para = aPara - 1;
	TBool child;

	while (aPara >= 0)
	{
		if (iOutlineTable->At(para).iLastEntry) childCount++;
		child = iOutlineTable->At(para).iIsChild;

		if (child && childCount == 0)
		{
			// para is a child of the same level, so open its parent
			ExpandParagraphL(para-1); 
			TInt item = GetItemByParagraph(para-1);
			if (item != -1) ExpandItemL(item);
			return GetItemByParagraph(aPara);
		}

		if (child) childCount--;

		para--;
	}

	return 0;
}





