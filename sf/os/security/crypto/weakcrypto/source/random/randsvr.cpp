/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
*/

#include "randsvr.h"
#include "randcliserv.h"
#include <e32math.h>
#include "randsvrimpl.h"


//const TInt KFastTickTimer=1000000;			// These are testing numbers!
//const TInt KSlowTickTimer=30000000;
const TInt KThreshold=1024;

const TInt KFastTickTimer=30000000;			// These are the real numbers!
const TInt KSlowTickTimer=0x7fffffff;


void SignalClient()
//
// Signal the owning thread that the server has started successfully
// This may itself fail
//
	{
	RProcess::Rendezvous(KErrNone);
	}


EXPORT_C TInt RunRandomServer(TAny* /*aUnused*/)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	if (!cleanup)
		{
		return KErrNoMemory;
		}

	TInt ret = User::RenameThread(KRandomServerName);

	__ASSERT_ALWAYS(ret==KErrNone,User::Panic(KRandomServerName,KErrServerTerminated));

	if (CRandomScheduler::New())
		return KErrNoMemory;
	ret = CRandomServer::New();
	if (ret != KErrNone )
		return ret;
	// Initialisation complete, now signal the client
	SignalClient();
	
	CRandomScheduler::Start();
	delete cleanup;
	return KErrNone;
	}

TBool CRandomScheduler::New(void)
	{
	CRandomScheduler* rs;
	rs=new CRandomScheduler;
	CRandomScheduler::Install(rs);
	return (rs == NULL);
	}

void CRandomScheduler::Error(TInt /*aError*/) const 
	{
	User::Panic(KRandomServerName, 3);
	}

CRandomServer::CRandomServer(void) : CServer2(EPriorityLow)
	{
	}

CRandomServer::~CRandomServer(void)
	{
	// This should never happen....but in case it does:
	delete iHash;
	delete iTicker;
	delete iPool;
	}

TInt CRandomServer::New(void)
	{
	CRandomServer* self;
	self=new CRandomServer;
	if (!self)
		{
		return KErrNoMemory;
		}
	TRAPD(ret,self->ConstructL());
	if (ret)
		{
		return ret;
		}
	
	return self->Start(KRandomServerName);
	}

void CRandomServer::ConstructL(void)
	{
	iPool=new (ELeave) TUint8[KRandomPoolSize];
	iHash=CSHA1::NewL();
	iPoolIn=0;
	iPoolOut=0;
	iTicker=CPeriodic::NewL(EPriorityLow);
	TCallBack callback(Tick,this);
	iTicker->Start(KFastTickTimer,KFastTickTimer,callback);		// **** these figures might need tweaking!
	iQuality=0;
	iFast=ETrue;
	}

TInt CRandomServer::Tick(TAny* aServer)
	{
	CRandomServer* svr=(CRandomServer*)aServer;
	svr->Stir();
	svr->iQuality+=30;
	if (svr->iFast)
		{
		if (svr->iQuality>KThreshold)
			{
			TCallBack callback(Tick,svr);
			svr->iTicker->Cancel();
			svr->iTicker->Start(KSlowTickTimer,KSlowTickTimer,callback);		// **** these figures might need tweaking!
			svr->iFast=EFalse;
			}
		}
	if (svr->iQuality>(KRandomPoolSize<<3))
		{
		svr->iQuality=(KRandomPoolSize<<3);
		}
	return ETrue;
	}

CSession2* CRandomServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
	{
	return CRandomSession::NewL(const_cast<CRandomServer*>(this));
	//CRandomSession::NewL(CONST_CAST(CRandomServer*,this),Message().Client());
	}

TPtrC8 CRandomServer::GetRandom(void)
	{
	TPtr8 res(&iPool[iPoolOut],iHash->HashSize(),iHash->HashSize());
	iPoolOut+=iHash->HashSize();
	if ((iPoolOut+iHash->HashSize())>KRandomPoolSize)
		{
		iPoolOut=0;
		}
	return iHash->Hash(res);
	}

CMessageDigest* CRandomServer::Hash(void) const
	{
	return iHash;
	}

void CRandomServer::Stir(void) 
	{
	TInt rnd;
	rnd=Math::Random();
	TPtrC8 r((TUint8*)&rnd,sizeof(TInt));
	iHash->Hash(r);
	TPtr8 dest(&iPool[iPoolIn],iHash->HashSize());
	dest.Copy(iHash->Hash(dest));
	iPoolIn+=iHash->HashSize();
	if ((iPoolIn+iHash->HashSize())>KRandomPoolSize)
		{
		iPoolIn=0;
		}
	}

CRandomSession* CRandomSession::NewL(CRandomServer* aServer)
	{
	CRandomSession* self;
	self=new (ELeave) CRandomSession(aServer);
	return self;
	}

CRandomSession::CRandomSession(CRandomServer* aServer) : CSession2(), iServer(aServer)
	{
	}

CRandomSession::~CRandomSession(void)
	{
	}

void CRandomSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case KRandomRequest:
		{
		TInt ret = FillBuffer(aMessage);
		aMessage.Complete(ret);
		break;
		}
	default:
		aMessage.Complete(KErrNotSupported);
		break;
		};
	}

TInt CRandomSession::FillBuffer(const RMessage2& aMessage)
	{
	TInt length = aMessage.Int1();
	if (length < 0 || length > KRandomBlockSize)
		return KErrArgument;
	iServer->iQuality -= length;
	if (iServer->iQuality<0)
		{
		iServer->iQuality=0;
		}
	if (!iServer->iFast)
		{
		if (iServer->iQuality<KThreshold)
			{
			TCallBack callback(CRandomServer::Tick,iServer);
			iServer->iTicker->Cancel();
			iServer->iTicker->Start(KFastTickTimer,KFastTickTimer,callback);		// **** these figures might need tweaking!
			iServer->iFast=ETrue;
			}
		}
	TBuf8<KRandomBlockSize> buf(0);
	iServer->Stir();
	TInt i;
	TInt hashsize=iServer->Hash()->HashSize();
	for (i=0; i+hashsize < length; i+=hashsize)
		{
		buf.Append(iServer->GetRandom());
		iServer->Stir();
		}
	TPtrC8 ptr(iServer->GetRandom().Ptr(), length-i);
	buf.Append(ptr);
	
	TRAPD(ret, aMessage.WriteL(0, buf));
	return ret;
	}

GLDEF_C TInt E32Main(void)
	{
	return RunRandomServer(NULL);
	}
