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
* Description: Defines CATAlloc class.
*
*/


#ifndef __CATALLOC_H__
#define __CATALLOC_H__

#include "../inc/ATCommonDefines.h"

/**
* Class representing single memory allocation.
*/
class CATAlloc
{
public:
	/**
	* Constructor.
	*/
	CATAlloc() : m_iCSCount(0) {}

	/**
	* Destructor.
	*/
	~CATAlloc(){}

	/**
	* Get alloc string after address, it contains:
	* time, size, call stack..
	* @return string allocation string
	*/
	string GetAllocString();

	// Members

	string m_sTime; /** Allocation time */

	string m_sSize; /** Allocation size */

	string m_iThreadId; /** Thread id*/

	unsigned long m_iCSCount; /** Call stack address count */

	map<unsigned long,string> m_vCallStack; /** Call stack where packet number is key */
	
};
#endif
//EOF
