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
// Client-side API via which the key stream decoder uses the streaming test agent service.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef STACLIENT_H_
#define STACLIENT_H_

#include <scs/scsclient.h>
#include <caf/rightsinfo.h>
#include "stacommon.h"
#include "testkeyassociation.h"
#include "testkeystreamsink.h"

class CSdpMediaField;
class CSdpDocument;
namespace StreamAccess
	{

	NONSHARABLE_CLASS(RStaClient) : public RScsClientBase
	/**
		The Key Stream Decoder uses this class to establish a connection with the Streaming Test
		Agent Server and to get/set a variety of information and objects from the connected server. 
		
		@see StreamAccess::TStaFunction
	 */
		{
	public:
		IMPORT_C RStaClient();
		IMPORT_C TInt Connect();
		IMPORT_C void GetAttributeL(const TAttribute& aAttribute, TBool& aValue);
		IMPORT_C HBufC* GetStringAttributeLC(const TStringAttribute& aAttribute);
		IMPORT_C void GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType);
		IMPORT_C void SendKeyStreamL(const TDesC8& aKey) const;
		IMPORT_C void SetKeyStreamSinkL(const CKeyStreamSink& aSink) const;
		IMPORT_C void SetSdpMediaFieldL(const CSdpMediaField& aSdp) const;
		IMPORT_C void SetSdpDocumentL(const CSdpDocument& aSdpDoc) const;
		IMPORT_C void SendIpSecAssociationL(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey) const;
		IMPORT_C void Close();
		};
	}//End of the namespace StreamAccess

#endif /*STACLIENT_H_*/
