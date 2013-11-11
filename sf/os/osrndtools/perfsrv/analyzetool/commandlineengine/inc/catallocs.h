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
* Description: Defines CATAllocs "collection" class.
*/

#ifndef __CATALLOCS_H__
#define __CATALLOCS_H__

#include "../inc/ATCommonDefines.h"
#include "../inc/CATBase.h"
#include "../inc/catalloc.h"

/**
* Class representing a collection of memory allocations.
* It is used to acquire the data of memory leaks by giving all
* data on single allocations and frees.
*/
class CATAllocs : public CATBase
{
public:
	
	/**
	* "old" alloc message (prior to 1.6).
	* @param sAllocString allocation string
	*/
	void Alloc( const string& sAllocString );

	/*
	* Alloc header message (multimessage call stack).
	* @param sAllocHString allocation string
	*/
	void AllocH( const string& sAllocHString, const string& aTimeString );

	/*
	* Alloc fragment message (fragment of call stack in multimessage alloc).
	* @param sAllocFString allocation string
	*/
	void AllocF( const string& sAllocFString, const string& aTimeString );

    /*
	* Realloc header message (multimessage call stack).
	* @param sReallocHString allocation string
	*/
	void ReallocH( const string& sReallocHString, const string& aTimeString );

	/*
	* Relloc fragment message (fragment of call stack in multimessage alloc).
	* @param sReallocFString allocation string
	*/
	void ReallocF( const string& sReallocFString, const string& aTimeString );

	/**
	* Free message.
	* @param sFreeString string
	*/
	void Free( const string& sFreeString );
	
	/**
	* Free header message.
	* (not implemented yeat.)
	* @param sFreeHString string
	*/
	void FreeH( const string& sFreeHString, const string& aTimeString );

	/**
	* Free fragment message.
	* (not implemented yeat.)
	* @param sFreeFString string
	*/
	void FreeF( const string& sFreeFString, const string& aTimeString );

	/**
	* Get "leak" list ordered by allocation time.
	* @param vLeakList container where leaks stored
	*/
	void GetLeakList( vector<string>& vLeakList);
	
	/**
	* Clear leaks.
	*/
	void ClearAllocs();
#ifndef MODULE_TEST
private:
#endif
	map<string, CATAlloc> m_vAllocs; /** Collection of allocation(s) */
};
#endif
//EOF
