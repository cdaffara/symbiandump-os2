/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "FRMPAGE.H"
#include "FRMCONST.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FormLinePagTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "FRMCONST_INTERNAL.H"
#include "FRMCONST_PARTNER.H"
#endif

TLinePaginator::TLinePaginator():
	iPageList(NULL),
	iDocPos(0),
	iKeepWithPrev(ETrue),
	iPageHeight(0),
	iHeightRem(0),
	iBreakOnPage(EFalse),
	iPrevPageBreak(0),
	iHeightLines(0),
	iFirstLine(ETrue)
	{
	}

TBool TLinePaginator::AppendLineL(TPageLine aLine)
    //
	{
	TBool pageBreak=EFalse;

	if(aLine.iStartNewPage && !iFirstLine)	  
		{
		SetPotentialBreakPoint(aLine.iDocPos);
		InsertPageBreakL();
		pageBreak=ETrue;
		}
	else
		{ 
		if (!iKeepWithPrev)
			SetPotentialBreakPoint(aLine.iDocPos);
		if (iHeightRem<iHeightLines+aLine.iLineHeight /*&& !iFirstLine*/)
			{
			pageBreak=ETrue;
			//__ASSERT_DEBUG(!iFirstLine || !iBreakOnPage,Panic());		//###
			if (iBreakOnPage)
				{
				InsertPageBreakL();
				if (aLine.iLineHeight > iHeightRem)
					{
					if (iHeightLines!=0)
						{
						SetPotentialBreakPoint(aLine.iDocPos);
						InsertPageBreakL();
						}
					CheckTallLineL(aLine);
					}
				}
			else 
				{
				if (!iFirstLine)
					{
					SetPotentialBreakPoint(aLine.iDocPos);
					InsertPageBreakL();
					}
				CheckTallLineL(aLine);		//This picks up tall lines at the start of the document the above one picks up the rest
				}
			}
		}
	
	iFirstLine=EFalse;
	iHeightLines+=aLine.iLineHeight;
	iKeepWithPrev=aLine.iKeepWithNext;
	return pageBreak;
	}

void TLinePaginator::InsertPageBreakL()
	{
	iBreakOnPage=EFalse;
	iHeightRem=iPageHeight;
	iKeepWithPrev=EFalse;
	TInt deltaDocPos=iDocPos-iPrevPageBreak; 
	if (deltaDocPos<=0)
	    {
	    OstTrace0( TRACE_DUMP, TLINEPAGINATOR_INSERTPAGEBREAKL, "EFInvalidNumberCharsOnPage" );
	    }
	__ASSERT_DEBUG(deltaDocPos>0,FormPanic(EFInvalidNumberCharsOnPage));
	iPageList->AppendL(deltaDocPos);
	iPrevPageBreak=iDocPos;
	}

void TLinePaginator::CheckTallLineL(TPageLine& aLine)
	{
	while (aLine.iLineHeight>iPageHeight)
		{
		aLine.iLineHeight-=iPageHeight;
		iPageList->AppendL(0);
		}
	}

void TLinePaginator::FlushL(TInt aEndDocPos)
	{
	SetPotentialBreakPoint(aEndDocPos);
	InsertPageBreakL();
	}

void TLinePaginator::SetPageHeight(TInt aPageHeight)
	{
	iPageHeight=aPageHeight;
	iHeightRem=aPageHeight;
	Reset();
	}

// It is expected that the array passed in will be an empty one
void TLinePaginator::SetArray(CArrayFix<TInt>* aCharsPerPage)
	{
	iPageList=aCharsPerPage;
	
	// reset if the array is not (for some reason) already cleared.
	if (iPageList->Count() > 0)
		Reset();
	}

void TLinePaginator::Reset()
	{
	iBreakOnPage=EFalse;
	iKeepWithPrev=ETrue;
	iPrevPageBreak=iDocPos=0;
	iFirstLine=ETrue;
	iHeightRem=iPageHeight;

	ResetArray();
	}

void TLinePaginator::ResetArray()
	{
	iPageList->Reset();
	iPageList->Compress();
	}

void TLinePaginator::SetPotentialBreakPoint(TInt aDocPos)
	{
	iBreakOnPage=ETrue;
	iDocPos=aDocPos;
	iHeightRem-=iHeightLines;
	iHeightLines=0;
	}
