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



#include <e32std.h>

#define ASSERT_BUFFER	256

/********************************************************************************
 *
 * Assert_StripPath
 *
 * Remove the drive and folders from a full path and leave only 
 * the filename.
 ********************************************************************************/
void Assert_StripPath( TDes8 & file )
{
	static const TUint8 backSlash = '\\';
	TInt index = 0;
	if(KErrNotFound != (index = file.LocateReverse(backSlash)))
	{
		TBuf8<ASSERT_BUFFER> temp = file.Right(file.Length() - (index + 1));
		file = temp;
	}
}

/********************************************************************************
 *
 * AssertE
 *
 ********************************************************************************/
void AssertE( TPtrC8 _file, TInt _line )
{
	TBuf8<ASSERT_BUFFER> buffer8;
	TBuf8<256> file( _file );
	Assert_StripPath( file );
	buffer8.Format( (TPtrC8(reinterpret_cast<const TText8*>("!%S"))), &(file) );

#if defined (_UNICODE)
	TBuf16<ASSERT_BUFFER> buffer16;
	buffer16.Copy(buffer8);
	User::Panic(buffer16, _line);
#else // defined(_UNICODE)
	User::Panic(buffer8, _line);
#endif //defined(_UNICODE)
}

