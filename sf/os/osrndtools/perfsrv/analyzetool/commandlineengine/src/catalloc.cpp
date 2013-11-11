/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "../inc/ATCommonDefines.h"
#include "../inc/catalloc.h"

string CATAlloc::GetAllocString( void )
{
	LOG_LOW_FUNC_ENTRY("CATAlloc::GetAllocString");
	// Create alloc string
	string sLine("");
	sLine.append( m_sTime ); sLine.append(" " );
	sLine.append( m_sSize );

	// Add all call stack fragments to line.
	string sCallStack;
	map<unsigned long, string>::iterator it;
	for( it = m_vCallStack.begin() ; it != m_vCallStack.end() ; it++ )
	{
		sCallStack.append( (*it).second );
		sCallStack.append( " " );
	}
	// Remove the last space
	if ( sCallStack.size() > 0 )
		sCallStack.erase( sCallStack.size()-1, 1 );
	
	if ( m_iCSCount != 0 )
	{
		// Check integrity (calculate number of spaces in call stack)
		unsigned long iCount = 0;
		size_t pos = 0;
		while ( pos != string::npos )
		{
			iCount++;
			pos = sCallStack.find_first_of( ' ', pos+1 );
		}
		if ( iCount != m_iCSCount )
			cout << AT_MSG << "Error, integrity check failed in alloc." << endl;
	}

	// Add call stack to line.
	if ( sCallStack.size() > 0 )
	{
		sLine.append(" " );
		sLine.append( sCallStack );
	}

	// return alloc line.
	return sLine;
}

