// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : signalsession.cpp
// Part of     : Signal Server
//



#include "signalsession.h"

CSignalSession::~CSignalSession()
	{
	Server().RemoveSession();	
	}

CSession2* CSignalSession::NewL()
	{
	CSignalSession* self = new (ELeave)CSignalSession();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSignalSession::ConstructL()
	{

	}

void CSignalSession::CreateL()
	{
	Server().AddSession();
	}

void CSignalSession::ServiceL(const RMessage2& aMessage)
	{
	Server().ServiceL(aMessage);
	return;
	}

CSignalServer& CSignalSession::Server()
    {
    return *(static_cast<CSignalServer*>(const_cast<CServer2*>(CSession2::Server())));
    }
