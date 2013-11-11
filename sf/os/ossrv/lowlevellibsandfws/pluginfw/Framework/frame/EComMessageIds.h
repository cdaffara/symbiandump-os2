// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Define the enumeration for the service op codes
// 
//

#ifndef __ECOMMESSAGEIDS_H__
#define __ECOMMESSAGEIDS_H__

#include <e32base.h>

/**
A Struct class used to encapsulate the parameters
required during ListImplementations
@internalComponent
*/
class TListImplParam
	{
public:
	TInt iMatchType;
	TInt iBufferSize;
	TBool iCapabilityCheck;
	};

/**
ECom Framework service request codes
@internalComponent
*/
enum TEComServiceMessages
	{
	ENotifyOnChange,
	ECancelNotifyOnChange,
	EListImplementations,
	EListResolvedImplementations,
	EListCustomResolvedImplementations,
	ECollectImplementationsList,
	EGetImplementationCreationMethod,
	EGetResolvedCreationMethod,
	EGetCustomResolvedCreationMethod,
	//EDestroyedImplementation obsolete due to implementation
	//creation relocation to client side from server
	EDestroyedImplementation,
	EEnableImplementation,
	EDisableImplementation,
	// List interfaces
	EListExtendedInterfaces,
	//This is a generic message intended for testing purposes, should be the last one.
	ESetGetParameters
	};


/**
Constants for UidType locations for service message UID parameters
@internalComponent
*/
const TInt KInterfaceUidIndex	= 0;
const TInt KDtorKeyUidIndex		= 1;
const TInt KResolverUidIndex	= 2;


/**
Message slot constants for
IPC parameter passing consistency
Note : the slot range is 0-3
@internalComponent
*/
const TInt KIPCParameterMessageDes				= 0;
const TInt KIPCParameterUids					= 0;
const TInt KIPCParameterDesLength				= 1;
const TInt KIPCParameterResolverParams			= 1;
const TInt KIPCParameterResolverParamsType		= 2;
const TInt KIPCParameterMatchStrExtInf			= 1;

/**
Message slot constants for List Interfaces parameter passing
@internalComponent
*/
const TInt KIPCParameterInterfaceParam			= 0;
const TInt KIPCParameterImplementationUid		= 0;
const TInt KIPCParameterBufferSize				= 1;
const TInt KIPCParameterInterfaceData			= 2;

/**
Matching pointer access methods for ReadL
@internalComponent
*/
#define KIPCParameterMessageDesPtr				aMessage.Ptr0()
#define KIPCParameterUidsPtr					aMessage.Ptr0()
#define KIPCParameterResolverParamsPtr			aMessage.Ptr1()
#define KIPCParameterResolverParamsTypePtr		aMessage.Int2()

/**
@internalComponent
*/
const TInt KIPCReturnParameterUids				= 0;
const TInt KIPCReturnParameterIIArray			= 3;
const TInt KIPCReturnParameterIIArraySize		= 3;
const TInt KIPCReturnParameterCreationMethod	= 3;

/**
Matching pointer access methods for WriteL
@internalComponent
*/
#define KIPCReturnParameterUidsPtr				0
#define KIPCReturnParameterIIArrayPtr			3
#define KIPCReturnParameterIIArraySizePtr		3
#define KIPCReturnParameterCreationMethodPtr	3

#endif	// __ECOMMESSAGEIDS_H__
