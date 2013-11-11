// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32std.h>
#include <redircli.h>
#include <redircliinternal.h>

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
	{
	return KErrNone;
	}
#endif

// server name
_LIT(KRedirServerName,"RedirServer");

enum TRedirStream 
	{
	ERedirRead,
	ERedirWrite,
	ERedirFlush
	};

// A version must be specified when creating a session with the server
const TUint KRedirServMajorVersionNumber=1;
const TUint KRedirServMinorVersionNumber=0;
const TUint KRedirServBuildVersionNumber=0;


//**********************************
// CRedirSession
//**********************************
// This class represents a session in the server.
// Functions are provided to respond appropriately to client messages.
NONSHARABLE_CLASS(CRedirSession) : public CSession2
	{
public:
	static CRedirSession* NewL(CStreamBase2* aStream);
	//service request
	virtual void ServiceL(const RMessage2 &aMessage);
private :
	// construct/destruct
	CRedirSession(CStreamBase2* aStream);
	~CRedirSession();
	//services available to handle read/write requests
	TInt WriteStreamL(const RMessage2& aMessage);
private:
	CStreamBase2* iStream;
	};

// ------------------- CRedirServer2 implementation --------------------------


/**
Constructs and allocates memory for a new CRedirServer2 object.
@return RedirServer
@param aStreamFactory Stream factory.
*/ 
EXPORT_C CRedirServer2* CRedirServer2::NewL(CStreamFactoryBase2* aStreamFactory)
	{
	CRedirServer2* r=new(ELeave) CRedirServer2(aStreamFactory);
	CleanupStack::PushL(r);
	r->StartL(KRedirServerName);
	CleanupStack::Pop();
	return r;
	}

/**
Default constructor
@param aStreamFactory Stream factory.
*/ 
CRedirServer2::CRedirServer2(CStreamFactoryBase2* aStreamFactory)
	: CServer2(EPriority)
	{
	iStreamFactory = aStreamFactory;
	}

/**
Virtual destructor
*/ 	
CRedirServer2::~CRedirServer2()
	{
	}

/**
Create a new server session. 
Checks if the aVersion is the right version and make a new session.
@leave KErrNotSupported
@return Sharable Session for all threads within a single process.
@param aVersion Contains version information.
A version is defined by a set of three numbers:major,minor,build version number
*/
CSession2* CRedirServer2::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
	{

	TVersion v(KRedirServMajorVersionNumber,KRedirServMinorVersionNumber,
		KRedirServBuildVersionNumber);
	if (!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);
	// get a stream object from the current stream factory
	CStreamBase2* stream = iStreamFactory->GetStream();
	return CRedirSession::NewL(stream);
	}

/**
Sets the stream factory
@param aStreamFactory a factory to be set
*/
EXPORT_C void CRedirServer2::SetStreamFactory(CStreamFactoryBase2* aStreamFactory) 
	{
	// set the stream factory
	iStreamFactory = aStreamFactory;
	}

// ------------------- CRedirSession2 implementation --------------------------
CRedirSession* CRedirSession::NewL(CStreamBase2* aStream)
	{
	return new(ELeave) CRedirSession(aStream);
	}

CRedirSession::CRedirSession(CStreamBase2* aStream)
	: iStream(aStream)
	{}

CRedirSession::~CRedirSession()
	{
	// The stream returned by GetStream() is effectively a singleton as 
	// implemented in the current redirector, so we leave deletion to
	// the factory
	}

void CRedirSession::ServiceL(const RMessage2& aMessage)
	{
	TInt ret = KErrNone;
	switch (aMessage.Function())
		{
	case ERedirRead:
		// call the Stream to do the read for us
		iStream->Read(aMessage);
		break;
	case ERedirWrite:
		TRAP(ret,WriteStreamL(aMessage));
		aMessage.Complete(ret);
		break;
	case ERedirFlush:
		// TO DO: ignore flushing?
		aMessage.Complete(ret);
		break;
	default:
		aMessage.Complete(KErrNotSupported);
		break;
		}
	return;
	}

TInt CRedirSession::WriteStreamL(const RMessage2& aMessage)
	{
	const TInt KMaxTransfer=256;
	//
	// retrieve the length of the buffer the client wants to write
	TInt length = aMessage.Int1();
	// retrieve the descriptor to be written
	TBuf8<KMaxTransfer> bufDes;
	for (TInt offset=0;offset<length;offset+=KMaxTransfer)
		{
		aMessage.ReadL(0, bufDes, offset);
		// send descriptor to stream for printing
		iStream->Write(bufDes);
		}
	return KErrNone;
	}

// -------------- CLIENT IMPLEMENTATION -----------------------------

EXPORT_C TInt RRedirSession2::Connect()
	{
	TVersion version(KRedirServMajorVersionNumber,KRedirServMinorVersionNumber,
		KRedirServBuildVersionNumber);
	return CreateSession(KRedirServerName,version,1);	// only one message allowed - no concurrency
	}

TInt RRedirSession2::CheckEOF(TRequestStatus& aStatus)
	{
	if (Handle()!=0)
		return KErrNone;
	TRequestStatus* aStatusPtr=&aStatus;
	User::RequestComplete(aStatusPtr,KErrEof);
	return KErrEof;
	}
	
EXPORT_C void RRedirSession2::Read(TRequestStatus& aStatus, TDes8& aDes)
	{
	Read(aStatus, aDes, aDes.MaxLength());
	}

EXPORT_C void RRedirSession2::Read(TRequestStatus& aStatus, TDes8& aDes, TInt aLength)
	{
	if (CheckEOF(aStatus))
		return;
	SendReceive(ERedirRead,TIpcArgs(&aDes,aLength),aStatus);
	}

EXPORT_C void RRedirSession2::Write(TRequestStatus& aStatus, const TDesC8& aDes)
	{
	Write(aStatus, aDes, aDes.Length());
	}

EXPORT_C void RRedirSession2::Write(TRequestStatus& aStatus, const TDesC8& aDes, TInt aLength)
//
// Write aLength bytes
//
	{
	if (CheckEOF(aStatus))
		return;
	SendReceive(ERedirWrite,TIpcArgs(&aDes,aLength),aStatus);
	}

EXPORT_C void RRedirSession2::Flush(TRequestStatus& aStatus)
//
// Flush output
//
	{
	if (CheckEOF(aStatus))
		return;
	SendReceive(ERedirFlush,aStatus);
	}



