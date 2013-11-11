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
* This contains TestBlockController.inl
* This contains CTestBlockController inline methods
*
*/



/**
 @prototype
 @test
*/
#ifndef __TEST_BLOCK_CONTROLLER_INL_
#define __TEST_BLOCK_CONTROLLER_INL_

/**
 * Increment the number of active async operations
 */
inline void	CTestBlockController::IncAsyncCount() { ++iAsyncCount; }

/**
 * Decrement the number of active async operations
 */
inline void	CTestBlockController::DecAsyncCount() { --iAsyncCount; }

#endif // __TEST_BLOCK_CONTROLLER_INL_
