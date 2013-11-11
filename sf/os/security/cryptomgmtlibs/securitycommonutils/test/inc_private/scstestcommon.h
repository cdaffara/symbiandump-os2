/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Data structures and values which are used by both client and server
* implementations, but which are not exposed to the client.
*
*/


/**
 @internalComponent
 @test
 @file
*/

#ifndef SCSTESTCOMMON_H
#define SCSTESTCOMMON_H


namespace ScsTestImpl
	{
	/** The server name, to which clients connect. */
	_LIT(KServerName, "ScsTest");

	const TInt KScsTestVerMajor = 1;	///< SCS test server version major component.
	const TInt KScsTestVerMinor = 0;	///< SCS test server version minor component.
	const TInt KScsTestVerBuild = 0;	///< SCS test server version build number.

	inline TVersion Version();

	/** Executable image which runs the server process. */
	_LIT(KServerImageName, "scstestserver.exe");

	/** SCS test server SID. */
	const TUid KScsTestServerUid = {0x10283512};

	inline TUidType ServerImageFullUid();
	
	enum TSessionFunction
	/** Functions supported by SCS test session. */
		{
		/**
			This unused function is provided for test code to confirm the
			session handles unrecognized function identifiers correctly.
		 */
		ESessUnused = 0,
		ESessDouble = 1,		///< Doubles a client-side TInt.
		ESessTreble = 2,		///< Asynchronously trebles a client-side TInt.
		ESessNukeServer = 4,	///< Call with no arguments which panics server
		ESessSubsessFromInt = 4	///< Create subsession curried on an integer value.
		};
	
	enum TSubsessionFunction
	/** Functions supported by test server subsession. */
		{
		/**
			This function identifier is only defined to ensure the test subsession
			implementation correctly handles unrecognized functions.
		 */
		ESubsessUnused = 0,
		ESubsessQuadruple = 1,	///< Quadruple the curried value.
		ESubsessTreble			///< Asynchronously trebles a client-side TInt.
		};

	/** Delay before the asynchronous treble request is completed after construction. */
	static const TInt KTrebleTimerDelayUs = 5 * 1000 * 1000;
	
	/** The server is shut down if no sessions have been connected for this amount of time. */
	const TInt KShutdownPeriodUs = 2 * 1000 * 1000;
	}	// namespace UserPromptService

template <class T>
inline T TAnyPtrToFuncPtr(TAny* aPtr);


#include "scstestcommon.inl"

#endif	// SCSTESTCOMMON_H

