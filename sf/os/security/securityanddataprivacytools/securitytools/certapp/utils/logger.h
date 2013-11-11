#ifndef __LOGGER_H__
#define __LOGGER_H__/*
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


#include <iostream>

/**
 * @file
 * @internalComponent
 */

extern void FatalError();


class Log
	{
public:
	struct Indent {}; // log << Log::Endl() will do line end
	struct Endl {}; // log << Log::Endl() will do line end

	Log(const char *aPrefix = 0);

	void SetStream(std::ostream *aStream);
	std::ostream &Stream();

	// Write the current indent level to the progress stream
	void WriteIndent();
	// Increase indent level
	void IncIndent();
	// Decrease indent level
	void DecIndent();
	// Return current indent level
	int IndentLevel();

	template <typename T>
	inline Log &operator<<(const T &anObject)
		{
			*iStream << anObject;
			return *this;
		}

	inline Log &operator<<(const Indent &)
		{
			WriteIndent();
			return *this;
		}

	inline Log &operator<<(const Endl &)
		{
			*iStream << std::endl;
			return *this;
		}

private:
	std::ostream *iStream;
	int iIndent;
	const char *iPrefix;
	};


extern Log dbg;
extern Log prog;

class AutoIndent
	{
public:
	AutoIndent(Log &aLog)
		: iLog(aLog)
		{
			iLog.IncIndent();
		}
	~AutoIndent()
		{
			iLog.DecIndent();
		}
private:
	Log &iLog;
	};

#ifndef BULLSEYE_OFF
#ifdef _BullseyeCoverage
#define BULLSEYE_OFF "BullseyeCoverage save off";
#define BULLSEYE_RESTORE "BullseyeCoverage restore";
#else
#define BULLSEYE_OFF 
#define BULLSEYE_RESTORE 
#endif
#endif

#endif
