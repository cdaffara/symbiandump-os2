/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalTechnology
*/

#ifndef __FLOGSVR_INL__
#define __FLOGSVR_INL__

inline void CFileLogger::IncrementAccessCount()
/**
Increments access count for every new client

*/
	{
	iAccessCount++;
	}

inline void CFileLogger::DecrementAccessCount()
/**
Decrements server acess count
*/
	{

	iAccessCount--;
	__ASSERT_DEBUG(iAccessCount>=0, User::Invariant());
	}

inline TInt CFileLogger::AccessCount() const
/**
Returns the count of accesses in the server

@return Acess count of server session
*/
	{
	return iAccessCount;
	}

inline TLogFile CFileLogger::LogFile() const
	{
	return iLogFile;
	}

inline TBool CFileLogger::DeletionTimerActive() const
	{
	return iTimer->IsActive();
	}

#endif // __FLOGSVR_INL__

