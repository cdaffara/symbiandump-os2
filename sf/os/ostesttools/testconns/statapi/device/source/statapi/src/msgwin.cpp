/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



 /********************************************************************************
 *
 * System Includes
 *
 ********************************************************************************/
#include <e32std.h>
#include <e32base.h>
#include <eikenv.h>

/********************************************************************************
 *
 * Local Includes
 *
 ********************************************************************************/
#include "MsgWin.h"

/********************************************************************************
 *
 * TMsgWinDesOverflow
 *
 * Locally defined class to allow us to handle a text buffer overflow
 * within the call to 'AppendFormat' used to format the text for
 * the message window.
 *
 * If the overflow occurs the overridden method 'Overflow' is called.
 *
 ********************************************************************************/
class TMsgWinDesOverflow : public TDesOverflow
{
public:
	// From base interface class TDesOverflow.
	/* virtual */ void Overflow( TDes &aDes );
};

/********************************************************************************
 *
 * TMsgWinDesOverflow -- Overflow implementation
 *
 * Called if the buffer overflowed in the call to AppendFormat.
 * We replace the last few character with ellipses to indicate there
 * was more text that could not be shown.
 * The user has lost the end of their text anyway so changing the last few
 * characters will not make things worse.
 *
 ********************************************************************************/
void TMsgWinDesOverflow::Overflow( TDes &aDes )
{
	_LIT( ellipses, "..." );
	const TInt ellipsesLength = ellipses.operator const TDesC&().Length();

	TInt textLength = aDes.MaxLength();
	aDes.Replace( textLength - ellipsesLength, ellipsesLength, ellipses );
}

/********************************************************************************
 *
 * TMsgWin::Show
 *
 * Format the text the user passed and show a simple message window.
 * If the text was too long for our fixed buffer then we handle
 * the buffer overflow.
 *
 ********************************************************************************/
void TMsgWin::Show( TPtrC text, ... )
{
	static const TInt defLength = 64;
	TBuf<defLength> buffer(  _L("") );

	VA_LIST list;
	VA_START( list, text );

	buffer.Zero();

	// Use locally defined call for handling overflows.
	TMsgWinDesOverflow	overflowHandler;
	buffer.AppendFormatList( text, list, &overflowHandler);

	VA_END(list);

	CEikonEnv::Static()->AlertWin( buffer, _L("") );
}
