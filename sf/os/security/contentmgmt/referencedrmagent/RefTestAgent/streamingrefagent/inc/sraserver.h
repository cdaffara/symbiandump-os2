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
* Server-side classes which are required to implement a streaming reference agent server.
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef SRASERVER_H
#define SRASERVER_H


#include <e32base.h>
#include "sracommon.h"

using namespace StreamAccess;

// forward declarations
class CSraServer;
class CSraSession;
class CSraRightsObject;
class CSraProcessor;

class CShutdownTimer : public CTimer
/**
	An instance of this class is constructed with a delay period.
	If the object runs to completion, i.e. if it is not cancelled, then it stops
	the active scheduler, which has the effect of terminating the server.
 */
	{
public:
	static CShutdownTimer* NewL(TInt aDelay);
	void Restart();

private:
	CShutdownTimer(TInt aDelay);

	// implement CTimer
	virtual void RunL();

private:
	TInt iDelay;			///< Timer delay in microseconds.
	};


class CSraSession : public CSession2
/**
 	Represents a session for a client thread on the server-side of the reference agent.
 	A client thread can have multiple concurrent sessions with the reference agent server.
 */
	{
	friend class CSraProcessor;
public:
	static CSraSession* NewL();
	~CSraSession();
	void CreateL();
	
	// implement CSession2
	virtual void ServiceL(const RMessage2& aMessage);
	// override CSession2
	virtual void ServiceError(const RMessage2& aMessage, TInt aError);
	
private:
	CSraSession();
	CSraServer* GetServerHandle();
	
	void GetAttributeL(const RMessage2& aMessage);
	void GetStringAttributeSizeL(const RMessage2& aMessage);
	void GetStringAttributeDataL(const RMessage2& aMessage);
	void GetPostDeliveryRightsL(const RMessage2& aMessage);
	void SetKeyStreamSinkL(const RMessage2& aMessage);
	void SetSdpMediaFieldL(const RMessage2& aMessage);
	void SetSdpDocumentL(const RMessage2& aMessage);
	
	void FindAndLoadRightsObjectL(const TDesC8& aCid);
	void VerifyRoL();
	void StartProcessorL();
	
private:
	CSraProcessor* iProcessor;
	CKeyStreamSink* iKeyStreamSink;
	CSraRightsObject* iRo;
	CSdpMediaField* iSdp;
	CSdpDocument* iSdpDoc;
	/**
	 A handle to a mutex which is used to signal the client that the key has been implemented. 
	 Not necessary in real-life agents and is present here only for testing.
	 */
	RMutex iMutex;
	};
	
	
class CSraServer : public CPolicyServer
/**
	The reference agent server object allocates sessions.	
	It also uses a shutdown timer to stop the server when 
	there have been no open sessions for a set period.
 */
	{
	friend class CSraSession;
public:
	static CSraServer* NewLC();
	~CSraServer();
	
private:
	CSraServer();
	void ConstructL();
	void AddSession();
	void RemoveSession();
	// implement CServer2
	virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

private:
	CShutdownTimer* iShutdownTimer;
	const TVersion iVersion;
	/** Number of open sessions.  Used to start and cancel the shutdown timer. */
	TInt iSessionCount;
	/** File server handle. */
	RFs iFs;
	};


/**
	The server is panicked with this category when it detects an invalid state.
 */
_LIT(KSraServerPanicCat, "SRA-Server");

/** The mime type of the post-acquisition rights object file. */
_LIT8(KPostAcquisitionRoMimeType,"application/x-sra.drm.postacqrights");

/** The mime type of the post-acquisition rights object file. */
_LIT8(KPostAcquisitionContentMimeType,"application/x-sra.drm.postacqcontent");

/** The content Id of the post-acquisition rights object file. */
_LIT8(KPostAcquisitionCid,"SamplePostAcquisitionCid");

/** The private folder of the Streaming Reference Agent. */
_LIT(KSraPrivateFolder,"!:\\private\\10285a1c\\");

/** 
 	Mutex name used to synchronize the reference agent with the test harness.
 	@see CSraSession::iMutex
 */
_LIT(KSraMutexName, "MutexBtwTestHarnessAndRefAgent");

#endif

