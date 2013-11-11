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
* Name        : ipclimits.h
* Part of     : ipc server
* System defined limits for semaphore, message queue and 
* shared memory
* 
*
*/



#ifndef IPCLIMITS_H_
#define IPCLIMITS_H_

//Message Queue Limits
const TInt KIpcMaxMsgSize	= 16*1024;		//Max message size in bytes
const TInt KIpcMaxQueueSize	= 1024*16*128;	//Max message queue size in bytes	
const TInt KIpcMaxNumberOfQueues = 128;	//Max message queue in the system

//Semaphore Limits
const TInt KIpcMaxSemArray  = 128; 		//max number of arrays
const TInt KIpcMaxSem		= 128; 		//max semaphores per array
const TInt KIpcMaxSystemSem = KIpcMaxSemArray*KIpcMaxSem; 
										//max semaphores system wide = 32000
const TInt KIpcMaxSemVal 	= 32767;	//semaphore max value
const TInt KIpcMaxSemOp 	= 32;		//max ops per semop call
const TInt KIpcMaxUn		= KIpcMaxSemOp;  	//max num of undo entries per process 
const TInt KIpcMaxSystemUn  = KIpcMaxSystemSem; //num of undo structures system wide

//Shared Limits
const TInt KIpcMaxShmObjects = 128;            //Max shared memory objects
const TInt KIpcMaxShmSize	 = 512*1024;        //Max shared memory size


#endif //IPCLIMITS_H_
