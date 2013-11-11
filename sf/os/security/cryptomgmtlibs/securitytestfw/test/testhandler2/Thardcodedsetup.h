/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __T_HARDCODEDSETUP_H__
#define __T_HARDCODEDSETUP_H__

#include "t_testsetup.h"
#include "tHardcodedTests.h"

class CTestAction;
class TTestActionSpec;
class CTestSetup;
class CTestSpec;


class CHardcodedSetup : public CTestSetup
	{
public:
	/**
	 * This function creates a new CHardcodedSetup object.
	 */
	IMPORT_C static CHardcodedSetup* NewLC();

public:
	//aTestSpec is the class that holds the array of test actions, and has the GetNextTest function
	//theHardcodedTests is the static array of tests 
	//aCommandLineSettings contains the command line parameters
	IMPORT_C virtual void SetupTestsL(RFs& aFs,	CTestSpec& aTestSpec, THardcodedTests theHardcodedTests[],
		const CTestHandlerSettings & aCommandLineSettings);
	~CHardcodedSetup();
	IMPORT_C TBool InitialiseL(RFs &aFs, const TDesC& aDefaultLog = KNullDesC);

protected:
	CHardcodedSetup(void);
	
private:
	RFs iFs;

	};

#endif
