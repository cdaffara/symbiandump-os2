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
// This file contains the definition of the class MComponentTestObserver
// 
//

/**
 @test
*/

#ifndef __COMPONENTTESTOBSERVER_H__
#define __COMPONENTTESTOBSERVER_H__

class CComponentTester;

/**
	@internalAll
	Comments : Allows a 'callback' through the Complete() function to indicate that the 
	observed CComponentTester has completed its task
 */

class MComponentTestObserver
	{
public:
/**
	@fn				virtual void Complete(CComponentTester* aTester, TInt aStatus) = 0
	Intended Usage	:	Indicates that the CComponentTester has completed a unit test
	@since			7.0
	@param			aTester The observed component tester.
	@param			aUnitTestId The id of the unit test which has completed.
	@pre 			The specified unit test should have been launched
	@post			If all unit tests are complete then the specified CComponentTester and 
	associated library are marked for deletion/unloading, or the next unit test is run.
 */
	
	virtual void Complete(CComponentTester* aTester, TInt aUnitTestId) = 0;
	};

#endif
