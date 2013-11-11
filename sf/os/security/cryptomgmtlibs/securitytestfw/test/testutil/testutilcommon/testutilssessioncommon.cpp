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
* Implementation of common class for testutil and testutilswi
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include "testutilssessioncommon.h"

CTestUtilSessionCommon::CTestUtilSessionCommon()
  	{
  	}


CTestUtilSessionCommon::~CTestUtilSessionCommon()
	{
	}

void CTestUtilSessionCommon::Send(const TDesC& aMessage)
	{
	if (ReceivePending())
		{
		TPtrC m(aMessage);
		TInt r=iReceiveMsg.Write(0,m);
		if (r==KErrNone)
			iReceiveMsg.Complete(KErrNone);
		else
			PanicClient(iReceiveMsg,EPanicBadDescriptor);
		}
	}

HBufC* CTestUtilSessionCommon::AllocateInputBufferLC(const RMessage2& aMessage,TInt aParam)
	{
	TInt srcLen = aMessage.GetDesLength(aParam);
	HBufC* inputBuffer=HBufC::NewLC(srcLen);
	TPtr ptr(inputBuffer->Des());
	TRAPD(err, aMessage.ReadL(aParam, ptr));
	if (err!=KErrNone)
		{
		PanicClient(aMessage, EPanicBadDescriptor);
		}
	return inputBuffer;
	}
	

	
