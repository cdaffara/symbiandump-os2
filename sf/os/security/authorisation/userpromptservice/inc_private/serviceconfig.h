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
* Class definitions for configuration data used by the User Prompt Service client library.
*
*/


/**
 @file
 @internalComponent
 @released 
*/

#ifndef SERVICECONFIG_H
#define SERVICECONFIG_H

#include <e32base.h>
#include <e32cmn.h>

namespace UserPromptService
	{
	/**
	Specifies whether authorisation from the User Prompt Service is required for 
	requests from clients that passed the system server's security check.

	Note that protected SID checks are based on the SID of the application excutable
	issuing the request, not the UID of the package owning that executable.

	If a policy file is not defined for a system server or service then a default
	value of ECheckNever will be used because this is compatible with the existing
	platform security behavior.	
	*/
	enum TAuthorisationPolicy
		{		
		/**
		Ignore the system server (platsec) checks, and always ask the UPS what to do.
		*/
		EAlwaysCheck = 0,	

		/**
		For application executables with a protected SID,  launched from the
		Z drive, where the system server checks have passed, allow the request.
		Otherwise call the UPS which may still choose to  allow the request.

		For all other executables, ignore the system server (platsec) checks, and 
		always ask the UPS what to do.
		 */		 
		ECheckPostManufacture = 1,

		/** 
		For application executables with a protected SID (regardless of drive), where
		the system server checks have passed, allow the request.
		Otherwise call the UPS which may still choose to  allow the request.

		For all other executables, ignore the system server (platsec) checks, and 
		always ask the UPS what to do.
		 */
		ECheckUnprotectedSids = 2,

		/** 
		If the system server checks passed, allow the request.
		If they failed, call the UPS which may still choose to	allow the request.
		*/
		ECheckIfFailed = 3,

		/** 
		If the system server checks passed, allow the request.
		If the system server checks failed, reject the request.
			
		Never query the UPS - just use existing security check result implemented
		by system server.
		*/
		ENeverCheck = 4		
	};
		
	/**
	Encapsulates the configuration data for an individual service provided by a system server.
	This is used by the User Prompt Service client library and is derived from policy files.

	- Associates a service id with the policy that determines whether the system
	server should ask the User Prompt Service whether the client's request is authorised.
	*/
	struct TServiceConfig
		{
		/** The UID of the service that this policy applies to. */
		TUint	iServiceId; // This MUST be a TUint and MUST be at offset 0 in this struct
		/** Whether addtional authorisation is required from the user prompt service. */
		TAuthorisationPolicy iPolicy;
		TUint16 iMajorVersion;		   ///< Major version no. of UPS policy file.
		TUint16 iMinorVersion;		   ///< Minor version no. of UPS policy file.
		};
	}

#endif // SERVICECONFIG_H
