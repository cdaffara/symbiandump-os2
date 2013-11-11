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



#ifndef OBEXINITIATOR_H
#define OBEXINITIATOR_H
	
_LIT(KObexDescription, "Obex Server Interface");
_LIT(KConsoleName, "Obex Server");
/**
 * CObexInitiator implements the MObexServerNotify interface. 
 * It has implementations of virtual functions from the MObexServerNotify
 * It also allows for an CObexServer to be created, started and stopped.
 * This class is implemented in the CSession2 derived class, this class can be extended
 * to make a greater use of the CObexServer class.
 *
 */
NONSHARABLE_CLASS(CObexInitiator) : public CBase, private MObexServerNotify
	{
	
	public:
		static CObexInitiator* NewL();
		~CObexInitiator();
		void StartObexServerL();
		void StopObexServer();

	private:
		void ConstructL();
		CObexInitiator();
		// These are the MObexServerNotify functions
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
	
	private:
		CObexServer* iObexServer;
		CConsoleBase* iConsole;
	
	
	};
	
	
#endif // OBEXINITIATOR_H