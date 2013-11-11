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
// This file contains the definition of the class CComponentInfo
// 
//

/**
 @test
*/

#ifndef __COMPONENTINFO_H__
#define __COMPONENTINFO_H__

#include <e32base.h>
#include <f32file.h>

#include <ecom/test_bed/unittestinfo.h>
#include <ecom/test_bed/componenttester.h>

/**
	@internalAll
	Comments : Holds information about all the tests within a component.
 */

class CComponentInfo : public CBase
	{
public:

/**
	@fn			static CComponentInfo* NewLC(ComponentTesterInitialiserLC aEntryFunc,  RPointerArray<CUnitTestInfo>* aUnitTestsInfo)
	Intended Usage	:	Standard two phase construction which leaves a CComponentInfo on the 
	cleanupstack.  This object takes ownership of both parameters.
	Error Condition	:	
	@since			7.0
	@param			aEntryFunc Ptr to function used to create componentTester
	@param			aUnitTestsInfo The transition sets in this component test
	@return			CComponentInfo* A pointer to the newly created class
	@pre 			None
	@post			CComponentInfo is on the CleanupStack
 */
	
	static CComponentInfo* NewLC(ComponentTesterInitialiserLC aEntryFunc, RPointerArray<CUnitTestInfo>* aUnitTestsInfo);

/**
	@fn			static CComponentInfo* NewL(ComponentTesterInitialiserLC aCreateFuncLC,  RPointerArray<CUnitTestInfo>* aUnitTestsInfo)
	Intended Usage	:	Standard two-phase construction which leaves nothing on the 
	cleanup stack.  This object takes ownership of both parameters.
	Error Condition	:	
	@since			7.0
	@param			aCreateFuncLC Ptr to function used to create componentTester
	@param			aUnitTestsInfo The transition sets in this component test
	@return			CComponentInfo* A pointer to the newly created class
	@pre 			None
	@post			Nothing is on the CleanupStack
 */
	
	IMPORT_C static CComponentInfo* NewL(ComponentTesterInitialiserLC aCreateFuncLC, RPointerArray<CUnitTestInfo>* aUnitTestsInfo);

/**
	@fn				~CComponentInfo()
	Intended Usage	:	Standard destructor
	Error Condition	:	
	@since			7.0
 */
	
	~CComponentInfo();


/**
	@fn				inline ComponentTesterInitialiserLC GlobalEntryFunc() const
	Intended Usage	:	Get the pointer to the function entry point used to
						create the ComponentTester derived object which kick 
						starts the tests.
	Error Condition	:	
	@since			7.0
	@return			ComponentTesterInitialiserLC Function pointer
	@pre 			None
 */
	
	inline ComponentTesterInitialiserLC GlobalEntryFunc() const;

/**
	@fn				inline RPointerArray<CUnitTestInfo>& UnitTestsInfo() const
	Intended Usage	:	Get a list of the transition sets in this component
	Error Condition	:	
	@since			7.0
	@return			RPointerArray<CUnitTestInfo>& Information on the unit tests contained in 
	this component.
	@pre 			None
 */
	
	inline const RPointerArray<CUnitTestInfo>& UnitTestsInfo() const;

private:
/**
	@fn			CComponentInfo()
	Intended Usage	:	Standard constructor.  This object takes ownership of both parameters.
	Error Condition	:	
	@since			7.0
 */
	
	CComponentInfo();

/**
	@fn				void Construct(ComponentTesterInitialiserLC aEntryFunc, RPointerArray<CUnitTestInfo>* aUnitTestsInfo)
	Intended Usage	:	Standard second phase of construction
	Error Condition	:	
	@since			7.0
	@pre 			First phase of construction is complete
	@param			aEntryFunc Ptr to function used to create componentTester
	@param			aUnitTestsInfo The transition sets in this component test
	@post			Object is fully constructed
 */
	
	void Construct(ComponentTesterInitialiserLC aEntryFunc, RPointerArray<CUnitTestInfo>* aUnitTestsInfo);

private:
	/** Holds a pointer to the function capable of creating a CComponentTester derived object
	   which will run the unit tests outlined in iUnitTestsInfo.*/
	ComponentTesterInitialiserLC iComponentTesterGlobalCreationFuncLC;
/** Array of the subsets of transitions in this component. This object takes ownership of 
	iUnitTestsInfo at construction.*/
	
	RPointerArray<CUnitTestInfo>* iUnitTestsInfo;
	};

#include <ecom/test_bed/componentinfo.inl>

#endif
