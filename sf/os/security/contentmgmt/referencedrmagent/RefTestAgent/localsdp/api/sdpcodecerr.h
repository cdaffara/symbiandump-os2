/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : SdpCodecErr.h
* Part of       : Local SDP Codec
* Interface     : SDK API, Local SDP Codec API
* Version       : 1.0
*
*/



#ifndef SDPCODECERR_H
#define SDPCODECERR_H

/**
* @file
* @publishedAll
* @released
*/

/**
 * Base value for the SdpCode error code range
 */
const TInt KSdpCodecErrorBase = -17875;
/**
 * Codec encoding errors for SDP fields
 */
const TInt KErrSdpCodecVersionField			= KSdpCodecErrorBase;
const TInt KErrSdpCodecOriginField			= KSdpCodecErrorBase - 1; 
const TInt KErrSdpCodecSessionField			= KSdpCodecErrorBase - 2; 
const TInt KErrSdpCodecInfoField			= KSdpCodecErrorBase - 3; 
const TInt KErrSdpCodecUriField				= KSdpCodecErrorBase - 4; 
const TInt KErrSdpCodecEmailField			= KSdpCodecErrorBase - 5; 
const TInt KErrSdpCodecPhoneField			= KSdpCodecErrorBase - 6; 
const TInt KErrSdpCodecConnectionField		= KSdpCodecErrorBase - 7; 
const TInt KErrSdpCodecBandwidthField		= KSdpCodecErrorBase - 8; 
const TInt KErrSdpCodecTimeField			= KSdpCodecErrorBase - 9; 
const TInt KErrSdpCodecRepeatField			= KSdpCodecErrorBase - 10; 
const TInt KErrSdpCodecZoneField			= KSdpCodecErrorBase - 11; 
const TInt KErrSdpCodecKeyField				= KSdpCodecErrorBase - 12; 
const TInt KErrSdpCodecAttributeField		= KSdpCodecErrorBase - 13; 
const TInt KErrSdpCodecMediaField			= KSdpCodecErrorBase - 14; 
const TInt KErrSdpCodecMediaInfoField		= KSdpCodecErrorBase - 15; 
const TInt KErrSdpCodecMediaConnectionField	= KSdpCodecErrorBase - 16; 
const TInt KErrSdpCodecMediaBandwidthField	= KSdpCodecErrorBase - 17; 
const TInt KErrSdpCodecMediaKeyField	    = KSdpCodecErrorBase - 18; 
const TInt KErrSdpCodecMediaAttributeField	= KSdpCodecErrorBase - 19; 
const TInt KErrSdpCodecTypedTime            = KSdpCodecErrorBase - 20;

const TInt KErrSdpCodecStringPool           = KSdpCodecErrorBase - 21;
const TInt KErrSdpCodecDecode				= KSdpCodecErrorBase - 22;

#endif //SDPCODECERR_H
