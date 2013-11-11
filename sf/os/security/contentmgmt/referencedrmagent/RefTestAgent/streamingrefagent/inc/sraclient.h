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
* Client-side API via which the key stream decoder uses the streaming reference agent service.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRACLIENT_H_
#define SRACLIENT_H_

#include "sracommon.h"

using namespace StreamAccess;

//Forward declarations
class CRightsObject;
class CSdpMediaField;
class CSdpDocument;

NONSHARABLE_CLASS(RSraClient) : public RSessionBase
/**
	The Key Stream Decoder uses this class to establish a connection with the Streaming Reference
	Agent Server and to get/set a variety of information and objects from the connected server. 
		
	@see TSraFunction
 */
		{
	public:
		IMPORT_C RSraClient();
		IMPORT_C TInt Connect();
		
		IMPORT_C void GetAttributeL(const TAttribute& aAttribute, TBool& aValue);
		IMPORT_C HBufC* GetStringAttributeLC(const TStringAttribute& aAttribute);
		IMPORT_C void GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType);
		IMPORT_C void SendKeyStreamSinkL(const CKeyStreamSink& aSink) const;
		IMPORT_C void SendSdpMediaFieldL(const CSdpMediaField& aSdp) const;
		IMPORT_C void SendSdpDocumentL(const CSdpDocument& aSdpDoc) const;

	private:
		TInt StartServerProcess(const TDesC& aExeName, const TUidType& aFullExeUid);
		};

#endif /*SRACLIENT_H_*/
