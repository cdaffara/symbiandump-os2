/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* M_testspec.H
*
*/


#ifndef __MTESTSPEC_H__
#define __MTESTSPEC_H__

class CTestAction;

/**
 * This class defines the interface that must be provided by the objects passed
 * to a CTestHandler. It allows the CTestHandler object to get the next test it has to
 * run.
 */
class MTestSpec
	{
public:
	/**
	 * This function returns the next test to be run.
	 * @param aAction This is a reference to a pointer which will be initialized
	 * by the function so that it points to the next test to run. The action is owned
	 * by the MTestSpec and must not be deleted by the CTestHandler object.
	 * @return ETrue if there is another test to run. In this case aAction points to the 
	 * test to run.
	 * @return EFalse if there is no more test to run. In this case aAction is meaningless.
	 */
	virtual TBool GetNextTest(CTestAction*& aAction) = 0;
	};

#endif
