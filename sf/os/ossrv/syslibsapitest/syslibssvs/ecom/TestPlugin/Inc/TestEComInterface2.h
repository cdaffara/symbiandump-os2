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
*
*/



#if (!defined __TEST_ECOM_INTERFACE2_H__)
#define __TEST_ECOM_INTERFACE2_H__

#include "TestEComInterface.h"

#define CTestEComInterface2_UID	0x20003724

// ____________________________________________________________________________
// 
/**
	@internalComponent
	Intended usage: This class implements the functionality promised by 
	the CTestEComInterface defintion class. It does little apart from provides a test instance
	which may be retrieved and run for testing purposes.
	Its resolution is based upon its registered default data string.
	@since 7.0
 */
class CTestEComInterface2 : public CTestEComInterface
{
// Methods
public:
/**
	@fn				NewL(TAny* aInitParams)
	Intended Usage	: Standardised safe construction which leaves nothing the cleanup stack.
	Error Condition	: Leaves with error code.
	@leave			KErrNoMemory.
	@since			7.0
	@return			CTestEComInterface2* The class instance.
	@pre 			None
	@post			CTestEComInterface2 has been constructed,
					and initialised.
 */
	static CTestEComInterface2*	NewL();

/**
	@fn				~CTestEComInterface2()
	Intended Usage	: Default Destructor
	Error Condition	: None
	@since			7.0
	@pre 			CTestEComInterface2 has been constructed
	@post			CTestEComInterface2 has been completely destroyed.
 */
	virtual ~CTestEComInterface2();

/**
	@fn				DoMethod1L()
	Intended Usage	: Overload of the pure interface method
					Representative of a method provided on 
					the interface by the interface definer.
	Error Condition	: Leaves with an error code.
	@leave			KErrNoMemory, KErrNotSupported.
	@since			7.0
	@return			None
	@pre 			CTestEComInterface2 has been constructed
	@post			Unspecified
 */
	void	DoMethod1L();

/**
	@fn				DoMethod2L()
	Intended Usage	: Overload of the pure interface method
					Representative of a method provided on 
					the interface by the interface definer.
	Error Condition	: Leaves with an error code.
	@leave			KErrNoMemory, KErrNotSupported.
	@since			7.0
	@return			Number of calls to DoMethod1L()
	@pre 			CTestEComInterfaceImplementation1 has been constructed
	@post			Unspecified
 */	
	TInt	DoMethod2L();

private:
/**
	@fn				CTestEComInterface2()
	Intended Usage	: Default Constructor : usable only by derived classes
	Error Condition	: None
	@since			7.0
	@pre 			None
	@post			CTestEComInterface2 has been constructed
 */
	CTestEComInterface2();

/**
	@fn				ConstructL(TAny* aInitParams)
	Intended Usage	: Completes the safe construction of the CTestEComInterface2 object
	Error Condition	: Leaves with the error code.
	@leave			KErrNoMemory.
	@since			7.0
	@pre 			CTestEComInterface2 has been constructed
	@post			CTestEComInterface2 has been fully initialised.
 */
	void	ConstructL();

private:
	TInt	iDoMethodL1Calls;
};  // End of CTestEComInterface2 definition
#endif /* __TEST_ECOM_INTERFACE2_H__ */
