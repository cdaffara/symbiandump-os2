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

template <class T>
inline TBool AssertEquals(const T& aExpected, const T& aActual)
/**
 * AssertEquals
 *
 * @prototype
 * @test
 *
 * @param aExpected - Expected result
 * @param aActual - Actual result
 * @return - True if equal
 */
	{
	if( aExpected==aActual )
		{
		return ETrue;
		}
	return EFalse;
	}

template <class T>
inline TBool AssertNull(const T* aPtr)
/**
 * AssertNull
 *
 * @prototype
 * @test
 *
 * @param aPtr - Pointer
 * @return - True if NULL
 */
	{
	if( aPtr==NULL )
		{
		return ETrue;
		}
	return EFalse;
	}

template <class T>
inline TBool AssertSame(const T* aExpectedPtr, const T* aActualPtr)
/**
 * AssertSame
 *
 * @prototype
 * @test
 *
 * @param aExpectedPtr - Expected pointer
 * @param aActualPtr - Actual pointer
 * @return - True if equal
 */
	{
	if( aExpectedPtr==aActualPtr )
		{
		return ETrue;
		}
	return EFalse;
	}

inline TBool AssertTrue(const TBool& aCondition)
/**
 * AssertTrue
 *
 * @prototype
 * @test
 *
 * @param aCondition - Condition
 * @return - True if aCondition is true
 */
	{
	if( !aCondition )
		{
		return EFalse;
		}
	return ETrue;
	}

// End of File
