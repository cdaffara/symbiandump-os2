#ifndef __COMMANDLINEARGS_H__
#define __COMMANDLINEARGS_H__/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <in_sock.h>

_LIT(KCommandLineDaylightSavings, "-savings");
_LIT(KCommandLineOffset, "-offset");

class TCommandLineArgs
	{
	
public:

	TInt iOffset;
	TBool iApplyDaylightSavings;
	RPointerArray<HBufC> iServers;
	
	};

#endif /* __COMMANDLINEARGS_H__ */
