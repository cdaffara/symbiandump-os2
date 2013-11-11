/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TESTHANDLERSETTINGS_H__
#define __TESTHANDLERSETTINGS_H__

#include <e32base.h>

/**
 * This class stores the various settings that define the behaviour of
 * the test handler.
 *
 * this is a C-class, but it doesn't derived from CBase!
 */
class CTestHandlerSettings
	{
public:
	/**
	 * This function creates a new CTestHandlerSettings object.
	 */
	IMPORT_C static CTestHandlerSettings* NewLC();
	/**
	 * This function creates a new copy of the argument.
	 */
	IMPORT_C static CTestHandlerSettings* NewL(const CTestHandlerSettings& aOther);
	/**
	 * The destructor.
	 */
	IMPORT_C ~CTestHandlerSettings();

private:
	/**
	 * The default constructor produces settings adequate for running automated tests.
	 * The settings are all set to false.
	 */
	CTestHandlerSettings();
	/**
	 * The second-phase constructor.
	 */
	void ConstructL();
	
public:
	/**
	 * If this is set to ETrue, then at the end of the tests, the handler will not
	 * quit immediately : it will wait until the user has pressed a key to quit.
	 * This is to allow the user to read the contents of the screen.
	 */
	TBool iWaitForKeyPressAtEnd;

	/**
	 * Used to hold the bitflag values for the switches read in  
	 * from the commandline
	 */
	TBool iOOM;
	TBool iCancel;
	TBool iSkip;
	TBool iExhaust;
	TBool iInt;
	};

#endif
