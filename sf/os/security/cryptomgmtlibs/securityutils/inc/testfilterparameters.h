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
* This file contains the UID and key ids of Publish and Subscribe flags used for IPC between the test code and the test http filter
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __TESTFILTERPARAMETERS_H__
#define __TESTFILTERPARAMETERS_H__

#include <e32def.h>

// UID used to reference the P&S flags
const TInt KFilterParametersCategoryUID							= 0x1000A405;

// Keys for the various parameters
const TInt KFilterParameterNumDelayResp							=  1; // Delay by n milliseconds
const TInt KFilterParameterCountDropResp 						=  2; // Drop n requests/responses
const TInt KFilterParameterCountCorruptHTTPDataHeader 			=  3; // Corrupt HTTP data
const TInt KFilterParameterCountCorruptHTTPDataBodySizeLarge	=  4; // Corrupt HTTP data
const TInt KFilterParameterCountCorruptHTTPDataBodySizeSmall 	=  5; // Corrupt HTTP data
const TInt KFilterParameterCountCorruptOCSPData 				=  6; // Corrupt OCSP data
const TInt KFilterParameterCountInternalErrorResp 				=  7; // Respond with internalError
const TInt KFilterParameterCountTryLaterResp 					=  8; // Respond with tryLater
const TInt KFilterParameterCountSigValidateFailure 				=  9; // Cause signature validation failure

#endif // __TESTFILTERPARAMETERS_H__
