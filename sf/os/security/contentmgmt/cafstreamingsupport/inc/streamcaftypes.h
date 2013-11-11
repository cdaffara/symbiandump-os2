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
// This file contains enumerations and constants used in the StreamAccess functionality of the Content Access Framework
// content access framework
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef STREAM_CAF_TYPES_H
#define STREAM_CAF_TYPES_H

/**
@namespace StreamAccess
StreamAccess namespace defines protection functionality specific for streams.
*/
namespace StreamAccess
	{
	/** 
	The list of stream access attributes. Each of these attributes has a 
	value that is either ETrue or EFalse unless otherwise specified.\n
	Remember to update any streaming DRM agents if new values are added here.
	*/
	enum TAttribute
		{
		EIsProgramProtected			= 0, ///< The specific program is protected
		EIsServiceProtected			= 1, ///< The whole service is protected
		ECanExport					= 2, ///< The content can be exported
		EMustProtectIfRecording		= 3, ///< The content must be protected if recorded. If ECanRecord is EFalse, then this attribute is undefined.
		ECanPlay					= 4, ///< The content can be played
		EAgentSpecificAttributeBase	= 32768	///< Base index the agents should use when implementing new agent specific attributes
		};
	
	
	
	/**  The list of generic string attributes. Each of these attributes has a value that is returned to the client as a string.
		 Remember to update any DRM agents if new values are added here.
	*/
	enum TStringAttribute
		{
		ERating						= 0,	///< Rating of the protected content
		ECountry					= 1,    ///< Target country of the content
		ERightsIssuerURI  			= 2,	///< URI where the rights for this content may be acquired from 
		EContentID					= 3,    ///< Content ID
		EAgentSpecificStringAttributeBase	= 32768 ///< Base index the agents should use when implementing new string attributes
		};
	} // Namespace StreamAccess

#endif // STREAM_CAF_TYPES_H

