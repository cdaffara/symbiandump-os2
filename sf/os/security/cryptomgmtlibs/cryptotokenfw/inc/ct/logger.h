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
* ct/logger.h
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CT_LOGGER_H__
#define __CT_LOGGER_H__

#include <e32std.h>

/**
 * 
 * Defines logging functionality for use by crypto tokens framework and file
 * tokens server.  This is only compiled in debug builds when the macro
 * __CT_LOGGING__ is defined.
 *
 * @internalTechnology
 */

#if defined(_DEBUG) && defined(__CT_LOGGING__)

#define LOG(X)				CTLogger::Log(this, (X))
#define LOG1(X, Y)			CTLogger::Log(this, (X), (Y))
#define LOG2(X, Y, Z)		CTLogger::Log(this, (X), (Y), (Z))
#define LOG_INC_INDENT()	CTLogger::UpdateIndent(1)
#define LOG_DEC_INDENT()	CTLogger::UpdateIndent(-1)

#else

#define LOG(X)
#define LOG1(X, Y)
#define LOG2(X, Y, Z)
#define LOG_INC_INDENT()
#define LOG_DEC_INDENT()

#endif

/**
 * Class providing methods used by logging macros.  These are not designed to be
 * called directly.  If logging is not turned on they will simply panic.
 */
class CTLogger
	{
public:
	IMPORT_C static void Log(TAny* aObject, TRefByValue<const TDesC16> aFmt, ...);
	IMPORT_C static void UpdateIndent(TInt aInc);
private:
	static void LogL(const TDesC& aString);
	};

#endif // __CTLOGGER_H__
