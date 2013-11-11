// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : factory.cpp
// Part of     : ipc server
// To implement factory class for ipc objects
// 
//

#include "factory.h"
#include "messagequeue.h"
#include "sharedmem.h"
#include "semaphorequeue.h"
#include "filesocketaddrmanager.h"

//gives message queue handler
 MRequestHandler* PosixIpcFactory::CreateMsgQueueHandlerL()
	{
	return CMsgQueueHandler::NewL();
	}

//returns a message queue
CMessageQueue* PosixIpcFactory::CreateMessageQueueL(const RMessage2& aMessage)
	{
	return CMessageQueue::NewL(aMessage);
	}

//gives shared memory handler
MRequestHandler* PosixIpcFactory::CreateSharedMemoryHandlerL()
	{
	return CSharedMemHandler::NewL();
	}

//returns a shared memory
CSharedMem* PosixIpcFactory::CreateSharedMemoryL(const RMessage2& aMessage)
	{
	return CSharedMem::NewL(aMessage);
	}

	
//gives semaphore queue handler
MRequestHandler* PosixIpcFactory::CreateSemQueueHandlerL()
	{
	return CSemQueueHandler::NewL();
	}
	
//returns a semaphore queue
CSemaphoreQueue* PosixIpcFactory::CreateSemaphoreQueueL(const RMessage2& aMessage)
	{
	return CSemaphoreQueue::NewL(aMessage);
	}

//creates a local socket address manager
MRequestHandler* PosixIpcFactory::CreateLocSocketAddrManagerL()
	{
	return CLocSocketAddrManager::NewL();
	}
	
