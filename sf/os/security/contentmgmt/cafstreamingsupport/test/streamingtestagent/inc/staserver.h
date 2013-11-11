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
// Defines server-side classes which are used to implement the Streaming Test Agent.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef STASERVER_H_
#define STASERVER_H_

#include <scs/scsserver.h>
#include <caf/rightsinfo.h>
#include <f32file.h>
#include <s32file.h>
#include "stacommon.h"
#include "srarightsobject.h"
#include "testkeyassociation.h"
#include "testkeystreamsink.h"

class CSdpMediaField;
class CSdpDocument;

namespace StreamAccess
	{
	//forward declaration
	class CStaServer;
	
	class CStaSession : public CScsSession
	/**
		This session object is created for each key stream decoder client.
		It is used to perform client requests.
	 */
		{
	public:
		static CStaSession* NewL(CStaServer &aServer);
		~CStaSession();
			
		// implement CStreamingTestAgentSession
		TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
		inline CStaServer* Server();
		
	private:
		CStaSession(CStaServer& aServer);
		
		void GetAttributeL(const RMessage2& aMessage);
		void GetStringAttributeSizeL(const RMessage2& aMessage);
		void GetStringAttributeDataL(const RMessage2& aMessage);
		void GetPostDeliveryRightsL(const RMessage2& aMessage);
		void GetKeyStreamL(const RMessage2& aMessage);
		void SetKeyStreamSinkL(const RMessage2& aMessage);
		void SetSdpMediaFieldL(const RMessage2& aMessage);
		void SetIpSecAssociationL(const RMessage2& aMessage);
		void SetSdpDocumentL(const RMessage2& aMessage);
		
		void VerifyRoL();
		void PanicClient(const RMessagePtr2& aMessage, TInt aReason);
		
	private:
		HBufC8* iBuffer;
		CKeyStreamSink* iKeyStreamSink;
		CSdpMediaField* iSdp;
		CSdpDocument* iSdpDoc;
		CSraRightsObject* iRo;
		};
	
	/** The mime type of the post-acquisition rights object file. */
	_LIT8(KPostAcquisitionRoMimeType,"application/x-rta.drm.postacqrights");
	/** The mime type of the post-acquisition rights object file. */
	_LIT8(KPostAcquisitionContentMimeType,"application/x-rta.drm.postacqcontent");
	/** The content Id of the post-acquisition rights object file. */
	_LIT8(KPostAcquisitionCid,"PostAcquisitionTest");
	
	class CStaServer : public CScsServer
	/**
		The CScsServer derived object which is used to generate Streaming 
		Test Agent sessions for the key stream decoder client.
	 */
		{
	public:
		static CStaServer* NewLC();
				
		~CStaServer();
	
		//Implement from CScsServer
		//void DoPreHeapMarkOrCheckL();
		//void DoPostHeapMarkOrCheckL();
		CScsSession* DoNewSessionL(const RMessage2& aMessage);
				
	private:
		CStaServer();
		void ConstructL();	

	public:
		/** 
		 	File server handle. Public, because it is accessed by session objects. 
		 	Since session objects write/read files, they need a file server handle.
		 	Sessions use this file handle instead of creating a new one. 
		 */
		RFs iFs;
		};
	
	inline CStaServer* CStaSession::Server()
		{
		CStaServer *server = dynamic_cast<CStaServer*>(&iServer);
		ASSERT(server);
		return server;
		}
		
	/** The private folder of the Streaming Test Agent. */
	_LIT(KStaPrivateFolder,"!:\\private\\1028588b\\");	
	} //End of the namespace StreamAccess

#endif /*STASERVER_H_*/
