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
// This file contains the definition of the class MStateAccessor
// 
//

/**
 @test
*/

#ifndef __STATEACCESSOR_H__
#define __STATEACCESSOR_H__

#include <e32base.h>

/**
	@internalAll	
	Comments : Mixin which is used to define a class used for state checking of 
	a test object.
 */

class MStateAccessor
	{
public:
/**
	@fn				InvariantTest(TAny* aTestObject) = 0
	Intended Usage	:	Override this in the derived class to implement 
						invariant testing
	@since			7.0
	@param			aTestObject The object to be tested
	@return			One of the EPOC standard error codes indicating the result of the test
	@pre 			A valid test object must be passed in
	@post			Returns an error code dependant on the result of the test
 */
	
	virtual TInt InvariantTest(TAny* aTestObject) = 0;
/**
	@fn				Internalize(TAny* aTestObject) = 0
	Intended Usage	:	Override this in the derived class to implement 
						object state loading.
	@since			7.0
	@param			aTestObject The object to be tested
	@return			One of the EPOC standard error codes indicating the outcome of the attempt
	to load the object state.
	@pre 			A valid test object must be passed in
	@post			Returns an error code dependant on the result of the test
 */
	
	virtual TInt Internalize(TAny* aTestObject) = 0;
/**
	@fn				Externalize(TAny* aTestObject) = 0
	Intended Usage	:	Override this in the derived class to implement 
						object state persistence.
	@since			7.0
	@param			aTestObject The object to be tested
	@return			One of the EPOC standard error codes indicating the outcome of attempt
	to externalize the object.
	@pre 			A valid test object must be passed in
	@post			Returns an error code dependant on the result of the test
 */
	
	virtual TInt Externalize(TAny* aTestObject) = 0;
	};

#endif
