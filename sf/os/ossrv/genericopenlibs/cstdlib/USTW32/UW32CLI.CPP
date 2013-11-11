// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "POSIXIF.H"
#include <estw32.h>
#include <redircli.h>

#ifdef __WINS__

// Largely default STDLIB file descriptor for accessing Win32 streams
// We could in principle widen the RWin32Stream interface to allow for
// things like stat, but that can wait until it seems to be necessary!

class CW32StreamDesc : public CFileDescBase
	{
public:
	TInt Attach(TInt aStream);
	virtual void Read(TDes8& aDesc, TRequestStatus& aStatus);
	virtual void Write(TDes8& aDesc, TRequestStatus& aStatus);
	virtual void Flush(TRequestStatus& aStatus);
protected:
	virtual TInt FinalClose();
private:
	RWin32Stream iStream;
	};

TInt CW32StreamDesc::Attach(TInt aStream)
	{
	return iStream.Open(aStream);
	}

void CW32StreamDesc::Read(TDes8& aDesc, TRequestStatus& aStatus)
	{
	iStream.Read(aStatus, aDesc);
	}

void CW32StreamDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	iStream.Write(aStatus, aDesc);
	}

void CW32StreamDesc::Flush(TRequestStatus& aStatus)
	{
	iStream.Flush(aStatus);
	}

TInt CW32StreamDesc::FinalClose()
	{
	iStream.Close();
	return KErrNone;
	}

#endif

void CPosixServer::DefaultConsoleL()
	{
#ifdef __WINS__
	// Try to attach Win32 stdin/stdout/stderr
	TInt i;
	for (i=0; i<3; i++)
		{
		CW32StreamDesc* stream=new(ELeave) CW32StreamDesc;
		if (stream->Attach(i)==KErrNone)
			iFids.Attach(i, stream);
		else
			delete stream;
		}
#endif
	// set up default fids
	CRedirDesc* redirector = new(ELeave) CRedirDesc;
//	redirector->Connect();
	iFids.Default(redirector);
	redirector->Close();
	}


