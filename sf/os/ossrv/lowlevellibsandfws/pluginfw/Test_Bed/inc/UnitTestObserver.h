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
// This file contains the definition of the class MUnitTestObserver
// 
//

/**
 @test
*/

#ifndef __UNITTESTOBSERVER_H__
#define __UNITTESTOBSERVER_H__

class CUnitTest;

/**
	@internalAll
	Comments : Provides a callback method to the CComponentTester to 
	indicate that the unit test has completed
 */

class MUnitTestObserver
	{
public:
/**
	@fn				virtual Complete(CUnitTest* aUnitTest) = 0
	Intended Usage	:	Allows a 'callback' to indicate that the observed class has
					completed its task
	Error Condition	: None	
	@since			7.0
	@param			aUnitTest The unit test which has completed
	@pre 			Should be called to indicate that all transitions in the unit test have completed
	@post			The unit test should have completed all operation as it could now be unloaded
 */
	
	virtual void Complete(CUnitTest* aUnitTest) = 0;
	};

#endif
