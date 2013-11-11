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


#include "FRMTLAY.H"
#include "FRMLAYDT.H"
#include "FRMCONST.H"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CARETTraces.h"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "FRMCONST_INTERNAL.H"
#include "FRMCONST_PARTNER.H"
#include "TAGMA_INTERNAL.H"
#endif

// Set the top of the line containing aDocPos to the y coordinate aYPos.
TInt TCursorPosition::ViewTopOfLineL(const TTmDocPos& aDocPos,TInt& aYPos)
	{
	TViewYPosQualifier yPosQualifier;
	yPosQualifier.SetHotSpot(TViewYPosQualifier::EFViewTopOfLine);
	yPosQualifier.SetFillScreen(ETrue);
	return iLayout->SetViewL(aDocPos,aYPos,yPosQualifier);
	}

void TCursorPosition::CheckSelection(TBool aSelect)
	{
	if (IsSelection() && iDocPos.iPos == iAnchor)
	    {
	    OstTrace0( TRACE_DUMP, TCURSORPOSITION_CHECKSELECTION, "EFSelectedRangeZeroLen" );
	    }
	    
	__ASSERT_DEBUG(!IsSelection() || iDocPos.iPos != iAnchor,FormPanic(EFSelectedRangeZeroLen));
	CheckNullSelection();
	TUint drawSelectionFlags = 0;
	iOldDocPos=iDocPos.iPos;
	iOldAnchor=iAnchor;
	if (IsPictureFrame())
		{
		drawSelectionFlags |= EDrawOldPictureFrame;
		}
	if (iFlags & ESelected)
		{
		if (!aSelect)
			iFlags &= ~ESelected;
		if (aSelect || !IsPictureFrame())
			drawSelectionFlags |= EDrawHighlight;
		if (aSelect && IsPictureFrame())
			iAnchor=iOldDocPos;
		}
	else
		{
		if (aSelect)
			{
			iAnchor=iDocPos.iPos;	//Change the value of iOldDocPos so that the old highlight appears to be empty
			iFlags |= ESelected;
			drawSelectionFlags |= EDrawHighlight;
			}
		iOldDocPos=iOldAnchor=iAnchor;
		}
	iFlags &= ~(EDrawHighlight | EDrawOldPictureFrame | EDrawNewPictureFrame);
	iFlags |= drawSelectionFlags;
	}

// The cursor pos has changed so the selection might be zero length
void TCursorPosition::CheckNullSelection()
	{
	if ((iFlags & ESelected) && iDocPos.iPos == iAnchor)
		iFlags &= ~ESelected;
	}

TBool TCursorPosition::IsPictureFrame() const
	{
	if (iFlags & EReturnPreviousHighlight)
		return (iFlags & EDrawOldPictureFrame) != 0;
	else
		return (iFlags & EDrawNewPictureFrame) != 0;
	}

TBool TCursorPosition::IsNewPictureFrame() const
	{
	return (iFlags & EDrawNewPictureFrame) != 0;
	}

TInt TCursorPosition::SetSelectionL(const TCursorSelection& aSelection)
	//
	//Change the highlighted region to be that specified
	//
	{
	TInt scroll=0;
 	TInt botOfWin=iLayout->BandHeight();
	TInt firstVisChar;
	TTmLineInfo lower_line_info;
	TTmLineInfo higher_line_info;
	TTmDocPos lower_doc_pos = TTmDocPos(aSelection.LowerPos(), ETrue);
	TTmDocPos higher_doc_pos = TTmDocPos(aSelection.HigherPos(), EFalse);
	TInt lowerPos = iLayout->PosInBand(lower_doc_pos,&lower_line_info) ? EFInside : EFAbove;
	TInt higherPos = iLayout->PosInBand(higher_doc_pos,&higher_line_info) ? EFInside : EFAbove;
	TInt selectionPos = EFInside;
	TInt extraScroll;
	TBool cursorVisible = EFalse;

	if (aSelection.iCursorPos<0 || aSelection.iCursorPos>iLayout->DocumentLength())
	    {
	    OstTrace0( TRACE_DUMP, TCURSORPOSITION_SETSELECTIONL, "EFInvalidDocPos" );
	    }
	__ASSERT_DEBUG(aSelection.iCursorPos>=0 && aSelection.iCursorPos<=iLayout->DocumentLength()
																		,FormPanic(EFInvalidDocPos));
	iLayout->PosRangeInBand(firstVisChar);
	if (lowerPos == EFInside)
		{
		if (lower_line_info.iOuterRect.iTl.iY < 0)
			lowerPos = EFAbove;
		}
	else if (firstVisChar < aSelection.LowerPos())
		lowerPos = EFBelow;

	if (higherPos==EFInside)
		{
		if (higher_line_info.iOuterRect.iBr.iY > botOfWin + 1)
			higherPos=EFBelow;
		}
	else if (firstVisChar < aSelection.HigherPos())
		higherPos = EFBelow;
	CheckSelection(ETrue);

	iAnchor = aSelection.iAnchorPos;
	if (lower_doc_pos.iPos == aSelection.iCursorPos)
		iDocPos = lower_doc_pos;
	else
		iDocPos = higher_doc_pos;

	if (lowerPos==EFAbove)
		{
		selectionPos=EFBelow;
		if (aSelection.iCursorPos==aSelection.HigherPos() && higherPos==EFBelow)
			selectionPos=EFAbove;
		}
	else if (higherPos==EFBelow)
		selectionPos=EFAbove;

	if (selectionPos == EFBelow || selectionPos == EFAbove)
		{
		TViewYPosQualifier yPosQualifier;
		yPosQualifier.SetMakeLineFullyVisible();
		TInt docPos = aSelection.LowerPos();
		TInt desiredY = scroll;
		if (selectionPos == EFBelow)
			{
			yPosQualifier.SetHotSpot(TViewYPosQualifier::EFViewTopOfLine);
			yPosQualifier.SetFillScreen(TRUE);
			}
		else
			{
			desiredY = botOfWin;
			docPos = aSelection.iCursorPos < aSelection.iAnchorPos?
				aSelection.iAnchorPos - 1 : aSelection.iCursorPos;
			yPosQualifier.SetHotSpot(TViewYPosQualifier::EFViewBottomOfLine);
			}
		if (aSelection.iCursorPos == docPos)
			cursorVisible = ETrue;
		scroll = iLayout->SetViewL(docPos, desiredY, yPosQualifier);
		}

	if (cursorVisible)
		extraScroll=DoSetVisibleDocPosL(TTmDocPos(aSelection.iCursorPos, ETrue));
	else
		extraScroll=DoSetDocPosL(TTmDocPos(aSelection.iCursorPos, ETrue));
	if (scroll!=CTextLayout::EFScrollRedrawWholeScreen)
		scroll+=extraScroll;
	return scroll;
	}

void TCursorPosition::SetPendingSelection(const TCursorSelection& aSelection)
	{
	iFlags &= ~(ESelected | EDrawHighlight | EDrawOldPictureFrame | EDrawNewPictureFrame);
	iDocPos.iPos = aSelection.iCursorPos;
	iDocPos.iLeadingEdge = aSelection.iCursorPos < aSelection.iAnchorPos ? ETrue : EFalse;
	iAnchor = aSelection.iAnchorPos;
	if (iDocPos.iPos != iAnchor)
		iFlags |= ESelected;
	iLayout->SetTextViewCursorPos(this); // Put in for INC092568
	}

void TCursorPosition::GetOldSelection(TCursorSelection& aSelection) const
	{
	aSelection.iAnchorPos = iOldAnchor;
	aSelection.iCursorPos = iOldDocPos;
	}

   // Return the highlighted range
void TCursorPosition::GetSelection(TCursorSelection& aSelection) const
	{
	if (iFlags & EReturnPreviousHighlight)
		GetOldSelection(aSelection);
	else
		{
		aSelection.iCursorPos = iDocPos.iPos;
		aSelection.iAnchorPos = iFlags & ESelected? iAnchor : iDocPos.iPos;
		}
	}

/**
Selection of the left end or right end of a run of text. If both ends are in 
one line, the logical end that is visually in the direction specified will be 
chosen. If aStart and aEnd are in different lines, then we look at whether 
aEnd is choosing the end that is furthest forwards or furthest back by comparing
it against the directionality of the paragraph containing aStart.
@return Reference to aStart or aEnd.
@internalComponent
*/
const TTmDocPos& TCursorPosition::VisualEndOfRunL(
	const TTmDocPos& aStart, const TTmDocPos& aEnd,
	TVisualEnd aDirection)
	{
	iLayout->ExtendFormattingToCoverPosL(aStart.iPos);
	iLayout->ExtendFormattingToCoverPosL(aEnd.iPos);

	TTmPosInfo2 startInfo;
	TTmLineInfo startLine;
	iLayout->FindDocPos(aStart, startInfo, &startLine);
	TTmPosInfo2 endInfo;
	iLayout->FindDocPos(aEnd, endInfo);

	TVisualEnd startEnd = startInfo.iEdge.iX < endInfo.iEdge.iX?
		EVisualLeft : EVisualRight;

	if (startInfo.iEdge.iY != endInfo.iEdge.iY)
		{
		// not the same line
		TVisualEnd backwards = startLine.iFlags & TTmLineInfo::EParRightToLeft?
			EVisualRight : EVisualLeft;
		TVisualEnd forwards = backwards? EVisualLeft : EVisualRight;
		startEnd = aStart < aEnd? backwards : forwards;
		}
	return startEnd == aDirection? aStart : aEnd;
	}

// Set the cursor position to aDocPos and update the stored selection according to the value of aDragSelectOn.
TInt TCursorPosition::SetDocPosL(TBool aDragSelectOn,const TTmDocPos& aDocPos)
	{
	CheckSelection(aDragSelectOn);
	return DoSetDocPosL(aDocPos);
	}

// Set the cursor position to aDocPos.
TInt TCursorPosition::DoSetDocPosL(const TTmDocPos& aDocPos)
	{
	TInt scroll = 0;
	if (iLayout->PosInBand(aDocPos))
		scroll = DoSetVisibleDocPosL(aDocPos);
	else
		{
		iDocPos = aDocPos; // in case the next line leaves
		scroll = ViewTopOfLineL(aDocPos,scroll);
		DoSetVisibleDocPosL(aDocPos);
		}
	return scroll;
	}

// Set the cursor position to the specified document position.
TInt TCursorPosition::DoSetVisibleDocPosL(const TTmDocPos& aDocPos)
	{
	iDocPos = aDocPos;
	CheckNullSelection();
	TTmPosInfo2 pos_info;
	iLayout->FindDocPos(aDocPos,pos_info);
	TInt pixels = CheckCursorOnScreenL(pos_info.iEdge.iY);
	UpdateLatentX(pos_info.iEdge.iX);
	UpdateLatentY(pos_info.iEdge.iY);
	return pixels;
	}

TInt TCursorPosition::SetXyPosL(TBool aDragSelectOn,TPoint aPos,TBool aAllowPictureFrame)
	//
	//Move the cursor back to the point on the screen nearest to the position specified
	//and return the document position and screen position
	//and update the saved X and Y positions
	//
	{
	TPoint originalPoint = aPos;
	TInt pixels=0;

	CheckSelection(aDragSelectOn);
	DoSetXyPos(aPos);
	TRAPD(ret,pixels=CheckCursorOnScreenL(aPos.iY));
	if (ret)
	    {
	    OstTrace0( TRACE_FATAL, TCURSORPOSITION_SETXYPOSL, "EFShouldNotLeave" );
	    }
	__ASSERT_ALWAYS(!ret,FormPanic(EFShouldNotLeave));
	UpdateLatentX(aPos.iX);
	UpdateLatentY(aPos.iY);
	if (!aDragSelectOn && aAllowPictureFrame)
		{
		TRect pict;
		TInt pos = iLayout->XyPosToDocPosL(originalPoint);
		if (iLayout->PictureRectangleL(pos, pict))
			{
			TTmDocPos docPos(pos + 1, EFalse);
			SetPictureFrame(docPos, pos, pict);
			}
		}
	return pixels;
	}

/**
Returns a reference to the more suitable of the positions, taking
the cursor positioning hint into account.
@param aPreferred
	The candidate returned if the positioning hint does not decide it.
@param aBackup
	The alternative candidate; only returned if the positioning hint
	makes it more desirable than aPreferred.
@return either aPreferred or aBackup.
@internalComponent
*/
TTmPosInfo2& TCursorPosition::ChoosePosition(TTmPosInfo2& aPreferred,
	TTmPosInfo2& aBackup)
	{
	TTmPosInfo2 posInfo;
	// The following part is to find out whether the aPreferred or aBackup is
	// overlapped. For overlapped position, because it could visually exist at two
	// different places/XY-coordinates, the iLayout->FindDocPos may return an aPos
	// with a different iEdge against the original, so that we can know which doc position
	// is overlapped.
	iLayout->FindDocPos(aPreferred.iDocPos, posInfo, NULL);
	TBool preferredOverlapped = (aPreferred.iEdge != posInfo.iEdge);
	
	iLayout->FindDocPos(aBackup.iDocPos, posInfo, NULL);
	TBool backupOverlapped = ( aBackup.iEdge != posInfo.iEdge);
	
	if(preferredOverlapped && !backupOverlapped)	
		return aBackup;
	if(!preferredOverlapped && backupOverlapped)
		return aPreferred;
	
	if (iPositioningHint == EInsertStrongL2R)
		{
		if (!aBackup.iRightToLeft && aPreferred.iRightToLeft)
			return aBackup;
		}
	else if (iPositioningHint == EInsertStrongR2L)
		{
		if (aBackup.iRightToLeft && !aPreferred.iRightToLeft)
			return aBackup;
		}
	return aPreferred;
	}

/*
Move the cursor to the point on the screen nearest to the position specified
and return the document position and screen position.
*/
void TCursorPosition::DoSetXyPos(TPoint& aXyPos)
	{
	TTmPosInfo2 leftPosInfo;
	TTmPosInfo2 rightPosInfo;
	TTmLineInfo lineInfo;
	CTextLayout::TTagmaForwarder forwarder(*iLayout);
	if (forwarder.FindXyPosWithDisambiguation(aXyPos,
		leftPosInfo, rightPosInfo, lineInfo))
		{
		TTmPosInfo2& chosen = ChoosePosition(leftPosInfo, rightPosInfo);
		iDocPos = chosen.iDocPos;
		aXyPos = chosen.iEdge;
		CheckNullSelection();
		}
	}

// Set up a picture frame.
void TCursorPosition::SetPictureFrame(const TTmDocPos& aDocPos,TInt aAnchor,const TRect& aPictureRect)
	{
	iFlags |= (ESelected | EDrawNewPictureFrame);
	iDocPos = aDocPos;
	iAnchor = aAnchor;
	iLatentX = (aPictureRect.iTl.iX + aPictureRect.iBr.iX) / 2;
	}

TInt TCursorPosition::MoveL(TBool aDragSelectOn, TMovementType& aMove,
	TBool aAllowPictureFrame)
	{
	// Get the anchor point, either old or new.
	TBool oldSelection = iFlags & ESelected;
	TInt anchor = oldSelection? iAnchor : iDocPos.iPos;

	iLayout->ExtendFormattingToCoverPosL(iDocPos.iPos);

	TMovementType move = aMove;
	TInt pixels = 0;
	TBool pagingMove = EFalse;
	TTmPosInfo2 suggestedCursorPos;
	TInt picturePos = KErrNotFound;
	TTmLineInfo line;
	TRect pictureRect;

	// Get a value for suggestedCursorPos.
	// Also scroll if aMove is EFPageUp or EFPageDown.
	switch (move)
		{
	case EFNoMovement:
		iLayout->FindDocPos(iDocPos, suggestedCursorPos);
		break;
	case EFLeft:
	case EFRight:
		// MoveL is not called if a picture frame is moved away from,
		// so we will not deal with that situation.
		if (IsPictureFrame())
			{
			suggestedCursorPos.iDocPos.iPos = iDocPos.iPos;
			suggestedCursorPos.iDocPos.iLeadingEdge = ETrue;
			suggestedCursorPos.iEdge.iX = iLatentX;
			suggestedCursorPos.iEdge.iY = iLatentY;
			if (aDragSelectOn)
				{
				TTmDocPos anchorPos;
				anchorPos.iPos = anchor;
				anchorPos.iLeadingEdge = anchor < iDocPos.iPos? ETrue : EFalse;
				suggestedCursorPos.iDocPos = VisualEndOfRunL(
					iDocPos, anchorPos,
					move == EFLeft ? EVisualLeft : EVisualRight);
				anchor = iDocPos.iPos;
				}
			}
		else if (LeftRightL(suggestedCursorPos, line, move)
			&& aAllowPictureFrame && !aDragSelectOn)
			{
			// Have we just moved over a picture?
			TPoint c = suggestedCursorPos.iEdge;
			c.iX += aMove == EFLeft? 1 : -1;
			picturePos = iLayout->PictureRectangleAndPosL(c, pictureRect);
			}
		break;
	case EFLineUp:
	case EFLineDown:
		UpDownL(suggestedCursorPos, line, move);
		if (aAllowPictureFrame && !aDragSelectOn && move == aMove)
			{
			TPoint c(iLatentX, line.iBaseline);
			picturePos = iLayout->PictureRectangleAndPosL(c, pictureRect);
			}
		break;
	case EFPageUp:
	case EFPageDown:
		pixels = PageScrollL(move);
		pagingMove = ETrue;
		break;
	case EFLineBeg:
	case EFLineEnd:
		StartEnd(suggestedCursorPos, line, move);
		break;
		}

	// Do the scrolling, if not already done.
	if (!pagingMove)
		pixels = iLayout->ScrollDocPosIntoViewL(suggestedCursorPos.iDocPos);

	// Work out if the cursor has not moved.
	if (iDocPos == suggestedCursorPos.iDocPos
		&& !IsPictureFrame() && picturePos < 0)
		move = EFNoMovement;

	// Set old values
	iOldDocPos = iDocPos.iPos;
	iOldAnchor = oldSelection? iAnchor : iOldDocPos;
	if (iFlags & EDrawNewPictureFrame)
		iFlags |= EDrawOldPictureFrame;
	else
		iFlags &= ~EDrawOldPictureFrame;
	
	// Finally, set everything worked out.
	if (!pagingMove)
		iDocPos = suggestedCursorPos.iDocPos;

	if (!aDragSelectOn || anchor == suggestedCursorPos.iDocPos.iPos)
		{
		iFlags &= ~ESelected;
		iAnchor = suggestedCursorPos.iDocPos.iPos;
		}
	else
		{
		iFlags |= ESelected;
		iAnchor = anchor;
		}
	switch (move)
		{
	case EFLeft:
	case EFRight:
	case EFLineBeg:
	case EFLineEnd:
		iLatentX = suggestedCursorPos.iEdge.iX;
		if (0 <= picturePos)
			iLatentX = (pictureRect.iTl.iX + pictureRect.iBr.iX) >> 1;
		// fall through to:
	case EFLineUp:
	case EFLineDown:
		iLatentY = suggestedCursorPos.iEdge.iY;
	default:
		break;
		}
	if (0 <=picturePos)
		{
		iFlags |= (ESelected | EDrawNewPictureFrame);
		iDocPos.iPos = picturePos;
		iDocPos.iLeadingEdge = ETrue;
		iAnchor = picturePos + 1;
		}
	else
		iFlags &= ~EDrawNewPictureFrame;
	if (oldSelection ||
		((iFlags & ESelected) && !(iFlags & EDrawNewPictureFrame)))
		iFlags |= EDrawHighlight;
	else
		iFlags &= ~EDrawHighlight;
	aMove = move;
	return pixels;
	}

void TCursorPosition::UpdateLatentPosition()
	{
	TPoint pos;
	CalculateCursorPos(pos);
	UpdateLatentY(pos.iY);
	UpdateLatentX(pos.iX);
	}

void TCursorPosition::TextMoveVertically()
	//
	//Reset the Vertical Latent position
	//
	{
	TPoint scrPos;

	CalculateCursorPos(scrPos);
	UpdateLatentY(scrPos.iY);
	}

/** Find the position one position to the left or right of the current cursor.
If the cursor is already at that extreme of the line, the cursor will move
to the opposite of the previous (if moving backwards) or next (if moving
forwards) line. The cursor is considered to be moving forwards if it is
moving in the same direction as the paragraph directionality. This may, of
course, be different to the local direction of the text.

@param aToLeft ETrue if moving left, EFalse if moving right.
@param aPos returns the position found.
@param aLine returns details of the line containing aPos.
@return ETrue if the cursor moved one position within the line. */
TBool TCursorPosition::LeftRightL(TTmPosInfo2& aPos, TTmLineInfo& aLine, TMovementType& aMove)
	{
	TTmPosInfo2 posInfo;
	if (!(aMove == EFLeft || aMove == EFRight))
	    {
	    OstTrace0( TRACE_DUMP, TCURSORPOSITION_LEFTRIGHTL, "EFIncorrectCursorMovement" );
	    }
	__ASSERT_DEBUG(aMove == EFLeft || aMove == EFRight,
		FormPanic(EFIncorrectCursorMovement));
	TBool found = EFalse;
	TTmPosInfo2 otherPosInfo;
	CTextLayout::TTagmaForwarder forwarder(*iLayout);
	if (aMove == EFLeft)
		{
		found = forwarder.GetNextPosLeftWithDisambiguation(
			iDocPos, otherPosInfo, posInfo);
		}
	else
		{
		found = forwarder.GetNextPosRightWithDisambiguation(
			iDocPos, posInfo, otherPosInfo);
		}
	if (found)
		{
		iLayout->FindDocPos(ChoosePosition(posInfo, otherPosInfo).iDocPos,
			aPos, &aLine);
		return ETrue;
		}

	// We have reached the extreme of the line.
	// But the beginning or the end?
	TTmLineInfo currentLineInfo;
	iLayout->FindDocPos(iDocPos, posInfo, &currentLineInfo);
	TBool toLeft = aMove == EFLeft? ETrue : EFalse;
	TBool movingBackwards = currentLineInfo.iFlags & TTmLineInfo::EParRightToLeft?
		!toLeft : toLeft;
	TInt lineToGoToY = movingBackwards?
		currentLineInfo.iOuterRect.iTl.iY - 1
		: currentLineInfo.iOuterRect.iBr.iY;

	iLayout->ExtendFormattingToCoverYL(lineToGoToY);
	const CTmTextLayout& tm = iLayout->TagmaTextLayout();
	TTmDocPos newPos;
	TPoint c(0, lineToGoToY);
	if (iLayout->FindXyPos(c, posInfo, &aLine))
		{
		// moving to next/previous line
		TBool toRightEnd = aLine.iFlags & TTmLineInfo::EParRightToLeft?
			!movingBackwards : movingBackwards;
		tm.LineExtreme(posInfo.iDocPos, toRightEnd, newPos);
		}
	else
		{
		// We appear to have reached the end of the document.
		if (movingBackwards || currentLineInfo.iStart == currentLineInfo.iEnd)
			// before start of line
			newPos = TTmDocPos(currentLineInfo.iStart, EFalse);
		else
			// after end of line (last character is paragraph delimiter)
			newPos = TTmDocPos(currentLineInfo.iEnd - 1, ETrue);
		}
	tm.FindDocPos(newPos, aPos, aLine);
	// Cursor either changed line, or did not move.
	return EFalse;
	}

/** Find the visual start or end of the line containing the cursor.
@param aToStart ETrue if finding the visual start of the line, EFalse if finding the end.
@param aPos Returns the position found.
@param aLine Returns details of the line containing aPos.*/
void TCursorPosition::StartEnd(TTmPosInfo2& aPos, TTmLineInfo& aLine, TMovementType& aMove)
	{
	if (aMove != EFLineBeg && aMove != EFLineEnd)
	    {
	    OstTrace0( TRACE_DUMP, TCURSORPOSITION_STARTEND, "EFIncorrectCursorMovement" );
	    }
	__ASSERT_DEBUG(aMove == EFLineBeg || aMove == EFLineEnd,
		FormPanic(EFIncorrectCursorMovement));
	iLayout->FindDocPos(iDocPos, aPos, &aLine);
	TBool toBeginning = aMove == EFLineBeg? ETrue : EFalse;
	TBool toRight = aLine.iFlags & TTmLineInfo::EParRightToLeft?
		toBeginning : !toBeginning;
	TTmDocPos newPos;
	iLayout->TagmaTextLayout().LineExtreme(iDocPos, toRight, newPos);
	iLayout->FindDocPos(newPos, aPos, &aLine);
	}

/** Find a position in the previous or next line close to the current cursor
position.

@param aUp ETrue if moving up, EFalse if moving down.
@param aPos returns the position found.
@param aLine returns details of the line containing aPos. */
void TCursorPosition::UpDownL(TTmPosInfo2& aPos, TTmLineInfo& aLine, TMovementType& aMove)
	{
	if (aMove != EFLineUp && aMove != EFLineDown)
	    {
	    OstTrace0( TRACE_DUMP, TCURSORPOSITION_UPDOWNL, "EFIncorrectCursorMovement" );
	    }
	__ASSERT_DEBUG(aMove == EFLineUp || aMove == EFLineDown,
		FormPanic(EFIncorrectCursorMovement));
	iLayout->FindDocPos(iDocPos, aPos, &aLine);
	TPoint c(iLatentX, aMove == EFLineUp?
		aLine.iOuterRect.iTl.iY - 1
		: aLine.iOuterRect.iBr.iY);
	
	// Ensure formatting extends to the new cursor position above or
	// below the current line. Solves the problem where scrolling down
	// at the bottom of a view to the next paragraph needed two
	// down cursor key presses.
	iLayout->ExtendFormattingToCoverYL(c.iY);

	TTmPosInfo2 otherPosInfo;
	CTextLayout::TTagmaForwarder forwarder(*iLayout);
	if (!forwarder.FindXyPosWithDisambiguation(c,
		otherPosInfo, aPos, aLine))
		{
		TBool toStart;
		if (aMove == EFLineUp)
			{
			toStart = ETrue;
			aMove = EFLineBeg;
			}
		else
			{
			toStart = EFalse;
			aMove = EFLineEnd;
			}
		TBool toRight = aLine.iFlags & TTmLineInfo::EParRightToLeft?
			toStart : !toStart;
		TTmDocPos newPos;
		iLayout->TagmaTextLayout().LineExtreme(aPos.iDocPos, toRight, newPos);
		iLayout->FindDocPos(newPos, aPos, &aLine);
		
		// as FindXyPosWithDisambiguation returned EFalse, there is no 
		// "otherPosInfo" so no need to ChoosePosition() just return
		return;
		}
	aPos = ChoosePosition(aPos, otherPosInfo);
	}

// Move the display one page and put the cursor back in the same place (as far as possible).
TInt TCursorPosition::PageScrollL(TMovementType& aMove)
	{
	if (aMove!=EFPageUp && aMove!=EFPageDown)
	    {
	    OstTrace0( TRACE_DUMP, TCURSORPOSITION_PAGESCROLLL, "EFIncorrectCursorMovement" );
	    }
	__ASSERT_DEBUG(aMove==EFPageUp || aMove==EFPageDown,FormPanic(EFIncorrectCursorMovement));
	TPoint scrPos;
	TInt pixelsScrolled;
	TTmDocPos docPos=iDocPos;

	if (!iLayout->PosInBand(iDocPos,scrPos))
		{
		if (aMove==EFPageUp)
			iLatentY=0;
		else
			iLatentY=(iLayout->BandHeight()-1);
		}
	if (aMove==EFPageUp)
		iLayout->PageUpL(iLatentY,pixelsScrolled);
	else
		iLayout->PageDownL(iLatentY,pixelsScrolled);
	scrPos.SetXY(iLatentX,iLatentY);
	DoSetXyPos(scrPos);
	if (docPos.iPos == iDocPos.iPos && pixelsScrolled == 0) // Cursor in first or last line
		{
		TPoint pos;
		CalculateCursorPos(pos);
		TInt y = pos.iY;
		if (aMove == EFPageUp)
			DoSetDocPosL(TTmDocPos(0, ETrue));
		else
			DoSetDocPosL(TTmDocPos(iLayout->DocumentLength(), EFalse));
		CalculateCursorPos(pos);
		// If the line didn't change change the movement type to reflect this.
		if (pos.iY == y)
			aMove = aMove == EFPageUp? EFLineBeg : EFLineEnd;
		}

	return pixelsScrolled;
	}

/*
Find the position of the line on the screen
Scroll it completely on if it is partly off.
*/
TInt TCursorPosition::CheckCursorOnScreenL(TInt& aY)
	{
	TRect cursorLine;
	const TInt botRow=iLayout->BandHeight();
	TInt linesToScroll=0;
	TInt pixels=0;

	iLayout->GetLineRect(aY,cursorLine);
	if (cursorLine.iTl.iY>aY || cursorLine.iBr.iY<aY)
	    {
	    OstTrace0( TRACE_DUMP, TCURSORPOSITION_CHECKCURSORONSCREENL, "EFPixelNotInFormattedLine" );
	    }
	__ASSERT_DEBUG(cursorLine.iTl.iY<=aY && cursorLine.iBr.iY>=aY,FormPanic(EFPixelNotInFormattedLine));
	if (cursorLine.iTl.iY<0 && cursorLine.iBr.iY<botRow)
		linesToScroll=1;
	else if (cursorLine.iBr.iY>botRow && cursorLine.iTl.iY>0)
		linesToScroll=-1;
	if (linesToScroll!=0)
		{
		pixels=iLayout->ScrollLinesL(linesToScroll);

		aY+=pixels;
		}
	if (aY<0 && aY>=botRow)
	    {
	    OstTrace0( TRACE_DUMP, DUP1_TCURSORPOSITION_CHECKCURSORONSCREENL, "EFCursorOffDisplay" );
	    }
	__ASSERT_DEBUG(aY>=0 || aY<botRow,FormPanic(EFCursorOffDisplay));
	if (!(cursorLine.Height()>=botRow || cursorLine.iBr.iY>=botRow 
	        || (cursorLine.iTl.iY+pixels>=0 && cursorLine.iBr.iY+pixels<=botRow)) )
	    {
	    OstTrace0( TRACE_DUMP, DUP2_TCURSORPOSITION_CHECKCURSORONSCREENL, "EFCursorOffDisplay" );
	    }
	__ASSERT_DEBUG(cursorLine.Height()>=botRow || cursorLine.iBr.iY>=botRow 
		|| (cursorLine.iTl.iY+pixels>=0 && cursorLine.iBr.iY+pixels<=botRow),FormPanic(EFCursorOffDisplay));
	return pixels;
	}

TBool TCursorPosition::GetCursor(TTmCursorPlacement aPlacement,TPoint& aOrigin,TInt& aWidth,TInt& aAscent,TInt& aDescent) const
	{
	TRect line_rect;
	TBool result = iLayout->GetCursor(iDocPos,aPlacement,line_rect,aOrigin,aWidth,aAscent,aDescent);
	return result;
	}
