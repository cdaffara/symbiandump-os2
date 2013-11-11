// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// LOGCLISERVSHARED.CPP
// 
//

#include "LogCliServShared.h"



/////////////////////////////////////////////////////////////////////////////////////////
// -----> TLogWindow (source)
/////////////////////////////////////////////////////////////////////////////////////////

EXPORT_C TLogWindow::TLogWindow()
	{
	Reset();
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

EXPORT_C TBool TLogWindow::Contains(TInt aPosition) const
	{
	return (aPosition >= iLower && aPosition <= iUpper);
	}

EXPORT_C TInt TLogWindow::Range() const
	{
	if	(iUpper < 0)
		return 0;
	return (iUpper - iLower) + 1;
	}

EXPORT_C TInt TLogWindow::WindowIndexFromCursorPosition(TInt aCursorPosition) const
	{
	const TInt mapped = aCursorPosition - iLower;
	return mapped;
	}

EXPORT_C void TLogWindow::Reset()
	{
	iLower = -1;
	iUpper = -1;
	}

void TLogWindow::Normalize()
	{
	if	(iLower > iUpper)
		iLower = iUpper;
	}







/////////////////////////////////////////////////////////////////////////////////////////
// -----> TLogWindowAndCursor (source)
/////////////////////////////////////////////////////////////////////////////////////////

TLogWindowAndCursor::TLogWindowAndCursor()
	{
	Reset();
	}

TLogWindowAndCursor::TLogWindowAndCursor(const TLogWindow& aWindow, TInt aCursorPosition)
:	TLogWindow(aWindow), iCursorPosition(aCursorPosition)
	{
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

TLogWindowAndCursor::TAffected TLogWindowAndCursor::AdjustForItemDeletion(TInt aItemIndex)
	{
	const TInt KDelta = -1;
	//
	TAffected changed = EWindowUnaffected;
	if	(aItemIndex <= iUpper)
		{
		changed = EWindowAffected;

		if	(aItemIndex < iCursorPosition)
			--iCursorPosition;
		else if (aItemIndex == iCursorPosition && iCursorPosition == iUpper)
			--iCursorPosition;

		iUpper += KDelta;
		if	(aItemIndex < iLower)
			iLower += KDelta;
		//
		TLogWindow::Normalize();
		}
	return changed;
	}

TLogWindowAndCursor::TAffected TLogWindowAndCursor::AdjustForItemAddition(TInt aItemIndex)
	{
	///////////////////////////////////////
	// USE CASE 1:
	///////////////////////////////////////
	// Cursor position:  *
	// Window:			 ---------
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item X is added => 
	// 
	// Cursor position:    *
	// Window:			   ---------
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    X A B C D E F G
	// 
	///////////////////////////////////////
	// USE CASE 2:
	///////////////////////////////////////
	// Cursor position:    *
	// Window:			   ---------
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item X is added => 
	// 
	// Cursor position:      *
	// Window:			     ---------
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    X A B C D E F G
	// 
	///////////////////////////////////////
	// USE CASE 3:
	///////////////////////////////////////
	// Cursor position:          *
	// Window:			 ---------
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item X is added => 
	// 
	// Cursor position:            *
	// Window:			 -----------
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    A B C D X E F G
	// 
	///////////////////////////////////////
	// USE CASE 4:
	///////////////////////////////////////
	// Cursor position:        *  
	// Window:			 ---------
	// View Index:       0 1 2 3 4 5 6
	// View Contents:    A B C D E F G
	//
	// Then, item X is added => 
	// 
	// Cursor position:        *   
	// Window:			 ---------
	// View Index:       0 1 2 3 4 5 6 7
	// View Contents:    A B C D E X F G
	// 
	///////////////////////////////////////
	const TInt KDelta = 1;
	//
	TAffected changed = EWindowUnaffected;
	if	(aItemIndex <= iUpper) // UC4
		{
		changed = EWindowAffected;

		if	(aItemIndex <= iCursorPosition) // UC1
			++iCursorPosition;
		//
		iUpper += KDelta; // UC1
		if	(aItemIndex <= iLower) // UC1
			iLower += KDelta;
		//
		TLogWindow::Normalize();
		}
	return changed;
	}

TInt TLogWindowAndCursor::WindowIndexFromCursorPosition() const
	{
	return TLogWindow::WindowIndexFromCursorPosition(iCursorPosition);
	}

void TLogWindowAndCursor::Reset()
	{
	TLogWindow::Reset();
	iCursorPosition = -1;
	iValid = EFalse;
	}

void TLogWindowAndCursor::NormalizeWindowAndCursor()
	{
	TLogWindow::Normalize();
	if	(iCursorPosition < iLower)
		iCursorPosition = iLower;
	if	(iCursorPosition > iUpper)
		iCursorPosition = iUpper;
	}














/////////////////////////////////////////////////////////////////////////////////////////
// -----> TLogTransferWindow (source)
/////////////////////////////////////////////////////////////////////////////////////////

EXPORT_C TLogTransferWindow::TLogTransferWindow() :
	iBufferSize(0),
	iServerDataSize(0)
	{
	}

EXPORT_C void TLogTransferWindow::Reset()
	{
	TLogWindow::Reset();
	iBufferSize = iServerDataSize = 0;
	}


/////////////////////////////////////////////////////////////////////////////////////////
// -----> LogUtils (source)
/////////////////////////////////////////////////////////////////////////////////////////
/**  Retrieves appropriate date format string for the current locale.

@return    Format string for this locale. */
EXPORT_C const TDesC& LogUtils::DateFormatForLocale()
	{
    _LIT(KSQLDateFormatColon,"%D%*M%Y%1 %2 %3 %H:%T:%S%.%C"); 
    _LIT(KSQLDateFormatDot,"%D%*M%Y%1 %2 %3 %H.%T.%S%.%C");

	TLocale current; 
	TBool dateSeparatorIsColon=EFalse;
	for (TInt i=0; i<4; i++)
		{
		TChar c = current.DateSeparator(i);
		if (c==':')
			{
			dateSeparatorIsColon=ETrue;
			break;
			}
		}

	if (dateSeparatorIsColon)
		return KSQLDateFormatDot;
	return KSQLDateFormatColon;
	}


