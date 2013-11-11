// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// The functions defined in this file provide constant data which is
// used by both the Streaming Test Agent server and client side implementations.
// 
//

/**
 @file
 @internalComponent
 @test
*/
 
#ifndef STACOMMON_INL
#define STACOMMON_INL

#include "stacommon.h"
#include "sratemplate.inl"

namespace StreamAccess
	{
	inline TVersion StaVersion()
	/**
		This function is defined because there is no literal constructor for TVersion.

		@return					Defines a version number which the client side code	can use 
								to open the Streaming Test Agent server. If the client code
								was built with a higher version number, then it cannot open
								the Streaming Test Agent Server. This ensures that a client
								only talks to the Streaming Test Agent Server version is at
								least as high as its own.
	*/
		{
		TVersion v(KStaVerMajor, KStaVerMinor, KStaVerBuild);
		return v;
		}

	inline TUidType StaImageFullUid()
	/**
		This function is defined because there is no literal constructor
		for TUidType.  It returns the Streaming Test Agent executable's UID, which is used
		to ensure the client side code launches the correct server process, as opposed
		to another application which uses the same executable name.

		@return TUidType		The Streaming Test Agent executable's full UID.
	 */
		{
		TUidType t(KExecutableImageUid, KNullUid, KUidStreamingTestAgent);
		return t;
		}
	
	} // End of the namespace StreamAccess
	
#endif /*STACOMMON_INL*/
