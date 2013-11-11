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
* Description: STIFUnit macros declarations
*
*/

#ifndef STIFUnit_MACROS_H
#define STIFUnit_MACROS_H

// Enumeration types
// Reason for running test method
enum TCallReason
    {
    EEnumerateTestCases,
    ERunTestCase,
    ECountTestCases, //Used to count test cases.
                     //This should be already done by enumerating procedure,
                     //however in case of test crash all internal data are
                     //being lost.
    };


// Strings
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


// Defines a separate test case which consists of two blocks - one for enumeration of test cases
// second for running the testcase.
#define STIF_TESTDEFINE( aTestName ) \
_test_case_no++; \
if( aRunReason == EEnumerateTestCases ) \
	{ \
  TTestCaseInfo* newCase = new( ELeave ) TTestCaseInfo(); \
  CleanupStack::PushL( newCase ); \
  newCase->iCaseNumber = _test_case_no; \
  newCase->iTitle.Copy( _L( #aTestName ) ); \
  User::LeaveIfError(aTestCases.Append ( newCase ) ); \
  CleanupStack::Pop( newCase ); \
	} \
else if(aRunReason == ERunTestCase && _test_case_no == aTestToRun)

#define STIF_RUN_SETUP -1
#define STIF_RUN_TEARDOWN -2

// Defines a setup section of MainTestL method
#define STIF_SETUP \
	if( aRunReason == ERunTestCase && aTestToRun == STIF_RUN_SETUP )

// Defines a teardown section of MainTestL method
#define STIF_TEARDOWN \
	if( aRunReason == ERunTestCase && aTestToRun == STIF_RUN_TEARDOWN )
		
/*********************************************************************************
 * Assert Macros
 *********************************************************************************/
#define __STIF_ASSERT_SHARED( aFunction, aMessage ) \
	if(!aFunction) \
		{ \
		iLog->Log( aMessage, __STIF_DBG_FILE__, __LINE__ );\
		aResult.SetResult( KErrGeneral, _L("Testcase failed"));\
		User::Leave( KErrGeneral );\
		}

#define __STIF_ASSERT_SHARED_DESC( aFunction, aMessage, aDesc ) \
	if(!aFunction) \
		{ \
		iLog->Log( aMessage, __STIF_DBG_FILE__, __LINE__ );\
		aResult.SetResult( KErrGeneral, aDesc );\
		User::Leave( KErrGeneral );\
		} \
	else \
		{ \
		aResult.SetResult( KErrNone, aDesc ); \
		}
		
		

#define STIF_ASSERT_EQUALS( aExpected, aActual ) \
	__STIF_ASSERT_SHARED( AssertEquals( aExpected, aActual ) , KAssertFailedEquals );

#define STIF_ASSERT_EQUALS_DESC( aExpected, aActual, aDescription ) \
	__STIF_ASSERT_SHARED_DESC( AssertEquals( aExpected, aActual ) , KAssertFailedEquals, aDescription ); 

#define STIF_ASSERT_NOT_EQUALS( aExpected, aActual ) \
	__STIF_ASSERT_SHARED( !AssertEquals( aExpected, aActual ) , KAssertFailedNotEquals );

#define STIF_ASSERT_NOT_EQUALS_DESC( aExpected, aActual, aDescription ) \
	__STIF_ASSERT_SHARED_DESC( !AssertEquals( aExpected, aActual ) , KAssertFailedNotEquals, aDescription );

#define STIF_ASSERT_NULL( aPtr ) \
	__STIF_ASSERT_SHARED( AssertNull( aPtr ), KAssertFailedNull );

#define STIF_ASSERT_NULL_DESC( aPtr, aDescription ) \
	__STIF_ASSERT_SHARED_DESC( AssertNull( aPtr ), KAssertFailedNull, aDescription );

#define STIF_ASSERT_NOT_NULL( aPtr ) \
	__STIF_ASSERT_SHARED( !AssertNull( aPtr ), KAssertFailedNotNull );

#define STIF_ASSERT_NOT_NULL_DESC( aPtr, aDescription ) \
	__STIF_ASSERT_SHARED_DESC( !AssertNull( aPtr ), KAssertFailedNotNull, aDescription );

#define STIF_ASSERT_SAME( aExpectedPtr, aActualPtr ) \
	__STIF_ASSERT_SHARED( AssertSame( aExpectedPtr, aActualPtr ), KAssertFailedSame );

#define STIF_ASSERT_SAME_DESC( aExpectedPtr, aActualPtr, aDescription ) \
	__STIF_ASSERT_SHARED_DESC( AssertSame( aExpectedPtr, aActualPtr ), KAssertFailedSame, aDescription );

#define STIF_ASSERT_NOT_SAME( aExpectedPtr, aActualPtr) \
	__STIF_ASSERT_SHARED( !AssertSame( aExpectedPtr, aActualPtr ), KAssertFailedNotSame );

#define STIF_ASSERT_NOT_SAME_DESC( aExpectedPtr, aActualPtr, aDescription ) \
	__STIF_ASSERT_SHARED_DESC( !AssertSame( aExpectedPtr, aActualPtr ), KAssertFailedNotSame, aDescription );

#define STIF_ASSERT_TRUE( aCondition ) \
	__STIF_ASSERT_SHARED( AssertTrue( aCondition ), KAssertFailedTrue );

#define STIF_ASSERT_TRUE_DESC( aCondition, aDescription ) \
	__STIF_ASSERT_SHARED_DESC( AssertTrue( aCondition ), KAssertFailedTrue, aDescription );

#define STIF_ASSERT_FALSE( aCondition ) \
	__STIF_ASSERT_SHARED( !AssertTrue( aCondition ), KAssertFailedFalse );

#define STIF_ASSERT_FALSE_DESC( aCondition, aDescription ) \
	__STIF_ASSERT_SHARED_DESC( !AssertTrue( aCondition), KAssertFailedFalse, aDescription );

// Eclosing block is used to create the scope for the __leaveValue	
#define STIF_ASSERT_NOT_LEAVES( aStatement ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED( AssertEquals( __leaveValue, KErrNone ), KAssertFailedNotLeaves ); \
    }

#define STIF_ASSERT_NOT_LEAVES_DESC( aStatement, aDescription ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED_DESC( AssertEquals( __leaveValue, KErrNone ), KAssertFailedNotLeaves, aDescription ); \
    }    

// Eclosing block is used to create the scope for the __leaveValue	
#define STIF_ASSERT_LEAVES( aStatement ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED( !AssertEquals( __leaveValue, KErrNone ), KAssertFailedLeaves ); \
    }

#define STIF_ASSERT_LEAVES_DESC( aStatement, aDescription ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED_DESC( !AssertEquals( __leaveValue, KErrNone ), KAssertFailedLeaves, aDescription ); \
    }   

// Eclosing block is used to create the scope for the __leaveValue	
#define STIF_ASSERT_LEAVES_WITH( aLeaveCode, aStatement ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED( AssertEquals( __leaveValue, aLeaveCode ), KAssertFailedLeaves ); \
    }

#define STIF_ASSERT_LEAVES_WITH_DESC( aLeaveCode, aStatement, aDescription ) \
    { \
    TRAPD( __leaveValue, aStatement ); \
    __STIF_ASSERT_SHARED_DESC( AssertEquals( __leaveValue, aLeaveCode ), KAssertFailedLeaves, aDescription ); \
    }

#define STIF_ASSERT_PANIC( aPanicCode, aStatement ) \
	{ \
	TestModuleIf().SetExitReason( CTestModuleIf::EPanic, aPanicCode ); \
	aStatement; \
	}

#define STIF_ASSERT_PANIC_DESC( aPanicCode, aStatement, aDescription ) \
	{ \
	TestModuleIf().SetExitReason( CTestModuleIf::EPanic, aPanicCode ); \
	aResult.SetResult(KErrNone, aDescription); \
	aStatement; \
	}

#include <StifUnitUtils.inl>

#endif

// End of File
