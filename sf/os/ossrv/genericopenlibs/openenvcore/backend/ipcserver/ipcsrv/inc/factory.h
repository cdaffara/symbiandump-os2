/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
 *  Name        : factory.h
 *  Part of     : ipc server
* To implement the message queue
* 
*
*/

 

#ifndef FACTORY_H_
#define FACTORY_H_

class MRequestHandler;
class CMessageQueue;
class CSemaphoreQueue;
class RMessage2;
class CSharedMem;

class PosixIpcFactory 
	{
public:
static MRequestHandler* CreateMsgQueueHandlerL();
static CMessageQueue* CreateMessageQueueL(const RMessage2& aMessage);
static MRequestHandler* CreateSharedMemoryHandlerL();
static CSharedMem* CreateSharedMemoryL(const RMessage2& aMessage);
static MRequestHandler* CreateSemQueueHandlerL();
static CSemaphoreQueue* CreateSemaphoreQueueL(const RMessage2& aMessage);
static MRequestHandler* CreateLocSocketAddrManagerL();
};

#endif /*FACTORY_H_*/
