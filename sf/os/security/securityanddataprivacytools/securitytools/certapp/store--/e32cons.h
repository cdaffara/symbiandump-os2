#ifndef __e32cons_h__
#define __e32cons_h__/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
/**
 * @file
 * @internalComponent
 */
const TInt KConsFullScreen=-1;

typedef TInt TKeyCode;

class CConsoleBase
	{
public:
	void Printf(const TDesC16 &aFmt, ...);
	//	void Printf(const std::wstring str);
	TKeyCode Getch() { return 0x2a; }
	void Close() {}
	};

class Console
	{
public:
	static CConsoleBase *NewL(const TDesC &aTitle, TSize aSize);
	};

#endif
