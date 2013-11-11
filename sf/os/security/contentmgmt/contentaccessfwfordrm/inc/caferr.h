/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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



/** 
@file

This contains error code definitions required by clients accessing the 
Content Access Framework.

All CAF error codes lie within the range \c KErrCA_LowerLimit to 
\c KErrCA_UpperLimit inclusive.

@publishedAll
@released
*/



#ifndef __CAFERR_H__
#define __CAFERR_H__

#include <e32std.h>


/**
This is the upper limit of the CAF error code range.

Example usage:
@code
if (KErrCA_LowerLimit <= err && err <= KErrCA_UpperLimit)
	{
	// handle CAF-specific error
	}
@endcode
*/
const TInt KErrCA_UpperLimit = -17450;


/**
This is the lower limit of the CAF error code range.

Example usage:
@code
if (KErrCA_LowerLimit <= err && err <= KErrCA_UpperLimit)
	{
	// handle CAF-specific error
	}
@endcode
*/
const TInt KErrCA_LowerLimit = -17549;



/**
Agent does not support the requested capability.
*/
const TInt KErrCANotSupported = -17450;

/** 
Rights evaluation failed for the supplied intent. This implies that
rights were available, but their evaluation, in the context of
supplied intent, failed.
*/
const TInt KErrCANoPermission = -17451; 

/** 
No rights were available for evaluation. A number of causes may
generate this error: Perhaps the rights were removed or lost
during a separate delivery
*/
const TInt KErrCANoRights = -17452; 

/**
The Agent required is not present. This may occur if content
'belongs' to an agent that is no longer available on the 
device. 
*/
const TInt KErrCANoAgent = -17453;

/**
Values specified are out of range. 
*/
const TInt KErrCAOutOfRange = -17454;

/** 
The rights were not available but are 'expected'. Will only be
returned if the agent supports this capability. 
*/
const TInt KErrCAPendingRights = -17455;

/**
A request to get the content size failed because the agent was unable
to determine it. This error may occur on circumstances where, say, the
content is streamed over HTTP.
*/
const TInt KErrCASizeNotDetermined = -17456;

/**
The function failed because the agent needs the client to provide another
file handle before the import can continue
*/
const TInt KErrCANewFileHandleRequired = -17457;


/**
The rights object failed an integrity check.

This error is returned when a client attempts an operation which requires 
accessing a rights object.

*/ 
const TInt KErrCACorruptRights = -17458;


/**
The content object failed an integrity check.

This error is returned when a client attempts an operation which requires 
accessing a content object.

*/ 
const TInt KErrCACorruptContent = -17459;

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
/**

This error is returned when only a part of encrypted input packet provided for decryption.
*/
const TInt KErrInsufficientDataPacketLength = -17460;

/**

This error is returned when only a part of encrypted input packet provided for decryption.
*/
const TInt KErrMissingWmdrmHeaderData = -17461;

#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
#endif // __CAFERR_H__
