/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Includes APIs for common security checks
*
*/


/**
 @file 
 @publishedPartner
 @released
*/

#ifndef SECURITYUTILS_H
#define SECURITYUTILS_H

#include <e32base.h>

namespace SecCommonUtils
{	
	/**
	 Returns a set of capabilities required for doing a file modification. 
	 This function is useful for cases where an FS operation is performed by a higher-trust process on behalf of a lower-trust process, 
	 and the higher-trust process wants to verify whether the operation is legal.
	 Please note that this function returns TCB as required for paths with relative indirections (".."), as well as paths with wild cards.
	 
	 @param aFileName			FileName, for which the client is verified
	 @param aClientSid			SID of the client requesting the file modification
	 @return					Set of capabilities. If a process has any of these, it can have modification access
	 */
	IMPORT_C TCapabilitySet FileModificationRequiredCapabilitiesL(const TDesC& aFileName, TSecureId aClientSid);
}

#endif /* SECURITYUTILS_H*/
