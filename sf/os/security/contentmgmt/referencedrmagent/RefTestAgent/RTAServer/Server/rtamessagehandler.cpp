/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistry - server session implementation
*
*/


/**
 @file
 @test
 @internalComponent
*/
#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>

#include "rtamessagehandler.h"
#include "RTAserver.h"
#include "RTAsession.h"

using namespace ReferenceTestAgent;



CRtaMessageHandler::CRtaMessageHandler(const CSession2& aSession) :
	iSession(aSession)
	{
	}
		
CRtaMessageHandler::~CRtaMessageHandler()
	{
	}


CRtaServer& CRtaMessageHandler::Server()
	{
	return *static_cast<CRtaServer*>(const_cast<CServer2*>(iSession.Server()));
	}

HBufC16* CRtaMessageHandler::ReadDes16LC(const RMessage2& aMessage, 
										   TInt aParam)
	{
	HBufC16* retVal = HBufC16::NewLC(aMessage.GetDesMaxLength(aParam));
	TPtr16 ptr(retVal->Des());
	aMessage.Read(aParam, ptr);
	return retVal;
	}

HBufC8* CRtaMessageHandler::ReadDes8LC(const RMessage2& aMessage, 
										 TInt aParam)
	{
	HBufC8* retVal = HBufC8::NewLC(aMessage.GetDesMaxLength(aParam));
	TPtr8 ptr(retVal->Des());
	aMessage.Read(aParam, ptr);
	return retVal;
	}

HBufC16* CRtaMessageHandler::ReadDesC16LC(const RMessage2& aMessage, 
										   TInt aParam)
	{
	HBufC16* retVal = HBufC16::NewLC(aMessage.GetDesLength(aParam));
	TPtr16 ptr(retVal->Des());
	aMessage.Read(aParam, ptr);
	return retVal;
	}

HBufC8* CRtaMessageHandler::ReadDesC8LC(const RMessage2& aMessage, 
										 TInt aParam)
	{
	HBufC8* retVal = HBufC8::NewLC(aMessage.GetDesLength(aParam));
	TPtr8 ptr(retVal->Des());
	aMessage.Read(aParam, ptr);
	return retVal;
	}


void CRtaMessageHandler::WriteL(const RMessage2& aMessage, 
		TInt aParam, const TDesC8& aBuffer)
	{
	TInt length = aMessage.GetDesMaxLength(aParam);

	if (aBuffer.Length() <= length)
		{
		// Everything is OK. We can fit our descriptor into the
		// client side buffer
		aMessage.WriteL(aParam, aBuffer);
		}
	else
		{
		// Otherwise, the client hasn't allocated enough space for the
		// descriptor, so write a package with the required length, and
		// then leave with KErrOverflow so the client knows to take this
		// action
		aMessage.WriteL(aParam, TPckgC<TInt>(aBuffer.Length()));
		User::Leave(KErrOverflow);
		}
	}

void CRtaMessageHandler::WriteL(const RMessage2& aMessage, TInt aParam, 
	const TDesC16& aBuffer)
	{
	TInt length = aMessage.GetDesMaxLength(aParam);

	if (aBuffer.Length() <= length)
		{
		// Everything is OK. We can fit our descriptor into the
		// client side buffer
		aMessage.WriteL(aParam, aBuffer);
		}
	else
		{
		// Otherwise, the client hasn't allocated enough space for the
		// descriptor, so write a package with the required length, and
		// then leave with KErrOverflow so the client knows to take this
		// action
		aMessage.WriteL(aParam, TPckgC<TInt>(aBuffer.Length()));
		User::Leave(KErrOverflow);
		}
	}

