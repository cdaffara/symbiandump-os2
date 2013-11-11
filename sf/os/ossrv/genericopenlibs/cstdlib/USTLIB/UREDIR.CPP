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
// RedirCli.CPP
// 
//

#include <redircli.h>
#include "FDESC.H"
#include <stdio.h>

CRedirDesc::CRedirDesc() : iAttemptedConnection(EFalse)
	{}

TInt CRedirDesc::Connect()
	{
	iAttemptedConnection = ETrue;
	return iRedirSession.Connect();
	}

void CRedirDesc::Read(TDes8& aDesc, TRequestStatus& aStatus)
	{
	//see if we are connected at this point?
	if (!iAttemptedConnection && !iRedirSession.Handle())
		Connect();
	iRedirSession.Read(aStatus, aDesc);
	}

void CRedirDesc::Write(TDes8& aDesc, TRequestStatus& aStatus)
	{
	//see if we are connected at this point?
	if (!iAttemptedConnection && !iRedirSession.Handle())
		Connect();

	iRedirSession.Write(aStatus, aDesc);
	}

void CRedirDesc::Sync(TRequestStatus& aStatus)
	{
	iRedirSession.Flush(aStatus);
	}

TInt CRedirDesc::FinalClose()
	{
	iRedirSession.Close();
	return KErrNone;
	}



