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
 *  Name        : server.h
 *  Part of     : ipc server
* To implement the Ipc utilites functions
* 
*
*/

 

#ifndef SERVER_H_
#define SERVER_H_

#include <e32base.h>
#include <errno.h>
#include "factory.h"
#include "ipclimits.h"


#define UNIX_BASE   TTime(MAKE_TINT64(0x00dcddb3,0x0f2f8000))    // 00:00, Jan 1st 1970

//Semaphore specific macro
#define GETSIZE 100
#define MAXINT  12

class RMessage2;

class MRequestHandler
	{
public:
	virtual void ServiceL(const RMessage2& aMessage) = 0;
	virtual ~MRequestHandler(){}
	};
	
class IpcUtils
	{
public:
	static TInt GetPid(const RMessage2&);
	static TInt CurrentTimeL();
	static TInt GetRandom();
private:
	static TInt64 iSeed;
	};



enum TIpcService
	{
	EMsgGet,//message Queue
	EMsgCtl,
	EMsgRcv,
	EMsgSnd,
	EShmGet,//shared memory
	EShmAt,
	EShmDt,
	EShmCtl,
	ESemGet,
	ESemCtl,
	ESemOp,
#ifdef SYMBIAN_OE_LIBRT
	ESetShmSize,
	EGetShmSize,
#endif //SYMBIAN_OE_LIBRT
	EFileSocketGetPathByPort,
	EFileSocketGetPortByPath,
	EFileSocketAddPath,
	EFileSocketRemovePath
	};

//ipc permissions
enum TAccessPermisstions
	{
	EUserRead  = 0400,
	EUserWrite   = 0200,
	EOthersRead  = 0004,
	EOthersWrite = 0002
	};	
	
#endif /*SERVER_H_*/

