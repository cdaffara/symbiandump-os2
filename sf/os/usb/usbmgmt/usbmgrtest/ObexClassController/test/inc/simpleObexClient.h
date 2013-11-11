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


#ifndef SIMPLEOBEXCLIENT_H
#define SIMPLEOBEXCLIENT_H


#include <e32base.h>
#include <e32cons.h>
#include <obex.h>
#include <btsdp.h>
#include "simpleObexApp.h"



class CRFCOMMServiceFinder;
	

/**
 * MRFCOMMServiceSeeker is a mixin class that contains a virtual function,
 * SearchResult, that is overridden by the SearchResult funtion in the
 * CObexClientHandler class. 
 *
 */
class MRFCOMMServiceSeeker
	{
	public:
		virtual void SearchResult(TInt aError, TUint8 aPort)=0;	// user implemented
	};
	


/**
 * CObexClientHandler is a wrapper class that is responsible for dealing
 * with an OBEX client. It implements MRFCOMMServiceSeeker to deal with
 * Bluetooth searches on other OBEX devices.
 *
 */
class CObexClientHandler : public CActive, public MRFCOMMServiceSeeker
	{
	public:
		static CObexClientHandler* NewL(CActiveConsole* aParent, TTransport aTransport);
		~CObexClientHandler();
		
		void SetMode(TUint mode);
		void Connect();
		void Disconnect();
		void GetByNameL();
		void Put(TDes& filename);
		void SetName(TDes& aName);
		void SearchResult(TInt aError, TUint8 aPort);

		void ConnectWithAuthenticationL();

	private:
		void ConstructL(TTransport aTransport);
		CObexClientHandler(CActiveConsole* aParent);
		void RunL ();
		void DoCancel ();
		TInt SetUpObjectFromFile (TDes& filename);
		void DisplayObjectInformation();

	public:
		TBuf<64> iObexName;
		TBuf<64> iFilename1;
		TBuf<64> iFilename2;
		TBuf<64> iFilename3;
		TBuf<32> iSessionPath;	
		TBuf<16> iChallengePassword;
		TBuf<16> iResponsePassword;
		TBuf<32> iGetType;
	private:
		enum TSendState
			{
			EIdle,
			EConnecting,
			EConnected,
			EPutting,
			EGetting,
			EDisconnecting
			};

		CActiveConsole* iParent;
		CObexClient* iClient;
		CObexFileObject* iFileObject;
		CObexBufObject* iObject;
		CBufFlat* iObjectBuffer;
		TSendState iState;
		CObexNullObject* iTargetHeaderObject;
		CRFCOMMServiceFinder* iSdpServiceFinder;
		TBTDevAddr iDevAddr;

	};
	

/**
 * CRFCOMMServiceFinder is the clas that is responsible for searching the SDP
 * records of a specified device (by address) to see if OBEX FTP is supported
 *
 */	
class CRFCOMMServiceFinder : public CBase, public MSdpAgentNotifier
	{
	public:
		static CRFCOMMServiceFinder* NewL(	const TUUID& aServiceClass,const TBTDevAddr& aDevAddr,MRFCOMMServiceSeeker& aSeeker);
		~CRFCOMMServiceFinder();

		void FindPortL();	//Starts the search

		
		// MSdpAgentNotifier functions
		void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount);
		void AttributeRequestResult(TSdpServRecordHandle,TSdpAttributeID,CSdpAttrValue*) {User::Panic(_L("RFCOMMSEEK"), 0);} // Not using this API form
		void AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError);

	private:
		CRFCOMMServiceFinder(MRFCOMMServiceSeeker& aSeeker);
		void ConstructL(const TBTDevAddr& aDevAddr, const TUUID& aServiceClass);

	private:
		CSdpAgent* iAgent;
		CSdpSearchPattern* iPattern;
		TBool iFoundRFCOMMUUID;
		TUint8 iPort;
		MRFCOMMServiceSeeker& iSeeker;//initialised from aSeeker in the constructor
		CActiveConsole* iParent;
	};
	
#endif // SIMPLEOBEXCLIENT_H