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
* @file
* This contains inline methods of CDataWrapper
*
*/



/**
 @prototype
 @test
*/

#ifndef __DATA_WRAPPER_INL_
#define __DATA_WRAPPER_INL_

/**
 * Get the operation to cleanup the object returned by GetObject
 *
 * @see		TCleanupOperation
 *
 * @return	The cleanup operation. NULL if there is not one defined for the object.
 */
inline TCleanupOperation CDataWrapper::CleanupOperation()	{ return NULL; }

/**
 * Set the object that the wrapper contains
 *
 * @param	aObject - the object that the wrapper contains.
 *
 * @leave	KErrNotSupported if this operation is not supported
 */
inline void	 CDataWrapper::SetObjectL(TAny* /*aObject*/)	{ User::Leave(KErrNotSupported); }

/**
 * Removes the object that the wrapper contains. A subsequent call to
 * GetObject should return NULL until SetObject is called.
 *
 * @param	aObject - the object that the wrapper contains.
 *
 * @leave	KErrNotSupported if this operation is not supported
 */
inline void	 CDataWrapper::DisownObjectL()			{ User::Leave(KErrNotSupported); }

/**
 * Increment the number of outstanding async calls that are active
 */
inline void	 CDataWrapper::IncOutstanding()			{ iOutstanding++; }

/**
 * Decrement the number of outstanding async calls that are active
 */
inline void  CDataWrapper::DecOutstanding()			{ iOutstanding--; }

/**
 * Query if there are any outstanding async calls that are active.
 *
 * @return	ETrue if there are any oustnading active async calls.
 */
inline TBool CDataWrapper::Outstanding()			{ return iOutstanding>0; }

/**
 * Initialse the wrapper
 *
 * @leave	system wide error
 */
inline void  CDataWrapper::InitialiseL()			{}

#endif // ___DATA_WRAPPER_INL_
