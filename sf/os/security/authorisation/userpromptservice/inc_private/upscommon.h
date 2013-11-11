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
* Implementation data shared between UPS client and server implementations,
* but not with the UPS client.
*
*/


/**
 @file
 @internalComponent
 @released
*/


#ifndef UPSCOMMON_H
#define UPSCOMMON_H

#include <e32ver.h>
#include <e32uid.h>

#ifndef BULLSEYE_OFF
#ifdef _BullseyeCoverage
#define BULLSEYE_OFF "BullseyeCoverage save off";
#define BULLSEYE_RESTORE "BullseyeCoverage restore";
#else
#define BULLSEYE_OFF 
#define BULLSEYE_RESTORE 
#endif
#endif

namespace UserPromptService
	{
	_LIT(KUpsServerName, "!UpsServer");		///< Identifies UPS Symbian OS server.

	const TInt KUpsVerMajor = 1;		///< UPS server version major component.
	const TInt KUpsVerMinor = 0;		///< UPS server version minor component.
	const TInt KUpsVerBuild = 0;		///< UPS server version build component.

	inline TVersion Version();

	/**
		Executable which hosts UPS server.  Required to launch the process.

		@see KUpsServerUid
	 */
	_LIT(KServerImageName, "upsserver.exe");

	/**
		Hosting executable's secure ID.  Required to launch the process.

		@see KServerImageName
	 */
	const TUid KUpsServerUid = {0x10283558};

	inline TUidType ServerImageFullUid();

	/// The UPS uses this property to notify the clients RUpsSession objects (via their CPolicyChangeWatcher objects)
	/// that they need to re-read the service config information because it may have changed.
	/// This is particularly important if the setting is changing from "never check" to "always check".
	const TInt KUpsServiceConfigProperty = 42;

	enum TSessionFunction
	/**
		Functions supported by a UPS session, as opposed to subsession.
	 */
		{
			ESessSubsessFromThreadId = 0,	///< Create a subsession current on a thread.
			EGetClientConfigLength,			///< Get the number of elements in the client config array
			EGetClientConfigData,			///< Get the client config array
			EMngmntRead = 0x100,
			ECreateView,
			ENextMatch,
			ECloseView,
			EMngmntDelete = 0x200,
			EDeleteDatabase,
			ERemoveDecisions,
			EMngmntUpdate = 0x300,
			EUpdateDecision,
			ESwiObserver = 0x400,
			EDeleteDecisionsForExe,
			ENotifyPluginsMayHaveChanged,
			ENotifyPolicyFilesChanged,
			ECancelNotifyPolicyFilesChanged
		};

	enum TSubsessionFunction
	/**
		Functions supported by a UPS subsession.
	 */
		{
			ESubsessPreparePrompt = 0,
			ESubsessExecutePrompt
		};
	
	/**
		Delay in microseconds before the server is shut down, after
		the last remaining session has been closed.
	 */
	const TInt KShutdownPeriodUs = 2 * 1000 * 1000;

	//Shows the flag array index of each column in a filter key 
	static const TUint8  KPosClientSid			= 0x00;
	static const TUint8  KPosEvaluatorId		= 0x01;
	static const TUint8  KPosServiceId			= 0x02;
	static const TUint8  KPosServerSid 			= 0x03;
	static const TUint8  KPosFingerprint		= 0x04;
	static const TUint8  KPosClientEntity		= 0x05;
	static const TUint8  KPosMajorPolicyVersion	= 0x06;
	static const TUint8  KPosRecordId			= 0x07;
	static const TUint8  KPosDescription		= 0x08;
	static const TUint8  KPosResult				= 0x09;
	static const TUint8  KPosEvaluatorInfo		= 0x0A;
	
	//Shows which columns are set
	static const TUint32 KSetClientSid    		= 0x00000001;
	static const TUint32 KSetEvaluatorId  		= 0x00000002;
	static const TUint32 KSetServiceId    		= 0x00000004;
	static const TUint32 KSetServerSid    		= 0x00000008;
	static const TUint32 KSetFingerprint  		= 0x00000010;
	static const TUint32 KSetClientEntity 		= 0x00000020;
	static const TUint32 KSetMajorPolicyVersion = 0x00000040;
	static const TUint32 KSetRecordId  			= 0x00000080;
	static const TUint32 KSetDescription  		= 0x00000100;
	static const TUint32 KSetResult  			= 0x00000200;
	static const TUint32 KSetEvaluatorInfo  	= 0x00000400;

} // End of namespace UserPromptService

#include "upscommon.inl"

#endif	// #ifndef UPSCOMMON_H

