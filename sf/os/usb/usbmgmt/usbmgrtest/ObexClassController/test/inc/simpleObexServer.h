/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef SIMPLEOBEXSERVER_H
#define SIMPLEOBEXSERVER_H

#include <e32base.h>
#include <e32cons.h>
#include <obex.h>
#include <btsdp.h>
#include "simpleObexApp.h"


/**
 * CObexServerHandler is a wrapper class that is responsible for setting up 
 * and managing an OBEX Server. It implements MObexServerNotify as an interface 
 * as is required to start an OBEX server.
 *
 */
	
class CObexServerHandler : public CBase, private MObexServerNotify
	{
	public:
		static CObexServerHandler* NewL(CActiveConsole* aParent, TTransport aTransport);
		
		void Stop();
		~CObexServerHandler();

		void EnableAuthenticationL();
		void DisableAuthentication();
	
	private:
		CObexServerHandler(CActiveConsole* aParent);
		void ConstructL(TTransport aTransport);

		// Implementation of MObexServerNotify interface
		virtual void ErrorIndication (TInt aError);
		virtual void TransportUpIndication ();
		virtual void TransportDownIndication ();
		virtual TInt ObexConnectIndication  (const TObexConnectInfo& aRemoteInfo, const TDesC8& aInfo);
		virtual void ObexDisconnectIndication (const TDesC8& aInfo);
		virtual CObexBufObject* PutRequestIndication ();
		virtual TInt PutPacketIndication ();
		virtual TInt PutCompleteIndication ();
		virtual CObexBufObject* GetRequestIndication (CObexBaseObject *aRequestedObject);
		virtual TInt GetPacketIndication ();
		virtual TInt GetCompleteIndication ();
		virtual TInt SetPathIndication (const CObex::TSetPathInfo& aPathInfo, const TDesC8& aInfo);
		virtual void AbortIndication ();

		void SetUpGetObjectL(CObexBaseObject *aRequestedObject);

	public:
		TBuf<16> iChallengePassword;
		TBuf<16> iResponsePassword;
		TBool iAcceptPuts;


	private:
		CActiveConsole* iParent;
		CObexServer*  iServer;
		CObexBufObject*  iObject;
		CBufFlat *iBuf;
		TTransport iTransportLayer;
		TBuf<32> iSessionPath;	
		
		RSdp iSdp;
		RSdpDatabase iSdpdb;
		CSdpAttrValueDES* iProtDescList;
	
		TBool iBuffering;
		TBool iUseRFile;
		RFs iFs;
		RFile iFile;
		TBool iIsAuthenticationEnabled;

	};
	
	
#endif // SIMPLEOBEXSEVER_H