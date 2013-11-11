/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Random Server Codes
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef RANDSVRIMPL_H
#define RANDSVRIMPL_H


const TInt KRandomBlockSize=1024;
const TInt KRandomPoolSize=2048;

class CRandomScheduler:public CActiveScheduler
	{
public:
	static TBool New(void);
	void Error(TInt aError) const;
	};

class CRandomSession;

class CRandomServer : public CServer2
	{
public:
	static TInt New(void);
	~CRandomServer(void);
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	friend class CRandomSession;
private:
	CRandomServer(void);
	void ConstructL(void);
	void Stir(void);
	TPtrC8 GetRandom(void); 
	CMessageDigest* Hash(void) const;
	static TInt Tick(TAny* aPointer);
	TUint8* iPool;
	TInt iPoolIn;
	TInt iPoolOut;
	TInt iQuality;
	TBool iFast;
	CMessageDigest* iHash;
	CPeriodic* iTicker;
	};

class CRandomSession : public CSession2 
	{
public:
	enum { KRandomRequest };
	static CRandomSession* NewL(CRandomServer* aServer);
	~CRandomSession(void);
	void ServiceL(const RMessage2& aMessage);
private:
	TInt FillBuffer(const RMessage2& aMessage);
	CRandomServer* iServer;
	CRandomSession(CRandomServer* aServer);
	};

#endif // RANDSVRIMPL_H
