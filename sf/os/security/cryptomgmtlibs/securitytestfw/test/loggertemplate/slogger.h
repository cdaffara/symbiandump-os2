/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __SLOGGER_H__
#define __SLOGGER_H__

#include <e32std.h>

#if defined (_DEBUG) && defined (_LOGGING)
#define __LOG(X,Y)   SLogger::Log(X,Y, TPtrC8((const TText8 *) __FILE__), __LINE__)
#else
#define __LOG(X,Y)
#endif //defined (_DEBUG) && defined (_LOGGING)

class SLogger
	{
 public:
	IMPORT_C static void Log(const TDesC& aLogFileName, const TDesC& aString, 
		const TDesC8& aSourceFileName, TInt aLineNumber);
	IMPORT_C static void Log(const TDesC& aLogFileName, TInt aInt, 
		const TDesC8& aSourceFileName, TInt aLineNumber);
	};

#endif
