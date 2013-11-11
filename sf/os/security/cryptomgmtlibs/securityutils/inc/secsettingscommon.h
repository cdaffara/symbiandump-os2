/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation data shared between SecSettings client and server implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/


#ifndef SECSETTINGSCOMMON_H
#define SECSETTINGSCOMMON_H

#include <e32ver.h>
#include <e32uid.h>


namespace SecuritySettingsServer
	{
	_LIT(KSecSettingsServerName, "!SecSettingsServer");		///< Identifies SecSettings Symbian OS server.

	const TInt KSecSettingsVerMajor = 1;		///< SecSettings server version major component.
	const TInt KSecSettingsVerMinor = 0;		///< SecSettings server version minor component.
	const TInt KSecSettingsVerBuild = 0;		///< SecSettings server version build component.

	inline TVersion Version();

	/**
		Executable which hosts SecSettings server.  Required to launch the process.

		@see KSecSettingsServerUid
	 */
	_LIT(KServerImageName, "secsettingsserver.exe");

	/**
		Hosting executable's secure ID.  Required to launch the process.

		@see KServerImageName
	 */
	const TUid KSecSettingsServerUid = {0x2002B170};

	inline TUidType ServerImageFullUid();


	enum TSessionFunction
	/**
		Functions supported by a SecSettings session.
	 */
		{
			ESettingValue = 0
		};

	/**
		Delay in microseconds before the server is shut down, after
		the last remaining session has been closed.
	 */
	const TInt KShutdownPeriodUs = 2 * 1000 * 1000;



} // End of namespace SecuritySettingsServer

#include "secsettingscommon.inl"

#endif	// #ifndef SECSETTINGSCOMMON_H

