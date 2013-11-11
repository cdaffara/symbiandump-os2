/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_DUMMYCONSOLE_H__
#define __T_DUMMYCONSOLE_H__

#include <e32base.h>
#include <f32file.h>

/**
 * A console that discards all output.
 */
class CDummyConsole : public CConsoleBase
	{
public:
	CDummyConsole() {}
public:
	// From CConsoleBase
	virtual TInt Create(const TDesC& /*aTitle*/, TSize /*aSize*/) { return KErrNone; }
	virtual void Read(TRequestStatus &aStatus) { TRequestStatus* status = &aStatus; User::RequestComplete(status, KErrNone); }
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
