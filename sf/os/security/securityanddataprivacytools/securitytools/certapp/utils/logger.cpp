/*
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


#include "logger.h"


Log dbg("*** ");
Log prog;

Log::Log(const char *aPrefix)
	: iStream(0), iIndent(0), iPrefix(aPrefix)
{
}


void Log::SetStream(std::ostream *aStream)
{
	iStream = aStream;
	*iStream << std::hex;
}

std::ostream &Log::Stream()
{ 
BULLSEYE_OFF
	if(!iStream) FatalError();
BULLSEYE_RESTORE
	return *iStream; 
}

// Write the current indent level to the progress stream
void Log::WriteIndent()
{
BULLSEYE_OFF
	if(!iStream) FatalError();
BULLSEYE_RESTORE
	if(iPrefix)
		{
		*iStream << iPrefix;
		}
	
	for(int i=0; i < iIndent; ++i)
		{
		*iStream << '\t';
		}
	
}

// Increase indent level
void Log::IncIndent()
{
	++iIndent;
}

// Decrease indent level
void Log::DecIndent()
{
	--iIndent;
BULLSEYE_OFF
	if(iIndent < 0) FatalError();
BULLSEYE_RESTORE
}


int Log::IndentLevel()
{
	return iIndent;
}


void FatalError()
{
	dbg << Log::Indent() << "Fatal Error - processing aborted" << Log::Endl();
	exit(-1);
}


// End of file
