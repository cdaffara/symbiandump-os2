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
// The definitions of the transition classes upon the CDataLogger class methods.
// 
//

#ifndef __DATALOGGERTRANSITIONS_H__
#define __DATALOGGERTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <ecom/test_bed/datalogger.h>
#include "DataLoggerStateAccessor.h"
#include <ecom/test_bed/unittest.h>

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CDataLogger specific
	parameters and behaviour on the CDataLogger
	test class for a transition.
 */
class CDataLogger_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CDataLogger_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CDataLogger class under test.
		@pre 			None.
		@post			CDataLogger_UnitTestContext is fully constructed, and initialised.
	*/
	inline CDataLogger_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CDataLogger_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CDataLogger_UnitTestContext is fully constructed.
		@post			CDataLogger_UnitTestContext is fully destroyed
		*/
		virtual inline ~CDataLogger_UnitTestContext();

	/** The instance of the class under test */
	CDataLogger* iDataLogger;
	/** The logging info to be used by the data logger */
	TLoggingInfo* iLoggingInfo;
	/** The address of the start of the block to be logged */
	const TUint8* iAddress;
	/** The number of bytes to be logged */
	TInt iLength;
	/** The argument set to be logged */
	HBufC* iArg;
	/** A narrow descriptor version of the argument set to be logged */
	HBufC8* iArgNarrow;
	/** The text to be added to the report */
	const TDesC* iReportComment;
	/** The text to be formatted and added to the report */
	const TDesC* iFormatReport;
	/** The argument set to be added to the report */
	const TDesC* iReportArg;

	}; // CDataLogger_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent
 
	Comments : Transition test of the CDataLogger::NewL method.
 */
class CDataLogger_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDataLogger_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_NewL_Transition is fully constructed.
	*/
	CDataLogger_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::NewL)
		@since			7.0
		@return			None
		@pre 			CDataLogger_NewL_Transition is fully constructed.
		@post			No change in the CDataLogger_NewL_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::NewL post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_NewL_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDataLogger::Dtor method.
 */
class CDataLogger_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CDataLogger_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_Dtor_Transition is fully constructed.
	*/
	CDataLogger_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::Dtor)
		@since			7.0
		@return			None
		@pre 			CDataLogger_Dtor_Transition is fully constructed.
		@post			No change in the CDataLogger_Dtor_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::Dtor post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_Dtor_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDataLogger::DumpMemoryBlock method.
 */
class CDataLogger_DumpMemoryBlock_Transition : public CTransition
	{
public:
	/**
		@fn				CDataLogger_DumpMemoryBlock_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_DumpMemoryBlock_Transition is fully constructed.
	*/
	CDataLogger_DumpMemoryBlock_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::DumpMemoryBlock method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::DumpMemoryBlock)
		@since			7.0
		@return			None
		@pre 			CDataLogger_DumpMemoryBlock_Transition is fully constructed.
		@post			No change in the CDataLogger_DumpMemoryBlock_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::DumpMemoryBlock post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_DumpMemoryBlock_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_DumpMemoryBlock_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDataLogger::LogInformation method.
 */
class CDataLogger_LogInformation_Transition : public CTransition
	{
public:
	/**
		@fn				CDataLogger_LogInformation_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_LogInformation_Transition is fully constructed.
	*/
	CDataLogger_LogInformation_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::LogInformation method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::LogInformation)
		@since			7.0
		@return			None
		@pre 			CDataLogger_LogInformation_Transition is fully constructed.
		@post			No change in the CDataLogger_LogInformation_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::LogInformation post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_LogInformation_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_LogInformation_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDataLogger::LogInformationNarrow method.
 */
class CDataLogger_LogInformationNarrow_Transition : public CTransition
	{
public:
	/**
		@fn				CDataLogger_LogInformationNarrow_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_LogInformationNarrow_Transition is fully constructed.
	*/
	CDataLogger_LogInformationNarrow_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::LogInformationNarrow method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::LogInformationNarrow)
		@since			7.0
		@return			None
		@pre 			CDataLogger_LogInformationNarrow_Transition is fully constructed.
		@post			No change in the CDataLogger_LogInformationNarrow_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::LogInformationNarrow post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_LogInformationNarrow_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_LogInformationNarrow_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDataLogger::LogInformationWithParameters method.
 */
class CDataLogger_LogInformationWithParameters_Transition : public CTransition
	{
public:
	/**
		@fn				CDataLogger_LogInformationWithParameters_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_LogInformationWithParameters_Transition is fully constructed.
	*/
	CDataLogger_LogInformationWithParameters_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::LogInformationWithParameters method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::LogInformationWithParameters)
		@since			7.0
		@return			None
		@pre 			CDataLogger_LogInformationWithParameters_Transition is fully constructed.
		@post			No change in the CDataLogger_LogInformationWithParameters_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::LogInformationWithParameters post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_LogInformationWithParameters_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_LogInformationWithParameters_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDataLogger::LogInformationWithParametersNarrow method.
 */
class CDataLogger_LogInformationWithParametersNarrow_Transition : public CTransition
	{
public:
	/**
		@fn				CDataLogger_LogInformationWithParametersNarrow_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_LogInformationWithParametersNarrow_Transition is fully constructed.
	*/
	CDataLogger_LogInformationWithParametersNarrow_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::LogInformationWithParametersNarrow method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::LogInformationWithParametersNarrow)
		@since			7.0
		@return			None
		@pre 			CDataLogger_LogInformationWithParametersNarrow_Transition is fully constructed.
		@post			No change in the CDataLogger_LogInformationWithParametersNarrow_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::LogInformationWithParametersNarrow post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_LogInformationWithParametersNarrow_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_LogInformationWithParametersNarrow_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDataLogger::ReportInformation method.
 */
class CDataLogger_ReportInformation_Transition : public CTransition
	{
public:
	/**
		@fn				CDataLogger_ReportInformation_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_ReportInformation_Transition is fully constructed.
	*/
	CDataLogger_ReportInformation_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::ReportInformation method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::ReportInformation)
		@since			7.0
		@return			None
		@pre 			CDataLogger_ReportInformation_Transition is fully constructed.
		@post			No change in the CDataLogger_ReportInformation_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::ReportInformation post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_ReportInformation_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_ReportInformation_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Transition test of the CDataLogger::ReportInformationWithParameters method.
 */
class CDataLogger_ReportInformationWithParameters_Transition : public CTransition
	{
public:
	/**
		@fn				CDataLogger_ReportInformationWithParameters_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDataLogger_ReportInformationWithParameters_Transition is fully constructed.
	*/
	CDataLogger_ReportInformationWithParameters_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDataLogger::ReportInformationWithParameters method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDataLogger::ReportInformationWithParameters)
		@since			7.0
		@return			None
		@pre 			CDataLogger_ReportInformationWithParameters_Transition is fully constructed.
		@post			No change in the CDataLogger_ReportInformationWithParameters_Transition apart
						from iDataLogger, which may have changed state.
						(@see CDataLogger::ReportInformationWithParameters post-condition) for iDataLogger's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDataLogger_UnitTestContext
		@pre 			iUTContext is a valid CDataLogger_UnitTestContext.
		@post			No change in the CDataLogger_ReportInformationWithParameters_Transition
	*/
	inline CDataLogger_UnitTestContext& Context() const;
	};	// CDataLogger_ReportInformationWithParameters_Transition


// Add additional Transition class definitions here...

#include "DataLoggerTransitions.inl"

#include "DataLoggerUnitTestContext.inl"

#endif // __DATALOGGERTRANSITIONS_H__

