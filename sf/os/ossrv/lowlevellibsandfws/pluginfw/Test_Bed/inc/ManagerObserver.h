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
// This file contains the definition of the class MManagerObserver
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

/**
 @test
*/

#ifndef __MANAGEROBSERVER_H__
#define __MANAGEROBSERVER_H__

/**
	@internalAll
	Comments : Observer to allow the CTestManager to indicate when all the
	tests are complete.
*/
class MManagerObserver
	{
public:
/**
	@fn				TestsComplete()
	Intended Usage	: 
	@since			7.0
	@pre 			Observer class is constructed
	@post			Depends on implementation
 */
	virtual void TestsComplete() = 0;
	};

#endif // __MANAGEROBSERVER_H__
