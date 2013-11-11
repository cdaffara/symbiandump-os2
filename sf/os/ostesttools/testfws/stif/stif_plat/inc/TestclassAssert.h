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
* Description: This file contains the header file of the testclass
* assert macros.
*
*/

#ifndef TestclassAssert_MACROS_H
#define TestclassAssert_MACROS_H

_LIT( KAssertFailedEquals, "AssertEquals Failed [F:%s][L:%d]" );
_LIT( KAssertFailedNotEquals, "AssertNotEquals Failed [F:%s][L:%d]" );
_LIT( KAssertFailedNull, "AssertNull Failed [F:%s][L:%d]" );
_LIT( KAssertFailedNotNull, "AssertNotNull Failed [F:%s][L:%d]" );
_LIT( KAssertFailedSame, "AssertSame Failed [F:%s][L:%d]" );
_LIT( KAssertFailedNotSame, "AssertNotSame Failed [F:%s][L:%d]" );
_LIT( KAssertFailedTrue, "AssertTrue Failed [F:%s][L:%d]" );
_LIT( KAssertFailedFalse, "AssertFalse Failed [F:%s][L:%d]" );
_LIT( KAssertFailedNotLeaves, "AssertNotLeaves Failed [F:%s][L:%d]" );
_LIT( KAssertFailedLeaves, "AssertLeaves Failed [F:%s][L:%d]" );
_LIT( KAssertFailedLeavesWith, "AssertLeavesWith Failed [F:%s][L:%d]" );


#ifdef _UNICODE
	#define __STIF_WIDEN2(x) L ## x
	#define __STIF_WIDEN(x) __STIF_WIDEN2(x)
	#define __STIF_DBG_FILE__ __STIF_WIDEN(__FILE__)
#else
	#define __STIF_DBG_FILE__ __FILE__
#endif


// Logs to the STIF log file AND to the RDebug
#define STIF_LOG( aMessage ) \
    iLog->Log( _L( aMessage ) ); RDebug::Print( _L( aMessage ) );

		
/*********************************************************************************
 * Assert Macros
 *********************************************************************************/
#define __STIF_ASSERT_SHARED( aFunction, aMessage ) \
	if(!aFunction) \
		{ \
		iLog->Log( aMessage, __STIF_DBG_FILE__, __LINE__ );\
		return KErrGeneral;\
		}

#define __STIF_ASSERT_SHARED_RET( aFunction, aMessage, aFailedReturn ) \
	if(!aFunction) \
		{ \
		iLog->Log( aMessage, __STIF_DBG_FILE__, __LINE__ );\
		return aFailedReturn;\
		}
		

#define STIF_ASSERT_EQUALS( aExpected, aActual ) \
	__STIF_ASSERT_SHARED( AssertEquals( aExpected, aActual ) , KAssertFailedEquals );

#define STIF_ASSERT_EQUALS_RET( aExpected, aActual, aFailedRet ) \
	__STIF_ASSERT_SHARED_RET( AssertEquals( aExpected, aActual ) , KAssertFailedEquals, aFailedRet );

#define STIF_ASSERT_NOT_EQUALS( aExpected, aActual ) \
	__STIF_ASSERT_SHARED( !AssertEquals( aExpected, aActual ) , KAssertFailedNotEquals );

#define STIF_ASSERT_NOT_EQUALS_RET( aExpected, aActual, aFailedRet ) \
	__STIF_ASSERT_SHARED_RET( !AssertEquals( aExpected, aActual ) , KAssertFailedNotEquals, aFailedRet );

#define STIF_ASSERT_NULL( aPtr ) \
	__STIF_ASSERT_SHARED( AssertNull( aPtr ), KAssertFailedNull );

#define STIF_ASSERT_NULL_RET( aPtr, aFailedRet ) \
	__STIF_ASSERT_SHARED_RET( AssertNull( aPtr ), KAssertFailedNull, aFailedRet );

#define STIF_ASSERT_NOT_NULL( aPtr ) \
	__STIF_ASSERT_SHARED( !AssertNull( aPtr ), KAssertFailedNotNull );

#define STIF_ASSERT_NOT_NULL_RET( aPtr, aFailedRet ) \
	__STIF_ASSERT_SHARED_RET( !AssertNull( aPtr ), KAssertFailedNotNull, aFailedRet );

#define STIF_ASSERT_SAME( aExpectedPtr, aActualPtr ) \
	__STIF_ASSERT_SHARED( AssertSame( aExpectedPtr, aActualPtr ), KAssertFailedSame );

#define STIF_ASSERT_SAME_RET( aExpectedPtr, aActualPtr, aFailedRet ) \
	__STIF_ASSERT_SHARED_RET( AssertSame( aExpectedPtr, aActualPtr ), KAssertFailedSame, aFailedRet );

#define STIF_ASSERT_NOT_SAME( aExpectedPtr, aActualPtr ) \
	__STIF_ASSERT_SHARED( !AssertSame( aExpectedPtr, aActualPtr ), KAssertFailedNotSame );

#define STIF_ASSERT_NOT_SAME_RET( aExpectedPtr, aActualPtr, aFailedRet ) \
	__STIF_ASSERT_SHARED_RET( !AssertSame( aExpectedPtr, aActualPtr ), KAssertFailedNotSame, aFailedRet );

#define STIF_ASSERT_TRUE( aCondition ) \
	__STIF_ASSERT_SHARED( AssertTrue( aCondition ), KAssertFailedTrue );

#define STIF_ASSERT_TRUE_RET( aCondition, aFailedRet ) \
	__STIF_ASSERT_SHARED_RET( AssertTrue( aCondition ), KAssertFailedTrue, aFailedRet );

#define STIF_ASSERT_FALSE( aCondition ) \
	__STIF_ASSERT_SHARED( !AssertTrue( aCondition ), KAssertFailedFalse );

#define STIF_ASSERT_FALSE_RET( aCondition, aFailedRet ) \
	__STIF_ASSERT_SHARED_RET( !AssertTrue( aCondition ), KAssertFailedFalse, aFailedRet );

// Eclosing block is used to create the scope for the __leaveValue	
#define STIF_ASSERT_NOT_LEAVES( aStatement ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED( AssertEquals( __leaveValue, KErrNone ), KAssertFailedNotLeaves ); \
    }

#define STIF_ASSERT_NOT_LEAVES_RET( aStatement, aFailedRet ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED_RET( AssertEquals( __leaveValue, KErrNone ), KAssertFailedNotLeaves, aFailedRet ); \
    }

// Eclosing block is used to create the scope for the __leaveValue	
#define STIF_ASSERT_LEAVES( aStatement ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED( !AssertEquals( __leaveValue, KErrNone ), KAssertFailedLeaves ); \
    }

#define STIF_ASSERT_LEAVES_RET( aStatement, aFailedRet ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED_RET( !AssertEquals( __leaveValue, KErrNone ), KAssertFailedLeaves, aFailedRet ); \
    }

// Eclosing block is used to create the scope for the __leaveValue	
#define STIF_ASSERT_LEAVES_WITH( aLeaveCode, aStatement ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED( AssertEquals( __leaveValue, aLeaveCode ), KAssertFailedLeaves ); \
    }

#define STIF_ASSERT_LEAVES_WITH_RET( aLeaveCode, aStatement, aFailedRet ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED_RET( AssertEquals( __leaveValue, aLeaveCode ), KAssertFailedLeaves, aFailedRet ); \
    }

#define STIF_ASSERT_PANIC( aPanicCode, aStatement ) \
	{ \
	TestModuleIf().SetExitReason( CTestModuleIf::EPanic, aPanicCode ); \
	aStatement; \
	}

template <class T>
inline TBool AssertEquals(const T& aExpected, const T& aActual)
/**
 * AssertEquals
 *
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

#endif

// End of File
