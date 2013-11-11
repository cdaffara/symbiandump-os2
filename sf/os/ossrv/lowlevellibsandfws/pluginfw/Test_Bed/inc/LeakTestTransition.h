// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Overload of the CTransition test that provides
// Heap and Handle leak testing upon a test method.
// 
//

/**
 @test
*/

#ifndef __LEAKTESTTRANSITIONS_H__
#define __LEAKTESTTRANSITIONS_H__

#include <ecom/test_bed/testutilities.h>
#include <ecom/test_bed/transition.h>
#include <ecom/test_bed/testbeddefinitions.h>

// Constants used to specify various internal states of the CLeakTestTransition
const TInt KMemoryLeakTestBreakNone = -1;
const TInt KMemoryLeakTestFailNone = 0;
const TInt KMemoryLeakTestFailInit = 1;
/** RunError depth limit test */
const TInt KProbablyInfinitelyLooping = 10000;

/**
	@internalAll
	Comments :  Overloaded Base class from which test developers 
				can derive their own transitions for both
				synchronous and asynchronous methods on
				the test class.	
				@see CTransition.
				This overload checks that memory does not
				leak when using a test method, and that 
				open handles are closed correctly.
				It is therefore not suitable for testing methods 
				which allocate/ or deallocate memory that persists after the method has
				completed, or which open/close handles to resources for the class
				which remain open/closed after the method completes.
			
				This base class is not really suitable for use in testing non-leaving methods.
				(The result being that sections of the code called from within the 
				leaving method will if trapped, fail and not exercise the code fully.)
 */

class CLeakTestTransition : public CTransition
	{
protected:
/**
	@fn				 IMPORT_C CLeakTestTransition(const TDesC&			aTransitionId, 
												  CUnitTestContext&		aUTContext,
												  TTransitionValidator&	aValidator)
	Intended Usage	: Default Constructor
	@since			7.0
	@param			aTransactionId The unique id of this transition.
	@param			aUTContext The context to run this transition in
	@param			aValidator Provides Pre & Post Validation checking
	@pre 			None
	@post			First phase of two phase construction is complete
 */
	IMPORT_C CLeakTestTransition(const TDesC&			aTransitionId, 
								 CUnitTestContext&		aUTContext,
								 TTransitionValidator&	aValidator);
/**
	@fn				IMPORT_C virtual void RunL()
	Intended Usage	:	Override of CTransition virtual method.  
	@see CTransition::RunL() for normal CTransition behaviour.
	This overload sets up the heap allocation to fail upon a deterministic
	count that is incremented once for each time the RunError method fires
	with a KErrNoMemory error.
	Whichever method is used the second call of RunL() will complete the request 
	of the CUnitTest which called this transition by setting its status word.
	If it has been requested that this transition is repeated then the status 
	word will be set to KTestBedRepeatTest.
	Error Condition	:
	@since			7.0
	@pre 			RunTransition() ensures the preconditions for this function
	@post			With iTransitionFinished == ETrue test has been completed.
					Otherwise - Leak test has been initialized and 
					test method has been called via CTransition::RunL()
 */
	
	IMPORT_C virtual void RunL();


/**
	@fn				IMPORT_C virtual TInt RunError(TInt aErrorCode)
	Intended Usage	: Handle the RunL leave that upsets the Active Scheduler.
	Error Condition	: @see CTransition.
	@since			7.0
	@param			anErrorCode The error code that was returned by the Leave;
	@return			TInt KErrNone  
	@pre 			CLeakTestTransition has been fully constructed, and initialized and 
					RunL left with an error code.
	@post			CLeakTestTransition is returned to a sensible state and the test is 
					resceduled if aErrorCode was KErrNoMemory.
 */
	
	IMPORT_C virtual TInt RunError(TInt aErrorCode);

/**
	@fn				PostTransitionCleanup()
	Intended Usage	: Called from the RunL immediately after TransitMethodL() executes
	@since			7.0
	@pre 			Must be called from RunL only after TransitMethodL() has completed.
	@post			Heap failure is immediately disabled.
 */
	
	IMPORT_C virtual void PostTransitionCleanup();

/** The count of process open handles at the start of the RunL. */
	
	TInt iStartProcessHandleCount;
/** The count of thread open handles at the start of the RunL. */
	
	TInt iStartThreadHandleCount;
/** The count of process open handles at the end of the RunL/RunError. */
	
	TInt iEndProcessHandleCount;
/** The count of thread open handles at the end of the RunL/RunError. */
	
	TInt iEndThreadHandleCount;
/** The current thread handle */
	
	RThread iThread;
				
/** The current iteration of the test method upon which to fail. */
	
	TInt iFailStep;
/** The iteration of the test method upon which an unexpected failure occurred. */
	
	TInt iBreakStep;
	};

#endif	// __LEAKTESTTRANSITIONS_H__

