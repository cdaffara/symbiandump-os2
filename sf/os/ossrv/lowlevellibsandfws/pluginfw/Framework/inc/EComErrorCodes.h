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
// All the ECOM specific error codes returned by the ECOM API to a client
// are defined here.
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef __ECOMERRORCODES_H__
#define __ECOMERRORCODES_H__

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <ecom/ecomextendedinterfaceerrorcodes.h>
#endif

/**
The base value for Shared Error codes for the ECOM framework and client
*/
const TInt KEComErrBase								= -17000;

/**
The last value for ECom Frmework errors
This must be modified when error codes are added/deleted
*/
const TInt KEComErrLast								= -17033;

//
// Error codes which may be received by a client application
//
/**
The call to instantiate an Interface Implementation failed
because no registrations exist for this interface.
*/
const TInt KEComErrNoInterfaceIdentified			= -17004;

/**
The Non-Default resolver could not be instantiated.  The most likely
situation is that an incorrect implementation uid was supplied.
*/
const TInt KEComErrNoResolver						= -17014;

/**
The client has requested too many notifications.  ECom can only support
a finite number of notifications
*/
const TInt KEComErrTooManyNotificationsOutstanding	= -17026;

/**
An ECom service was requested with an essential parameter missing.  For example,
this error code will be supplied when calling an overload of CreateImplementationL
which uses a non-default resolver if you fail to supply the resolver
*/
const TInt KEComErrMissingParameter					= -17028;

/** The registry index is invalid but the client has requested notifications - so
they will receive a notification when the index is ready again
*/
const TInt KEComErrListInvalidAwaitNotification		= -17029;

/**
The registry index is invalid and the client has not requested notifications.
They will have to try again later after discoveries have completed.
*/
const TInt KEComErrListCurrentlyUnavailable			= -17030;


//
// Codes which are only used for Panics
//
/**
The Client requested a service which is not supplied by this framework
*/
const TInt KEComErrUnknownService					= -17002;

/**
The Interface Implementation reference count is invalid
*/
const TInt KEComErrReferenceCountInvalid			= -17013;

/**
The Interface Implementations destruction could not be recorded
*/
const TInt KEComErrDestructionFailed				= -17017;

/**
The uid from a resource plugin does not match the uid from its
corrsponding DLL.
*/
const TInt KEComErrMismatchedTags					= -17023;

/**
The scanning timer for re-discovering plugins is not found in the list
of scanning timers.
*/
const TInt KEComErrNotFoundScanTimer					= -17033;

/**
The client called DestroyedImplementation with a destruction
key of NULL.  This is an irrecoverable error because the object
cannot be deleted.
*/
const TInt KEComErrDestroyingWithNullDtorKey		= -17027;

//
// Codes which are currently unused but reserved for future expansion
//
/**
The call to Enable the registered Interface Implementation failed
*/
const TInt KEComErrEnableFailed						= -17005;

/**
The call to Disable the registered Interface Implementation failed
*/
const TInt KEComErrDisableFailed					= -17006;

/**
The call to instantiate an Interface Implementation failed
because no appropriate implementation was found.
*/
const TInt KEComErrNoRegistrationsFound				= -17007;

/**
The call to Remove the registered Interface Implementation failed
*/
const TInt KEComErrUninstallFailed					= -17009;

/**
The call to Re-register an Interface Implementation failed
*/
const TInt KEComErrReinstallFailed					= -17010;

/**
The call to Register an Interface Implementation failed
*/
const TInt KEComErrRegistrationFailed				= -17011;

/**
The client does not have a session open with the framework
*/
const TInt KEComErrNotConnected						= -17012;

/**
The call to Suspend registration activities failed
*/
const TInt KEComErrSuspendFailed					= -17015;

/**
The call to Resume registration activities failed
*/
const TInt KEComErrResumeFailed						= -17016;

/**
An attempt has been made to add a drive to the registry which
already exists
*/
const TInt KEComErrDriveAlreadyInstalled			= -17018;

/**
An IIC has been found which does not have a corresponding
registration information file
*/
const TInt KEComErrNoRegistrationData				= -17021;



#endif	// __ECOMERRORCODES_H__
