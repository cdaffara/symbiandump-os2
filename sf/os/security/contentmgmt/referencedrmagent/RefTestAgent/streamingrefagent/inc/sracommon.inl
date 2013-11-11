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
* used by both the Streaming Reference Agent server and client side implementations.
*
*/


/**
 @file
 @internalComponent
 @released
*/
 
#ifndef SRACOMMON_INL
#define SRACOMMON_INL

#include "sracommon.h"
#include "sratemplate.inl"

inline TVersion SraVersion()
/**
	This function is defined because there is no literal constructor for TVersion.

	@return		Defines a version number which the client side code	can use to launch 
			    the Streaming Reference Agent server. If the client code was built with a higher 
			    version number, then it cannot launch the Streaming Reference Agent Server. 
			    This ensures that a client only talks to the Streaming Reference Agent Server 
			    version is at least as high as its own.
	*/
	{
	TVersion ver(KSraVerMajor, KSraVerMinor, KSraVerBuild);
	return ver;
	}

inline TUidType SraImageFullUid()
/**
	This function is defined because there is no literal constructor
	for TUidType.  It returns the Streaming Reference Agent executable's UID, which is used
	to ensure the client side code launches the correct server process, as opposed
	to another application which uses the same executable name.

	@return TUidType		The Streaming Reference Agent executable's full UID.
 */
	{
	TUidType t(KExecutableImageUid, KNullUid, KUidStreamingReferenceAgent);
	return t;
	}
	
#endif /*SRACOMMON_INL*/
