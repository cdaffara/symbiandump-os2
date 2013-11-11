/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __T_CERTSTORETESTS_H__
#define __T_CERTSTORETESTS_H__

#include <e32cons.h>

/** Run the standard test. */
void DoTests();

/** Run the specified test script. */
void DoTests(const TDesC& aScriptFile, const TDesC& aLogFile, TBool isConcurrent=EFalse);

// Implementation of dummy console for concurrency testing
class CNullConsole : public CConsoleBase
	{
public: // From CConsoleBase
	virtual TInt Create(const TDesC& /*aTitle*/, TSize /*aSize*/) { return KErrNone; }
	virtual void Read(TRequestStatus& aStatus)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrNone);
		}
	virtual void ReadCancel() {}
	virtual void Write(const TDesC& /*aDes*/) {}
	virtual TPoint CursorPos() const { return TPoint(); }
	virtual void SetCursorPosAbs(const TPoint& /*aPoint*/) {}
	virtual void SetCursorPosRel(const TPoint& /*aPoint*/) {}
	virtual void SetCursorHeight(TInt /*aPercentage*/) {}
	virtual void SetTitle(const TDesC& /*aTitle*/) {}
	virtual void ClearScreen() {}
	virtual void ClearToEndOfLine() {}
	virtual TSize ScreenSize() const { return TSize(); }
	virtual TKeyCode KeyCode() const { return TKeyCode(); }
	virtual TUint KeyModifiers() const { return 0; }
	};

#endif
