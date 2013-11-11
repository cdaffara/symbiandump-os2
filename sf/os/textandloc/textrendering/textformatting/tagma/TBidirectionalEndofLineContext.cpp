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


#include "TMSTD.H"
#include "frmUtils.h"

/**
Find the next bidirectional category and the next strong bidirectional category
from the text.
@param aText What to look at.
@param aStartPosOfNextLine Where to start looking.
*/
void TBidirectionalEndOfLineContext::Set(RTmTextCache& aText, TInt aStartPosOfNextLine)
	{
	TInt documentLength = aText.Source().DocumentLength();
	if (documentLength <= aStartPosOfNextLine)
		{
		iFirstCategory = iFirstStrongCategory = TChar::EOtherNeutral;
		return;
		}

	iFirstCategory = BdCategory(aText.Char(aStartPosOfNextLine));
	
	// Only search for the first strong category if the text, or part of the text following aStartPosOfNextLine
	// has not already been searched through
	if (aStartPosOfNextLine >= iPositionOfLastStrongCategory || aStartPosOfNextLine < iStartPosOfThisLine)
		iFirstStrongCategory = FirstStrongCategoryInParagraph(aStartPosOfNextLine, aText, iPositionOfLastStrongCategory, iStartPosOfThisLine);
	iStartPosOfThisLine = aStartPosOfNextLine;

	}

void TBidirectionalEndOfLineContext::ExternalizeL(RWriteStream& aDest)
	{
	aDest.WriteInt8L(iFirstCategory);
	aDest.WriteInt8L(iFirstStrongCategory);
	}

void TBidirectionalEndOfLineContext::InternalizeL(RReadStream& aSource)
	{
	iFirstCategory = static_cast<TChar::TBdCategory>(aSource.ReadInt8L());
	iFirstStrongCategory = static_cast<TChar::TBdCategory>(aSource.ReadInt8L());
	iPositionOfLastStrongCategory = 0;
	iStartPosOfThisLine = 0;
	}

void TBidirectionalContext::InternalizeL(RReadStream& aSource)
	{
	TBidirectionalState::InternalizeL(aSource);
	iEndOfLine.InternalizeL(aSource);
	}
