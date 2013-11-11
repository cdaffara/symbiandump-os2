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
* The functions defined in this file provide constant data which is
* used by both the client and server implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/


#ifndef UPSCOMMON_INL
#define UPSCOMMON_INL

#include "upscommon.h"


namespace UserPromptService
	{
	inline TVersion Version()
	/**
		This function is defined because there is no literal constructor for TVersion.

		@return					Defines a version number which the client	
								can use to open the server.  If the client
								was built with a higher version number, then
								it cannot open the server.  This ensures that
								a client only talks to a server whose version
								is at least as high as its own.
	 */
		{
		TVersion v(KUpsVerMajor, KUpsVerMinor, KUpsVerBuild);
		return v;
		}

	inline TUidType ServerImageFullUid()
	/**
		This function is defined because there is no literal constructor
		for TUidType.  It returns the server executable's UID, which is used
		to ensure the client launches the correct server process, as opposed
		to another application which uses the same executable name.

		@return TUidType		The server executable's full UID.
	 */
		{
		TUidType uidType(KExecutableImageUid, KNullUid, KUpsServerUid);
		return uidType;
		}
	}	// namespace UserPromptService
	
#endif	// #ifndef UPSCOMMON_INL

